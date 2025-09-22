#include "lob/LimitOrderBook.h"
#include "lob/Order.h"
#include <iomanip>
#include <iostream>
#include <numeric>
#include <utility>

/* Remember to comment out assert when testing speed */

/* Function that adds an order to the LOB */
void LimitOrderBook::add_order(OrderId id, Price price, Quantity quantity,
                               Side side) {

  // Invalid quantity, end the function
  if (quantity == 0) {
    return;
  }
  // Bug check
  assert(price > 0 && quantity > 0 &&
         "Order price and quantity must be greater than zero!");
  // End bug check

  /* Order of nothing doesn't make sense */
  if (side == Side::Buy) {
    // Find / Create the price level. Output is a pair: iterator, bool
    std::pair<BidBookIterator, bool> res =
        bids_.try_emplace(price, OrderList{});
    // Add new order to end of price level list
    BidBookIterator map_iter = res.first;

    map_iter->second.emplace_back(id, price, quantity, side);
    auto list_iter =
        --map_iter->second.end(); // Iterator of most recent order we just added

    // Explicitly define book variant type for compiler
    std::variant<BidBookIterator, AskBookIterator> book_variant{
        std::in_place_index<0>, map_iter};

    // Save the iterators into the order hashmap
    OrderInfo info_to_insert{list_iter, book_variant};

    order_map_.emplace(id, info_to_insert);

  } else {
    std::pair<AskBookIterator, bool> res =
        asks_.try_emplace(price, OrderList{});

    AskBookIterator map_iter = res.first;
    map_iter->second.emplace_back(id, price, quantity, side);
    auto list_iter = --map_iter->second.end();

    std::variant<BidBookIterator, AskBookIterator> book_variant{
        std::in_place_index<1>, map_iter};

    OrderInfo info_to_insert{list_iter, book_variant};

    order_map_.emplace(id, info_to_insert);
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
  Price price = order_info.order_iter->price;

  // Retrieve iterators from ordermap, update lob (delete order and node if
  // necessary) Lamda function (by reference) to retrieve ordermap and update
  // lob; using the book_iter object
  std::visit(
      [&](auto book_iter) {
        /* Get iterator of price_level_list; erase relevant node using
         * order_info iterator*/
        auto &price_level_list = book_iter->second;
        price_level_list.erase(order_info.order_iter);

        /* If price level list is empty, delete the whole node (Deleting it will
         * require O(logn) rebalancing; should be worthwhile to delete */
        if (price_level_list.empty()) {
          // Get type definition of book_iter
          using BookIteratorType = std::decay_t<decltype(book_iter)>;

          // Used so compiler understands book iterator type. Only compile part
          // of the "if else"
          if constexpr (std::is_same_v<BookIteratorType, BidBookIterator>) {
            bids_.erase(book_iter);
          } else {
            asks_.erase(book_iter);
          }
        }
      },
      order_info.book_iter); // book_iter is the object we want to visit

  /* Delete order Ordermap */
  order_map_.erase(it);
}

// Deal with the case when a bid data structure is emtpy, but ask is still
// alive; deletion ends early here. Try to always use begin() to avoid dangling
// iterators
/* Function that matches orders ont he bid and ask offers */
void LimitOrderBook::match_orders() {
  /* Bid and ask offers existing, and bid price >= sell price  */
  while (!bids_.empty() && !asks_.empty() &&
         bids_.begin()->first >= asks_.begin()->first) {

    /* Bug checking */
    std::cout << "Before orders are being matched..." << std::endl;
    print_book(5);

    /* Retrieve best price node; save order list and first element of order
     * list
     */
    auto best_bid_iter = bids_.begin();
    auto &best_bid_list = best_bid_iter->second;

    auto best_ask_iter = asks_.begin();
    auto &best_ask_list = best_ask_iter->second;

    assert(!best_bid_list.empty() && !best_ask_list.empty() &&
           "bid and ask list shouldn't be empty");

    Order &best_bid = best_bid_list.front();
    Order &best_ask = best_ask_list.front();

    Quantity fill_quantity = std::min(best_bid.quantity, best_ask.quantity);

    std::cout << "Check iterator validity before trade created..." << std::endl;
    print_book(5);

    /* Check whether bid and ask where resting / aggressor; save into log*/
    if (best_bid.id < best_ask.id) {
      std::cout << "Creating trade..." << std::endl;
      create_trade(best_bid, best_ask, fill_quantity);
    } else {
      std::cout << "Creating trade..." << std::endl;
      create_trade(best_ask, best_bid, fill_quantity);
    }

    std::cout << "Check iterator validity after trade created..." << std::endl;
    print_book(5);

    /* Bug testing */
    std::cout << "Trade should have been created" << std::endl;
    print_trades();
    /* End of bug test */

    /* Update order quantity, depending on fill amount */
    best_bid.quantity -= fill_quantity;
    best_ask.quantity -= fill_quantity;

    bool bid_filled = (best_bid.quantity == 0);
    bool ask_filled = (best_ask.quantity == 0);

    if (ask_filled) {

      std::cout << "Ask price level is filled, starting to delete it "
                << std::endl;
      remove_filled_ask_order(best_ask_iter, best_ask_list.begin());
      std::cout << "Ask Price level should be deleted" << std::endl;
      print_book(2);
    }

    // using continue doesn't fix the bug... Now theres a seg fault...
    if (bid_filled) {
      remove_filled_bid_order(best_bid_iter, best_bid_list.begin());

      std::cout << "Bid Price level should be deleted" << std::endl;
      print_book(2);
    }
  }

  /* Because we use continue; need to cater for case when one of bid/ask is
   * empty and other bid/ask has quantity 0 order */
  // while (!bids_.empty() && bids_.begin()->second.front().quantity == 0) {
  //   remove_filled_order(bids_.begin(), bids_.begin()->second.begin());
  // }

  // while (!asks_.empty() && asks_.begin()->second.front().quantity == 0) {
  //   remove_filled_order(asks_.begin(), asks_.begin()->second.begin());
  // }
}

// Implementation for Bids
void LimitOrderBook::remove_filled_bid_order(BidBookIterator book_it,
                                             OrderList::iterator list_it) {
  order_map_.erase(list_it->id);
  book_it->second.erase(list_it);
  if (book_it->second.empty()) {
    bids_.erase(book_it);
  }
}

// Implementation for Asks (Notice the duplicated code!)
void LimitOrderBook::remove_filled_ask_order(AskBookIterator book_it,
                                             OrderList::iterator list_it) {
  order_map_.erase(list_it->id);
  book_it->second.erase(list_it);
  if (book_it->second.empty()) {
    asks_.erase(book_it);
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
// void LimitOrderBook::remove_filled_order(OrderList &order_list,
//                                          OrderList::iterator &it, Price
//                                          price, Side side) {
//   /* Remove from order hashmap */
//   order_map_.erase(it->id);
//   order_list.erase(it); // Remove specific order form order_list
//
//   /* Remove order list if empty */
//   if (order_list.empty()) {
//
//     /* Either bid or ask list */
//     if (side == Side::Buy) {
//       bids_.erase(price);
//     } else { // Sell side
//       asks_.erase(price);
//     }
//   }
// }

/* Function that returns the current best bid price and the quantity */
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

/* Function that prints the tradelog */
void LimitOrderBook::print_trades() const {
  std::cout << "\n --- Trade Log --- " << std::endl;

  /* If no trades */
  if (trades_.empty()) {
    std::cout << "No Trades have occured yet" << std::endl;
  }

  /* Print out all trades */
  for (const auto &trade : trades_) {
    std::cout << "-Filled " << trade.quantity << " @ " << trade.price
              << " Maker: " << trade.resting_order_id
              << " Taker: " << trade.aggressing_order_id << ")" << std::endl;
  }

  std::cout << "-----------" << std::endl;
}

/* Function that prints the whole limit order book; includes bids and asks.
 * Used for debugging as well as display
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
