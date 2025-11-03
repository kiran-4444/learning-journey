//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// count_min_sketch.cpp
//
// Identification: src/primer/count_min_sketch.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/count_min_sketch.h"

#include <mutex>
#include <stdexcept>
#include <string>

std::mutex Mutex;

namespace bustub {

/**
 * Constructor for the count-min sketch.
 *
 * @param width The width of the sketch matrix.
 * @param depth The depth of the sketch matrix.
 * @throws std::invalid_argument if width or depth are zero.
 */
template <typename KeyType>
CountMinSketch<KeyType>::CountMinSketch(uint32_t width, uint32_t depth) : width_(width), depth_(depth) {
  /** @TODO(student) Implement this function! */

  if (width_ == 0 || depth_ == 0) {
    throw std::invalid_argument("Invalid width or depth for CountMinSketch.");
  }

  matrix_ = new uint32_t *[width_];
  for (uint32_t i = 0; i < width_; ++i) {
    matrix_[i] = new u_int32_t[depth_];
  }

  for (uint32_t i = 0; i < width_; ++i) {
    for (uint32_t j = 0; j < depth_; ++j) {
      matrix_[i][j] = 0;
    }
  }

  /** @fall2025 PLEASE DO NOT MODIFY THE FOLLOWING */
  // Initialize seeded hash functions
  hash_functions_.reserve(depth_);
  for (size_t i = 0; i < depth_; i++) {
    hash_functions_.push_back(this->HashFunction(i));
  }
}

template <typename KeyType>
CountMinSketch<KeyType>::~CountMinSketch() {
  if (matrix_ != nullptr) {
    for (uint32_t i = 0; i < width_; ++i) delete[] matrix_[i];
    delete[] matrix_;
  }
}

template <typename KeyType>
CountMinSketch<KeyType>::CountMinSketch(CountMinSketch &&other) noexcept
    : width_(other.width_), depth_(other.depth_), matrix_(other.matrix_) {
  /** @TODO(student) Implement this function! */

  hash_functions_.clear();
  hash_functions_.reserve(depth_);
  for (size_t i = 0; i < depth_; i++) {
    hash_functions_.push_back(this->HashFunction(i));
  }

  other.matrix_ = nullptr;
  other.width_ = 0;
  other.depth_ = 0;
  other.hash_functions_.clear();
}

template <typename KeyType>
auto CountMinSketch<KeyType>::operator=(CountMinSketch &&other) noexcept -> CountMinSketch & {
  /** @TODO(student) Implement this function! */

  if (this != &other) {
    if (matrix_ != nullptr) {
      for (size_t i = 0; i < width_; i++) {
        delete[] matrix_[i];
      }
      delete[] matrix_;
      matrix_ = nullptr;
    }

    width_ = other.width_;
    depth_ = other.depth_;
    matrix_ = other.matrix_;

    hash_functions_.clear();
    hash_functions_.reserve(depth_);
    for (size_t i = 0; i < depth_; i++) {
      hash_functions_.push_back(this->HashFunction(i));
    }

    other.matrix_ = nullptr;
    other.width_ = 0;
    other.depth_ = 0;
    other.hash_functions_.clear();
  }
  return *this;
}

template <typename KeyType>
void CountMinSketch<KeyType>::Insert(const KeyType &item) {
  /** @TODO(student) Implement this function! */

  for (uint32_t i = 0; i < depth_; ++i) {
    size_t hash_value = hash_functions_[i](item);
    std::lock_guard<std::mutex> lock(Mutex);
    matrix_[hash_value][i]++;
  }
}

template <typename KeyType>
void CountMinSketch<KeyType>::Merge(const CountMinSketch<KeyType> &other) {
  if (width_ != other.width_ || depth_ != other.depth_) {
    throw std::invalid_argument("Incompatible CountMinSketch dimensions for merge.");
  }
  /** @TODO(student) Implement this function! */

  for (uint32_t i = 0; i < width_; ++i) {
    for (uint32_t j = 0; j < depth_; ++j) {
      matrix_[i][j] += other.matrix_[i][j];
    }
  }
}

template <typename KeyType>
auto CountMinSketch<KeyType>::Count(const KeyType &item) const -> uint32_t {
  uint32_t min_value = UINT32_MAX;
  for (uint32_t i = 0; i < depth_; ++i) {
    size_t hash_value = hash_functions_[i](item);
    if (matrix_[hash_value][i] < min_value) {
      min_value = matrix_[hash_value][i];
    }
  }
  return min_value;
}

template <typename KeyType>
void CountMinSketch<KeyType>::Clear() {
  /** @TODO(student) Implement this function! */

  for (uint32_t i = 0; i < width_; ++i) {
    for (uint32_t j = 0; j < depth_; ++j) {
      matrix_[i][j] = 0;
    }
  }
}

template <typename KeyType>
auto CountMinSketch<KeyType>::TopK(uint16_t k, const std::vector<KeyType> &candidates)
    -> std::vector<std::pair<KeyType, uint32_t>> {
  /** @TODO(student) Implement this function! */

  std::vector<std::pair<KeyType, uint32_t>> result;

  // Get counts for all candidates
  for (const auto &candidate : candidates) {
    uint32_t count = Count(candidate);
    result.emplace_back(candidate, count);
  }

  // Sort by count in descending order
  std::sort(
      result.begin(), result.end(),
      [](const std::pair<KeyType, uint32_t> &a, const std::pair<KeyType, uint32_t> &b) { return a.second > b.second; });

  // Return top k elements
  if (result.size() > k) {
    result.resize(k);
  }

  return result;
}

// Explicit instantiations for all types used in tests
template class CountMinSketch<std::string>;
template class CountMinSketch<int64_t>;  // For int64_t tests
template class CountMinSketch<int>;      // This covers both int and int32_t
}  // namespace bustub
