#include "gtest/gtest.h"
#include "market/market_simulator.h"

namespace lvt {

TEST(MarketSimulatorTest, LoadReturnsFalseIfFileNotFound) {
  MarketSimulator sim("nonexistent.csv");
  EXPECT_FALSE(sim.Load());
}

}  // namespace lvt
