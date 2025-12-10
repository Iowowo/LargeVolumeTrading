// Example: VWAP Execution Demo
// Usage: ./vwap_example <input_csv> <total_volume>
// Input CSV: columns timestamp,price,volume (e.g., from download_data.py)
// This demonstrates loading market data and producing a VWAP schedule.
#include <iostream>
#include <iomanip>
#include "market/market_simulator.h"
#include "strategy/vwap_calculator.h"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <input_csv> <total_volume>\n";
    return 1;
  }
  std::string input_csv = argv[1];
  double total_volume = std::stod(argv[2]);

  std::cout << "[Log] Loading market data from " << input_csv << "...\n";
  lvt::MarketSimulator sim(input_csv);
  if (!sim.Load()) {
    std::cerr << "Failed to load market data!\n";
    return 1;
  }
  std::cout << "[Log] Loaded " << sim.GetMarketData().size() << " intervals." << std::endl;

  lvt::VWAPCalculator vwap;
  vwap.SetMarketData(sim.GetMarketData());
  vwap.ComputeVWAPSchedule(total_volume);
  const auto& schedule = vwap.GetSchedule();
  const auto& data = sim.GetMarketData();

  std::cout << "[Log] VWAP Trading Schedule:" << std::endl;
  std::cout << "timestamp,market_price,market_volume,trade_volume" << std::endl;
  double sum_trade = 0, vwap_pv = 0, vwap_nv = 0;
  for (size_t i = 0; i < schedule.size(); ++i) {
    std::cout << data[i].timestamp << "," << data[i].price << "," << data[i].volume << "," << schedule[i] << std::endl;
    sum_trade += schedule[i];
    vwap_pv += data[i].price * schedule[i];
    vwap_nv += schedule[i];
  }
  std::cout << std::fixed << std::setprecision(4);
  std::cout << "[Log] Total scheduled volume: " << sum_trade << std::endl;
  std::cout << "[Log] VWAP (realized): " << (vwap_pv / vwap_nv) << std::endl;
  return 0;
}
