/*******************************************************************
*
*    Author: Kareem Omar
*    kareem.h.omar@gmail.com
*    https://github.com/komrad36
*
*    Last updated Feb 14, 2021
*******************************************************************/

#include <cstdint>
#include <immintrin.h>

using U32 = uint32_t;
using U64 = uint64_t;
using U128 = __uint128_t;

static inline bool FitsHardwareDivL(U64 nHi, U64 nLo, U64 d)
{
    return !(nHi | (d >> 32)) && nLo < (d << 32);
}

static inline U64 HardwareDivL(U64 n, U64 d, U64& rem)
{
    U32 nHi = U32(n >> 32);
    U32 nLo = U32(n);
    asm("divl %[d]"
        : "+d" (nHi), "+a" (nLo)
        : [d] "r" (U32(d))
        : "cc"
    );
    rem = nHi;
    return nLo;
}

static inline U64 HardwareDivQ(U64 nHi, U64 nLo, U64 d, U64& rem)
{
    asm("divq %[d]"
        : "+d" (nHi), "+a" (nLo)
        : [d] "r" (d)
        : "cc"
    );
    rem = nHi;
    return nLo;
}

static inline bool IsPow2(U64 hi, U64 lo)
{
    const U64 T = hi | lo;
    return !((hi & lo) | (T & (T - 1)));
}

U128 DivMod(U128 N, U128 D, U128& rem)
{
    if (D > N)
    {
        rem = N;
        return 0;
    }

    U64 nHi = U64(N >> 64);
    U64 nLo = U64(N);
    U64 dHi = U64(D >> 64);
    U64 dLo = U64(D);

    if (IsPow2(dHi, dLo))
    {
        const U64 n = dLo ? _tzcnt_u64(dLo) : 64ULL + _tzcnt_u64(dHi);
        rem = (U128(_bzhi_u64(nHi, n < 64 ? 0 : n - 64)) << 64) | U128(_bzhi_u64(nLo, n));
        return N >> n;
    }

    if (!dHi)
    {
        if (nHi < dLo)
        {
            U64 remLo;
            U64 Q;
            if (FitsHardwareDivL(nHi, nLo, dLo))
                Q = HardwareDivL(nLo, dLo, remLo);
            else
                Q = HardwareDivQ(nHi, nLo, dLo, remLo);
            rem = remLo;
            return Q;
        }

        U64 remLo;
        const U64 qHi = HardwareDivQ(0, nHi, dLo, remLo);
        const U64 qLo = HardwareDivQ(remLo, nLo, dLo, remLo);
        rem = remLo;
        return (U128(qHi) << 64) | (U128(qLo));
    }

    U64 n = _lzcnt_u64(dHi) - _lzcnt_u64(nHi);

    asm("shld %b[n], %[lo], %[hi] \n\
         shlx %[n], %[lo], %[lo]"
        : [hi] "+&r" (dHi), [lo] "+&r" (dLo)
        : [n] "c" (n)
        : "cc"
    );

    U64 Q = 0;
    n = -n - 1;

#pragma nounroll
    do
    {
        Q <<= 1;

        asm("sub %[dLo], %[nLo]   \n\
             sbb %[dHi], %[nHi]   \n\
             cmovc %[tLo], %[nLo] \n\
             cmovc %[tHi], %[nHi] \n\
             sbb $-1, %[Q]"
            : [nLo] "+&r" (nLo), [nHi] "+&r" (nHi), [dLo] "+&r" (dLo), [dHi] "+&r" (dHi), [Q] "+&r" (Q)
            : [tLo] "r" (nLo), [tHi] "r" (nHi)
            : "cc"
        );

        dLo = (dLo >> 1) | (dHi << 63);
        dHi >>= 1;
    } while (++n);

    rem = (U128(nHi) << 64) | U128(nLo);
    return Q;
}
