#include "lob/LimitOrderBook.h"
#include <iostream>


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

    std::cout << "--- Scenario 1: Seeding the book with resting orders ---" << std::endl;
    lob.add_order({order_id_counter++, 100, 10, Side::Buy});  // ID 1
    lob.add_order({order_id_counter++, 100, 5, Side::Buy});   // ID 2 (Time priority behind ID 1)
    lob.add_order({order_id_counter++, 99, 20, Side::Buy});   // ID 3
    lob.add_order({order_id_counter++, 101, 10, Side::Sell}); // ID 4
    lob.add_order({order_id_counter++, 102, 15, Side::Sell}); // ID 5
    lob.add_order({order_id_counter++, 101, 5, Side::Sell});  // ID 6 (Time priority behind ID 4)
    std::cout << "Book is seeded. No trades should have occurred." << std::endl;
    print_trades(lob);

    std::cout << "\n--- Scenario 2: Aggressive buy order crosses the spread ---" << std::endl;
    std::cout << "Adding a BUY order for 20 shares @ 101." << std::endl;
    std::cout << "Expected: Should fill 10 shares from Order 4 and 5 shares from Order 6." << std::endl;
    lob.add_order({order_id_counter++, 101, 20, Side::Buy}); // ID 7
    print_trades(lob);

    std::cout << "\n--- Scenario 3: Aggressive sell order fills multiple bids ---" << std::endl;
    std::cout << "Adding a SELL order for 15 shares @ 100." << std::endl;
    std::cout << "Expected: Should fill 10 shares from Order 1, then 5 from Order 2." << std::endl;
    lob.add_order({order_id_counter++, 100, 15, Side::Sell}); // ID 8
    print_trades(lob);

    std::cout << "\n--- Scenario 4: Cancelling an order ---" << std::endl;
    std::cout << "Cancelling order ID 3 (Buy 20 @ 99)." << std::endl;
    lob.cancel_order(3);
    std::cout << "Order 3 cancelled." << std::endl;

    std::cout << "\n--- Scenario 5: A sell that would have matched the cancelled order ---" << std::endl;
    std::cout << "Adding a SELL order for 5 shares @ 99." << std::endl;
    std::cout << "Expected: No trade should occur as Order 3 is gone." << std::endl;
    lob.add_order({order_id_counter++, 99, 5, Side::Sell}); // ID 9
    print_trades(lob);



    return 0;
}
