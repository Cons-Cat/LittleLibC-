// -*- mode: c++ -*-
// vim: set ft=cpp:
#pragma once

#include <cat/simd>
#include <cat/string>

// This function requires SSE4.2, unless it is used in a `constexpr` context.
constexpr auto cat::string_length(char const* p_string) -> ssize {
    if (::cat::is_constant_evaluated()) {
        ssize result = 0;
        while (true) {
            if (p_string[result.raw] == '\0') {
                return result;
            }
            result++;
        }
    } else {
        ssize result = 0;
        char1x16 const* p_memory = p_string_to_p_vector<char1x16>(p_string);
        constexpr char1x16 zeros = zeros_list<char, 16>;

        while (true) {
            char1x16 data = *p_memory;
            constexpr StringControl mask = StringControl::unsigned_byte |
                                           StringControl::compare_equal_each |
                                           StringControl::least_significant;

            // If there are one or more `0` bytes in `data`:
            if (compare_implicit_length_strings<mask>(data, zeros)) {
                int4 const index =
                    compare_implicit_length_strings_return_index<mask>(data,
                                                                       zeros);
                // Adding `1` is required to count the null terminator.
                return result + index + 1;
            }

            p_memory++;
            result += ssizeof<char1x16>();
        }

        __builtin_unreachable();
    }
}
