/*******************************************************************
*
*    Author: Kareem Omar
*    kareem.h.omar@gmail.com
*    https://github.com/komrad36
*
*    Last updated Feb 14, 2021
*******************************************************************/

#pragma once

__uint128_t DivMod(__uint128_t N, __uint128_t D, __uint128_t& rem);

static inline __uint128_t Div(__uint128_t a, __uint128_t b)
{
    __uint128_t rem;
    return DivMod(a, b, rem);
}

static inline __uint128_t Mod(__uint128_t a, __uint128_t b)
{
    __uint128_t rem;
    DivMod(a, b, rem);
    return rem;
}
