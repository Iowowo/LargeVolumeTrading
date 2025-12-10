#ifndef LARGE_VOLUME_TRADING_VWAP_CALCULATOR_H_
#define LARGE_VOLUME_TRADING_VWAP_CALCULATOR_H_

#include "market_simulator.h"
#include <vector>
#include <string>

namespace lvt {

class VWAPCalculator {
 public:
  VWAPCalculator();
  void SetMarketData(const std::vector<MarketData>& market_data);
  void ComputeVWAPSchedule(double total_volume);
  const std::vector<double>& GetSchedule() const;

 private:
  std::vector<MarketData> market_data_;
  std::vector<double> schedule_;
};

}  // namespace lvt

#endif  // LARGE_VOLUME_TRADING_VWAP_CALCULATOR_H_
