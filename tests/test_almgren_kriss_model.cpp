#include "gtest/gtest.h"
#include "strategy/almgren_kriss_model.h"
#include <numeric>
#include <cmath>

namespace lvt {

// 1. Edge: No price data should yield empty schedule.
TEST(AlmgrenKrissModelTest, EmptyPriceVector) {
  AlmgrenKrissModel model;
  model.SetMarketData({}, 100);
  model.SetParameters(1, 1, 1, 1);
  model.ComputeOptimalSchedule();
  EXPECT_TRUE(model.GetSchedule().empty());
}

// 2. Edge: Zero total volume should yield empty schedule.
TEST(AlmgrenKrissModelTest, ZeroTotalVolume) {
  AlmgrenKrissModel model;
  model.SetMarketData({1, 2}, 0.0);
  model.SetParameters(1, 1, 1, 1);
  model.ComputeOptimalSchedule();
  EXPECT_TRUE(model.GetSchedule().empty());
}

// 3. One interval: all volume should go in first interval.
TEST(AlmgrenKrissModelTest, SingleInterval) {
  AlmgrenKrissModel model;
  model.SetMarketData({100}, 37);
  model.SetParameters(1, 1, 1, 1);
  model.ComputeOptimalSchedule();
  auto sched = model.GetSchedule();
  ASSERT_EQ(sched.size(), 1);
  EXPECT_DOUBLE_EQ(sched[0], 37.0);
}

// 4. Uniform with zero risk aversion (lambda = 0): even allocation.
TEST(AlmgrenKrissModelTest, UniformWhenZeroLambda) {
  AlmgrenKrissModel model;
  model.SetMarketData({1,1,1}, 30);
  model.SetParameters(1, 1, 1, 0);
  model.ComputeOptimalSchedule();
  const auto& sched = model.GetSchedule();
  ASSERT_EQ(sched.size(), 3);
  for (auto x : sched) EXPECT_DOUBLE_EQ(x, 10.0);
}

// 5. Uniform with zero eta (no temporary impact): uniform allocation.
TEST(AlmgrenKrissModelTest, UniformWhenZeroEta) {
  AlmgrenKrissModel model;
  model.SetMarketData({5, 6, 7, 8}, 20);
  model.SetParameters(0, 1, 1, 1);
  model.ComputeOptimalSchedule();
  const auto& sched = model.GetSchedule();
  ASSERT_EQ(sched.size(), 4);
  for (auto x : sched) EXPECT_DOUBLE_EQ(x, 5.0);
}

// 6. Risk-averse (large lambda): U-shaped schedule, central value lowest.
TEST(AlmgrenKrissModelTest, UScheduleShapeLargeLambda) {
  AlmgrenKrissModel model;
  model.SetMarketData({1,1,1,1,1}, 100);
  model.SetParameters(1, 1, 1, 1000);
  model.ComputeOptimalSchedule();
  const auto& sched = model.GetSchedule();
  ASSERT_EQ(sched.size(), 5);
  // Central value less than outer ones
  EXPECT_LT(sched[2], sched[0]);
  EXPECT_LT(sched[2], sched[4]);
}

// 7. Risk-neutral (lambda -> 0): nearly flat schedule.
TEST(AlmgrenKrissModelTest, FlatWhenRiskNeutral) {
  AlmgrenKrissModel model;
  model.SetMarketData({1,1,1,1}, 40);
  model.SetParameters(1, 1, 1, 1e-10);
  model.ComputeOptimalSchedule();
  const auto& sched = model.GetSchedule();
  for (auto x : sched) EXPECT_NEAR(x, 10.0, 1e-8);
}

// 8. Schedule sum matches total volume.
TEST(AlmgrenKrissModelTest, ScheduleSumsToTotal) {
  AlmgrenKrissModel model;
  std::vector<double> prices(10, 1.0);
  model.SetMarketData(prices, 77.5);
  model.SetParameters(1, 1, 1, 0.5);
  model.ComputeOptimalSchedule();
  double sum = std::accumulate(model.GetSchedule().begin(), model.GetSchedule().end(), 0.0);
  EXPECT_NEAR(sum, 77.5, 1e-8);
}

// 9. Symmetry: schedule is symmetric for symmetric parameters.
TEST(AlmgrenKrissModelTest, SymmetricSolution) {
  AlmgrenKrissModel model;
  model.SetMarketData({1,1,1,1,1,1,1}, 70);
  model.SetParameters(1, 2, 3, 5);
  model.ComputeOptimalSchedule();
  const auto& sched = model.GetSchedule();
  ASSERT_EQ(sched.size(), 7);
  EXPECT_NEAR(sched[0], sched[6], 1e-8);
  EXPECT_NEAR(sched[1], sched[5], 1e-8);
  EXPECT_NEAR(sched[2], sched[4], 1e-8);
}

// 10. Schedule always non-negative and no NaN.
TEST(AlmgrenKrissModelTest, NonNegativeAndFinite) {
  AlmgrenKrissModel model;
  model.SetMarketData({1,2,3,4,5}, 50);
  model.SetParameters(7, 3, 2, 1);
  model.ComputeOptimalSchedule();
  for (auto x : model.GetSchedule()) {
    EXPECT_GE(x, 0.0);
    EXPECT_TRUE(std::isfinite(x));
  }
}

}  // namespace lvt
