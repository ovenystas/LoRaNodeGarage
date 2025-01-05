#pragma once

#include <stddef.h>

template <class T, size_t N>
class GenericRingBuffer {
 public:
  /**
   * @brief Checks the capacity of the ring buffer. I.e. the maximum number of
   * elements it can hold.
   *
   * @return size_t max number of elements the ring buffer can hold.
   */
  constexpr size_t capacity() const { return N; }

  /**
   * @brief Checks the number of elements in the ring buffer.
   *
   * @return size_t number of elements in the ring buffer.
   */
  size_t size() const {
    if (mFull) {
      return N;
    }
    if (mFront >= mBack) {
      return mFront - mBack;
    }
    return N + mFront - mBack;
  }

  /**
   * @brief Checks if the ring buffer is empty.
   *
   * @return true if ring buffer is empty.
   * @return false if ring buffer isn't empty.
   */
  bool isEmpty() const { return !mFull && mFront == mBack; }

  /**
   * @brief Checks if the ring buffer is full.
   *
   * @return true if ring buffer is full.
   * @return false if ring buffer isn't full.
   */
  bool isFull() const { return mFull; }

  /**
   * @brief Pops the item at the back of the ring buffer.
   *
   * @return T popped item.
   */
  T pop_back() {
    T item = mRb[mBack];
    mBack = (mBack + 1) % N;
    mFull = false;
    return item;
  }

  /**
   * @brief Pops the item at the front of the ring buffer.
   *
   * @return T popped item.
   */
  T pop_front() {
    if (mFront > 0) {
      mFront--;
    } else {
      mFront = N - 1;
    }
    T item = mRb[mFront];
    mFull = false;
    return item;
  }

  /**
   * @brief Pushes the given item to the back of the ring buffer.
   *
   * @param item to push.
   */
  void push_back(T item) {
    if (mBack > 0) {
      mBack--;
    } else {
      mBack = N - 1;
    }
    mRb[mBack] = item;
    if (mFull) {
      if (mFront > 0) {
        mFront--;
      } else {
        mFront = N - 1;
      }
    }
    mFull = mFront == mBack;
  }

  /**
   * @brief Pushes the given item to the front of the ring buffer.
   *
   * @param item to push.
   */
  void push_front(T item) {
    mRb[mFront] = item;
    mFront = (mFront + 1) % N;
    if (mFull) {
      mBack = (mBack + 1) % N;
    }
    mFull = mFront == mBack;
  }

  /**
   * @brief Fills remaing elements in the ring buffer with the same item.
   *
   * @param item to fill ring buffer with.
   */
  void fill(T item) {
    while (!mFull) {
      push_front(item);
    }
  }

  /**
   * @brief Returns a reference to the last element in the ring buffer.
   * Calling back on an empty ring buffer causes undefined behavior.
   *
   * @return T& referenece to the last element.
   */
  const T& back() const { return mRb[mBack]; }

  /**
   * @brief Returns a reference to the first element in the ring buffer.
   * Calling first on an empty ring buffer causes undefined behavior.
   *
   * @return T& referenece to the first element.
   */
  const T& front() const {
    if (mFront > 0) {
      return mRb[mFront - 1];
    }
    return mRb[N - 1];
  }

  /**
   * @brief Clears the contents of the ring buffer.
   *
   */
  void clear() {
    mFront = 0;
    mBack = 0;
    mRb[0] = 0;
    mFull = false;
  }

 private:
  T mRb[N]{};
  size_t mFront{};
  size_t mBack{};
  bool mFull{false};
};
