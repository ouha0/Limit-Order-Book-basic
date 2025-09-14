#pragma once

#include "Order.h"
#include "Trade.h"

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

/* Order objects stored within list rather than using Order pointers. This is to
 * prevent pointer chasing (bad cache performance?) */
using OrderList = std::list<Order>;

class LimitOrderBook {
public:
  void add_order(const Order &order);
  void cancel_order(OrderId id);

  /* Debugging */
  void print_book() const;
  // const std::map<Price, OrderList> &get_asks() const { return asks_; };
  // const std::map<Price, OrderList, std::greater<Price>> &get_bids() const {
  //   return bids_;
  // };
  std::optional<std::pair<Price, Quantity>> get_best_bid() const;
  std::optional<std::pair<Price, Quantity>> get_best_ask() const;

  const std::vector<Trade> &get_trades() const {
    return trades_;
  }; // Constant reference to get trade log, without changing anything (display)
  uint64_t get_missed_cancel_count() const { return missed_cancels_; }

private:
  /* Due to the nature of cancel order data generation , we may receive invalid
   * cancel orders */
  uint64_t missed_cancels_ = 0;

  /* Self balancing tree ordered by price, then time */
  std::map<Price, OrderList, std::greater<Price>>
      bids_; // Key value ordered pairs holding lists of orders (bid)
  std::map<Price, OrderList> asks_; // Ask
  std::vector<Trade> trades_;

  struct OrderInfo {
    OrderList::iterator iter;
    Side side;
  };

  std::unordered_map<OrderId, OrderInfo> order_map_;

  /* New order comes in, run this to see if any trades can be matched */
  void match_orders();

  void create_trade(Order &maker_order, Order &taker_order,
                    Quantity fill_quantity);

  /* Remove fully filled order from the book and map */
  void remove_filled_order(OrderList &order_list, OrderList::iterator &it,
                           Price price, Side side);
};
