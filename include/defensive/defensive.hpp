/*
BSD 3-Clause License

Copyright (c) 2018, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include <type_traits>

/**
 * Rule of 5
 */
namespace detail {
template <class T>
struct defensive {
	// Rule of 5
	struct five {
		static constexpr bool generated_ctors() {
			return destructible && copy_constructible && move_constructible
					&& copy_assignable && move_assignable;
		}
		static constexpr bool all_trivial() {
			return trivially_destructible && trivially_copy_constructible
					&& trivially_move_constructible && trivially_copy_assignable
					&& trivially_move_assignable;
		}
		static constexpr bool all_non_trivial() {
			return !trivially_destructible && !trivially_copy_constructible
					&& !trivially_move_constructible
					&& !trivially_copy_assignable && !trivially_move_assignable;
		}
		static constexpr bool rule_pass() {
			if constexpr (!generated_ctors()) {
				// If we don't have 5 constructors, don't trigger static_assert
				// for rule of 5 user defined constructors.
				return true;
			} else {
				return all_trivial() || all_non_trivial();
			}
		}
		static constexpr bool user_dtor_ok() {
			return rule_pass() || !trivially_destructible;
		}
		static constexpr bool user_copy_ctor_ok() {
			return rule_pass() || !trivially_copy_constructible;
		}
		static constexpr bool user_move_ctor_ok() {
			return rule_pass() || !trivially_move_constructible;
		}
		static constexpr bool user_copy_ass_ok() {
			return rule_pass() || !trivially_copy_assignable;
		}
		static constexpr bool user_move_ass_ok() {
			return rule_pass() || !trivially_move_assignable;
		}
	};

	// Rule of 5 + require default constructor.
	struct six {
		static constexpr bool generated_ctors() {
			return default_constructible && five::generated_ctors();
		}
	};

	struct fast_vector {
		static constexpr bool rule_pass() {
			return (trivially_copy_constructible && trivially_destructible)
					|| nothrow_move_constructible;
		}
		static constexpr bool trivial_dtor_ok() {
			if constexpr (rule_pass()) {
				return true;
			} else if constexpr (!trivially_copy_constructible) {
				// Only warn if type has trivial copy ctor
				return true;
			} else {
				return trivially_destructible;
			}
		}
		static constexpr bool trivial_copy_ctor_ok() {
			if constexpr (rule_pass()) {
				return true;
			} else if constexpr (!trivially_destructible) {
				// Only warn if type has trivial destructor
				return true;
			} else {
				return trivially_copy_constructible;
			}
		}
		static constexpr bool nothrow_move_ctor_ok() {
			if constexpr (rule_pass()) {
				return true;
			} else if constexpr (!trivially_destructible
					&& !trivially_copy_constructible) {
				// Fallback to less strict noexcept move ctor
				return nothrow_move_constructible;
			} else {
				return true;
			}
		}
	};

	struct move_only {
		static constexpr bool rule_pass() {
			return !copy_constructible && !copy_assignable && move_constructible
					&& move_assignable;
		}
		static constexpr bool copy_ctor_ok() {
			return rule_pass() || !copy_constructible;
		}
		static constexpr bool move_ctor_ok() {
			return rule_pass() || move_constructible;
		}
		static constexpr bool copy_ass_ok() {
			return rule_pass() || !copy_assignable;
		}
		static constexpr bool move_ass_ok() {
			return rule_pass() || move_assignable;
		}
	};

	static constexpr bool default_constructible
			= std::is_default_constructible_v<T>;
	static constexpr bool trivially_default_constructible
			= std::is_trivially_default_constructible_v<T>;
	static constexpr bool destructible = std::is_destructible_v<T>;
	static constexpr bool trivially_destructible
			= std::is_trivially_destructible_v<T>;
	static constexpr bool copy_constructible = std::is_copy_constructible_v<T>;
	static constexpr bool trivially_copy_constructible
			= std::is_trivially_copy_constructible_v<T>;
	static constexpr bool move_constructible = std::is_move_constructible_v<T>;
	static constexpr bool trivially_move_constructible
			= std::is_trivially_move_constructible_v<T>;
	static constexpr bool nothrow_move_constructible
			= std::is_nothrow_move_constructible_v<T>;
	static constexpr bool copy_assignable = std::is_copy_assignable_v<T>;
	static constexpr bool trivially_copy_assignable
			= std::is_trivially_copy_assignable_v<T>;
	static constexpr bool move_assignable = std::is_move_assignable_v<T>;
	static constexpr bool trivially_move_assignable
			= std::is_trivially_move_assignable_v<T>;
};
} // namespace detail

