#include "lob/LimitOrderBook.h"
#include "lob/Order.h"
#include <iomanip>
#include <iostream>
#include <numeric>
#include <utility>

/* Remember to comment out assert when testing speed */

/* Function that adds an order to the LOB*/
void LimitOrderBook::add_order(const Order &order) {
  /* Order of nothing doesn't make sense */
  if (order.quantity == 0) {
    std::cerr << "Order should never have 0 quantity" << std::endl;
    return;
  }

  /* Buy oder */
  /* O(1) time  */
  if (order.side == Side::Buy) {
    auto &price_level_list = bids_[order.price];
    price_level_list.emplace_back(order); // emplace faster/equal to push_back
    order_map_[order.id] = {--price_level_list.end(),
                            Side::Buy}; // Iterator, and side

  } else { // Side:: Sell
    auto &price_level_list = asks_[order.price];
    price_level_list.emplace_back(order);
    order_map_[order.id] = {--price_level_list.end(), Side::Sell};
  }

  /* Since new order came in, check for matching orders */
  match_orders();
}

/* Function that takes the order id as input and cancels the bid/ask order */
void LimitOrderBook::cancel_order(OrderId id) {
  /* O(1) */
  /* Find the id in hashtable */
  auto it = order_map_.find(id);
  /* Order is is not found */
  if (it == order_map_.end()) {
    missed_cancels_++; // count number of cancel order id's that can't be found
    return;
  }

  /* Save Order info */
  const auto &order_info = it->second;
  Price price = order_info.iter->price;

  /* Delete from bid / ask tree */
  if (order_info.side == Side::Buy) {
    auto &price_level_list =
        bids_.at(price); // Bottleneck here to search for price level
    price_level_list.erase(
        order_info.iter); // Delete order on doubly linked list
    /* Delete price level list if empty */
    if (price_level_list.empty()) {
      bids_.erase(price);
    }

  } else { // Sell side
    auto &price_level_list =
        asks_.at(price); // throws error if price list can't be found
    price_level_list.erase(order_info.iter);

    if (price_level_list.empty()) {
      asks_.erase(price);
    }
  }

  /* Delete order Ordermap */
  order_map_.erase(it);
}

/* Function that matches orders ont he bid and ask offers */
void LimitOrderBook::match_orders() {
  /* Bid and ask offers existing, and bid price >= sell price  */
  while (!bids_.empty() && !asks_.empty() &&
         bids_.begin()->first >= asks_.begin()->first) {
    /* Get best bid and asks price list */
    auto &best_bid_list = bids_.begin()->second;
    auto &best_ask_list = asks_.begin()->second;

    /* Get first bid and asks in best bid/ask lists */
    Order &first_best_bid = best_bid_list.front();
    Order &first_best_ask = best_ask_list.front();

    /* Match the bid and asks, and save to trade_log */
    /* uunt64_t is very large, and can hold more than enough ids, so maker and
     * aggressor order id is compared using order id */

    Quantity fill_quantitiy =
        std::min(first_best_bid.quantity, first_best_ask.quantity);
    /* By design of the LOB, the smaller id is the maker order, and the larger
     * id is the aggressor order */
    if (first_best_bid.id < first_best_ask.id) {
      create_trade(first_best_bid, first_best_ask, fill_quantitiy);
    } else { // ask is the maker order, and bid is the taker
      create_trade(first_best_ask, first_best_bid, fill_quantitiy);
    }

    first_best_bid.quantity -= fill_quantitiy;
    first_best_ask.quantity -= fill_quantitiy;

    /* Remove filled up order; Note both bid and ask orders can be removed at
     * the same time */
    if (first_best_bid.quantity == 0) {
      auto it_to_remove = best_bid_list.begin();
      remove_filled_order(best_bid_list, it_to_remove, bids_.begin()->first,
                          Side::Buy);
    }

    if (first_best_ask.quantity == 0) {
      auto it_to_remove = best_ask_list.begin();
      remove_filled_order(best_ask_list, it_to_remove, asks_.begin()->first,
                          Side::Sell);
    }
  }
}

