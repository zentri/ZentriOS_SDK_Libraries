/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

/**
 * NOTE: Algorithms taken from:
 *       https://chessprogramming.wikispaces.com/Flipping+Mirroring+and+Rotating#Rotationby90degreesAnticlockwise
 */

#include "zos.h"

#define C64(x) x ## ULL



/**
 * Flip a bitboard vertically about the centre ranks.
 * Rank 1 is mapped to rank 8 and vice versa.
 * . 1 1 1 1 . . .     . 1 . . . 1 . .
 * . 1 . . . 1 . .     . 1 . . 1 . . .
 * . 1 . . . 1 . .     . 1 . 1 . . . .
 * . 1 . . 1 . . .     . 1 1 1 . . . .
 * . 1 1 1 . . . .     . 1 . . 1 . . .
 * . 1 . 1 . . . .     . 1 . . . 1 . .
 * . 1 . . 1 . . .     . 1 . . . 1 . .
 * . 1 . . . 1 . .     . 1 1 1 1 . . .
 * @param x any bitboard
 * @return bitboard x flipped vertically
 */
uint64_t flipVertical(uint64_t x)
{
    const uint64_t k1 = C64(0x00FF00FF00FF00FF);
    const uint64_t k2 = C64(0x0000FFFF0000FFFF);

    x = ((x >>  8) & k1) | ((x & k1) <<  8);
    x = ((x >> 16) & k2) | ((x & k2) << 16);
    x = ( x >> 32)       | ( x       << 32);

    return x;
}

/**
 * Flip a bitboard about the diagonal a1-h8.
 * Square h1 is mapped to a8 and vice versa.
 *
 * . 1 1 1 1 . . /     . . . . . . . .
 * . 1 . . . 1 . .     . . . . . . . .
 * . 1 . . . 1 . .     1 . . . . 1 1 .
 * . 1 . . 1 . . .     . 1 . . 1 . . 1
 * . 1 1 1 . . . .     . . 1 1 . . . 1
 * . 1 . 1 . . . .     . . . 1 . . . 1
 * . 1 . . 1 . . .     1 1 1 1 1 1 1 1
 * / 1 . . . 1 . .     . . . . . . . .
 * @param x any bitboard
 * @return bitboard x flipped about diagonal a1-h8
 */
uint64_t flipDiagA1H8(uint64_t x)
{
    uint64_t t;
    const uint64_t k1 = C64(0x5500550055005500);
    const uint64_t k2 = C64(0x3333000033330000);
    const uint64_t k4 = C64(0x0f0f0f0f00000000);
    t  = k4 & (x ^ (x << 28));
    x ^=       t ^ (t >> 28) ;
    t  = k2 & (x ^ (x << 14));
    x ^=       t ^ (t >> 14) ;
    t  = k1 & (x ^ (x <<  7));
    x ^=       t ^ (t >>  7) ;

    return x;
}

/*************************************************************************************************/
/**
 * Rotate a bitboard by 90 degrees anticlockwise.
 *
 * . 1 1 1 1 . . .     . . . . . . . .
 * . 1 . . . 1 . .     . . . . . . . .
 * . 1 . . . 1 . .     . 1 1 . . . . 1
 * . 1 . . 1 . . .     1 . . 1 . . 1 .
 * . 1 1 1 . . . .     1 . . . 1 1 . .
 * . 1 . 1 . . . .     1 . . . 1 . . .
 * . 1 . . 1 . . .     1 1 1 1 1 1 1 1
 * . 1 . . . 1 . .     . . . . . . . .

 * @param x any bitboard
 * @return bitboard x rotated 90 degrees anticlockwise
 */
uint64_t rotate90antiClockwise(uint64_t x)
{
    return flipDiagA1H8 (flipVertical (x) );
}

/**
 * Rotate a bitboard by 90 degrees clockwise.
 *
 * . 1 1 1 1 . . .     . . . . . . . .
 * . 1 . . . 1 . .     1 1 1 1 1 1 1 1
 * . 1 . . . 1 . .     . . . 1 . . . 1
 * . 1 . . 1 . . .     . . 1 1 . . . 1
 * . 1 1 1 . . . .     . 1 . . 1 . . 1
 * . 1 . 1 . . . .     1 . . . . 1 1 .
 * . 1 . . 1 . . .     . . . . . . . .
 * . 1 . . . 1 . .     . . . . . . . .

 * @param x any bitboard
 * @return bitboard x rotated 90 degrees clockwise
 */
uint64_t rotate90clockwise (uint64_t x)
{
   return flipVertical (flipDiagA1H8 (x) );
}
