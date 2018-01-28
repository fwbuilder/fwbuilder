/*
 * Copyright (c) 2008
 * Evan Teran
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appears in all copies and that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the same name not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission. We make no representations about the
 * suitability this software for any purpose. It is provided "as is"
 * without express or implied warranty.
 */

/*
 * Commented out the line that #includes <bits/ios_base.h>
 * and ran through astyle:
 *  astyle --style=ansi --indent=spaces  --convert-tabs uint128.h
 *
 * --vk 11/16/2010
 */

#ifndef UINT128_20050119_H_
#define UINT128_20050119_H_

#include <stdexcept>
#include <string>
#include <iostream>

// #include <bits/ios_base.h>
#include <stdint.h>

#include <limits>
#include <limits.h>

#include <stdio.h>

// #define __STDC_FORMAT_MACROS
#include <inttypes.h>   // for sprintf formats for "long long"

// convinience macro
#define U128_C(s) uint128(#s)

class uint128
{
public:
    typedef uint64_t base_type;
    typedef uint128  Self;

public:
    static const int size = (sizeof(base_type) + sizeof(base_type)) * CHAR_BIT;

private:
    base_type lo;
    base_type hi;

public:
    uint128(base_type _lo, base_type _hi) : lo(_lo), hi(_hi)                    {}

    // constructors for all basic types
    uint128()                       : lo(0), hi(0)                              {}
    uint128(int value)              : lo(static_cast<base_type>(value)), hi(0)
    {
        if (value < 0) hi = static_cast<base_type>(-1);
    }
    uint128(unsigned int value)     : lo(static_cast<base_type>(value)), hi(0)  {}
    uint128(float value)            : lo(static_cast<base_type>(value)), hi(0)  {}
    uint128(double value)           : lo(static_cast<base_type>(value)), hi(0)  {}
    uint128(const Self &value)      : lo(value.lo), hi (value.hi)               {}
    uint128(base_type value)        : lo(value), hi(0)                          {}
    uint128(const uint32_t value[4]): lo((uint64_t(value[2])<<32)|value[3]),hi((uint64_t(value[0])<<32)|value[1]) {}

