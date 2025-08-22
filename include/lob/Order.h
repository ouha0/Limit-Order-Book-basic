#pragma once // Prevents compilation error 



#include <memory>
#include <list>


/* Type definitions */
using Price = int;
using Quantity = unsigned int;
using OrderId = unsigned long long; // For very long id's 



enum class Side {Buy, Sell}; // For clarity 

/* Define order object */
struct Order {
    OrderId id;
    Price price;
    Side side; // Type of order
    Quantity quantity;

};


/* Type definitions; clarity */
using OrderPtr = std::shared_ptr<Order>;
using OrderList = std::list<OrderPtr>;
