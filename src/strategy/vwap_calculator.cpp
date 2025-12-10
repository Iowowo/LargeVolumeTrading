#include "strategy/vwap_calculator.h"

namespace lvt {

VWAPCalculator::VWAPCalculator() = default;

void VWAPCalculator::SetMarketData(const std::vector<MarketData>& market_data) {
  market_data_ = market_data;
}

void VWAPCalculator::ComputeVWAPSchedule(double total_volume) {
  schedule_.clear();
  if (market_data_.empty() || total_volume <= 0) {
    return;
  }
  double sum_volume = 0.0;
  for (const auto& m : market_data_) sum_volume += m.volume;
  if (sum_volume <= 0) return;
  for (const auto& m : market_data_) {
    schedule_.push_back(total_volume * (m.volume / sum_volume));
  }
}

const std::vector<double>& VWAPCalculator::GetSchedule() const {
  return schedule_;
}

}  // namespace lvt
