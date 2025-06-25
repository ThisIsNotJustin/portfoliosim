"use client"; 

import Image from "next/image";
import Header from "../components/header";
import { useEffect, useState } from "react";
import { BarChart } from "@mui/x-charts/BarChart";

const currencySymbols = {
  USD: '$',
  GBP: '£',
  EUR: '€',
  CAD: 'C$',
};

const availableStocks = ['AAPL', 'MSFT', 'GOOG', 'AMZN', 'TSLA'];

export default function Home() {
  const [currency, setCurrency] = useState('USD');
  const [initial, setInitial] = useState('');
  const [monthly, setMonthly] = useState('');
  const [years, setYears] = useState('');
  const [allocations, setAllocations] = useState(
    availableStocks.reduce((acc, sym) => {
      acc[sym] = { initial: '', monthly: '', years: '' };
      return acc;
    }, {})
  );
  const [showAdvanced, setShowAdvanced] = useState(false);
  const [loading, setLoading] = useState(false);
  const [result, setResult] = useState(null);

  const currentYear = new Date().getFullYear();

  const handleCurrencyChange = (cur) => {
    if (currencySymbols[cur]) setCurrency(cur);
  };

  // keep existing and overwrite field with new value
  const handleAllocationChange = (symbol, field, value) => {
    setAllocations((prev) => ({
      ...prev,
      [symbol]: { ...prev[symbol], [field]: value },
    }));
  };

  const handleSubmit = async (e) => {
    e.preventDefault();
    setLoading(true);
    const payload = {
      ...(showAdvanced
        ? {
          /*
            Advanced should look like this I believe:
            {
              allocations: {
                AAPL: { initial: 1000, monthly: 100, years: 5 },
                MSFT: { initial: 500, monthly: 100, years: 3 },
              }
            }
          */
            allocations: Object.fromEntries(
              Object.entries(allocations)
                .filter(([, v]) => v.initial || v.monthly || v.years)
                .map(([sym, v]) => [
                  sym,
                  {
                    initial: parseFloat(v.initial) || 0,
                    monthly: parseFloat(v.monthly) || 0,
                    years: parseInt(v.years, 10) || 0,
                  },
                ])
            ),
          }
        : 
          /*
            Simple should look like:
            {
              initial: 1000,
              monthly: 100,
              years: 5
            }
          */
          {
            initial: parseFloat(initial) || 0,
            monthly: parseFloat(monthly) || 0,
            years: parseInt(years, 10) || 0,
          }),
    };

    try {
      const endpoint = showAdvanced ? 'http://localhost:5223/api/advanced' :
        'http://localhost:5223/api/simple';
      const response = await fetch(endpoint, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload),
      });
      const data = await response.json();
      setResult(data);
    } catch (error) {
      console.error('Error fetching portfolio:', error);
    } finally {
      setLoading(false);
    }
  };

  const symbol = currencySymbols[currency];

  const inputClasses =
    "appearance-none border border-accent-300 rounded-lg p-2 focus:outline-none focus:ring-2 focus:ring-white focus:ring-offset-0";

  const smallInputClasses =
    "appearance-none border border-accent-300 rounded-lg p-1 w-20 focus:outline-none focus:ring-2 focus:ring-white focus:ring-offset-0";

  const valuesArray = result && result.values ? Array.isArray(result.values) ? result.values :
    result.values[Object.keys(result.values)[0]] : [];

  const projectedValue = result && result.values ? (typeof result.projectedValue === 'number' ? result.projectedValue : result.projectedValue[Object.keys(result.projectedValue)[0]]) : undefined;
  
    return (
    <div>
      <Header />
      <div className="grid grid-rows-[auto_1fr_auto] items-center justify-items-center p-8 pb-20 gap-16 sm:p-20 font-[family-name:var(--font-geist-sans)]">
        <div className="flex gap-2">
          {Object.keys(currencySymbols).map((cur) => (
          <button
            key={cur}
            type="button"
            onClick={() => handleCurrencyChange(cur)}
            className={`px-4 py-2 rounded-full font-medium transition-colors ${
              currency === cur ? 'bg-accent-500 text-white' : 
              'bg-gray-200 text-gray-800 hover:bg-gray-300'}`
            }
          >
            {cur}
          </button>
          ))}
        </div>
        <main className="flex flex-col gap-8 row-start-2 min-w-full">
          <form onSubmit={handleSubmit} className="flex flex-col gap-6">
            {!showAdvanced ? (
              <>           
                <div className="flex flex-col">
                  <label className="mb-1 font-medium">Initial Investment ({symbol})</label>
                  <input
                    type="number"
                    value={initial}
                    onChange={(e) => setInitial(e.target.value)}
                    className={inputClasses}
                    placeholder="e.g. 10000"
                    required
                  />
                </div>

                <div className="flex flex-col">
                  <label className="mb-1 font-medium">Monthly Contribution ({symbol})</label>
                  <input
                    type="number"
                    value={monthly}
                    onChange={(e) => setMonthly(e.target.value)}
                    className={inputClasses}
                    placeholder="e.g. 200"
                    required
                  />
                </div>

                <div className="flex flex-col">
                  <label className="mb-1 font-medium">Time (years)</label>
                  <input
                    type="number"
                    value={years}
                    onChange={(e) => setYears(e.target.value)}
                    className={inputClasses}
                    placeholder="e.g. 10"
                    required
                  />
                </div>
              </>
            ) : (
              <div className="max-h-64 overflow-auto border border-gray-200 rounded-lg p-4 flex flex-col gap-4">
                <h4 className="font-semibold">Stock Allocations ({symbol})</h4>
                {availableStocks.map((sym) => (
                  <div key={sym} className="flex gap-3 items-center justify-between">
                    <span className="w-12 font-medium">{sym}</span>
                    <div className="flex gap-4">
                      <input
                        type="number"
                        value={allocations[sym].initial}
                        onChange={(e) => handleAllocationChange(sym, 'initial', e.target.value)}
                        placeholder="Initial"
                        className={smallInputClasses}
                      />
                      <input
                        type="number"
                        value={allocations[sym].monthly}
                        onChange={(e) => handleAllocationChange(sym, 'monthly', e.target.value)}
                        placeholder="Monthly"
                        className={smallInputClasses}
                      />
                      <input
                        type="number"
                        value={allocations[sym].years}
                        onChange={(e) => handleAllocationChange(sym, 'years', e.target.value)}
                        placeholder="Years"
                        className={smallInputClasses}
                      />
                    </div>
                  </div>
                ))}
              </div>
            )}

            <button
              type="button"
              onClick={() => setShowAdvanced((p) => !p)}
              className="text-sm text-accent-500 hover:underline self-start"
            >
              {showAdvanced ? 'Use Simple View' : 'Use Advanced Allocations'}
            </button>

            <button
              type="submit"
              disabled={loading}
              className="mt-4 rounded-full bg-accent-500 text-white px-6 py-2 font-medium hover:bg-accent-600 disabled:opacity-50"
            >
              {loading ? 'Calculating...' : 'Calculate'}
            </button>
          </form>

          {result && (
            <div className="mt-6 p-4 border border-accent-300 rounded-lg">
              <div className="flex justify-between items-center">
                <h3 className="text-4xl font-medium">Projected Value</h3>
                <h3 className="text-4xl font-bold">{symbol}{projectedValue !== undefined ? projectedValue.toLocaleString() : valuesArray[years - 1]}</h3>
              </div>
              <div className="mt-4 h-48 flex items-center justify-center rounded-lg">
                <BarChart
                  series = {[
                    {
                      data: valuesArray,
                      color: '#fff',
                    },
                  ]}
                  height = {180}
                  xAxis = {[
                    {
                      data: valuesArray.map((_, i) => `${i + 1 + currentYear}`),
                      scaleType: 'band',
                      label: 'Year',
                      tickLabelStyle: { fill: '#fff' },
                    },
                  ]}
                  yAxis={[
                    {
                      tickLabelStyle: { fill: '#fff' },
                    },
                  ]}
                  slotProps={{
                    tickLabel: { style: { fill: '#fff' } },
                  }}
                  borderRadius={10}
                />
              </div>
            </div>
          )}
        </main>

        <footer className="row-start-3 flex gap-[24px] flex-wrap items-center justify-center">
        </footer>
      </div>
    </div>
  );
}