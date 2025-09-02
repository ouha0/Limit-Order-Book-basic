#include <gtest/gtest.h>
#include "lob/LimitOrderBook.h"

/* Test fixture */
class LOBTest : public::testing::Test {
protected:
    LimitOrderBook lob; // new lob object for each test case
};

/* Test suite */
/* Add a buy and sell order at the same price and check that a 
 * trade will occur. Also make sure that the ask and bid tree is empty */
TEST_F(LOBTest, BuySellOrder) {
    lob.add_order({1, 100, 10, Side::Buy});
    lob.add_order({2, 100, 10, Side::Sell});

    ASSERT_EQ(lob.get_trades().size(), 1); // Exactly one trade should occur
    const auto& trade = lob.get_trades().front(); // get first trade 
    
    /* Check the member functions are correct */
    ASSERT_EQ(trade.price, 100);
    ASSERT_EQ(trade.quantity, 10);
    ASSERT_EQ(trade.resting_order_id, 1);
    ASSERT_EQ(trade.aggressing_order_id, 2);

    /* Check the bid and ask data structure is emtpy */
    ASSERT_EQ(lob.get_asks().empty(), true);
    ASSERT_EQ(lob.get_bids().empty(), true);

}

/* Tests the limit order book time priority rule, given the same price */
TEST_F(LOBTest, FifoPriority) {
    lob.add_order({1, 100, 10, Side::Buy});
    lob.add_order({2, 100, 5, Side::Buy});

    lob.add_order({3, 100, 10, Side::Sell});

    ASSERT_EQ(lob.get_trades().size(), 1); // Only one trade has occured
    const auto &trade = lob.get_trades().front(); // Reference only
    ASSERT_EQ(trade.price, 100);
    ASSERT_EQ(trade.resting_order_id, 1); // Check that that resting order of first trade is id 1
}


