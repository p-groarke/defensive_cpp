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
struct trivial_checker {
	static constexpr bool all_trivial() {
		return trivially_destructible && trivially_copy_constructible
				&& trivially_move_constructible && trivially_copy_assignable
				&& trivially_move_assignable;
	}
	static constexpr bool all_non_trivial() {
		return !trivially_destructible && !trivially_copy_constructible
				&& !trivially_move_constructible && !trivially_copy_assignable
				&& !trivially_move_assignable;
	}
	static constexpr bool trivially_destructible
			= std::is_trivially_destructible_v<T>;
	static constexpr bool trivially_copy_constructible
			= std::is_trivially_copy_constructible_v<T>;
	static constexpr bool trivially_move_constructible
			= std::is_trivially_move_constructible_v<T>;
	static constexpr bool trivially_copy_assignable
			= std::is_trivially_copy_assignable_v<T>;
	static constexpr bool trivially_move_assignable
			= std::is_trivially_move_assignable_v<T>;
};
} // namespace detail

#define FEA_ASSERT_DESTRUCTIBLE(t) \
	static_assert(std::is_destructible_v<t>, " - " #t " : must be destructible")

#define FEA_ASSERT_NOT_TRIVIALLY_DESTRUCTIBLE(t) \
	static_assert(!std::is_trivially_destructible_v<t>, \
			" - " #t " : must declare destructor manually")

#define FEA_ASSERT_COPY_CONSTRUCTIBLE(t) \
	static_assert(std::is_copy_constructible_v<t>, \
			" - " #t " : must be copy constructible")

#define FEA_ASSERT_NOT_TRIVIALLY_COPY_CONSTRUCTIBLE(t) \
	static_assert(!std::is_trivially_copy_constructible_v<t>, \
			" - " #t " : must declare copy constructor manually")

#define FEA_ASSERT_MOVE_CONSTRUCTIBLE(t) \
	static_assert(std::is_move_constructible_v<t>, \
			" - " #t " : must be move constructible")

#define FEA_ASSERT_NOT_TRIVIALLY_MOVE_CONSTRUCTIBLE(t) \
	static_assert(!std::is_trivially_move_constructible_v<t>, \
			" - " #t " : must declare move constructor manually")

#define FEA_ASSERT_COPY_ASSIGNABLE(t) \
	static_assert(std::is_copy_assignable_v<t>, \
			" - " #t " : must be copy assignable")

#define FEA_ASSERT_NOT_TRIVIALLY_COPY_ASSIGNABLE(t) \
	static_assert(!std::is_trivially_copy_assignable_v<t>, \
			" - " #t " : must declare copy assignement operator manually")

#define FEA_ASSERT_MOVE_ASSIGNABLE(t) \
	static_assert(std::is_move_assignable_v<t>, \
			" - " #t " : must be move assignable")

#define FEA_ASSERT_NOT_TRIVIALLY_MOVE_ASSIGNABLE(t) \
	static_assert(!std::is_trivially_move_assignable_v<t>, \
			" - " #t " : must declare move assignement operator manually")

#define FEA_GENERATED_5_IMPL(t) \
	std::is_destructible_v<t>&& std::is_copy_constructible_v<t>&& \
			std::is_move_constructible_v<t>&& std::is_copy_assignable_v<t>&& \
					std::is_move_assignable_v<t>

#define FEA_FULFILLS_5_CTORS_IMPL_ALL(t) \
	[]() constexpr { \
		constexpr bool generated_5 = FEA_GENERATED_5_IMPL(t); \
		static_assert(generated_5, \
				#t " : doesn't declare all 5 constructors and operators"); \
		return generated_5; \
	}

#define FEA_FULFILLS_5_CTORS_IMPL(t) \
	[]() constexpr { \
		constexpr bool generated_5 = FEA_GENERATED_5_IMPL(t); \
		if constexpr (!FEA_FULFILLS_5_CTORS_IMPL_ALL(t)()) { \
			FEA_ASSERT_DESTRUCTIBLE(t); \
			FEA_ASSERT_COPY_CONSTRUCTIBLE(t); \
			FEA_ASSERT_MOVE_CONSTRUCTIBLE(t); \
			FEA_ASSERT_COPY_ASSIGNABLE(t); \
			FEA_ASSERT_MOVE_ASSIGNABLE(t); \
		} \
		return generated_5; \
	}

#define FEA_FULFILLS_5_CTORS_IMPL_TRUE(t) \
	[]() constexpr { \
		if constexpr (!FEA_FULFILLS_5_CTORS_IMPL_ALL(t)()) { \
			FEA_ASSERT_DESTRUCTIBLE(t); \
			FEA_ASSERT_COPY_CONSTRUCTIBLE(t); \
			FEA_ASSERT_MOVE_CONSTRUCTIBLE(t); \
			FEA_ASSERT_COPY_ASSIGNABLE(t); \
			FEA_ASSERT_MOVE_ASSIGNABLE(t); \
		} \
		return true; \
	}


#define FEA_FULFILLS_5_CTORS(t) \
	static_assert(FEA_FULFILLS_5_CTORS_IMPL_TRUE(t)(), "")

#define FEA_FULFILLS_RULE_OF_5_IMPL(t) \
	[]() constexpr { \
		if constexpr (FEA_FULFILLS_5_CTORS_IMPL(t)() \
				&& !(detail::trivial_checker<t>::all_trivial() \
						   || detail::trivial_checker< \
									  t>::all_non_trivial())) { \
			FEA_ASSERT_NOT_TRIVIALLY_DESTRUCTIBLE(t); \
			FEA_ASSERT_NOT_TRIVIALLY_COPY_CONSTRUCTIBLE(t); \
			FEA_ASSERT_NOT_TRIVIALLY_MOVE_CONSTRUCTIBLE(t); \
			FEA_ASSERT_NOT_TRIVIALLY_COPY_ASSIGNABLE(t); \
			FEA_ASSERT_NOT_TRIVIALLY_MOVE_ASSIGNABLE(t); \
		} \
		return !(FEA_GENERATED_5_IMPL(t) \
				&& !(detail::trivial_checker<t>::all_trivial() \
						   || detail::trivial_checker<t>::all_non_trivial())); \
	}

#define FEA_FULFILLS_RULE_OF_5(t) \
	static_assert(FEA_FULFILLS_RULE_OF_5_IMPL(t)(), \
			#t " : doesn't fulfill rule of 5")
