#pragma once

#include "Order.h"
#include "Trade.h"

#include <functional>
#include <list>
#include <map>
// #include <memory>
// #include <iostream>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

using OrderList = std::list<Order>;

class LimitOrderBook {
public:
  void add_order(OrderId id, Price price, Quantity quantity, Side side);
  void cancel_order(OrderId id);
  void print_trades() const;

  /* Debugging */
  void print_book(size_t depth) const;
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
  using BidBook = std::map<Price, OrderList, std::greater<Price>>;
  using AskBook = std::map<Price, OrderList>;

  using BidBookIterator = BidBook::iterator;
  using AskBookIterator = AskBook::iterator;

  /* Due to the nature of cancel order data generation , we may receive invalid
   * cancel orders */
  uint64_t missed_cancels_ = 0;

  /* Self balancing tree ordered by price, then time; define the ask and bid
   * members */
  BidBook bids_;
  AskBook asks_;
  std::vector<Trade> trades_;

  struct OrderInfo {
    OrderList::iterator order_iter;
    // Holds iterator to the map node
    std::variant<BidBookIterator, AskBookIterator> book_iter;

    template <typename BookIterator>
    OrderInfo(OrderList::iterator l_iter, BookIterator b_iter)
        : order_iter(l_iter), book_iter(b_iter) {}
  };

  std::unordered_map<OrderId, OrderInfo> order_map_;

  /* New order comes in, run this to see if any trades can be matched */
  void match_orders();

  void create_trade(Order &maker_order, Order &taker_order,
                    Quantity fill_quantity);

  // The bug was a C++ template issue, and it didn't let me use function
  // overloading...........
  void remove_filled_bid_order(BidBookIterator book_it,
                               OrderList::iterator list_it);
  void remove_filled_ask_order(AskBookIterator book_it,
                               OrderList::iterator list_it);
  // Function overloading; doesn't work at the moment
  void remove_price_level(BidBook::iterator bid_it);
  void remove_price_level(AskBook::iterator ask_it);
};
