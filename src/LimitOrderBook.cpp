#include "lob/LimitOrderBook.h"
#include "lob/Order.h"
#include <iostream>



/* Function that adds an order to the LOB*/
void LimitOrderBook::add_order(const Order& order) {
    /* Order of nothing doesn't make sense */
    if (order.quantity == 0) {
        assert(order.quantity > 0 && "Error: order should never have zero quantity");
        return;
    }

    /* Buy oder */
    /* O(1) time  */
    if (order.side == Side::Buy) {
        auto& price_level_list = bids_[order.price];
        price_level_list.emplace_back(order); // emplace faster/equal to push_back
        order_map_[order.id] = {--price_level_list.end(), Side::Buy}; // Iterator, and side 

    } else { // Side:: Sell
        auto& price_level_list = asks_[order.price];
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
    if (it == order_map_.end()) {
        assert(true && "Order not found, this shouldn't happen");
        return;
    }

    /* Save Order info */
    const auto& order_info = it -> second;
    Price price = order_info.iter -> price;

    /* Delete from bid / ask tree */
    if (order_info.side == Side::Buy) {
        auto& price_level_list = bids_.at(price); // Error if price_level_list value not found 
        price_level_list.erase(order_info.iter); // Delete order on doubly linked list 
        /* Delete price level list if empty */
        if (price_level_list.empty()) {
            bids_.erase(price); 
        }

        
    } else { // Sell side 
        auto& price_level_list = asks_.at(price); // throws error if price list can't be found 
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
    while (!bids_.empty() && !asks_.empty() && bids_.begin() -> first >= asks_.begin() -> first) {
        /* Get best bid and asks price list */
        auto& best_bid_list = bids_.begin() -> second;
        auto& best_ask_list = asks_.begin() -> second;

        /* Get first bid and asks in best bid/ask lists */
        Order& first_best_bid = best_bid_list.front();
        Order& first_best_ask = best_ask_list.front();


        /* Match the bid and asks, and save to trade_log */
        /* uunt64_t is very large, and can hold more than enough ids, so maker and aggressor order
         * id is compared using order id */


        Quantity fill_quantitiy = std::min(first_best_bid.quantity, first_best_ask.quantity);
        /* By design of the LOB, the smaller id is the maker order, and the larger id is the 
         * aggressor order */
        if (first_best_bid.id < first_best_ask.id) {
            create_trade(first_best_bid, first_best_ask, fill_quantitiy);
        } else { // ask is the maker order, and bid is the taker 
            create_trade(first_best_ask, first_best_bid, fill_quantitiy);
        }

        first_best_bid.quantity -= fill_quantitiy;
        first_best_ask.quantity -= fill_quantitiy;

        /* Remove filled up order; Note both bid and ask orders can be removed at the same time */
        if (first_best_bid.quantity == 0) {
            auto it_to_remove = best_bid_list.begin();
            remove_filled_order(best_bid_list, it_to_remove, bids_.begin() -> first, Side::Buy);

        }

        if (first_best_ask.quantity == 0) {
            auto it_to_remove = best_ask_list.begin();
            remove_filled_order(best_ask_list, it_to_remove, asks_.begin() -> first, Side::Sell);
        }
    }
}

/* Creates the trade and includes it into the log book */
void LimitOrderBook::create_trade(Order& maker_order, Order& taker_order, Quantity fill_quantitiy) {
    trades_.emplace_back(maker_order.id, taker_order.id, maker_order.price, fill_quantitiy);

    /* Display on standard output */
    // Price-time priority of trade is that the trade price is determined by the price of the maker / resting order 
    std::cout << "TRADE: " << fill_quantitiy << " @ " << maker_order.price 
        << " Maker: " << maker_order.id << " , Taker: " << taker_order.id << " )\n";
}

/* Removes orders that have already been filled */
void LimitOrderBook::remove_filled_order(OrderList& order_list, OrderList::iterator& it, 
                                         Price price, Side side) {
    /* Remove from order hashmap */
    order_map_.erase(it -> id);
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
