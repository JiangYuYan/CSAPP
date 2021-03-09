/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  //           00 01 10 11
  // ~x & y    0  1  0  0
  // x & ~y    0  0  1  0
  // x & y     0  0  0  1
  // ~x & ~y   1  0  0  0
  return ~(x & y) & ~(~x & ~y);
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 1 << 31;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  int xInverse = ~x;
  int isTmaxOrZero = (x + 1) ^ xInverse; // x == -1 or Tmax ? 0 : others
  return !(isTmaxOrZero | !xInverse);

  // int xInverse = ~x;
  // int notMinusOne = !!xInverse; // if x = -1, notMinusOne = 1; others = 0;
  // int isTmaxOrZero = !((x + 1) ^ xInverse); // if x = -1 or x = Tmax, isTmaxOrZero = 1; others 0;
  // return isTmaxOrZero & notMinusOne;
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  int aaaa = (0xAA << 8) + 0xAA;
  int mask = (aaaa << 16) + aaaa;
  return !((mask & x) ^ mask);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  int isOx3X = !((x >> 4) ^ 3); // x == 0x3[0-F] ? 
  int Digit = x + 6;
  int NotOverflow = !((Digit & 0x40) >> 4);
  return  isOx3X & NotOverflow;
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  int isZero = !(x ^ 0); // x == 0 ? 1 : 0
  int isZeroMask = (~isZero + 1); // x == 0 ? 0xFFFFFFFF : 0x0
  return (z & isZeroMask) + (y & (~isZeroMask));
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  /* solution 1 : 13 */
  int xSign = x >> 31;
  int ySign = y >> 31; // y < 0 ? 0xFFFFFFFF : 0x0;
  int diffSign = xSign ^ ySign; // x and y have same sign ? 0x0 : 0xFFFFFFFF;
  int yMinusX = ~x + 1 + y;
  int minusSign = yMinusX >> 31; // -x + y < 0 ? 0xFFFFFFFF : 0x0;
  return (diffSign & !ySign) + ((~diffSign) & !minusSign); // diffSign ? !ySign : !minusSign;
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  /* solution 1: 11 operations */
  // int lastSixteen = (x >> 16) | x;
  // int lastEight = (lastSixteen >> 8) | lastSixteen;
  // int lastFour = (lastEight >> 4) | lastEight;
  // int lastTwo = (lastFour >> 2) | lastFour;
  // int lastOne = (lastTwo >> 1) | lastTwo;
  // return lastOne & 1;

  /* solution 2: 6 operations */
  int minusX = ~x + 1;
  int leading = (x | minusX) >> 31; // leading = x == 0 ? 0 : 0xFFFFFFFF;
  return (~leading) & 1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  /* solution 1 : 31 operations
  reserve the last consecutive identical digit*/
  int bits16, bits8, bits4, bits2, bits1, bits0;
  int mask = (x >> 31) ^ x; // mask = 0+1.*;
  bits16 = (!!(mask >> 16)) << 4; // bits16 = front16 == 0 ? 0 : 16;
  mask = mask >> bits16;
  bits8 = (!!(mask >> 8)) << 3; // bits8 = front8 == 0 ? 0 : 8;
  mask = mask >> bits8;
  bits4 = (!!(mask >> 4)) << 2; // bits4 = front4 == 0 ? 0 : 4;
  mask = mask >> bits4;
  bits2 = (!!(mask >> 2)) << 1; // bits2 = front2 == 0 ? 0 : 2;
  mask = mask >> bits2;
  bits1 = (mask >> 1) << 1; // bits1 = front1 == 0 ? 0 : 2;
  bits0 = mask >> bits1;
  return 1 + bits0 + bits1 + bits2 + bits4 + bits8 + bits16;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  unsigned signal = 0x80000000 & uf;
  unsigned exp = 0x7F800000 & uf;
  unsigned frac = 0x007FFFFF & uf;
  if (exp ^ 0x7F800000) { // if exp != 0x7F800000
    if (exp ^ 0) { // if exp != 0x00000000, Normalized
      exp = exp + 0x800000;
      if (!(exp ^ 0x7F800000)) { // if exp + 1 = 0x7F800000, overflow, then let frac = 0
        frac = 0;
      }
    } else { // exp == 0, Denormalized
      frac = frac << 1;
    }
  } // else exp == 0x7F800000, Infinity and NaN do nothing;
  return frac | exp | signal;
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  // +0, -0 -> 0; that is 0x80000000, 0x0 -> 0x0;
  // NaN and Infinity -> 0x80000000;
  // abis = 2^7 - 1 = 127
  // exp = 126, E = -1, M = [1, 2)
  // exp < 127, to 0
  // Tmax = 2^32 - 1, Tmin = -2^32
  // exp = 159, E = 32, M = [1, 2 - 2^-23]
  // exp > 158, to 0x80000000
  // 127 <= exp <= 158, normal int
  // E = [0, 31], normal; < 0, 0; > 31, Tmin; 32 = 0x100000
  int sign = uf & 0x80000000;
  int exp = (uf >> 23) & 0xFF;
  int E = exp - 127;
  if (E < 0) {
    return 0;
  }
  if (E > 31) {
    return 0x80000000;
  }
  uf = (uf & 0x7FFFFF) + 0x800000;
  if (E < 23) { // E-23 < 0
    uf = uf >> (23-E);
  } else {
    uf = uf << (E-23);
  }
  if (sign) {
    return ~uf + 1;
  }
  return uf;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  // int = [-2^32, 2^32-1]
  // denormalized = 2^-126 * [0, 2^-23, 1-2^-23]; 0, 2^-149, 2^-126 - 2^-149
  // normalized = 2^-126, 2^127 * [1, 2)
  // can't be NaN
  // x > 127 or x < -149 , INF;
  // 149 = 128 + 3   00000095 FFFFFF6B
  // [0x0000007F ~ 0xFFFFFF6B, INF;
  // [-126, 127]
  if (x < -149) {
    return 0;
  }
  if (127 < x) {
    return 0x7F800000;
  }
  x = x + 127;
  if (x <= 0) {
    return 0x00400000 >> (-x);
  }
  return x << 23;
}
