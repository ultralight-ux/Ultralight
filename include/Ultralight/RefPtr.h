// Copyright 2018 Ultralight, Inc. All rights reserved.
/*
* Portions of the below code are derived from 'RefPtr.h' from Apple's WTF,
* with the following license header:
*
* Copyright (C) 2013-2014 Apple Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
#include <Ultralight/Defines.h>
#include <assert.h>
#include <utility>

namespace ultralight {
  
class UExport RefCounted {
 public:
  virtual void AddRef() const = 0;
  virtual void Release() const = 0;
  virtual int ref_count() const = 0;
 protected:
  virtual ~RefCounted();
};

inline void adopted(const void*) { }

template<typename T> class Ref;

//
// All ref-counted object are created with an initial ref-count of '1'.
// The AdoptRef() helper returns a Ref<T> without calling AddRef().
// This is used for creating new objects, like so:
//
//   Ref<Object> ref = AdoptRef(*new ObjectImpl());
//
template<typename T> Ref<T> AdoptRef(T&);

/**
 * Smart pointer class that stores a RefCounted instance and is guaranteed
 * to not be NULL (stores a reference, not a pointer).
 */
template<typename T> class Ref {
public:
  ~Ref()
  {
    if (instance_)
      instance_->Release();
  }

  Ref(T& object)
    : instance_(&object)
  {
    instance_->AddRef();
  }

  Ref(const Ref& other)
    : instance_(other.instance_)
  {
    instance_->AddRef();
  }

  template<typename U>
  Ref(Ref<U>& other)
    : instance_(other.ptr())
  {
    instance_->AddRef();
  }

  template<typename U>
  Ref(const Ref<U>& other)
    : instance_(other.ptr())
  {
    instance_->AddRef();
  }

  Ref(Ref&& other)
    : instance_(&other.LeakRef())
  {
    assert(instance_);
  }

  template<typename U>
  Ref(Ref<U>&& other)
    : instance_(&other.LeakRef())
  {
    assert(instance_);
  }

  Ref& operator=(T& object)
  {
    assert(instance_);
    object.AddRef();
    instance_->Release();
    instance_ = &object;
    assert(instance_);
    return *this;
  }

  Ref& operator=(const Ref& other)
  {
    assert(instance_);
    other.instance_->AddRef();
    instance_->Release();
    instance_ = other.instance_;
    assert(instance_);
    return *this;
  }

  template<typename U>
  Ref& operator=(const Ref<U>& other)
  {
    assert(instance_);
    other.instance_->AddRef();
    instance_->Release();
    instance_ = other.instance_;
    assert(instance_);
    return *this;
  }

  Ref& operator=(Ref&& reference)
  {
    assert(instance_);
    instance_->Release();
    instance_ = &reference.LeakRef();
    assert(instance_);
    return *this;
  }

  template<typename U> Ref& operator=(Ref<U>&& reference)
  {
    assert(instance_);
    instance_->Release();
    instance_ = &reference.LeakRef();
    assert(instance_);
    return *this;
  }

  const T* operator->() const { assert(instance_); return instance_; }
  T* operator->() { assert(instance_); return instance_; }

  const T* ptr() const { assert(instance_); return instance_; }
  T* ptr() { assert(instance_); return instance_; }

  const T& get() const { assert(instance_); return *instance_; }
  T& get() { assert(instance_); return *instance_; }

  operator T&() { assert(instance_); return *instance_; }
  operator const T&() const { assert(instance_); return *instance_; }

  template<typename U> Ref<T> Replace(Ref<U>&&);

  T& LeakRef() {
    assert(instance_);

    T* result = std::move(instance_);
    instance_ = std::forward<T*>(nullptr);
    return *result;
  }

  friend inline bool operator==(const Ref& a, const Ref& b) {
    return a.instance_ == b.instance_;
  }

  friend inline bool operator!=(const Ref& a, const Ref& b) {
    return a.instance_ != b.instance_;
  }

  friend inline bool operator<(const Ref& a, const Ref& b) {
    return a.instance_ < b.instance_;
  }

protected:
  friend Ref AdoptRef<T>(T&);
  template<typename U> friend class RefPtr;

  enum AdoptTag { Adopt };
  Ref(T& object, AdoptTag)
    : instance_(&object)
  {
  }

  T* instance_;
};

template<typename T> template<typename U> Ref<T> Ref<T>::Replace(Ref<U>&& reference)
{
  auto oldReference = AdoptRef(*instance_);
  instance_ = &reference.LeakRef();
  return oldReference;
}

template<typename T>
Ref<T> AdoptRef(T& reference)
{
  adopted(&reference);
  return Ref<T>(reference, Ref<T>::Adopt);
}

/**
 * Smart pointer class that stores a RefCounted instance (may be NULL).
 */
