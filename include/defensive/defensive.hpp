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

/*
#define FEA_DEFAULT_CONSTRUCTIBLE(t) \
	[]() { \
		static_assert(std::is_default_constructible_v<t>, \
				#t " : must be default constructible"); \
		return std::is_default_constructible_v<t>; \
	}

#define FEA_TRIVIALLY_DEFAULT_CONSTRUCTIBLE(t) \
	[]() { \
		static_assert(std::is_trivially_default_constructible_v<t>, \
				#t " : must be trivially default constructible"); \
		return std::std::is_trivially_default_constructible_v<t>; \
	}

#define FEA_COPY_CONSTRUCTIBLE(t) \
	[]() { \
		static_assert(std::is_copy_constructible_v<t>, \
				#t " : must be copy constructible"); \
		return std::is_copy_constructible_v<t>; \
	}

#define FEA_NOT_COPY_CONSTRUCTIBLE(t) \
	[]() { \
		static_assert(!std::is_copy_constructible_v<t>, \
				#t " : must not be copy constructible"); \
		return !std::is_copy_constructible_v<t>; \
	}

#define FEA_TRIVIALLY_COPY_CONSTRUCTIBLE(t) \
	[]() { \
		static_assert(std::is_trivially_copy_constructible_v<t>, \
				#t " : must be trivially copy constructible"); \
		return std::is_trivially_copy_constructible_v<t>; \
	}

#define FEA_COPY_ASSIGNABLE(t) \
	[]() { \
		static_assert(std::is_copy_assignable_v<t>, \
				#t " : must be copy assignable"); \
		return std::is_copy_assignable_v<t>; \
	}

#define FEA_NOT_COPY_ASSIGNABLE(t) \
	[]() { \
		static_assert(!std::is_copy_assignable_v<t>, \
				#t " : must not be copy assignable"); \
		return !std::is_copy_assignable_v<t>; \
	}

#define FEA_TRIVIALLY_COPY_ASSIGNABLE(t) \
	[]() { \
		static_assert(std::is_trivially_copy_assignable_v<t>, \
				#t " : must be trivially copy assignable"); \
		return std::is_trivially_copy_assignable_v<t>; \
	}

#define FEA_MOVE_CONSTRUCTIBLE(t) \
	[]() { \
		static_assert(std::is_move_constructible_v<t>, \
				#t " : must be move constructible"); \
		return std::is_move_constructible_v<t>; \
	}

#define FEA_TRIVIALLY_MOVE_CONSTRUCTIBLE(t) \
	[]() { \
		static_assert(std::is_trivially_move_constructible_v<t>, \
				#t " : must be trivially move constructible"); \
		return std::is_trivially_move_constructible_v<t>; \
	}

#define FEA_NOTHROW_MOVE_CONSTRUCTIBLE(t) \
	[]() { \
		static_assert(std::is_nothrow_move_constructible_v<t>, \
				#t " : must be nothrow move constructible"); \
		return std::is_nothrow_move_constructible_v<t>; \
	}

#define FEA_MOVE_ASSIGNABLE(t) \
	[]() { \
		static_assert(std::is_move_assignable_v<t>, \
				#t " : must be move assignable"); \
		return std::is_move_assignable_v<t>; \
	}

#define FEA_TRIVIALLY_MOVE_ASSIGNABLE(t) \
	[]() { \
		static_assert(std::is_trivially_move_assignable_v<t>, \
				#t " : must be trivially move assignable"); \
		return std::is_trivially_move_assignable_v<t>; \
	}

#define FEA_DESTRUCTIBLE(t) \
	[]() { \
		static_assert( \
				std::is_destructible_v<t>, #t " : must be destructible"); \
		return std::is_destructible_v<t>; \
	}

#define FEA_TRIVIALLY_DESTRUCTIBLE(t) \
	[]() { \
		static_assert(std::is_trivially_destructible_v<t>, \
				#t " : must be trivially destructible"); \
		return std::is_trivially_destructible_v<t>; \
	}

#define FEA_TRIVIALLY_COPYABLE(t) \
	[]() { \
		static_assert(std::is_trivially_copyable_v<t>, \
				#t " : must be trivially copyable"); \
		return std::is_trivially_copyable_v<t>; \
	}

#define FEA_FULFILLS_RULE_OF_5(t) \
	static_assert(FEA_DESTRUCTIBLE(t)() && FEA_COPY_CONSTRUCTIBLE(t)() \
					&& FEA_MOVE_CONSTRUCTIBLE(t)() && FEA_COPY_ASSIGNABLE(t)() \
					&& FEA_MOVE_ASSIGNABLE(t)(), \
			#t " : doesn't fulfill rule of 5")

#define FEA_FULFILLS_RULE_OF_6(t) \
	static_assert(FEA_DESTRUCTIBLE(t)() && FEA_DEFAULT_CONSTRUCTIBLE(t)() \
					&& FEA_COPY_CONSTRUCTIBLE(t)() \
					&& FEA_MOVE_CONSTRUCTIBLE(t)() && FEA_COPY_ASSIGNABLE(t)() \
					&& FEA_MOVE_ASSIGNABLE(t)(), \
			#t " : doesn't fulfill rule of 5")

// is_trivially_copyable broken everywhere
#define FEA_FULFILLS_FAST_VECTOR(t) \
	static_assert((std::is_trivially_copy_constructible_v< \
						   t> && std::is_trivially_destructible_v<t>) \
					|| std::is_nothrow_move_constructible_v<t>, \
			#t " : doesn't fulfill fast vector (trivially copy constructible " \
			   "and trivially destructible, or nothrow move constructible)")

#define FEA_FULFILLS_MOVE_ONLY(t) \
	static_assert(FEA_NOT_COPY_CONSTRUCTIBLE(t)() \
					&& FEA_MOVE_CONSTRUCTIBLE(t)() \
					&& FEA_NOT_COPY_ASSIGNABLE(t)() \
					&& FEA_MOVE_ASSIGNABLE(t)(), \
			#t " : doesn't fulfill move only")

*/

