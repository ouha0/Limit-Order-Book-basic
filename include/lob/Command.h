#pragma once

#include "lob/Order.h"

/* Command type for parser. To store IO before running experiment */
enum class CommandType { Add, Cancel };

/* Using Add and Cancel together for simplicity */
struct Command {
    CommandType type;
    OrderId id; // set to 0 for add commands, this variable is required for the cancel command 
    Side side; 
    Price price;
    Quantity quantity;

    /* Constructor for command type */
    Command(CommandType t, OrderId i, Side s, Price p, Quantity q)
    : type(t), id(i), side(s), price(p), quantity(q) {}
    
}; 
