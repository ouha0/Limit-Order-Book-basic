#pragma once

#include "Order.h"
#include "Trade.h"

#include <map>
#include <memory>
#include <list>
#include <unordered_map>
#include <functional>
#include <vector>


class LimitOrderBook {
    public:
        void add_order(const Order& order);
        void cancel_order(OrderId id);

        void print_book() const; // good for debugging 
        const std::vector<Trade>& get_trades() const {return trades_; }; // Constant reference to get trade log, without changing anything (display)


    private:
        
        using OrderList = std::list<Order>;

        /* Self balancing tree ordered by price, then time */
        std::map<Price, OrderList, std::greater<Price>> bids_; // Key value ordered pairs holding lists of orders (bid)
        std::map<Price, OrderList> asks_; // Ask 
        std::vector<Trade> trades_;

        struct OrderInfo{
            OrderList::iterator iter; // Address of order to be deleted inside the relevant OrderList 
            Side side;

        }; 

        std::unordered_map<OrderId, OrderInfo> order_map_;

        /* New order comes in, run this to see if any trades can be matched */
        void match_orders();

        void create_trade(Order& maker_order, Order& taker_order, Quantity fill_quantity);
        
    /* Remove fully filled order from the book and map */
        void remove_filled_order(OrderList& order_list, OrderList::iterator& it, Price price, Side side);

};



