#pragma once

#include "Order.h"
#include <chrono>


struct Trade {
    OrderId resting_order_id;
    OrderId aggressing_order_id;
    Price price;
    Quantity quantity;
    std::chrono::system_clock::time_point time_stamp; // C++ library that gets the current time of the system computer


    // constructor
    Trade(OrderId maker_id, OrderId taker_id, Price p, Quantity q) : resting_order_id(maker_id), 
        aggressing_order_id(taker_id), price(p), quantity(q), time_stamp(std::chrono::system_clock::now()){}
};