    uint128(const std::string &sz) : lo(0), hi(0)
    {

        // do we have at least one character?
        if (!sz.empty())
        {

            // make some reasonable assumptions
            int radix = 10;
            bool minus = false;

            std::string::const_iterator i = sz.begin();

            // check for minus sign, i suppose technically this should only apply
            // to base 10, but who says that -0x1 should be invalid?
            if (*i == '-')
            {
                ++i;
                minus = true;
            }

            // check if there is radix changing prefix (0 or 0x)
            if (i != sz.end())
            {
                if (*i == '0')
                {
                    radix = 8;
                    ++i;
                    if (i != sz.end())
                    {
                        if (*i == 'x')
                        {
                            radix = 16;
                            ++i;
                        }
                    }
                }

                while (i != sz.end())
                {
                    unsigned int n;
                    const char ch = *i;

                    if (ch >= 'A' && ch <= 'Z')
                    {
                        if (((ch - 'A') + 10) < radix)
                        {
                            n = (ch - 'A') + 10;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else if (ch >= 'a' && ch <= 'z')
                    {
                        if (((ch - 'a') + 10) < radix)
                        {
                            n = (ch - 'a') + 10;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else if (ch >= '0' && ch <= '9')
                    {
                        if ((ch - '0') < radix)
                        {
                            n = (ch - '0');
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        /* completely invalid character */
                        break;
                    }

                    (*this) *= radix;
                    (*this) += n;

                    ++i;
                }
            }

            // if this was a negative number, do that two's compliment madness :-P
            if (minus)
            {
                *this = -*this;
            }
        }
    }

    Self &operator=(const Self &other)
    {
        if (&other != this)
        {
            lo = other.lo;
            hi = other.hi;
        }
        return *this;
    }

public: // comparison operators

    bool operator==(const Self &o) const
    {
        return hi == o.hi && lo == o.lo;
    }

    bool operator!=(const Self &o) const
    {
        return hi != o.hi || lo != o.lo;
    }

    bool operator<(const Self &o) const
    {
        return (hi == o.hi) ? lo < o.lo : hi < o.hi;
    }

    bool operator>(const Self &o) const
    {
        return (hi == o.hi) ? lo > o.lo : hi > o.hi;
    }

    bool operator<=(const Self &o) const
    {
        return *this < o || *this == 0;
    }

    bool operator>=(const Self &o) const
    {
        return *this > o || *this == 0;
    }

public: // unary operators

    bool operator!() const
    {
        return !(hi != 0 || lo != 0);
    }

    Self operator-() const
    {
        // standard 2's compliment negation
        return ~Self(*this) + 1;
    }

    Self operator~() const
    {
        Self t(*this);
        t.lo = ~t.lo;
        t.hi = ~t.hi;
        return t;
    }

    Self &operator++()
    {
        if (++lo == 0)
        {
            ++hi;
        }
        return *this;
    }

    Self &operator--()
    {
        if (lo-- == 0)
        {
            --hi;
        }
        return *this;
    }

public: // basic math operators

    Self &operator+=(const Self &b)
    {
        const base_type old_lo = lo;

        lo += b.lo;
        hi += b.hi;

        if (lo < old_lo)
        {
            ++hi;
        }

        return *this;
    }

    Self &operator-=(const Self &b)
    {
        // it happens to be way easier to write it
        // this way instead of make a subtraction algorithm
        return *this += -b;
    }

    Self &operator*=(const Self &b)
    {

        // check for multiply by 0
        // result is always 0 :-P
        if (b == 0)
        {
            hi = 0;
            lo = 0;
        }
        else if (b != 1)
        {

            // check we aren't multiplying by 1

            Self a(*this);
            Self t = b;

            lo = 0;
            hi = 0;

            for (int i = 0; i < size; ++i)
            {
                if ((t & 1) != 0)
                {
                    *this += (a << i);
                }

                t >>= 1;
            }
        }

        return *this;
    }

    Self &operator|=(const Self &b)
    {
        hi |= b.hi;
        lo |= b.lo;
        return *this;
    }

    Self &operator&=(const Self &b)
    {
        hi &= b.hi;
        lo &= b.lo;
        return *this;
    }

    Self &operator^=(const Self &b)
    {
        hi ^= b.hi;
        lo ^= b.lo;
        return *this;
    }

    Self &operator/=(const Self &b)
    {
        Self remainder;
        __do_div(*this, b, *this, remainder);
        return *this;
    }

    Self &operator%=(const Self &b)
    {
        Self quotient;
        __do_div(*this, b, quotient, *this);
        return *this;
    }

    Self &operator<<=(const Self& rhs)
    {

        int n = rhs.to_integer();

        if (n >= size)
        {
            hi = 0;
            lo = 0;
        }
        else
        {
            const int halfsize = size / 2;

            if (n >= halfsize)
            {
                n -= halfsize;
                hi = lo;
                lo = 0;
            }

            if (n != 0)
            {
                // shift high half
                hi <<= n;

                const base_type mask(~(base_type(-1) >> n));

                // and add them to high half
                hi |= (lo & mask) >> (halfsize - n);

                // and finally shift also low half
                lo <<= n;
            }
        }

        return *this;
    }

    Self &operator>>=(const Self& rhs)
    {

        int n = rhs.to_integer();

        if (n >= size)
        {
            hi = 0;
            lo = 0;
        }
        else
        {
            const int halfsize = size / 2;

            if (n >= halfsize)
            {
                n -= halfsize;
                lo = hi;
                hi = 0;
            }

            if (n != 0)
            {
                // shift low half
                lo >>= n;

                // get lower N bits of high half
                const base_type mask(~(base_type(-1) << n));

                // and add them to low qword
                lo |= (hi & mask) << (halfsize - n);

                // and finally shift also high half
                hi >>= n;
            }
        }

        return *this;
    }

    Self operator+(const int &u)
    {
        Self temp(*this);
        temp += u;
        return temp;
    }

    Self operator&(const int &u)
    {
        Self temp(*this);
        temp &= u;
        return temp;
    }

    Self operator<<(const int &u)
    {
        Self temp(*this);
        temp <<= u;
        return temp;
    }

    Self operator>>(const int &u)
    {
        Self temp(*this);
        temp >>= u;
        return temp;
    }


public:
    int to_integer() const
    {
        return static_cast<int>(lo);
    }

    base_type to_base_type() const
    {
        return lo;
    }

    std::string to_string() const
    {
        char buf[33];
        if (hi)
            sprintf(buf, "%" PRIX64"%08" PRIX64, (uint64_t)hi,(uint64_t)lo);
        else
            sprintf(buf,"%" PRIX64,(uint64_t)lo);
        return buf;
    }

private:
    template <typename T>
    static void __do_div(const T &numerator, const T &denominator, T &quotient, T &remainder)
    {

        static const int bits = sizeof(T) * CHAR_BIT;

        if (denominator == 0)
        {
            throw std::domain_error("divide by zero");
        }
        else
        {
            T n         = numerator;
            T d         = denominator;
            T x         = 1;
            T answer    = 0;

            while ((n >= d) && (((d >> (bits - 1)) & 1) == 0))
            {
                x <<= 1;
                d <<= 1;
            }

            while (x != 0)
            {
                if (n >= d)
                {
                    n -= d;
                    answer |= x;
                }

                x >>= 1;
                d >>= 1;
            }

            quotient = answer;
            remainder = n;
        }
    }
};

std::ostream &operator<<(std::ostream &o, const uint128 &n);

typedef uint128 uint128_t;

namespace std
{
template<>
struct numeric_limits<uint128>
{
    static const bool is_specialized = true;
//    static uint128 min()
//    {
//        return 0;
//    }
//
//    static uint128 max()
//    {
//        return uint128(
//                   std::numeric_limits<uint64_t>::max(),
//                   std::numeric_limits<uint64_t>::max()
//               );
//    }

    static const bool is_signed = false;
    static const bool is_integer = true;
    static const bool is_exact = true;

    static const int digits = 128;
    static const int digits10 = 39;
    static const int radix = 2;

    static uint128 epsilon()
    {
        return 0;
    }

    static uint128 round_error()
    {
        return 0;
    }
    static const int min_exponent = 0;
    static const int min_exponent10 = 0;
    static const int max_exponent = 0;
    static const int max_exponent10 = 0;

    static const bool has_infinity = false;
    static const bool has_quiet_NaN = false;
    static const bool has_signaling_NaN = false;
    static const float_denorm_style has_denorm = denorm_absent;
    static const bool has_denorm_loss = false;

    static uint128 infinity()
    {
        return static_cast<uint128>(0);
    }

    static uint128 quiet_NaN()
    {
        return static_cast<uint128>(0);
    }

    static uint128 signaling_NaN()
    {
        return static_cast<uint128>(0);
    }

    static uint128 denorm_min()
    {
        return static_cast<uint128>(0);
    }

    static const bool is_iec559 = false;
    static const bool is_bounded = true;
    static const bool is_modulo = true;

    static const bool traps = false;
    static const bool tinyness_before = false;
    static const float_round_style round_style = round_toward_zero;
};
};

#endif
