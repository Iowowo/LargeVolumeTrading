#include "gtest/gtest.h"
#include "order/order_manager.h"

namespace lvt {

TEST(OrderManagerTest, ExecutionRecordInitiallyEmpty) {
  OrderManager mgr;
  EXPECT_TRUE(mgr.GetExecutions().empty());
}

}  // namespace lvt
