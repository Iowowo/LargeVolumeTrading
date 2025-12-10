#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include "market/market_simulator.h"
#include "strategy/vwap_calculator.h"
#include "strategy/limit_order_speed_model.h"
#include "strategy/almgren_kriss_model.h"

void PrintUsage(const char* prog_name) {
  std::cerr << "Usage: " << prog_name
            << " --strategy <VWAP|OptimalSpeed|AlmgrenKriss>"
            << " --input <csv_file>"
            << " --total_volume <volume>"
            << " [--output <output_file>]"
            << " [--intervals <N>] (for OptimalSpeed)"
            << " [--max_speed <speed>] (for OptimalSpeed)"
            << " [--eta <eta>] [--gamma <gamma>] [--sigma <sigma>] [--lambda <lambda>] (for AlmgrenKriss)\n";
}

int main(int argc, char* argv[]) {
  std::map<std::string, std::string> args;
  for (int i = 1; i < argc; i += 2) {
    if (i + 1 < argc) {
      args[argv[i]] = argv[i + 1];
    } else {
      std::cerr << "Error: Missing value for argument: " << argv[i] << "\n";
      PrintUsage(argv[0]);
      return 1;
    }
  }

  if (args.find("--strategy") == args.end() ||
      args.find("--input") == args.end() ||
      args.find("--total_volume") == args.end()) {
    PrintUsage(argv[0]);
    return 1;
  }

  std::string strategy = args["--strategy"];
  std::string csv_file = args["--input"];
  double total_volume;
  try {
    total_volume = std::stod(args["--total_volume"]);
  } catch (const std::exception& e) {
    std::cerr << "Error: Invalid total_volume value: " << args["--total_volume"] << "\n";
    return 1;
  }
  bool has_output = args.find("--output") != args.end();
  std::ostream* out_stream = has_output ? 
    new std::ofstream(args["--output"]) : &std::cout;

  if (has_output && !static_cast<std::ofstream*>(out_stream)->is_open()) {
    std::cerr << "Failed to open output file: " << args["--output"] << "\n";
    return 1;
  }

  lvt::MarketSimulator sim(csv_file);
  if (!sim.Load()) {
    std::cerr << "Failed to load market data from " << csv_file << "\n";
    return 1;
  }

  if (strategy == "VWAP") {
    lvt::VWAPCalculator vwap;
    vwap.SetMarketData(sim.GetMarketData());
    vwap.ComputeVWAPSchedule(total_volume);
    const auto& schedule = vwap.GetSchedule();
    const auto& data = sim.GetMarketData();

    *out_stream << "timestamp,trade_volume\n";
    for (size_t i = 0; i < schedule.size(); ++i) {
      *out_stream << data[i].timestamp << "," << schedule[i] << "\n";
    }
  } else if (strategy == "OptimalSpeed") {
    int intervals = args.find("--intervals") != args.end() ?
      std::stoi(args["--intervals"]) : static_cast<int>(sim.GetMarketData().size());
    double max_speed = 0.0;
    if (args.find("--max_speed") != args.end()) {
      try {
        max_speed = std::stod(args["--max_speed"]);
      } catch (const std::exception& e) {
        std::cerr << "Error: Invalid max_speed value: " << args["--max_speed"] << "\n";
        return 1;
      }
    }

    lvt::LimitOrderSpeedModel speed_model;
    speed_model.SetMarketData(sim.GetMarketData());
    speed_model.ComputeOptimalSpeedSchedule(total_volume, intervals, max_speed);
    const auto& schedule = speed_model.GetSchedule();

    *out_stream << "interval,trade_volume\n";
    for (size_t i = 0; i < schedule.size(); ++i) {
      *out_stream << i << "," << schedule[i] << "\n";
    }
  } else if (strategy == "AlmgrenKriss") {
    double eta = 1.0, gamma = 0.01, sigma = 0.5, lambda = 1.0;
    try {
      if (args.find("--eta") != args.end()) eta = std::stod(args["--eta"]);
      if (args.find("--gamma") != args.end()) gamma = std::stod(args["--gamma"]);
      if (args.find("--sigma") != args.end()) sigma = std::stod(args["--sigma"]);
      if (args.find("--lambda") != args.end()) lambda = std::stod(args["--lambda"]);
    } catch (const std::exception& e) {
      std::cerr << "Error: Invalid parameter value for AlmgrenKriss\n";
      return 1;
    }

    std::vector<double> prices;
    const auto& all_data = sim.GetMarketData();
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

    lvt::AlmgrenKrissModel ak;
    ak.SetMarketData(prices, total_volume);
    ak.SetParameters(eta, gamma, sigma, lambda);
    ak.ComputeOptimalSchedule();
    const auto& schedule = ak.GetSchedule();

    *out_stream << "interval,trade_volume\n";
    for (size_t i = 0; i < schedule.size(); ++i) {
      *out_stream << i << "," << schedule[i] << "\n";
    }
  } else {
    std::cerr << "Unknown strategy: " << strategy << "\n";
    PrintUsage(argv[0]);
    return 1;
  }

  if (has_output) {
    delete out_stream;
  }
  return 0;
}