//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// UNSUPPORTED: c++03, c++11, c++14, c++17

// Make sure that, when std::copy, std::move, etc., forward to memmove, the
// hardening checks from bounded iterators are preserved.

// REQUIRES: has-unix-headers, libcpp-has-abi-bounded-iterators
// UNSUPPORTED: libcpp-hardening-mode=none

#include <algorithm>
#include <span>

#include "check_assertion.h"

int main(int, char**) {
  // The destination iterator does not have enough space.
  {
    int src_array[3] = {1, 2, 3};
    int dst_array[3];
    std::span<const int> src = src_array;

    // Copy into a span that has more space behind it, to test that safety
    // checks kick in. Without the safety checks, the copy would cleanly write
    // into allocated memory.
    std::span<int> dst = std::span(dst_array).first(2);
    TEST_LIBCPP_ASSERT_FAILURE(
        std::ranges::copy(src, dst.begin()), "__bounded_iter::operator+=: Attempt to advance an iterator past the end");
    TEST_LIBCPP_ASSERT_FAILURE(std::copy(src.begin(), src.end(), dst.begin()),
                               "__bounded_iter::operator+=: Attempt to advance an iterator past the end");
    TEST_LIBCPP_ASSERT_FAILURE(std::copy_n(src.begin(), src.size(), dst.begin()),
                               "__bounded_iter::operator+=: Attempt to advance an iterator past the end");
    TEST_LIBCPP_ASSERT_FAILURE(std::ranges::copy_backward(src, dst.end()),
                               "__bounded_iter::operator-=: Attempt to rewind an iterator past the start");
    TEST_LIBCPP_ASSERT_FAILURE(std::copy_backward(src.begin(), src.end(), dst.end()),
                               "__bounded_iter::operator-=: Attempt to rewind an iterator past the start");
    TEST_LIBCPP_ASSERT_FAILURE(
        std::ranges::move(src, dst.begin()), "__bounded_iter::operator+=: Attempt to advance an iterator past the end");
    TEST_LIBCPP_ASSERT_FAILURE(std::move(src.begin(), src.end(), dst.begin()),
                               "__bounded_iter::operator+=: Attempt to advance an iterator past the end");
    TEST_LIBCPP_ASSERT_FAILURE(std::ranges::move_backward(src, dst.end()),
                               "__bounded_iter::operator-=: Attempt to rewind an iterator past the start");
    TEST_LIBCPP_ASSERT_FAILURE(std::move_backward(src.begin(), src.end(), dst.end()),
                               "__bounded_iter::operator-=: Attempt to rewind an iterator past the start");

    // Repeat the tests copying into an empty span. This is to ensure safety
    // checks happen before actually copying. If they happen in the wrong order,
    // we dereference null first and fail with the wrong eror.
    std::span<int> empty;
    TEST_LIBCPP_ASSERT_FAILURE(std::ranges::copy(src, empty.begin()),
                               "__bounded_iter::operator+=: Attempt to advance an iterator past the end");
    TEST_LIBCPP_ASSERT_FAILURE(std::copy(src.begin(), src.end(), empty.begin()),
                               "__bounded_iter::operator+=: Attempt to advance an iterator past the end");
    TEST_LIBCPP_ASSERT_FAILURE(std::copy_n(src.begin(), src.size(), empty.begin()),
                               "__bounded_iter::operator+=: Attempt to advance an iterator past the end");
    TEST_LIBCPP_ASSERT_FAILURE(std::ranges::copy_backward(src, empty.end()),
                               "__bounded_iter::operator-=: Attempt to rewind an iterator past the start");
    TEST_LIBCPP_ASSERT_FAILURE(std::copy_backward(src.begin(), src.end(), empty.end()),
                               "__bounded_iter::operator-=: Attempt to rewind an iterator past the start");
    TEST_LIBCPP_ASSERT_FAILURE(std::ranges::move(src, empty.begin()),
                               "__bounded_iter::operator+=: Attempt to advance an iterator past the end");
    TEST_LIBCPP_ASSERT_FAILURE(std::move(src.begin(), src.end(), empty.begin()),
                               "__bounded_iter::operator+=: Attempt to advance an iterator past the end");
    TEST_LIBCPP_ASSERT_FAILURE(std::ranges::move_backward(src, empty.end()),
                               "__bounded_iter::operator-=: Attempt to rewind an iterator past the start");
    TEST_LIBCPP_ASSERT_FAILURE(std::move_backward(src.begin(), src.end(), empty.end()),
                               "__bounded_iter::operator-=: Attempt to rewind an iterator past the start");
  }

  // The source iterator does not have enough space.
  {
    int src_array[3] = {1, 2, 3};
    int dst_array[3];
    std::span<const int> src = std::span(src_array).first(2);
    std::span<int> dst       = dst_array;

    // Copy from a span that has more space behind it, to test that safety
    // checks kick in. Without the safety checks, the copy would cleanly read
    // from allocated memory.
    TEST_LIBCPP_ASSERT_FAILURE(std::copy_n(src.begin(), dst.size(), dst.begin()),
                               "__bounded_iter::operator+=: Attempt to advance an iterator past the end");

    // Repeat the tests copying from an empty span. This is to ensure safety
    // checks happen before actually copying. If they happen in the wrong order,
    // we dereference null first and fail with the wrong eror.
    std::span<const int> empty;
    TEST_LIBCPP_ASSERT_FAILURE(std::copy_n(empty.begin(), dst.size(), dst.begin()),
                               "__bounded_iter::operator+=: Attempt to advance an iterator past the end");
  }

  return 0;
}
