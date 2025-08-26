#include "lob/LimitOrderBook.h"
#include <iostream>



/* Function that adds an order to the LOB*/
void LimitOrderBook::add_order(const Order& order) {
    if (order.quantity == 0) {
        return;
    }

    if (order.side == Side::Buy) {

    } else { // Side:: Sell

    }

    match_orders();
}
