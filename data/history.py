from alpha_vantage.timeseries import TimeSeries
import pandas as pd

key = ""
ts = TimeSeries(key=key, output_format='pandas')
ticker = "AAPL"
data, meta_data = ts.get_daily(symbol=ticker, outputsize='full')
data = data.sort_index()

csv = f'{ticker}_av_history.csv'
data.to_csv(csv)
print("Data saved to csv")