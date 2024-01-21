//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// UNSUPPORTED: c++03, c++11, c++14, c++17

// Make sure that, when std::equal forwards to memcmp, the hardening checks from
// bounded iterators are preserved.

// REQUIRES: has-unix-headers, libcpp-has-abi-bounded-iterators
// UNSUPPORTED: libcpp-hardening-mode=none

#include <algorithm>
#include <span>

#include "check_assertion.h"

int main(int, char**) {
  int array1[3]              = {1, 2, 3};
  int array2[3]              = {1, 2, 3};
  std::span<const int> span1 = array1;

  // Compare against a span that has more space behind it, to test that safety
  // checks kick in. Without the safety checks, the comparison would cleanly
  // read alloocated memory.
  std::span<const int> span2 = std::span(array2).first(2);
  TEST_LIBCPP_ASSERT_FAILURE(std::equal(span1.begin(), span1.end(), span2.begin()),
                             "__bounded_iter::operator+=: Attempt to advance an iterator past the end");

  // Repeat the tests reading from an empty span. This is to ensure safety
  // checks happen before actually reading. If they happen in the wrong order,
  // we dereference null first and fail with the wrong eror.
  std::span<const int> empty;
  TEST_LIBCPP_ASSERT_FAILURE(std::equal(span1.begin(), span1.end(), empty.begin()),
                             "__bounded_iter::operator+=: Attempt to advance an iterator past the end");

  return 0;
}
