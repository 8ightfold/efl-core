//===- Core/Unwrap.hpp ----------------------------------------------===//
//
// Copyright (C) 2024 Eightfold
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
//  This file implements a utility macro for unwrapping values.
//  Doesn't work on MSVC at the moment, but I will implement a 
//  plugin for it at some point...maybe.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_UNWRAP_HPP
#define EFL_CORE_UNWRAP_HPP

#include "Traits.hpp"
#include "Result.hpp"
#include "_Builtins.hpp"

// TODO: Implement fallback for MSVC, add plugin support?

#ifndef COMPILER_MSVC
# define $unwrap(val, ...) \
 ({ if(!static_cast<bool>(val)) return { __VA_ARGS__ }; \
    ::efl::unwrap(val); })
#endif

#ifndef COMPILER_MSVC
# define $assign_unwrap(name, ...) \
 auto&& name = $unwrap(val, __VA_ARGS__)
#endif

namespace efl {
/// @brief Allows for generic unwrapping via ADL.
/// @param t The value to unwrap, assumed to be valid.
/// @return The unwrapped value.
template <typename T>
constexpr auto unwrap(T&& t) 
 -> decltype(do_unwrap(CH::Decl<T>())) {
  return do_unwrap(CH::cxpr_forward<T>(t));
}

} // namespace efl

#endif // EFL_CORE_UNWRAP_HPP
