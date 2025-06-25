using System.Net.Http;
using System.Text.Json;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Data.Sqlite;

public class AlphaVantageService {
    private readonly string _apiKey;
    private readonly HttpClient _http;

    public AlphaVantageService(string apiKey) {
        _apiKey = apiKey;
        _http = new HttpClient();
    }

    public async Task<List<(DateTime date, double close)>> FetchDailyPricesAsync(string symbol) {
        var url = $"https://www.alphavantage.co/query?function=TIME_SERIES_DAILY_ADJUSTED&symbol={symbol}&outputsize=full&apikey={_apiKey}";
        var resp = await _http.GetStringAsync(url);
        using var doc = JsonDocument.Parse(resp);

        var prices = new List<(DateTime, double)>();
        if (!doc.RootElement.TryGetProperty("Time Series (Daily)", out var ts)) return prices;
    
        foreach (var day in ts.EnumerateObject()) {
            var date = DateTime.Parse(day.Name);
            var close = double.Parse(day.Value.GetProperty("5. adjusted close").GetString());
            prices.Add((date, close));
        }
        prices.Sort((a, b) => a.Item1.CompareTo(b.Item1));

        return prices;
    }

    public async Task PricesAndReturnsAsync(SqliteConnection conn, long tickerId, List<(DateTime date, double close)> prices) {
        foreach ((DateTime date, double close) in prices) {
            var cmd = conn.CreateCommand();
            cmd.CommandText = @"
                INSERT OR IGNORE INTO prices (ticker_id, date, close)
                VALUES ($tickerId, $date, $close)";
            cmd.Parameters.AddWithValue("$tickerId", tickerId);
            cmd.Parameters.AddWithValue("$date", date.ToString("yyyy-MM-dd"));
            cmd.Parameters.AddWithValue("$close", close);
            await cmd.ExecuteNonQueryAsync();
        }

        for (int i = 1; i < prices.Count; i++) {
            var prev = prices[i - 1];
            var curr = prices[i];
            var ret = (curr.close - prev.close) / prev.close;
            var cmd = conn.CreateCommand();
            cmd.CommandText = @"
                INSERT OR IGNORE INTO returns (ticker_id, date, daily_ret)
                VALUES ($tickerId, $date, $ret)";
            cmd.Parameters.AddWithValue("$tickerId", tickerId);
            cmd.Parameters.AddWithValue("$date", curr.date.ToString("yyyy-MM-dd"));
            cmd.Parameters.AddWithValue("$ret", ret);
            await cmd.ExecuteNonQueryAsync();
        }
    }

    public async Task SavePricesAsCsv(string symbol, string filePath) {
        var url = $"https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol={symbol}&outputsize=full&datatype=csv&apikey={_apiKey}";
        using var response = await _http.GetAsync(url);
        response.EnsureSuccessStatusCode();
        using var fs = new FileStream(filePath, FileMode.Create, FileAccess.Write, FileShare.None);
        await response.Content.CopyToAsync(fs);
    }
}