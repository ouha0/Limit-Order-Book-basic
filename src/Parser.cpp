#include "lob/Parser.h"
#include "lob/Order.h"
#include <fstream>
#include <sstream>
#include <iostream>



namespace Parser {


/* Expected input structure from .txt : 
 * "ADD side price quantity"
 * "CANCEL id"
 * */




/* Function that parses commands from a input file, and 
 * stores commands into a vector of commands (I/O is very slow)*/
std::vector<Command> parse_commands(const std::string& filename) {
    std::ifstream input_file(filename);
    if (!input_file.is_open()) {
        std::cerr << "Error opening file " << filename << std::endl;
        return {};
    }

    /* Vector that will contain all bid and ask commands, stored in heap 
     * commands "pointer" is local */
    std::vector<Command> commands; 
    std::string line; // tmp variable for storing single line 
    
    while (std::getline(input_file, line)) {
        /* Skip if current line is empty, or is intented for comments */
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line); // private terminal? easier to read 
        std::string command_str;

        ss >> command_str;

        /* Main parsing logic */
        if (command_str == "ADD") {
            std::string side_str; 
            Price price; 
            Quantity quantity; 

            // Read all members 
            ss >> side_str >> price >> quantity;
            if (!ss.fail()) { // check if ss line is behaving correctly 
                Side side;
                /* Check buy/sell command */
                if (side_str == "BUY") {
                    side = Side::Buy;
                } else if (side_str == "SELL") {
                    side = Side::Sell;
                }
                commands.emplace_back(CommandType::Add, 0, side, price, quantity);

            }

        } else if (command_str == "CANCEL") { 
            OrderId id; 
            ss >> id; 

            if (!ss.fail()) {
                /* Only the command type and the id matters here */
                commands.emplace_back(CommandType::Cancel, id, Side::Buy, 0, 0);
            }
        }
    } 

    return commands; // return vector of all commands in input file 
}

}
