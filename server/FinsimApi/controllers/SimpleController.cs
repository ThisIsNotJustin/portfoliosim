using Microsoft.AspNetCore.Mvc;
using System.Diagnostics;

[ApiController]
[Route("api/[controller]")]
public class SimpleController : ControllerBase {
    [HttpPost]
    public async Task<IActionResult> Calculate([FromBody] SimpleRequest req) {
        var info = new ProcessStartInfo {
            FileName = "../../engine/src/sim/standard_cli",
            Arguments = $"{req.Initial} {req.Monthly} {req.Years}",
            RedirectStandardOutput = true,
            UseShellExecute = false,
            CreateNoWindow = true
        };

        var process = Process.Start(info);
        var results = new List<double>();
        while (!process.StandardOutput.EndOfStream) {
            var line = await process.StandardOutput.ReadLineAsync();
            if (double.TryParse(line, out double value)) {
                results.Add(value);
            }
        }

        process.WaitForExit();

        return Ok(new {
            values = results,
            projectedValue = results.LastOrDefault()
        });
    }
}