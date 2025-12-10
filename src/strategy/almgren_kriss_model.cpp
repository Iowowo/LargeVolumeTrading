#include "strategy/almgren_kriss_model.h"
#include <cmath>
#include <algorithm>

namespace lvt {

AlmgrenKrissModel::AlmgrenKrissModel()
    : eta_(0), gamma_(0), sigma_(0), lambda_(0), total_volume_(0) {}

void AlmgrenKrissModel::SetMarketData(const std::vector<double>& prices, double total_volume) {
  prices_ = prices;
  total_volume_ = total_volume;
}

void AlmgrenKrissModel::SetParameters(double eta, double gamma, double sigma, double lam) {
  eta_ = eta;
  gamma_ = gamma;
  sigma_ = sigma;
  lambda_ = lam;
}

void AlmgrenKrissModel::ComputeOptimalSchedule() {
  schedule_.clear();
  const int N = static_cast<int>(prices_.size());
  if (N == 0 || total_volume_ <= 0) return;
  if (N == 1) {
    schedule_.push_back(total_volume_);
    return;
  }
  double uniform_chunk = total_volume_ / N;
  if (eta_ <= 1e-10 || lambda_ <= 1e-10 || sigma_ <= 1e-10) {
    schedule_ = std::vector<double>(N, uniform_chunk);
    return;
  }
  double kappa = std::sqrt(lambda_ * sigma_ * sigma_ / eta_);
  if (!std::isfinite(kappa) || kappa <= 0) {
    schedule_ = std::vector<double>(N, uniform_chunk);
    return;
  }
  const double MAX_KAPPA_DISTANCE = 700.0;
  double center = 0.5 * (N - 1);
  double max_distance = std::max(center, static_cast<double>(N - 1) - center);
  if (kappa * max_distance > MAX_KAPPA_DISTANCE) {
    schedule_ = std::vector<double>(N, uniform_chunk);
    return;
  }
  double norm = 0.0;
  for (int i = 0; i < N; ++i) {
    double distance = i - center;
    double arg = kappa * distance;
    double weight = std::cosh(arg);
    if (!std::isfinite(weight) || weight > 1e300) {
      schedule_ = std::vector<double>(N, uniform_chunk);
      return;
    }
    schedule_.push_back(weight);
    norm += weight;
    if (!std::isfinite(norm)) {
      schedule_ = std::vector<double>(N, uniform_chunk);
      return;
    }
  }
  if (norm <= 0 || !std::isfinite(norm)) {
    schedule_ = std::vector<double>(N, uniform_chunk);
    return;
  }
  for (auto& x : schedule_) {
    x = total_volume_ * x / norm;
    if (!std::isfinite(x)) x = uniform_chunk;  // Safety fallback
  }
}

const std::vector<double>& AlmgrenKrissModel::GetSchedule() const {
  return schedule_;
}

}  // namespace lvt
