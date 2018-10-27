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
 * Defensive traits
 *
 * Check all required type_traits and compose rules with them. Since
 * static_asserts cannot be in constexpr branches, the rules get a little iffy.
 * We need to make sure every rule only triggers in certain conditions, which is
 * unintuitive but required.
 */
namespace defensive_detail {
template <class T>
struct defensive {
	/**
	 * Rule of 5
	 *
	 * First we make sure all contructors are present, if not then bail out
	 * since you can't fulfill rule of 5 if you have some constructors missing.
	 * If all constructors are present, we check whether they are all trivial or
	 * all non-trivial.
	 * Due to a C++ bug, manually implementing a destructor
	 * will bypass checking the copy and move constructors. Nothing we can do
	 * about that :(
	 */
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

		// Rule of 5 pass.
		static constexpr bool rule_pass() {
			// If we don't have 5 constructors, don't trigger static_assert
			// for rule of 5 user defined constructors. That error will be
			// caught by another static assert.
			return !generated_ctors() || all_trivial() || all_non_trivial();
		}

		// Always silence specific error messages if the rule is passing.
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

	/**
	 * Fast Vector
	 *
	 * Ensures your class is optimized to be stored in a vector. Checks whether
	 * it is trivially destructible (skips destructor call on resize) and
	 * trivially copy constructible (use memcpy on resive). If not, falls back
	 * to ensure your class is noexcept move constructible (vector resize cannot
	 * use your move constructor if it isn't marked noexcept).
	 */
	struct fast_vector {
		static constexpr bool rule_pass() {
			return (trivially_copy_constructible && trivially_destructible)
					|| nothrow_move_constructible;
		}

		// Always silence specific error messages if the rule is passing.
		static constexpr bool trivial_dtor_ok() {
			// Only warn if type has trivial copy ctor
			return rule_pass() || !trivially_copy_constructible
					|| trivially_destructible;
		}
		static constexpr bool trivial_copy_ctor_ok() {
			// Only warn if type has trivial destructor
			return rule_pass() || !trivially_destructible
					|| trivially_copy_constructible;
		}
		static constexpr bool nothrow_move_ctor_ok() {
			return rule_pass() || trivially_destructible
					|| trivially_copy_constructible
					|| nothrow_move_constructible;
		}
	};

	/**
	 * Move Only
	 *
	 * Make sure an object is move only. There is no copy constructor/operator
	 * and the move constructor/operator is present.
	 */
	struct move_only {
		static constexpr bool rule_pass() {
			return !copy_constructible && !copy_assignable && move_constructible
					&& move_assignable;
		}

		// Always silence specific error messages if the rule is passing.
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

	// Object has no default constructor, destructor, copy constructor, move
	// constructor, copy operator, move operator.
	struct non_constructible {
		static constexpr bool rule_pass() {
			return !default_constructible && !destructible
					&& !copy_constructible && !move_constructible
					&& !copy_assignable && !move_assignable;
		}

		// Always silence specific error messages if the rule is passing.
		static constexpr bool ctor_ok() {
			return rule_pass() || !default_constructible;
		}
		static constexpr bool dtor_ok() {
			return rule_pass() || !destructible;
		}
		static constexpr bool copy_ctor_ok() {
			return rule_pass() || !copy_constructible;
		}
		static constexpr bool move_ctor_ok() {
			return rule_pass() || !move_constructible;
		}
		static constexpr bool copy_ass_ok() {
			return rule_pass() || !copy_assignable;
		}
		static constexpr bool move_ass_ok() {
			return rule_pass() || !move_assignable;
		}
	};

