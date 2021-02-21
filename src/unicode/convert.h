/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2020 Christian Parpart <christian@parpart.family>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <string_view>
#include <array>
#include <type_traits>
#include <iterator>
#include <optional>

namespace unicode {

template <typename T>
struct converter;

template<> struct converter<char> // {{{
{
    using char_type = char;

    template <typename OutputIterator>
    constexpr size_t write(char32_t _input, OutputIterator& _output)
    {
        if (_input <= 0x7F)
        {
            *_output++ = static_cast<uint8_t>(_input & 0b0111'1111);
            return 1;
        }
        else if (_input <= 0x07FF)
        {
            *_output++ = static_cast<uint8_t>(((_input >> 6) & 0b0001'1111) | 0b1100'0000);
            *_output++ = static_cast<uint8_t>(((_input >> 0) & 0b0011'1111) | 0b1000'0000);
            return 2;
        }
        if (_input <= 0xFFFF)
        {
            *_output++ = static_cast<uint8_t>(((_input >> 12) & 0b0000'1111) | 0b1110'0000);
            *_output++ = static_cast<uint8_t>(((_input >>  6) & 0b0011'1111) | 0b1000'0000);
            *_output++ = static_cast<uint8_t>(((_input >>  0) & 0b0011'1111) | 0b1000'0000);
            return 3;
        }
        else
        {
            *_output++ = static_cast<uint8_t>(((_input >> 18) & 0b0000'0111) | 0b1111'0000);
            *_output++ = static_cast<uint8_t>(((_input >> 12) & 0b0011'1111) | 0b1000'0000);
            *_output++ = static_cast<uint8_t>(((_input >>  6) & 0b0011'1111) | 0b1000'0000);
            *_output++ = static_cast<uint8_t>(((_input >>  0) & 0b0011'1111) | 0b1000'0000);
            return 4;
        }
    }

    template <typename InputIterator>
    constexpr std::optional<char32_t> read(InputIterator& _input)
    {
        using std::nullopt;

        auto const ch0 = uint8_t(*_input++);
        if (ch0 < 0x80) // 0xxx_xxxx
            return char32_t(ch0);

        if (ch0 < 0xC0)
            throw nullopt;

        if (ch0 < 0xE0) // 110x_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*_input++);
            if ((ch1 >> 6) != 2)
                return nullopt;
            return char32_t((ch0 << 6) + ch1 - 0x3080);
        }

        if (ch0 < 0xF0) // 1110_xxxx 10xx_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*_input++);
            if (ch1 >> 6 != 2)
                return nullopt;
            auto const ch2 = uint8_t(*_input++);
            if (ch2 >> 6 != 2)
                return nullopt;
            return char32_t((ch0 << 12) + (ch1 << 6) + ch2 - 0xE2080);
        }
        if (ch0 < 0xF8) // 1111_0xxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*_input++);
            if (ch1 >> 6 != 2)
                return nullopt;
            auto const ch2 = uint8_t(*_input++);
            if (ch2 >> 6 != 2)
                return nullopt;
            auto const ch3 = uint8_t(*_input++);
            if (ch3 >> 6 != 2)
                return nullopt;
            return char32_t((ch0 << 18) + (ch1 << 12) + (ch2 << 6) + ch3 - 0x3C82080);
        }
        if (ch0 < 0xFC) // 1111_10xx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*_input++);
            if (ch1 >> 6 != 2)
                return nullopt;
            auto const ch2 = uint8_t(*_input++);
            if (ch2 >> 6 != 2)
                return nullopt;
            auto const ch3 = uint8_t(*_input++);
            if (ch3 >> 6 != 2)
                return nullopt;
            auto const ch4 = uint8_t(*_input++);
            if (ch4 >> 6 != 2)
                return nullopt;
            return char32_t((ch0 << 24) + (ch1 << 18) + (ch2 << 12) + (ch3 << 6) + ch4 - 0xFA082080);
        }
        if (ch0 < 0xFE) // 1111_110x 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*_input++);
            if (ch1 >> 6 != 2)
                return nullopt;
            auto const ch2 = uint8_t(*_input++);
            if (ch2 >> 6 != 2)
                return nullopt;
            auto const ch3 = uint8_t(*_input++);
            if (ch3 >> 6 != 2)
                return nullopt;
            auto const ch4 = uint8_t(*_input++);
            if (ch4 >> 6 != 2)
                return nullopt;
            auto const ch5 = uint8_t(*_input++);
            if (ch5 >> 6 != 2)
                return nullopt;
            return char32_t((ch0 << 30) + (ch1 << 24) + (ch2 << 18) + (ch3 << 12) + (ch4 << 6) + ch5 - 0x82082080);
        }
        return nullopt;
    }
}; // }}}
template<> struct converter<char16_t> // {{{
{
    using char_type = char16_t;

    template <typename OutputIterator>
    constexpr size_t write(char32_t _input, OutputIterator& _output)
    {
        if (_input < 0xD800) // [0x0000 .. 0xD7FF]
        {
            *_output++ = _input;
            return 1;
        }
        else if (_input < 0x10000)
        {
            if (_input < 0xE000)
                return 0; // The UTF-16 code point can not be in surrogate range.

            // [0xE000 .. 0xFFFF]
            *_output++ = _input;
            return 1;
        }
        else if (_input < 0x110000) // [0xD800 .. 0xDBFF] [0xDC00 .. 0xDFFF]
        {
            *_output++ = 0xD7C0 + (_input >> 10);
            *_output++ = 0xDC00 + (_input & 0x3FF);
            return 2;
        }
        else
            return 0; // Too large the UTF-16  code point.
    }

    template <typename InputIterator>
    constexpr std::optional<char32_t> read(InputIterator& _input)
    {
        auto const ch0 = *_input++;

        if (ch0 < 0xD800) // [0x0000 .. 0xD7FF]
            return ch0;

        if (ch0 < 0xDC00) // [0xD800 .. 0xDBFF], [0xDC00 .. 0xDFFF]
        {
            auto const ch1 = *_input++;
            if ((ch1 >> 10) != 0x37)
                return std::nullopt; // The low UTF-16 surrogate character is expected.

            return (ch0 << 10) + ch1 - 0x35FDC00;
        }

        if (ch0 < 0xE000)
            return std::nullopt; // The high UTF-16 surrogate character is expected.

        // [0xE000 .. 0xFFFF]
        return ch0;
    }
}; // }}}
template<> struct converter<char32_t> // {{{ (no-op)
{
    using char_type = char32_t;

    template <typename OutputIterator>
    constexpr size_t write(char32_t _input, OutputIterator& _output)
    {
        *_output++ = _input;
        return 1;
    }

    template <typename InputIterator>
    constexpr std::optional<char32_t> read(InputIterator& _input)
    {
        return *_input++;
    }
}; // }}}
template<> struct converter<wchar_t> // {{{
{
    using char_type = wchar_t;

    template <typename OutputIterator>
    constexpr size_t write(char32_t _input, OutputIterator&& _output)
    {
        static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4);

        if constexpr (sizeof(wchar_t) == 2)
            return converter<char16_t>{}.write(_input, _output);
        else
            return converter<char32_t>{}.write(_input, _output);
    }

    template <typename InputIterator>
    constexpr std::optional<char32_t> read(InputIterator& _input)
    {
        static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4);

        if constexpr (sizeof(wchar_t) == 2)
            return converter<char16_t>{}.read(_input);
        else
            return converter<char32_t>{}.write(_input);
    }
}; // }}}

namespace detail // {{{
{
    template <typename SourceRange, typename OutputIterator>
    void convert_identity(SourceRange&& s, OutputIterator&& t)
    {
        for (auto const c : s)
            *t++ = c;
    }
} // }}}

/// @p _input with element type @p S to the appropricate type of @p _output.
template <typename T, typename OutputIterator, typename S>
void convert_to(std::basic_string_view<S> _input, OutputIterator&& _output)
{
    if constexpr (std::is_same_v<S, T>)
        detail::convert_identity(_input, _output);
    else
    {
        auto i = begin(_input);
        auto e = end(_input);
        while (i != e)
        {
            auto const outChar = converter<S>{}.read(i);
            if (outChar.has_value())
                converter<T>{}.write(outChar.value(), _output);
        }
    }
}

/// Converts a string of element type @p <S> into string of element type @p <T>.
template <typename T, typename S>
std::basic_string<T> convert_to(std::basic_string_view<S> in)
{
    std::basic_string<T> out;
    convert_to<T>(in, std::back_inserter(out));
    return out;
}

}