#include "lob/LimitOrderBook.h"
#include <gtest/gtest.h>

/* Test fixture */
class LOBTest : public ::testing::Test {
protected:
  LimitOrderBook lob; // new lob object for each test case
};

/* Test suite */
/* Basic Operations */
TEST_F(LOBTest, AddAndMatch) {
  lob.add_order(1, 100, 10, Side::Buy);
  lob.add_order(2, 100, 10, Side::Sell);

  ASSERT_EQ(lob.get_trades().size(), 1); // Exactly one trade should occur
  const auto &trade = lob.get_trades().front(); // get first trade

  /* Check the member functions are correct */
  ASSERT_EQ(trade.price, 100);
  ASSERT_EQ(trade.quantity, 10);
  ASSERT_EQ(trade.resting_order_id, 1);
  ASSERT_EQ(trade.aggressing_order_id, 2);

  /* Check the bid and ask data structure is emtpy */
  ASSERT_FALSE(lob.get_best_bid().has_value());
  ASSERT_FALSE(lob.get_best_ask().has_value());
}

/* Order is successfully cancelled */
TEST_F(LOBTest, CancelBestBid) {
  lob.add_order(1, 100, 10, Side::Buy); // Worse price
  lob.add_order(2, 105, 10, Side::Buy); // Best price

  // CHECK 1: The best bid should be order #2 at price 105.
  // We use .at() to look up a specific price level.
  ASSERT_TRUE(lob.get_best_bid().has_value());

  ASSERT_EQ(lob.get_best_bid()->first, 105);

  // ACTION: Cancel the BEST bid (order #2)
  lob.cancel_order(2);

  // CHECK 2: The NEW best bid should now be order #1 at price 100.

  ASSERT_EQ(lob.get_best_bid()->first, 100);
}

/* Tests the limit order book time priority rule, given the same price */
TEST_F(LOBTest, FifoPriority) {
  lob.add_order(1, 100, 10, Side::Buy);
  lob.add_order(2, 100, 5, Side::Buy);

  lob.add_order(3, 100, 10, Side::Sell);

  ASSERT_EQ(lob.get_trades().size(), 1);        // Only one trade has occured
  const auto &trade = lob.get_trades().front(); // Reference only
  ASSERT_EQ(trade.price, 100);
  ASSERT_EQ(trade.resting_order_id,
            1); // Check that that resting order of first trade is id 1
}

/* Future tests: Partial fill, Multiple trade matches at same price and
 *different match check zero quantity order, cancel non-existent order, cancel
 *partially filled order
 * */
