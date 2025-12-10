#ifndef LARGE_VOLUME_TRADING_ORDER_MANAGER_H_
#define LARGE_VOLUME_TRADING_ORDER_MANAGER_H_

#include <vector>
#include <string>

namespace lvt {

// Struct to record order executions.
struct ExecutionRecord {
  int order_id;
  double quantity;
  double price;
  std::string timestamp;
};

class OrderManager {
 public:
  OrderManager();
  void IssueOrder(double quantity, double price, const std::string& timestamp);
  const std::vector<ExecutionRecord>& GetExecutions() const;

 private:
  int next_order_id_;
  std::vector<ExecutionRecord> records_;
};

}  // namespace lvt

#endif  // LARGE_VOLUME_TRADING_ORDER_MANAGER_H_
