// Example: Optimal Limit Order Speed Demo
// Usage: ./optimal_speed_example <input_csv> <total_volume> <intervals> <max_speed>
// This demonstrates dividing total volume over time with a speed cap.
// Market data (timestamp,price,volume) required only for interval count/context.
#include <iostream>
#include <iomanip>
#include "market/market_simulator.h"
#include "strategy/limit_order_speed_model.h"

int main(int argc, char* argv[]) {
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0] << " <input_csv> <total_volume> <intervals> <max_speed>\n";
    return 1;
  }
  std::string input_csv = argv[1];
  double total_volume = std::stod(argv[2]);
  int intervals = std::stoi(argv[3]);
  double max_speed = std::stod(argv[4]);

  std::cout << "[Log] Loading market data from " << input_csv << "...\n";
  lvt::MarketSimulator sim(input_csv);
  if (!sim.Load()) {
    std::cerr << "Failed to load market data!\n";
    return 1;
  }
  std::cout << "[Log] Loaded " << sim.GetMarketData().size() << " intervals." << std::endl;

  lvt::LimitOrderSpeedModel speed_model;
  speed_model.SetMarketData(sim.GetMarketData());
  speed_model.ComputeOptimalSpeedSchedule(total_volume, intervals, max_speed);
  const auto& schedule = speed_model.GetSchedule();

  std::cout << "[Log] Optimal Speed Schedule:" << std::endl;
  std::cout << "interval,trade_volume" << std::endl;
  double total_sched = 0;
  for (size_t i = 0; i < schedule.size(); ++i) {
    std::cout << i << "," << schedule[i] << std::endl;
    total_sched += schedule[i];
  }
  std::cout << std::fixed << std::setprecision(4);
  std::cout << "[Log] Total scheduled volume: " << total_sched << std::endl;
  return 0;
}
