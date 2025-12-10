import yfinance as yf

def main():
    start = "2025-11-24"
    end = "2025-11-30"
    ticker = "AAPL"
    df = yf.download(ticker, interval="1m", start=start, end=end)
    out = df[['Close', 'Volume']].copy()
    out.reset_index(inplace=True)
    out.rename(columns={'Datetime': 'timestamp', 'Close': 'price', 'Volume': 'volume'}, inplace=True)
    out.to_csv("AAPL_sample.csv", index=False)
    print(f"Saved raw data to AAPL_sample.csv for {ticker}, 1m bars, {start} to {end}.")

    # We have to remove ',APPL,APPL' from csv
    with open("AAPL_sample.csv", "r") as fin:
        lines = fin.readlines()

    if len(lines) >= 2 and (lines[1].startswith(",") or "AAPL" in lines[1]):
        with open("AAPL_sample.csv", "w") as fout:
            fout.write(lines[0])
            fout.writelines(lines[2:])

if __name__ == "__main__":
    main()