/* Creates the trade and includes it into the log book */
void LimitOrderBook::create_trade(Order &maker_order, Order &taker_order,
                                  Quantity fill_quantitiy) {
  trades_.emplace_back(maker_order.id, taker_order.id, maker_order.price,
                       fill_quantitiy);

  /* Display on standard output */
  // Price-time priority of trade is that the trade price is determined by the
  // price of the maker / resting order

  /* Comment out: printing to stdout is slow */
  // std::cout << "TRADE: " << fill_quantitiy << " @ " << maker_order.price
  //     << " Maker: " << maker_order.id << " , Taker: " << taker_order.id << "
  //     )\n";
}

/* Removes orders that have already been filled */
void LimitOrderBook::remove_filled_order(OrderList &order_list,
                                         OrderList::iterator &it, Price price,
                                         Side side) {
  /* Remove from order hashmap */
  order_map_.erase(it->id);
  order_list.erase(it); // Remove specific order form order_list

  /* Remove order list if empty */
  if (order_list.empty()) {

    /* Either bid or ask list */
    if (side == Side::Buy) {
      bids_.erase(price);
    } else { // Sell side
      asks_.erase(price);
    }
  }
}

std::optional<std::pair<Price, Quantity>> LimitOrderBook::get_best_bid() const {
  if (bids_.empty()) {
    std::cout << "Bids currently empty" << std::endl;
    return std::nullopt; // when empty, use with optional library
  }

  const auto &best_price_level = bids_.begin()->second;
  Quantity total_quantity = 0;

  for (const auto &order : best_price_level) {
    total_quantity += order.quantity;
  }

  return std::make_pair(bids_.begin()->first, total_quantity);
}

/* Function that returns the current best bid price and the quantity */
std::optional<std::pair<Price, Quantity>> LimitOrderBook::get_best_ask() const {
  if (asks_.empty()) {
    std::cout << "Bids currently empty" << std::endl;
    return std::nullopt;
  }

  const auto &best_price_level = asks_.begin()->second;

  Quantity total_quantity = 0;
  for (const auto &order : best_price_level) {
    total_quantity += order.quantity;
  }

  return std::make_pair(asks_.begin()->first, total_quantity);
}

/* Function that prints the whole limit order book; includes bids and asks.
 * */
void LimitOrderBook::print_book(size_t depth) const {
  std::cout << "\n"
            << "========================================================="
            << std::endl
            << std::setw(12) << "BID" << std::setw(26) << "ASK" << std::endl
            << "---------------------------------------------------------"
            << std::endl
            << "Orders | Quantity | Price    || Price    | Quantity | Orders"
            << std::endl
            << "---------------------------------------------------------"
            << std::endl;

  /* Iterator to the nodes of bid and ask */
  auto bid_iter = bids_.begin();
  auto ask_iter = asks_.begin();

  /* Go through each price level depth times; 0 if end of price level  */
  for (size_t i = 0; i < depth; ++i) {
    /* Print BID side */
    if (bid_iter != bids_.end()) {
      const auto &[price, orders] = *bid_iter;
      Quantity total_quantity = std::accumulate(
          orders.begin(), orders.end(), 0u,
          [](Quantity sum, const Order &o) { return sum + o.quantity; });

      /* Formatting output. stdout relevant elements */
      std::cout << std::setw(6) << orders.size() << " | " << std::setw(8)
                << total_quantity << " | " << std::setw(8) << price;
      ++bid_iter; // Move to next bid node
    } else {
      // If out of bids, print empty space
      std::cout << std::setw(28) << "";
    }

    /* Print ASK SIDE */
    if (ask_iter != asks_.end()) {
      const auto &[price, orders] = *ask_iter;
      Quantity total_quantity = std::accumulate(
          orders.begin(), orders.end(), 0u,
          [](Quantity sum, const Order &o) { return sum + o.quantity; });

      /* Format and output relevant elements */
      std::cout << std::setw(8) << price << " | " << std::setw(8)
                << total_quantity << " | " << std::setw(6) << orders.size();
      ++ask_iter;
    } else {
      std::cout << std::setw(28) << "";
    }
    std::cout << std::endl;
  }
  std::cout << "========================================================="
            << std::endl;
}
