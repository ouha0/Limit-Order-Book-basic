#pragma once

#include "lob/Command.h"
#include <vector> 
#include <string>

/* The function that will read a file and return a vector of commands 
 * (client bid and ask orders)
 * */
namespace Parser {
    std::vector<Command> parse_commands(const std::string& filename);
}
