#include "gtest/gtest.h"
#include "strategy/limit_order_speed_model.h"
#include "market/market_simulator.h"

namespace lvt {

// Test 1: Empty market data, zero volume.
// Purpose: With nothing to trade, no schedule should be produced.
TEST(LimitOrderSpeedModelTest, EmptyDataZeroVolume) {
  LimitOrderSpeedModel model;
  model.SetMarketData({});
  model.ComputeOptimalSpeedSchedule(0.0, 3);
  EXPECT_TRUE(model.GetSchedule().empty());
}

// Test 2: Negative total volume is meaningless.
// Expectation: Should return empty schedule.
TEST(LimitOrderSpeedModelTest, NegativeVolume) {
  LimitOrderSpeedModel model;
  model.SetMarketData({{"t0", 100, 1}});
  model.ComputeOptimalSpeedSchedule(-100, 3);
  EXPECT_TRUE(model.GetSchedule().empty());
}

// Test 3: Number of intervals zero, use market data size.
// Expectation: 2 intervals (from data), split evenly.
TEST(LimitOrderSpeedModelTest, ZeroIntervalUsesMarketDataSize) {
  std::vector<MarketData> d = {{"t1", 1, 1},{"t2",2,2}};
  LimitOrderSpeedModel model;
  model.SetMarketData(d);
  model.ComputeOptimalSpeedSchedule(10, 0);
  const auto& sched = model.GetSchedule();
  ASSERT_EQ(sched.size(), 2);
  EXPECT_DOUBLE_EQ(sched[0], 5.0);
  EXPECT_DOUBLE_EQ(sched[1], 5.0);
}

// Test 4: Trivial one-interval split.
// Expectation: All volume in one step.
TEST(LimitOrderSpeedModelTest, SingleInterval) {
  LimitOrderSpeedModel model;
  model.SetMarketData({{"t1",0,10}});
  model.ComputeOptimalSpeedSchedule(33, 1);
  const auto& sched = model.GetSchedule();
  ASSERT_EQ(sched.size(), 1);
  EXPECT_DOUBLE_EQ(sched[0], 33.0);
}

// Test 5: 4-interval even split.
// Expectation: Volume split evenly.
TEST(LimitOrderSpeedModelTest, FourIntervalEvenSplit) {
  LimitOrderSpeedModel model;
  std::vector<MarketData> d = {{"t1",1,1},{"t2",2,2},{"t3",3,3},{"t4",4,4}};
  model.SetMarketData(d);
  model.ComputeOptimalSpeedSchedule(40, 4);
  auto sched = model.GetSchedule();
  for (auto s : sched) EXPECT_DOUBLE_EQ(s, 10.0);
}

// Test 6: max_speed constrains per-interval volume.
// Expectation: All chunks hit the maximum speed except remainder.
TEST(LimitOrderSpeedModelTest, MaxSpeedConstrains) {
  LimitOrderSpeedModel model;
  std::vector<MarketData> d = {{"t0",0,0},{"t1",0,0},{"t2",0,0}};
  model.SetMarketData(d);
  model.ComputeOptimalSpeedSchedule(50, 3, 15);
  const auto& sched = model.GetSchedule();
  ASSERT_EQ(sched.size(), 3);
  EXPECT_DOUBLE_EQ(sched[0], 15.0);
  EXPECT_DOUBLE_EQ(sched[1], 15.0);
  EXPECT_DOUBLE_EQ(sched[2], 20.0); // Remainder
}

// Test 7: Remainder logic, doesn't exceed max speed by more than minimally necessary.
// Purpose: If the leftover is less than max, it's fine to add.
TEST(LimitOrderSpeedModelTest, RemainderHandledOnLastInterval) {
  LimitOrderSpeedModel model;
  std::vector<MarketData> d = {{"t1",1,1},{"t2",2,2},{"t3",3,3}};
  model.SetMarketData(d);
  model.ComputeOptimalSpeedSchedule(32, 3, 10.0);
  auto sched = model.GetSchedule();
  EXPECT_DOUBLE_EQ(sched[0], 10.0);
  EXPECT_DOUBLE_EQ(sched[1], 10.0);
  EXPECT_DOUBLE_EQ(sched[2], 12.0);
}

// Test 8: Schedule matches input intervals if max_speed = 0 (unconstrained)
// Purpose: Splits evenly without limit.
TEST(LimitOrderSpeedModelTest, NoMaxSpeedEvenSplit) {
  LimitOrderSpeedModel model;
  std::vector<MarketData> d(5, {"t", 1, 1});
  model.SetMarketData(d);
  model.ComputeOptimalSpeedSchedule(25, 5, 0.0);
  auto sched = model.GetSchedule();
  ASSERT_EQ(sched.size(), 5);
  for (auto s : sched) EXPECT_DOUBLE_EQ(s, 5.0);
}

// Test 9: Schedule always adds up to total volume.
// Purpose: Numerical, doesn't lose volume.
TEST(LimitOrderSpeedModelTest, TotalVolumeMatchesSum) {
  LimitOrderSpeedModel model;
  std::vector<MarketData> d(7, {"t", 1, 1});
  model.SetMarketData(d);
  double total_vol = 13;
  model.ComputeOptimalSpeedSchedule(total_vol, 7, 2);
  double sum = 0.0;
  for (auto x : model.GetSchedule()) sum += x;
  EXPECT_DOUBLE_EQ(sum, total_vol);
}

// Test 10: Large number of intervals, small max_speed.
// Purpose: All but final interval at cap, large last chunk for remainder.
TEST(LimitOrderSpeedModelTest, LotOfIntervalsSmallCap) {
  LimitOrderSpeedModel model;
  std::vector<MarketData> d(10, {"t", 1, 1});
  model.SetMarketData(d);
  model.ComputeOptimalSpeedSchedule(53, 10, 1);
  auto sched = model.GetSchedule();
  for (size_t i = 0; i < sched.size()-1; ++i) EXPECT_DOUBLE_EQ(sched[i], 1.0);
  EXPECT_DOUBLE_EQ(sched.back(), 53-9.0);
}

}  // namespace lvt
