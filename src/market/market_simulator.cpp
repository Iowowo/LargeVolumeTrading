#include "market/market_simulator.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace lvt {

MarketSimulator::MarketSimulator(const std::string& csv_file_path) : csv_file_path_(csv_file_path) {}

// I had to debug this function because I had missed a couple of lines,
// so it looks a bit bloated and I have to remove half of the code
// from it, really
bool MarketSimulator::Load() {
  market_data_.clear();
  std::ifstream file(csv_file_path_);
  if (!file.is_open()) {
    std::cerr << "[Error] Cannot open file: " << csv_file_path_ << std::endl;
    return false;
  }
  std::string line;
  bool header_skipped = false;
  int lines_processed = 0;
  int lines_skipped = 0;
  while (std::getline(file, line)) {
    // Skip blank or all-whitespace lines
    if (line.empty() || std::all_of(line.begin(), line.end(), isspace)) {
      lines_skipped++;
      continue;
    }
    // Skip CSV header (first non-blank line containing "timestamp" or "price")
    if (!header_skipped && (
         line.find("timestamp") != std::string::npos ||
         line.find("price") != std::string::npos)) {
      header_skipped = true;
      lines_skipped++;
      continue;
    }
    std::istringstream iss(line);
    std::string timestamp, price_str, volume_str;
    if (std::getline(iss, timestamp, ',') &&
        std::getline(iss, price_str, ',') &&
        std::getline(iss, volume_str, ',')) {
      try {
        MarketData data = {timestamp, std::stod(price_str), std::stod(volume_str)};
        market_data_.push_back(data);
        lines_processed++;
      } catch (const std::exception& e) {
        lines_skipped++;
        continue;
      }
    } else {
      lines_skipped++;
    }
  }
  if (market_data_.empty()) {
    std::cerr << "[Error] No valid data loaded. Processed: " << lines_processed 
              << ", Skipped: " << lines_skipped << std::endl;
    return false;
  }
  return true;
}

const std::vector<MarketData>& MarketSimulator::GetMarketData() const {
  return market_data_;
}

}  // namespace lvt

