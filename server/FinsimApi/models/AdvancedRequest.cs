public class AdvancedRequest {
    public Dictionary<string, Allocation> allocations { get; set; }
}

public class Allocation {
    public double initial { get; set; }
    public double monthly { get; set; }
    public int years { get; set; }
}