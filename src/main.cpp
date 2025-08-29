#include "lob/LimitOrderBook.h"
#include "lob/Parser.h"
#include "lob/Command.h"
#include <iostream>
#include <vector>
#include <chrono>


/* Function that prints the tradelog */
void print_trades(const LimitOrderBook& lob) {
    std:: cout << "\n --- Trade Log --- " << std::endl;

    /* If no trades */
    if (lob.get_trades().empty()) {
        std::cout << "No Trades have occured yet" << std::endl;
    }

    /* Print out all trades */
    for (const auto& trade: lob.get_trades()) {
        std::cout << "-Filled " << trade.quantity << " @ " << trade.price 
            << " Maker: " << trade.resting_order_id << " Taker: " << trade.aggressing_order_id 
            << ")" << std::endl;
    }

    std::cout << "-----------" << std::endl;

}


/*  Main function that takes in trades */
int main () {

    LimitOrderBook lob;
    OrderId order_id_counter = 1;



    return 0;
}