#define FEA_FULFILLS_5_CTORS(t) \
	static_assert(detail::defensive<t>::five::generated_ctors(), \
			#t " : requires destructor, copy and move constructor, copy and " \
			   "move assignement operator"); \
	static_assert(detail::defensive<t>::destructible, \
			" - " #t " : must be destructible"); \
	static_assert(detail::defensive<t>::copy_constructible, \
			" - " #t " : must be copy constructible"); \
	static_assert(detail::defensive<t>::move_constructible, \
			" - " #t " : must be move constructible"); \
	static_assert(detail::defensive<t>::copy_assignable, \
			" - " #t " : must be copy assignable"); \
	static_assert(detail::defensive<t>::move_assignable, \
			" - " #t " : must be move assignable")

#define FEA_FULFILLS_RULE_OF_5(t) \
	FEA_FULFILLS_5_CTORS(t); \
	static_assert(detail::defensive<t>::five::rule_pass(), \
			#t " : doesn't fulfill rule of 5"); \
	static_assert(detail::defensive<t>::five::user_dtor_ok(), \
			" - " #t " : must implement user-defined destructor"); \
	static_assert(detail::defensive<t>::five::user_copy_ctor_ok(), \
			" - " #t " : must implement user-defined copy constructor"); \
	static_assert(detail::defensive<t>::five::user_move_ctor_ok(), \
			" - " #t " : must implement user-defined move constructor"); \
	static_assert(detail::defensive<t>::five::user_copy_ass_ok(), \
			" - " #t \
			" : must implement user-defined copy assignement operator"); \
	static_assert(detail::defensive<t>::five::user_move_ass_ok(), \
			" - " #t \
			" : must implement user-defined move assignement operator")

#define FEA_FULFILLS_6_CTORS(t) \
	static_assert(detail::defensive<t>::six::generated_ctors(), \
			#t " : requires default constructor, destructor, copy and move " \
			   "constructor, copy and move assignement operator"); \
	static_assert(detail::defensive<t>::default_constructible, \
			" - " #t " : must be default constructible"); \
	static_assert(detail::defensive<t>::destructible, \
			" - " #t " : must be destructible"); \
	static_assert(detail::defensive<t>::copy_constructible, \
			" - " #t " : must be copy constructible"); \
	static_assert(detail::defensive<t>::move_constructible, \
			" - " #t " : must be move constructible"); \
	static_assert(detail::defensive<t>::copy_assignable, \
			" - " #t " : must be copy assignable"); \
	static_assert(detail::defensive<t>::move_assignable, \
			" - " #t " : must be move assignable")

#define FEA_FULFILLS_RULE_OF_6(t) \
	FEA_FULFILLS_6_CTORS(t); \
	static_assert(detail::defensive<t>::five::rule_pass(), \
			#t " : doesn't fulfill rule of 5"); \
	static_assert(detail::defensive<t>::five::user_dtor_ok(), \
			" - " #t " : must implement user-defined destructor"); \
	static_assert(detail::defensive<t>::five::user_copy_ctor_ok(), \
			" - " #t " : must implement user-defined copy constructor"); \
	static_assert(detail::defensive<t>::five::user_move_ctor_ok(), \
			" - " #t " : must implement user-defined move constructor"); \
	static_assert(detail::defensive<t>::five::user_copy_ass_ok(), \
			" - " #t \
			" : must implement user-defined copy assignement operator"); \
	static_assert(detail::defensive<t>::five::user_move_ass_ok(), \
			" - " #t \
			" : must implement user-defined move assignement operator")

#define FEA_FULFILLS_FAST_VECTOR(t) \
	static_assert(detail::defensive<t>::fast_vector::rule_pass(), \
			#t " : doesn't fulfill fast vector requirements"); \
	static_assert(detail::defensive<t>::fast_vector::trivial_dtor_ok(), \
			" - " #t " : must generate trivial destructor"); \
	static_assert(detail::defensive<t>::fast_vector::trivial_copy_ctor_ok(), \
			" - " #t " : must generate trivial copy constructor"); \
	static_assert(detail::defensive<t>::fast_vector::nothrow_move_ctor_ok(), \
			" - " #t " : must implement either trivial destructor and " \
			"trivial copy constructor, or noexcept move constructor")

#define FEA_FULFILLS_MOVE_ONLY(t) \
	static_assert(detail::defensive<t>::move_only::rule_pass(), \
			#t " : doesn't fulfill move only"); \
	static_assert(detail::defensive<t>::move_only::copy_ctor_ok(), \
			" - " #t " : must not declare copy constructor"); \
	static_assert(detail::defensive<t>::move_only::move_ctor_ok(), \
			" - " #t " : must declare move constructor"); \
	static_assert(detail::defensive<t>::move_only::copy_ass_ok(), \
			" - " #t " : must not declare copy assignement operator"); \
	static_assert(detail::defensive<t>::move_only::move_ass_ok(), \
			" - " #t " : must declare move assignement operator")
