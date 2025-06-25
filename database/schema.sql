/*
    tickers and prices are for indexed historical market data
    simulations records parameters, timestamp, and result
    simulation_tickers is the asset associated
    returns is a precomputed monte carlo run
*/

-- Stores each distinct stock AAPL/GOOG
CREATE TABLE IF NOT EXISTS tickers (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    symbol TEXT UNIQUE NOT NULL,
    name TEXT
);

-- Holds close prices for each ticker, calculate returns
CREATE TABLE IF NOT EXISTS prices (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ticker_id INTEGER NOT NULL,
    date DATE NOT NULL,
    close REAL NOT NULL,
    FOREIGN KEY (ticker_id) REFERENCES tickers(id)
);

-- Logs each Monte Carlo, input parameters and time
CREATE TABLE IF NOT EXISTS simulations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    params TEXT,
    result TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Simulated portfolio
CREATE TABLE IF NOT EXISTS simulation_tickers (
    simulation_id INTEGER NOT NULL,
    ticker_id INTEGER NOT NULL,
    weight REAL NOT NULL,
    PRIMARY KEY (simulation_id, ticker_id),
    FOREIGN KEY (simulation_id) REFERENCES simulations(id),
    FOREIGN KEY (ticker_id) REFERENCES tickers(id)
);

-- Stores precomputed returns so we dont recompute
CREATE TABLE IF NOT EXISTS returns (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ticker_id INTEGER,
    date DATE,
    daily_ret REAL,
    FOREIGN KEY (ticker_id) REFERENCES tickers(id)
);