	// Required traits computed once.
	static constexpr bool default_constructible
			= std::is_default_constructible<T>::value;
	static constexpr bool trivially_default_constructible
			= std::is_trivially_default_constructible<T>::value;
	static constexpr bool destructible = std::is_destructible<T>::value;
	static constexpr bool trivially_destructible
			= std::is_trivially_destructible<T>::value;
	static constexpr bool copy_constructible
			= std::is_copy_constructible<T>::value;
	static constexpr bool trivially_copy_constructible
			= std::is_trivially_copy_constructible<T>::value;
	static constexpr bool move_constructible
			= std::is_move_constructible<T>::value;
	static constexpr bool trivially_move_constructible
			= std::is_trivially_move_constructible<T>::value;
	static constexpr bool nothrow_move_constructible
			= std::is_nothrow_move_constructible<T>::value;
	static constexpr bool copy_assignable = std::is_copy_assignable<T>::value;
	static constexpr bool trivially_copy_assignable
			= std::is_trivially_copy_assignable<T>::value;
	static constexpr bool move_assignable = std::is_move_assignable<T>::value;
	static constexpr bool trivially_move_assignable
			= std::is_trivially_move_assignable<T>::value;
};
} // namespace defensive_detail

/**
 * FEA_FULFULLS_5_CTORS
 *
 * Makes sure 5 constructors/operators are present (destructor, copy
 * constructor, move constructor, copy assignement constructor, move assignement
 * constructor). Useful when using rule of 0 or when using = default.
 *
 * ex.
 * struct p {};
 * FEA_FULFILLS_5_CTORS(p);
 */
#define FEA_FULFILLS_5_CTORS(t) \
	static_assert(defensive_detail::defensive<t>::five::generated_ctors(), \
			#t " : requires destructor, copy and move constructor, copy and " \
			   "move assignement operator"); \
	static_assert(defensive_detail::defensive<t>::destructible, \
			" - " #t " : must be destructible"); \
	static_assert(defensive_detail::defensive<t>::copy_constructible, \
			" - " #t " : must be copy constructible"); \
	static_assert(defensive_detail::defensive<t>::move_constructible, \
			" - " #t " : must be move constructible"); \
	static_assert(defensive_detail::defensive<t>::copy_assignable, \
			" - " #t " : must be copy assignable"); \
	static_assert(defensive_detail::defensive<t>::move_assignable, \
			" - " #t " : must be move assignable")

/**
 * FEA_FULFULLS_RULE_OF_5
 *
 * Makes sure a class fulfulls Rule of 5. All 5 constructors/operators are
 * present (destructor, copy constructor, move constructor, copy assignement
 * operator, move assignement operator). If you implement 1 custom
 * constructor/operator, you probably need to implement all of them.
 *
 * ex.
 * struct p {};
 * FEA_FULFILLS_RULE_OF_5(p);
 */
#define FEA_FULFILLS_RULE_OF_5(t) \
	FEA_FULFILLS_5_CTORS(t); \
	static_assert(defensive_detail::defensive<t>::five::rule_pass(), \
			#t " : doesn't fulfill rule of 5"); \
	static_assert(defensive_detail::defensive<t>::five::user_dtor_ok(), \
			" - " #t " : must implement user-defined destructor"); \
	static_assert(defensive_detail::defensive<t>::five::user_copy_ctor_ok(), \
			" - " #t " : must implement user-defined copy constructor"); \
	static_assert(defensive_detail::defensive<t>::five::user_move_ctor_ok(), \
			" - " #t " : must implement user-defined move constructor"); \
	static_assert(defensive_detail::defensive<t>::five::user_copy_ass_ok(), \
			" - " #t \
			" : must implement user-defined copy assignement operator"); \
	static_assert(defensive_detail::defensive<t>::five::user_move_ass_ok(), \
			" - " #t \
			" : must implement user-defined move assignement operator")

/**
 * FEA_FULFULLS_6_CTORS
 *
 * Make sure all 6 constructors/operators are
 * present (default, destructor, copy constructor, move constructor, copy
 * assignement operator, move assignement operator). Useful when using rule of 0
 * or = default.
 *
 * ex.
 * struct p {};
 * FEA_FULFILLS_6_CTORS(p);
 */
#define FEA_FULFILLS_6_CTORS(t) \
	static_assert(defensive_detail::defensive<t>::six::generated_ctors(), \
			#t " : requires default constructor, destructor, copy and move " \
			   "constructor, copy and move assignement operator"); \
	static_assert(defensive_detail::defensive<t>::default_constructible, \
			" - " #t " : must be default constructible"); \
	static_assert(defensive_detail::defensive<t>::destructible, \
			" - " #t " : must be destructible"); \
	static_assert(defensive_detail::defensive<t>::copy_constructible, \
			" - " #t " : must be copy constructible"); \
	static_assert(defensive_detail::defensive<t>::move_constructible, \
			" - " #t " : must be move constructible"); \
	static_assert(defensive_detail::defensive<t>::copy_assignable, \
			" - " #t " : must be copy assignable"); \
	static_assert(defensive_detail::defensive<t>::move_assignable, \
			" - " #t " : must be move assignable")

/**
 * FEA_FULFULLS_RULE_OF_6
 *
 * Rule of 5 with an extra check to make sure your class has a default
 * constructor. All 6 constructors/operators are present (default constructor,
 * destructor, copy constructor, move constructor, copy assignement operator,
 * move assignement operator). If you implement 1 of 5 custom
 * constructor/operator, you probably need to implement all of them (destructor,
 * copy constructor, move constructor, copy assignement operator, move
 * assignement operator).
 *
 * ex.
 * struct p {};
 * FEA_FULFILLS_RULE_OF_6(p);
 */
#define FEA_FULFILLS_RULE_OF_6(t) \
	FEA_FULFILLS_6_CTORS(t); \
	static_assert(defensive_detail::defensive<t>::five::rule_pass(), \
			#t " : doesn't fulfill rule of 5"); \
	static_assert(defensive_detail::defensive<t>::five::user_dtor_ok(), \
			" - " #t " : must implement user-defined destructor"); \
	static_assert(defensive_detail::defensive<t>::five::user_copy_ctor_ok(), \
			" - " #t " : must implement user-defined copy constructor"); \
	static_assert(defensive_detail::defensive<t>::five::user_move_ctor_ok(), \
			" - " #t " : must implement user-defined move constructor"); \
	static_assert(defensive_detail::defensive<t>::five::user_copy_ass_ok(), \
			" - " #t \
			" : must implement user-defined copy assignement operator"); \
	static_assert(defensive_detail::defensive<t>::five::user_move_ass_ok(), \
			" - " #t \
			" : must implement user-defined move assignement operator")

/**
 * Fast Vector
 *
 * Ensures your class is optimized to be stored in a vector. Checks whether
 * it is trivially destructible (skips destructor call on resize) and
 * trivially copy constructible (use memcpy on resive). If not, falls back
 * to ensure your class is noexcept move constructible (vector resize cannot
 * use your move constructor if it isn't marked noexcept).
 *
 * ex.
 * struct p {};
 * FEA_FULFILLS_FAST_VECTOR(p);
 */
#define FEA_FULFILLS_FAST_VECTOR(t) \
	static_assert(defensive_detail::defensive<t>::fast_vector::rule_pass(), \
			#t " : doesn't fulfill fast vector requirements"); \
	static_assert( \
			defensive_detail::defensive<t>::fast_vector::trivial_dtor_ok(), \
			" - " #t " : must generate trivial destructor"); \
	static_assert(defensive_detail::defensive< \
						  t>::fast_vector::trivial_copy_ctor_ok(), \
			" - " #t " : must generate trivial copy constructor"); \
	static_assert(defensive_detail::defensive< \
						  t>::fast_vector::nothrow_move_ctor_ok(), \
			" - " #t " : must implement either trivial destructor and " \
			"trivial copy constructor, or noexcept move constructor")

/**
 * Move Only
 *
 * Ensures your class is move only. There is no copy constructor and no copy
 * assignement operator. The move constructor and move assignement operator is
 * present.
 *
 * ex.
 * struct p {};
 * FEA_FULFILLS_MOVE_ONLY(p);
 */
#define FEA_FULFILLS_MOVE_ONLY(t) \
	static_assert(defensive_detail::defensive<t>::move_only::rule_pass(), \
			#t " : doesn't fulfill move only"); \
	static_assert(defensive_detail::defensive<t>::move_only::copy_ctor_ok(), \
			" - " #t " : must not declare copy constructor"); \
	static_assert(defensive_detail::defensive<t>::move_only::move_ctor_ok(), \
			" - " #t " : must declare move constructor"); \
	static_assert(defensive_detail::defensive<t>::move_only::copy_ass_ok(), \
			" - " #t " : must not declare copy assignement operator"); \
	static_assert(defensive_detail::defensive<t>::move_only::move_ass_ok(), \
			" - " #t " : must declare move assignement operator")

/**
 * Non constructible
 *
 * Ensures your class is non constructible, aka had no default constructor,
 * destructor, copy constructor, move constructor, copy assignement operator and
 * move assignement operator. Useful when writing static singleton classes.
 *
 * ex.
 * struct p {};
 * FEA_FULFILLS_NON_CONSTRUCTIBLE(p);
 */
#define FEA_FULFILLS_NON_CONSTRUCTIBLE(t) \
	static_assert( \
			defensive_detail::defensive<t>::non_constructible::rule_pass(), \
			#t " : doesn't fulfill non-constructible"); \
	static_assert( \
			defensive_detail::defensive<t>::non_constructible::ctor_ok(), \
			" - " #t " : must not declare default constructor"); \
	static_assert( \
			defensive_detail::defensive<t>::non_constructible::dtor_ok(), \
			" - " #t " : must not declare destructor"); \
	static_assert( \
			defensive_detail::defensive<t>::non_constructible::copy_ctor_ok(), \
			" - " #t " : must not declare copy constructor"); \
	static_assert( \
			defensive_detail::defensive<t>::non_constructible::move_ctor_ok(), \
			" - " #t " : must not declare move constructor"); \
	static_assert( \
			defensive_detail::defensive<t>::non_constructible::copy_ass_ok(), \
			" - " #t " : must not declare copy assignement operator"); \
	static_assert( \
			defensive_detail::defensive<t>::non_constructible::move_ass_ok(), \
			" - " #t " : must not declare move assignement operator")
