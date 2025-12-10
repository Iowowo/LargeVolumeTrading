// Example: Almgren-Kriss Optimal Execution Demo
// Usage: ./almgren_kriss_example <input_csv> <total_volume> <eta> <gamma> <sigma> <lambda>
// Input CSV: columns timestamp,price,volume.
// This demonstrates configuring the model, computing AK schedule, and logging outputs.
#include <iostream>
#include <iomanip>
#include <vector>
#include "market/market_simulator.h"
#include "strategy/almgren_kriss_model.h"

int main(int argc, char* argv[]) {
  if (argc < 7) {
    std::cerr << "Usage: " << argv[0] << " <input_csv> <total_volume> <eta> <gamma> <sigma> <lambda>\n";
    return 1;
  }
  std::string input_csv = argv[1];
  double total_volume = std::stod(argv[2]);
  double eta = std::stod(argv[3]);
  double gamma = std::stod(argv[4]);
  double sigma = std::stod(argv[5]);
  double lambda = std::stod(argv[6]);

  std::cout << "[Log] Loading market data from " << input_csv << "...\n";
  lvt::MarketSimulator sim(input_csv);
  if (!sim.Load()) {
    std::cerr << "Failed to load market data!\n";
    return 1;
  }
  const auto& all_data = sim.GetMarketData();
  std::cout << "[Log] Loaded " << all_data.size() << " prices." << std::endl;

  // Filter to first day only (extract date from first timestamp, use until date changes)
  std::vector<double> prices;
  if (!all_data.empty()) {
    std::string first_date = all_data[0].timestamp.substr(0, 10);
    for (const auto& d : all_data) {
      if (d.timestamp.substr(0, 10) == first_date) {
        prices.push_back(d.price);
      } else {
        break;
      }
    }
  }
  std::cout << "[Log] Using first day: " << prices.size() << " intervals." << std::endl;
  lvt::AlmgrenKrissModel ak;
  ak.SetMarketData(prices, total_volume);
  ak.SetParameters(eta, gamma, sigma, lambda);

  ak.ComputeOptimalSchedule();
  const auto& sched = ak.GetSchedule();

  std::cout << "[Log] Almgren-Kriss Trading Schedule:" << std::endl;
  std::cout << "interval,trade_volume" << std::endl;
  double totalsum = 0;
  for (size_t i = 0; i < sched.size(); ++i) {
    std::cout << i << "," << sched[i] << std::endl;
    totalsum += sched[i];
  }
  std::cout << std::fixed << std::setprecision(4);
  std::cout << "[Log] Total scheduled volume: " << totalsum << std::endl;
  return 0;
}
