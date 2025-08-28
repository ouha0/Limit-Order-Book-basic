#pragma once // Prevents compilation error 


#include <memory>
#include <cstdint>
#include <cassert>


/* Type definitions */
using Price = int32_t;
using Quantity = uint32_t;
using OrderId = uint64_t; // For very long id's 



enum class Side {Buy, Sell}; // For clarity 

/* Define order object */
struct Order {
    OrderId id;
    Price price;
    Quantity quantity;
    Side side; // Type of order

    // Constructor
    Order(OrderId order_id, Price p, Quantity q, Side s) :
        id(order_id), price(p), quantity(q), side(s) {}
};