template<typename T> class RefPtr {
 public:
  RefPtr() 
    : instance_(nullptr)
  {
  }

  RefPtr(std::nullptr_t)
    : instance_(nullptr)
  {
  }

  RefPtr(const RefPtr& other) 
    : instance_(other.instance_) 
  {
    if (instance_)
      instance_->AddRef();
  }

  template<typename U>
  RefPtr(const RefPtr<U>& other) 
    : instance_(other.instance_) 
  {
    if (instance_)
      instance_->AddRef();
  }

  RefPtr(RefPtr&& other) 
    : instance_(other.LeakRef())
  {
  }

  template<typename U>
  RefPtr(RefPtr<U>&& other) 
    : instance_(other.LeakRef())
  {
  }

  template<typename U>
  RefPtr(const Ref<U>& other)
    : instance_(other.instance_)
  {
    if (instance_)
      instance_->AddRef();
  }

  template<typename U>
  RefPtr(Ref<U>&& other);

  ~RefPtr()
  {
    T* old_value = std::move(instance_);
    instance_ = std::forward<T*>(nullptr);
    if (old_value)
      old_value->Release();
  }

  T* get() const { return instance_; }

  T* LeakRef() {
    T* result = std::move(instance_);
    instance_ = std::forward<T*>(nullptr);
    return result;
  }

  T& operator*() const { assert(instance_); return *instance_; }
  T* operator->() const { return instance_; }

  bool operator!() const { return !instance_; }

  // This conversion operator allows implicit conversion to bool but not to other integer types.
  typedef T* (RefPtr::*UnspecifiedBoolType);
  operator UnspecifiedBoolType() const { return instance_ ? &RefPtr::instance_ : nullptr; }

  RefPtr& operator=(const RefPtr&);
  RefPtr& operator=(T*);
  RefPtr& operator=(std::nullptr_t);
  template<typename U> RefPtr& operator=(const RefPtr<U>&);
  RefPtr& operator=(RefPtr&&);
  template<typename U> RefPtr& operator=(RefPtr<U>&&);
  template<typename U> RefPtr& operator=(Ref<U>&&);

  friend inline bool operator==(const RefPtr& a, const RefPtr& b) {
    return a.instance_ == b.instance_;
  }

  friend inline bool operator!=(const RefPtr& a, const RefPtr& b) {
    return a.instance_ != b.instance_;
  }

  friend inline bool operator<(const RefPtr& a, const RefPtr& b) {
    return a.instance_ < b.instance_;
  }

  void Swap(RefPtr&);

private:
  T* instance_;
};

template<typename T>
template<typename U>
RefPtr<T>::RefPtr(Ref<U>&& reference)
  : instance_(&reference.LeakRef())
{
}

template<typename T>
RefPtr<T>& RefPtr<T>::operator=(const RefPtr& other)
{
  RefPtr ptr = other;
  Swap(ptr);
  return *this;
}

template<typename T>
template<typename U>
RefPtr<T>& RefPtr<T>::operator=(const RefPtr<U>& other)
{
  RefPtr ptr = other;
  Swap(ptr);
  return *this;
}

template<typename T>
RefPtr<T>& RefPtr<T>::operator=(T* object)
{
  RefPtr ptr = object;
  Swap(ptr);
  return *this;
}

template<typename T>
RefPtr<T>& RefPtr<T>::operator=(std::nullptr_t)
{
  T* old_instance = std::move(instance_);
  instance_ = std::forward<T*>(nullptr);
  if (old_instance)
    old_instance->Release();
  return *this;
}

template<typename T>
RefPtr<T>& RefPtr<T>::operator=(RefPtr&& other)
{
  RefPtr ptr = std::move(other);
  Swap(ptr);
  return *this;
}

template<typename T>
template<typename U>
RefPtr<T>& RefPtr<T>::operator=(RefPtr<U>&& other)
{
  RefPtr ptr = std::move(other);
  Swap(ptr);
  return *this;
}

template<typename T>
template<typename U>
RefPtr<T>& RefPtr<T>::operator=(Ref<U>&& other)
{
  RefPtr ptr = std::move(other);
  Swap(ptr);
  return *this;
}

template<typename T>
void RefPtr<T>::Swap(RefPtr& other)
{
  std::swap(instance_, other.instance_);
}

template<class T>
void Swap(RefPtr<T>& a, RefPtr<T>& b)
{
  a.Swap(b);
}

template<typename T, typename U>
bool operator==(const RefPtr<T>& a, const RefPtr<U>& b)
{
  return a.get() == b.get();
}

template<typename T, typename U>
bool operator==(const RefPtr<T>& a, U* b)
{
  return a.get() == b;
}

template<typename T, typename U>
bool operator==(T* a, const RefPtr<U>& b)
{
  return a == b.get();
}

template<typename T, typename U>
bool operator!=(const RefPtr<T>& a, const RefPtr<U>& b)
{
  return a.get() != b.get();
}

template<typename T, typename U>
bool operator!=(const RefPtr<T>& a, U* b)
{
  return a.get() != b;
}

template<typename T, typename U>
bool operator!=(T* a, const RefPtr<U>& b)
{
  return a != b.get();
}

}  // namespace ultralight
