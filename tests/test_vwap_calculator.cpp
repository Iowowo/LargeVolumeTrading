#include "gtest/gtest.h"
#include "strategy/vwap_calculator.h"
#include "market/market_simulator.h"

namespace lvt {

// Test 1: Empty market data -> schedule should be empty.
// Purpose: Ensure calculator returns nothing if data is absent.
TEST(VWAPCalculatorTest, EmptyMarketData) {
  VWAPCalculator calc;
  calc.SetMarketData({});
  calc.ComputeVWAPSchedule(100);
  EXPECT_TRUE(calc.GetSchedule().empty());
}

// Test 2: Total volume is zero -> schedule should be empty.
// Purpose: No order to execute means nothing to schedule.
TEST(VWAPCalculatorTest, ZeroTotalVolume) {
  VWAPCalculator calc;
  calc.SetMarketData({{"t0", 100, 10}});
  calc.ComputeVWAPSchedule(0.0);
  EXPECT_TRUE(calc.GetSchedule().empty());
}

// Test 3: All market volumes zero -> schedule should be empty.
// Purpose: No liquidity means no proportional allocation.
TEST(VWAPCalculatorTest, AllMarketVolumesZero) {
  VWAPCalculator calc;
  std::vector<MarketData> d = {
      {"t0", 100, 0},{"t1", 200, 0}};
  calc.SetMarketData(d);
  calc.ComputeVWAPSchedule(50);
  EXPECT_TRUE(calc.GetSchedule().empty());
}

// Test 4: Single interval, nonzero volume.
// Purpose: All volume should be assigned to that interval.
TEST(VWAPCalculatorTest, SingleIntervalAllVolume) {
  VWAPCalculator calc;
  std::vector<MarketData> d = {{"t0", 99.9, 25}};
  calc.SetMarketData(d);
  calc.ComputeVWAPSchedule(100);
  const auto& sched = calc.GetSchedule();
  ASSERT_EQ(sched.size(), 1);
  EXPECT_DOUBLE_EQ(sched[0], 100);
}

// Test 5: Even intervals and volumes.
// Purpose: Volume split evenly across intervals.
TEST(VWAPCalculatorTest, EvenDistribution) {
  VWAPCalculator calc;
  std::vector<MarketData> d = {
      {"t0", 10, 10},
      {"t1", 15, 10},
      {"t2", 20, 10},
      {"t3", 25, 10}};
  calc.SetMarketData(d);
  calc.ComputeVWAPSchedule(40);
  auto sched = calc.GetSchedule();
  ASSERT_EQ(sched.size(), 4);
  for (auto x : sched) EXPECT_DOUBLE_EQ(x, 10.0);
}

// Test 6: Increasing interval volume.
// Purpose: Each successive interval gets more of the total.
TEST(VWAPCalculatorTest, IncreasingVolume) {
  VWAPCalculator calc;
  std::vector<MarketData> d = {
      {"t0", 11, 10},
      {"t1", 12, 20},
      {"t2", 13, 30}}
  ;
  calc.SetMarketData(d);
  calc.ComputeVWAPSchedule(60);
  auto sched = calc.GetSchedule();
  ASSERT_EQ(sched.size(), 3);
  EXPECT_DOUBLE_EQ(sched[0], 10.0);
  EXPECT_DOUBLE_EQ(sched[1], 20.0);
  EXPECT_DOUBLE_EQ(sched[2], 30.0);
}

// Test 7: One interval has all the volume.
// Purpose: Total volume assigned to the highest-volume interval.
TEST(VWAPCalculatorTest, OneIntervalDominant) {
  VWAPCalculator calc;
  std::vector<MarketData> d = {
      {"t0", 11, 0},
      {"t1", 11, 0},
      {"t2", 11, 100}};
  calc.SetMarketData(d);
  calc.ComputeVWAPSchedule(100);
  auto sched = calc.GetSchedule();
  ASSERT_EQ(sched.size(), 3);
  EXPECT_DOUBLE_EQ(sched[0], 0.0);
  EXPECT_DOUBLE_EQ(sched[1], 0.0);
  EXPECT_DOUBLE_EQ(sched[2], 100.0);
}

// Test 8: Negative total volume is meaningless: schedule should be empty.
// Purpose: Ensure scheduler rejects negative total target volume.
TEST(VWAPCalculatorTest, NegativeTotalVolume) {
  VWAPCalculator calc;
  std::vector<MarketData> d = {
    {"t0", 100, 10},
    {"t1", 100, 10}};
  calc.SetMarketData(d);
  calc.ComputeVWAPSchedule(-50.0);
  EXPECT_TRUE(calc.GetSchedule().empty());
}

// Test 9: Realistic pattern, last period largest (matches sample_market.csv).
// Purpose: Checks known cases from sample data for regression.
TEST(VWAPCalculatorTest, MatchesSampleMarket) {
  std::vector<MarketData> market_data = {
    {"2025-01-01T09:30:00", 100.0, 10},
    {"2025-01-01T09:31:00", 100.0, 10},
    {"2025-01-01T09:32:00", 100.0, 20},
    {"2025-01-01T09:33:00", 100.0, 20},
    {"2025-01-01T09:34:00", 100.0, 40},
  };
  VWAPCalculator calc;
  calc.SetMarketData(market_data);
  calc.ComputeVWAPSchedule(100.0);
  const auto& sched = calc.GetSchedule();
  ASSERT_EQ(sched.size(), 5);
  EXPECT_DOUBLE_EQ(sched[0], 10.0);
  EXPECT_DOUBLE_EQ(sched[1], 10.0);
  EXPECT_DOUBLE_EQ(sched[2], 20.0);
  EXPECT_DOUBLE_EQ(sched[3], 20.0);
  EXPECT_DOUBLE_EQ(sched[4], 40.0);
}

// Test 10: Unmatched vector sizes between market data and schedule.
// Purpose: Defensive: schedule always matches market intervals.
TEST(VWAPCalculatorTest, ScheduleSizeMatchesMarketData) {
  VWAPCalculator calc;
  for (int sz = 0; sz < 5; ++sz) {
    std::vector<MarketData> d(sz, {"t", 10, 10});
    calc.SetMarketData(d);
    calc.ComputeVWAPSchedule(100.0);
    EXPECT_EQ(calc.GetSchedule().size(), d.size());
  }
}

// Test 11: VWAP is less than max price for increasing price series.
// Purpose: If all prices are rising, the VWAP should be below the last (max) price,
// since early trades are at cheaper prices.
TEST(VWAPCalculatorTest, VWAPLowerThanMaxWhenIncreasing) {
  std::vector<MarketData> d = {
    {"t0", 10, 10},
    {"t1", 20, 10},
    {"t2", 30, 10}
  };
  VWAPCalculator calc;
  calc.SetMarketData(d);
  calc.ComputeVWAPSchedule(30);
  auto sched = calc.GetSchedule();
  // Realized VWAP = sum(price * amount) / total volume
  double vwap = 0;
  for (size_t i = 0; i < sched.size(); ++i) {
    vwap += d[i].price * sched[i];
  }
  vwap /= 30.0;
  double max_price = 30.0;
  EXPECT_LT(vwap, max_price);
}

// Test 12: VWAP is greater than min price for decreasing price series.
// Purpose: If all prices are falling, the VWAP will be above the last (min) price,
// since more trades occurred at the higher early prices.
TEST(VWAPCalculatorTest, VWAPHigherThanMinWhenDecreasing) {
  std::vector<MarketData> d = {
    {"t0", 30, 10},
    {"t1", 20, 10},
    {"t2", 10, 10}
  };
  VWAPCalculator calc;
  calc.SetMarketData(d);
  calc.ComputeVWAPSchedule(30);
  auto sched = calc.GetSchedule();
  double vwap = 0;
  for (size_t i = 0; i < sched.size(); ++i) {
    vwap += d[i].price * sched[i];
  }
  vwap /= 30.0;
  double min_price = 10.0;
  EXPECT_GT(vwap, min_price);
}

}  // namespace lvt
