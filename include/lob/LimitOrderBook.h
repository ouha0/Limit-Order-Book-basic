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
        void add_order(OrderId id, Side side, Price price, Quantity quantity);
        void cancel_order(OrderId id);
        void print_book();
        const std::vector<Trade>& get_trade_log() const; // Constant reference to get trade log, without changing anything (display)


    private:
        
        std::map<Price, OrderList, std::greater<Price>> bids_; // Key value ordered pairs holding lists of orders (bid )
        std::map<Price, OrderList> asks_; // Ask 
        std::vector<Trade> trade_log_;

        struct OrderInfo{
            OrderPtr order_ptr;
            OrderList::iterator list_iterator; // Address of order to be deleted inside the relevant OrderList 
            Side side;

        }; 

        /* New order comes in, run this to see if any trades can be matched */
        void match_trade();

};



