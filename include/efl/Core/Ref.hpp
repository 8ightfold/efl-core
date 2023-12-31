//===- Core/Ref.hpp -------------------------------------------------===//
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
//  This file implements a reference_wrapper-like type, 
//  allowing for rebindable references.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_REF_HPP
#define EFL_CORE_REF_HPP

#include "Fundamental.hpp"
#include "Traits.hpp"

namespace efl {
namespace C {
namespace H {
namespace ref_ {
  // Helper function for `Ref`.
  template <typename T>
  FICONSTEXPR T& ref_hcast(T& t) { return t; }

  template <typename T>
  void ref_hcast(T&& t) = delete;
} // namespace ref_
} // namespace H

template <typename T>
struct Ref {
  using type = T;
  Ref() = delete;

  template <typename U, typename = decltype(
    H::ref_::ref_hcast<T>(std::declval<U>())),
    MEflEnableIf(!std::is_same<Ref, remove_cvref_t<U>>::value)>
  constexpr Ref(U&& u) noexcept(
   noexcept(H::ref_::ref_hcast<T>(H::cxpr_forward<U>(u)))) 
   : data_(X11::addressof(
    H::ref_::ref_hcast(H::cxpr_forward<U>(u)))) { }
  
  Ref(const Ref&) NOEXCEPT = default;
  Ref& operator=(const Ref& lhs) NOEXCEPT = default;

  constexpr operator T&() const NOEXCEPT { return *data_; }
  constexpr T& operator*() const NOEXCEPT { return *data_; }
  constexpr T& get() const NOEXCEPT { return *data_; }

  template <typename...Args>
  constexpr invoke_result_t<T&, Args...>
   operator()(Args&&... args) const noexcept(
    noexcept(H::invoke(H::Decl<T&>(), 
     H::cxpr_forward<Args>(args)...))) 
  {
    return H::invoke(get(), 
      H::cxpr_forward<Args>(args)...);
  }

private:
  T* data_;
};

#if __cpp_deduction_guides >= 201907L
template <typename T>
Ref(T&) -> Ref<T>;
#endif // Deduction guides (C++17)
} // namespace C
} // namespace efl

#undef EFLI_REFMUTCXPR_

#endif // EFL_CORE_REF_HPP
