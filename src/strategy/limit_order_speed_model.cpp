#include "strategy/limit_order_speed_model.h"
#include <algorithm>

namespace lvt {

LimitOrderSpeedModel::LimitOrderSpeedModel() = default;

void LimitOrderSpeedModel::SetMarketData(const std::vector<MarketData>& market_data) {
  market_data_ = market_data;
}

void LimitOrderSpeedModel::ComputeOptimalSpeedSchedule(double total_volume, int intervals, double max_speed) {
  schedule_.clear();
  if (total_volume <= 0) return;
  int n = intervals > 0 ? intervals : (int)market_data_.size();
  if (n <= 0) return;
  double chunk = total_volume / n;
  if (max_speed > 0.0) chunk = std::min(chunk, max_speed);
  schedule_ = std::vector<double>(n, chunk);
  double remainder = total_volume - chunk * n;
  if (remainder > 0 && !schedule_.empty()) schedule_.back() += remainder;
}

const std::vector<double>& LimitOrderSpeedModel::GetSchedule() const {
  return schedule_;
}

}  // namespace lvt
