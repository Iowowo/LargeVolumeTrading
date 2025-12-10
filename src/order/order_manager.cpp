#include "order/order_manager.h"

namespace lvt {

OrderManager::OrderManager() : next_order_id_(1) {}

void OrderManager::IssueOrder(double quantity, double price, const std::string& timestamp) {
  ExecutionRecord record;
  record.order_id = next_order_id_++;
  record.quantity = quantity;
  record.price = price;
  record.timestamp = timestamp;
  records_.push_back(record);
}

const std::vector<ExecutionRecord>& OrderManager::GetExecutions() const {
  return records_;
}

}  // namespace lvt
