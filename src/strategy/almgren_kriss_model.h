#ifndef LARGE_VOLUME_TRADING_ALMGREN_KRISS_MODEL_H_
#define LARGE_VOLUME_TRADING_ALMGREN_KRISS_MODEL_H_

#include <vector>

namespace lvt {

class AlmgrenKrissModel {
 public:
  AlmgrenKrissModel();
  void SetMarketData(const std::vector<double>& prices, double total_volume);
  void SetParameters(double eta, double gamma, double sigma, double lam);
  void ComputeOptimalSchedule();
  const std::vector<double>& GetSchedule() const;

 private:
  double eta_;
  double gamma_;
  double sigma_;
  double lambda_;
  double total_volume_;
  std::vector<double> prices_;
  std::vector<double> schedule_;
};

}  // namespace lvt

#endif  // LARGE_VOLUME_TRADING_ALMGREN_KRISS_MODEL_H_
