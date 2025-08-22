#pragma once

#include "Order.h"
#include <chrono>

struct Trade {
    OrderId aggressing_order_id;
    OrderId resting_order_id;
    Price price;
    Quantity quantity;
    std::chrono::system_clock::time_point time_stamp; // C++ library that gets the current time of the system computer
};



