/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __MATH_H__
#define __MATH_H__

/// Return word count from byte count
#define toHWCOUNT(byteCnt) (((byteCnt)&0x1) ? (((byteCnt)/2)+1) : ((byteCnt)/2))
#define toWCOUNT(byteCnt)  (((byteCnt)&0x3) ? (((byteCnt)/4)+1) : ((byteCnt)/4))

U32 min(U32 a, U32 b);
U32 absv(S32 value);
U32 power(U32 x, U32 y);

#endif /* __MATH_H__ */
