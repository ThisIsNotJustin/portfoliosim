using Microsoft.AspNetCore.Mvc;
using Microsoft.Data.Sqlite;
using System.Text.Json; 
using System.Diagnostics;

[ApiController]
[Route("api/[controller]")]
public class AdvancedController : ControllerBase {
    private readonly AlphaVantageService _alphaVantage;

    public AdvancedController(AlphaVantageService alphaVantage) {
        _alphaVantage = alphaVantage;
    }

    [HttpPost]
    public async Task<IActionResult> Calculate([FromBody] AdvancedRequest req) {
        var databasePath = "Data Source=../../database/finsim.db";
        var totalAverage = 0.0;
        var stocksChecked = 0;
        var missingOrStale = new List<string>();
        var paramJson = JsonSerializer.Serialize(req);
        string? cachedResult = null;

        using var conn = new SqliteConnection(databasePath);
        await conn.OpenAsync();

        var simCmd = conn.CreateCommand();
        simCmd.CommandText = "SELECT result FROM simulations WHERE params = $params";
        simCmd.Parameters.AddWithValue("$params", paramJson);
        cachedResult = (string?)await simCmd.ExecuteScalarAsync();
        if (cachedResult != null) {
            return Ok(JsonSerializer.Deserialize<object>(cachedResult));
        }

        foreach (var allocation in req.allocations) {
            var symbol = allocation.Key;
            long tickerId;

            var tickerCmd = conn.CreateCommand();
            tickerCmd.CommandText = "SELECT id FROM tickers WHERE symbol = $symbol";
            tickerCmd.Parameters.AddWithValue("$symbol", symbol);
            var tickerIdObj = await tickerCmd.ExecuteScalarAsync();

            if (tickerIdObj == null) {
                var insertTickerCmd = conn.CreateCommand();
                insertTickerCmd.CommandText = "INSERT INTO tickers (symbol) VALUES ($symbol); SELECT last_insert_rowid();";
                insertTickerCmd.Parameters.AddWithValue("$symbol", symbol);
                tickerId = (long)(await insertTickerCmd.ExecuteScalarAsync());
            } else {
                tickerId = (long)tickerIdObj;
            }

            var priceCmd = conn.CreateCommand();
            priceCmd.CommandText = @"
                SELECT date
                FROM prices
                WHERE ticker_id = $tickerId
                ORDER BY date DESC LIMIT 1";
            priceCmd.Parameters.AddWithValue("$tickerId", tickerId);
            var lastDateStr = (string?)await priceCmd.ExecuteScalarAsync();

            bool needsUpdate = true;
            if (!string.IsNullOrEmpty(lastDateStr)) {
                var lastDate = DateTime.Parse(lastDateStr);
                if ((DateTime.UtcNow - lastDate).TotalDays <= 1) {
                    needsUpdate = false;
                }
            }

            if (needsUpdate) {
                try {
                    var prices = await _alphaVantage.FetchDailyPricesAsync(symbol);
                    await _alphaVantage.PricesAndReturnsAsync(conn, tickerId, prices);

                    var csvPath = $"../../data/{symbol}_av_history.csv";
                    await _alphaVantage.SavePricesAsCsv(symbol, csvPath);
                } catch {
                    return BadRequest(new { error = $"Could not update data for {symbol}" });
                }
            }
        }

        var engineInput = JsonSerializer.Serialize(req);

        var info = new ProcessStartInfo {
            FileName = "../../engine/src/sim/monte_carlo_cli",
            RedirectStandardOutput = true,
            RedirectStandardInput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
            CreateNoWindow = true
        };

        string output;
        Console.WriteLine("Sending to engine: " + engineInput);
        using (var process = Process.Start(info)) {
            await process.StandardInput.WriteAsync(engineInput);
            process.StandardInput.Close();
            output = await process.StandardOutput.ReadToEndAsync();
            string error = await process.StandardError.ReadToEndAsync();
            Console.WriteLine("Engine error: " + error);
            process.WaitForExit();
        }

        Console.WriteLine("Output from engine: " + output);
        var resultObj = JsonSerializer.Deserialize<object>(output);

        var insertCmd = conn.CreateCommand();
        insertCmd.CommandText = "INSERT INTO simulations (params, result) VALUES ($params, $result)";
        insertCmd.Parameters.AddWithValue("$params", paramJson);
        insertCmd.Parameters.AddWithValue("$result", output);
        await insertCmd.ExecuteNonQueryAsync();

        return Ok(resultObj);
    }
}