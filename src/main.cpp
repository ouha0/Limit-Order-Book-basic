#include "lob/LimitOrderBook.h"
#include "lob/Parser.h"
#include "lob/Command.h"
#include <iostream>
#include <vector>
#include <chrono> // use to measure efficiency


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
int main (int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Not enough arguments; input missing" << std::endl;
        return 1;
    }
    
    /* Part 1: Store .txt input into vector container */
    std::string filename = argv[1];
    std::vector<Command> commands = Parser::parse_commands(filename);
    if (commands.empty()) {
        std::cerr << "commands not successfully read from txt file" << std::endl;
        return 1;
    }
    std::cout << "Successfully parsed " << commands.size() << " commands." << std::endl;

    /* Part 2: lob takes in orders. Start measuring speed of LOB here */
    
    /* Setup */
    LimitOrderBook lob;
    OrderId order_id_counter = 1;

    std::cout << "Processing orders..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();

    /* Adding commands to LOB */
    for (const auto& cmd : commands) {
        if (cmd.type == CommandType::Add) {
            lob.add_order({order_id_counter++, cmd.price, cmd.quantity, cmd.side});

        } else if (cmd.type == CommandType::Cancel) {
            lob.cancel_order(cmd.id);

        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    /* Part 3: Check behaviour and time */
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Cancel orders not found: " << lob.get_missed_cancel_count() << std::endl;
    std::cout << "\nProcessing took " << duration.count() << " milliseconds" << std::endl;
    std::cout << "Total Trades: " << lob.get_trades().size() << std::endl;
    

    return 0;
}
