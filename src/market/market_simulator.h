#ifndef LARGE_VOLUME_TRADING_MARKET_SIMULATOR_H_
#define LARGE_VOLUME_TRADING_MARKET_SIMULATOR_H_

#include <string>
#include <vector>

namespace lvt {

struct MarketData {
  std::string timestamp;
  double price;
  double volume;
};

class MarketSimulator {
 public:
  explicit MarketSimulator(const std::string& csv_file_path);
  bool Load();
  const std::vector<MarketData>& GetMarketData() const;

 private:
  std::string csv_file_path_;
  std::vector<MarketData> market_data_;
};

}  // namespace lvt

#endif  // LARGE_VOLUME_TRADING_MARKET_SIMULATOR_H_
