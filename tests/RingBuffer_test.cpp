#include "RingBuffer.h"

#include <gtest/gtest.h>

TEST(RingBuffer_test, construct_type_uint16_capacity_3) {
  auto rb = RingBuffer<uint16_t, 3>();

  EXPECT_EQ(rb.capacity(), 3);
}

TEST(RingBuffer_test, new_shall_be_empty) {
  auto rb = RingBuffer<uint16_t, 3>();

  EXPECT_TRUE(rb.isEmpty());
}

TEST(RingBuffer_test, push_one_item_at_front_shall_not_be_empty) {
  auto rb = RingBuffer<uint16_t, 3>();
  rb.push_front(2);

  EXPECT_FALSE(rb.isEmpty());
}

TEST(RingBuffer_test, push_one_item_at_back_shall_not_be_empty) {
  auto rb = RingBuffer<uint16_t, 3>();
  rb.push_back(2);

  EXPECT_FALSE(rb.isEmpty());
}

TEST(RingBuffer_test,
     push_front_then_pop_back_shall_pop_same_value_as_pushed_and_be_empty) {
  auto rb = RingBuffer<uint16_t, 3>();
  rb.push_front(2);

  EXPECT_EQ(rb.pop_back(), 2);
  EXPECT_TRUE(rb.isEmpty());
}

TEST(RingBuffer_test,
     push_back_then_pop_front_shall_pop_same_value_as_pushed_and_be_empty) {
  auto rb = RingBuffer<uint16_t, 3>();
  rb.push_back(2);

  EXPECT_EQ(rb.pop_front(), 2);
  EXPECT_TRUE(rb.isEmpty());
}

TEST(RingBuffer_test, push_front_three_items_shall_be_full) {
  auto rb = RingBuffer<uint16_t, 3>();
  EXPECT_FALSE(rb.isFull());
  rb.push_front(1);
  EXPECT_FALSE(rb.isFull());
  rb.push_front(2);
  EXPECT_FALSE(rb.isFull());
  rb.push_front(3);
  EXPECT_TRUE(rb.isFull());

  rb.pop_back();
  EXPECT_FALSE(rb.isFull());
}

TEST(RingBuffer_test, push_back_three_items_shall_be_full) {
  auto rb = RingBuffer<uint16_t, 3>();
  EXPECT_FALSE(rb.isFull());
  rb.push_back(1);
  EXPECT_FALSE(rb.isFull());
  rb.push_back(2);
  EXPECT_FALSE(rb.isFull());
  rb.push_back(3);
  EXPECT_TRUE(rb.isFull());

  rb.pop_front();
  EXPECT_FALSE(rb.isFull());
}

TEST(RingBuffer_test, size) {
  auto rb = RingBuffer<uint16_t, 3>();

  EXPECT_EQ(rb.size(), 0);
  rb.push_front(1);
  EXPECT_EQ(rb.size(), 1);
  rb.push_front(2);
  EXPECT_EQ(rb.size(), 2);
  rb.push_front(3);
  EXPECT_EQ(rb.size(), 3);
  EXPECT_EQ(rb.pop_back(), 1);
  EXPECT_EQ(rb.size(), 2);
  EXPECT_EQ(rb.pop_back(), 2);
  EXPECT_EQ(rb.size(), 1);
  EXPECT_EQ(rb.pop_back(), 3);
  EXPECT_EQ(rb.size(), 0);
}

TEST(RingBuffer_test, fill) {
  auto rb = RingBuffer<uint16_t, 3>();
  rb.fill(9);

  EXPECT_FALSE(rb.isEmpty());
  EXPECT_TRUE(rb.isFull());

  EXPECT_EQ(rb.pop_back(), 9);
  EXPECT_EQ(rb.pop_back(), 9);
  EXPECT_EQ(rb.pop_back(), 9);
  EXPECT_TRUE(rb.isEmpty());
  EXPECT_FALSE(rb.isFull());
}

TEST(RingBuffer_test, clear) {
  auto rb = RingBuffer<uint16_t, 3>();
  rb.fill(4);

  rb.clear();

  EXPECT_TRUE(rb.isEmpty());
  EXPECT_FALSE(rb.isFull());
}

TEST(RingBuffer_test, access_element_a_front_and_back) {
  auto rb = RingBuffer<uint16_t, 3>();
  rb.push_front(1);
  rb.push_front(2);

  EXPECT_EQ(rb.front(), 2);
  EXPECT_EQ(rb.back(), 1);
}
