#ifndef LARGE_VOLUME_TRADING_LIMIT_ORDER_SPEED_MODEL_H_
#define LARGE_VOLUME_TRADING_LIMIT_ORDER_SPEED_MODEL_H_

#include <vector>
#include "market/market_simulator.h"

namespace lvt {

class LimitOrderSpeedModel {
 public:
  LimitOrderSpeedModel();

  // Sets the market data context (timestamp, price, volume for each interval).
  void SetMarketData(const std::vector<MarketData>& market_data);

  // Computes the speed (order size) for each interval, subject to max_speed (0 = unlimited).
  void ComputeOptimalSpeedSchedule(double total_volume, int intervals, double max_speed = 0.);

  // Returns per-interval order sizes.
  const std::vector<double>& GetSchedule() const;

 private:
  std::vector<MarketData> market_data_;
  std::vector<double> schedule_;
};

}  // namespace lvt

#endif  // LARGE_VOLUME_TRADING_LIMIT_ORDER_SPEED_MODEL_H_