// Rule of 5
#define FEA_ASSERT_DESTRUCTIBLE(t) \
	static_assert(std::is_destructible_v<t>, #t " : must be destructible")

#define FEA_ASSERT_COPY_CONSTRUCTIBLE(t) \
	static_assert(std::is_copy_constructible_v<t>, \
			#t " : must be copy constructible")

#define FEA_ASSERT_MOVE_CONSTRUCTIBLE(t) \
	static_assert(std::is_move_constructible_v<t>, \
			#t " : must be move constructible")

#define FEA_ASSERT_COPY_ASSIGNABLE(t) \
	static_assert(std::is_copy_assignable_v<t>, #t " : must be copy assignable")

#define FEA_ASSERT_MOVE_ASSIGNABLE(t) \
	static_assert(std::is_move_assignable_v<t>, #t " : must be move assignable")

#define FEA_CREATE_GENERATED_5_ALL_CHECK(t) \
	namespace detail { \
	inline constexpr bool assert_generated_5_##t() { \
		constexpr bool generated_5 \
				= std::is_destructible_v< \
						  t> && std::is_copy_constructible_v<t> && std::is_move_constructible_v<t> && std::is_copy_assignable_v<t> && std::is_move_assignable_v<t>; \
		static_assert(generated_5, \
				#t \
				" : doesn't implement required 5 constructors and operators " \
				"(dtor, copy ctor, move ctor, copy assignement, move " \
				"assignement)"); \
		return generated_5; \
	} \
	}

#define FEA_CREATE_GENERATED_5_INDIVUAL_CHECKS(t) \
	namespace detail { \
	inline constexpr void assert_individual_generated_5_##t( \
			bool everything_ok) { \
		if constexpr (!everything_ok) { \
			FEA_ASSERT_DESTRUCTIBLE(t); \
			FEA_ASSERT_COPY_CONSTRUCTIBLE(t); \
			FEA_ASSERT_MOVE_CONSTRUCTIBLE(t); \
			FEA_ASSERT_COPY_ASSIGNABLE(t); \
			FEA_ASSERT_MOVE_ASSIGNABLE(t); \
		} \
	} \
	}


#define FEA_GENERATED_5_CTORS(t) \
	FEA_CREATE_GENERATED_5_ALL_CHECK(t) \
	FEA_CREATE_GENERATED_5_INDIVIDUAL_CHECKS(t)
//if constexpr (!detail::assert_generated_5_##t()) { \
	//	FEA_ASSERT_DESTRUCTIBLE(t); \
	//	FEA_ASSERT_COPY_CONSTRUCTIBLE(t); \
	//	FEA_ASSERT_MOVE_CONSTRUCTIBLE(t); \
	//	FEA_ASSERT_COPY_ASSIGNABLE(t); \
	//	FEA_ASSERT_MOVE_ASSIGNABLE(t); \
	//}

//#define FEA_GENERATED_5_CTORS(t)
// static_assert(FEA_GENERATED_5_CTORS_IMPL(t)())
