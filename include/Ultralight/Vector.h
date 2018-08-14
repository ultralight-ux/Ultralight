// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>
#include <string.h>

namespace ultralight {

/**
 * Vector convenience class.
 */
template<typename T>
class Vector {
public: 
  // Construct empty Vector
  Vector() : size_(0), capacity_(0), data_(nullptr) {}

  // Construct Vector with initial size
  Vector(size_t initial_size) : size_(initial_size),
                                capacity_(initial_size),
                                data_(nullptr) {
    if (capacity_)
      data_ = new T[capacity_];
  }

  // Construct Vector by copying another Vector
  Vector(const Vector& rhs) : size_(rhs.size_),
                              capacity_(rhs.capacity_),
                              data_(nullptr) {
    *this = rhs;
  }

  // Construct Vector by copying another Vector.
  // We do a deep copy even in move-constructor
  // because this may move across ABI boundaries.
  Vector(Vector&& rhs) : size_(rhs.size_),
                         capacity_(rhs.capacity_),
                         data_(nullptr) {
    *this = rhs;
  }

  // Destruct Vector
  ~Vector() {
    if (data_)
      delete[] data_;
  }

  // Get number of elements in container
  size_t size() const { return size_; }

  // Get an element by index, returning a pointer to element or NULL if out of bounds.
  T* at(size_t index) const { return index >= size_ ? nullptr : &data_[index]; }

  // Get element by index without any bounds checking.
  T& operator[](size_t index) { return data_[index]; }
  const T& operator[](size_t index) const { return data_[index]; }

  // Get raw buffer
  T* data() { return data_; }
  const T* data() const { return data_; }

  // Assign another Vector to this one, a deep copy is made.
  void operator=(const Vector& rhs) {
    if (this == &rhs)
      return;

    if (data_)
      delete[] data_;

    size_ = rhs.size_;
    capacity_ = rhs.capacity_;
    data_ = nullptr;

    if (capacity_) {
      data_ = new T[capacity_];
      memcpy(data_, rhs.data_, size_ * sizeof(T));
    }
  }

  // Push a new element on to back of vector
  void push_back(const T& value) {
    // Fast-path: we can accomodate the element in our buffer
    if (size_ < capacity_) {
      data_[size_++] = value;
      return;
    }

    // Calculate new capacity and allocate new buffer.
    size_t new_capacity = static_cast<size_t>((size_ + 1) * 1.5);
    T* new_data = new T[new_capacity];

    // Copy over existing elements and deallocate old buffer
    if (size_) {
      memcpy(new_data, data_, size_ * sizeof(T));
      delete[] data_;
    }

    capacity_ = new_capacity;
    data_ = new_data;

    // Finally, add the new value
    data_[size_++] = value;
  }

private:
  size_t size_;
  size_t capacity_;
  T* data_;
};


}  // namespace ultralight
