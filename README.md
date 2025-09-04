## LOB_basic
This project is a simple implementation of a limit order book (LOB) in C++. It is designed as an educational tool to demonstrate the core functionality of matching buy and sell orders based on 
price-time priority. 

## Features 
- **Add Orders:** Accepts new buy and sell limit orders
- **Cancel Orders:** Order cancellation mechanism
- **Price-time Priority Matching:** Matches orders based on standard exchange rules(
Price first, then first-in-first-out)
- **Trade Log:** logs all trades with price, quantity, and parties (customer ID)
- **Data Generation**: Includes a python scrypt (jupyter lab) to generate "approximate" commands for benchmarking.
(Cancel orders for non-existent Customer IDs are skipped)
- **Automated Testing:** Unit test suite built with Google Test and automated via 
GitHub Actions CI pipeline.
- **Performance Metrics:** The application reports key metrics, including total throughput in trades/second.

**Benchmark Result:** The current implementation processes **~600,000 trades/second** on 
a stream of around **10,000,000 commands**.

## Project Goal 
The goal of this project is to build and benchmark a basic limit order book that has 
good time complexity, and then compare it with a cache-friendly version with performance optimization techniques. 
The two different implementations will be then compared. The basic implementation has been completed, and the 
cache-friendly and optimized version is currently under progress.

## Data Structures (Basic Implementation)
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

## Future work
While this implementation is algorithmically efficient (good Big-O complexity), its heavy reliance on node-based 
containers like std::map and std::list makes it cache-unfriendly. The scattered memory allocations for each node 
lead to frequent pointer chasing and CPU cache misses, which become the primary performance bottleneck at scale.
Future work will be more cache-friendly, focusing on maximizing CPU cache utilization, using **Object Pools**, **Intrusive Containers** 
and **Contiguous arrays.**

## Build Steps

1.  **Clone the repository**
    ```sh
    git clone https://github.com/ouha0/LOB_basic.git(https://github.com/ouha0/LOB_basic)
    cd LOB_basic.git
    ```
2.  **Configure and build with CMake:**
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```
3. **Running the application:**
    ```sh
    ./lob ../order_test.txt
    ```


## Generate Data 
1. **Go to data_generate directory from root directory**
    ```sh
    cd data_generate
    ```
2. **Open the jupyter lab and run all cells**
Change the filename in the last cell, and run the cells 


## Other
Google test was used for testing

