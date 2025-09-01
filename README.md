# LOB_basic
This project is a simple implementation of a limit order book (LOB) in C++. It is designed as an educational tool to demonstrate the core functionality of matching buy and sell orders based on 
price-time priority. 

This is not a low-latency system. The primary focus is on correctness and clarity of the fundamental LOB mechanics, not on advanced features ligh high-frequency performance, network protocols 
or complex order types. 


# Features 
- **Add Orders:** Accepts new buy and sell limit orders
- **Cancel Orders:** Order cancellation mechanism
- **Price-time Priority Matching:** Matches orders based on standard exchange rules (
Price first, then first-in-first-out)
- **Trade Log:** logs all trades with price, quantity, and parties (customer id)


*   **The Books (`std::map`):** 
    *    `Self-balancing tree`
    *   `std::map<Price, OrderList, std::greater<Price>> bids;`
    *   `std::map<Price, OrderList> asks;`
    *   `std::map` provides guaranteed `O(log P)` insertion and lookup (where `P` is the number of price levels) and, keeps the price levels automatically sorted. This makes finding the best bid and ask a `O(1)` operation (`.begin()`).

*   **The Order Queue (`std::list`):**
    *   At each price level, a `std::list<Order>` acts as a FIFO queue.
    *   A linked list provides `O(1)` insertion at the back (`push_back`) and, **`O(1)` erasure from anywhere in the middle** given an iterator. 

*   **The O(1) Cancellation Index (`std::unordered_map`):**
    *   `std::unordered_map<OrderId, OrderInfo> order_map_;`
    *    To avoid a slow `O(N)` search through the entire book for a single cancellation, this hash map provides a direct, `O(1)` average time lookup. It maps an `OrderId` to an `OrderInfo` struct, which contains an iterator pointing directly to the order's location in its Price List.

# Limitations
While this implementation is algorithmically efficient (good Big-O complexity), its heavy reliance on node-based 
containers like std::map and std::list makes it cache-unfriendly. The scattered memory allocations for each node 
lead to frequent pointer chasing and CPU cache misses, which become the primary performance bottleneck at scale.
A truly low-latency implementation would prioritize a contiguous memory layout to maximize cache utilization.

# Current Status 
This project is currently under development...
