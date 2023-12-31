//===- Core/Option/Cxx14Base.hpp ------------------------------------===//
//
// Copyright (C) 2023 Eightfold
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
//     limitations under the License.
//
//===----------------------------------------------------------------===//
//
//  This file implements the underlying object for the
//  pre-C++14 `Option<...>`. Based on the N3793 implementation.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_OPTION_CXX14BASE_HPP
#define EFL_CORE_OPTION_CXX14BASE_HPP

#include <cassert>
#include <efl/Core/_Fwd/Option.hpp>
#include <efl/Core/_Cxx11Assert.hpp>

#if CPPVER_MOST(14)
namespace efl {
namespace C {
namespace H {
/// Storage for trivial objects.
template <typename T, bool = 
  is_trivially_destructible<T>::value>
union OptionStorage {
  constexpr OptionStorage() NOEXCEPT : empty_() { }

  template <typename...TT>
  constexpr OptionStorage(in_place_t, TT&&...tt)
   : data_(cxpr_forward<TT>(tt)...) { }
  
  template <typename U, typename...TT>
  constexpr OptionStorage(InitList<U> il, TT&&...tt)
   : data_(il, cxpr_forward<TT>(tt)...) { }

public:
  Dummy empty_;
  T data_;
};

/// Storage for non-trivial objects.
template <typename T>
union OptionStorage<T, false> {
  constexpr OptionStorage() NOEXCEPT : empty_() { }
  ~OptionStorage() { }

  template <typename...TT>
  constexpr OptionStorage(in_place_t, TT&&...tt)
   : data_(cxpr_forward<TT>(tt)...) { }
  
  template <typename U, typename...TT>
  constexpr OptionStorage(InitList<U> il, TT&&...tt)
   : data_(il, cxpr_forward<TT>(tt)...) { }

public:
  Dummy empty_;
  T data_;
};

/// Base for trivial options.
template <typename T, bool = 
  is_trivially_destructible<T>::value>
struct OptionBase {
protected:
  using type_ = remove_const_t<T>;
public:
  OptionBase() = default;

  explicit constexpr OptionBase(const T& t) 
   : data_(in_place, t), active_(true) { }
  
  explicit constexpr OptionBase(T&& t) 
   : data_(in_place, cxpr_move(t)), active_(true) { }

  template <typename...TT>
  constexpr OptionBase(
    in_place_t ip, TT&&...tt)
   : data_(ip, cxpr_forward<TT>(tt)...), 
    active_(true) { }

  template <typename U, typename...TT>
  constexpr OptionBase(
    InitList<U> il, TT&&...tt)
   : data_(il, cxpr_forward<TT>(tt)...), 
    active_(true) { }
  
  ~OptionBase() = default;

public:
  ALWAYS_INLINE void destroy() NOEXCEPT { }
  void clear() NOEXCEPT { this->active_ = false; }

public:
  OptionStorage<type_, true> data_;
  bool active_ = false;
};

/// Base for non-trivial options.
template <typename T>
struct OptionBase<T, false> {
protected:
  using type_ = remove_const_t<T>;
public:
  OptionBase() = default;

  explicit constexpr OptionBase(const T& t) 
   : data_(in_place, t), active_(true) { }
  
  explicit constexpr OptionBase(T&& t) 
   : data_(in_place, cxpr_move(t)), active_(true) { }

  template <typename...TT>
  constexpr OptionBase(
    in_place_t ip, TT&&...tt)
   : data_(ip, cxpr_forward<TT>(tt)...), 
    active_(true) { }

  template <typename U, typename...TT>
  constexpr OptionBase(
    InitList<U> il, TT&&...tt)
   : data_(il, cxpr_forward<TT>(tt)...), 
    active_(true) { }
  
  ~OptionBase() {
    if(active_) 
      this->destroy();
  }

public:
  ALWAYS_INLINE void destroy() NOEXCEPT {
    data_.data_.type_::~type_();
  }

  void clear() NOEXCEPT {
    if(active_) this->destroy();
    this->active_ = false; 
  }

public:
  OptionStorage<type_, false> data_;
  bool active_ = false;
};
} // namespace H
} // namespace C
} // namespace efl

#endif // C++17 Check

#endif // EFL_CORE_OPTION_CXX14BASE_HPP
