/* ----------------------------------------------------------------------------
 *		 
 *        mboot for Lophilo embedded system
 *		
 * ----------------------------------------------------------------------------
 */

#include "types.h"
#include "math.h"

//------------------------------------------------------------------------------
/// Returns the minimum value between two integers.
/// \param a  First integer to compare.
/// \param b  Second integer to compare.
//------------------------------------------------------------------------------
U32 min(U32 a, U32 b)
{ 
    if (a < b) {

        return a;
    }
    else {

        return b;
    }
}

//------------------------------------------------------------------------------
/// Returns the absolute value of an integer.
/// \param value  Integer value.
//------------------------------------------------------------------------------
// Do not call this function "abs", problem with gcc !
U32 absv(S32 value)
{
    if (value < 0) {

        return -value;
    }
    else {

        return value;
    }
}

//------------------------------------------------------------------------------
/// Computes and returns x power of y.
/// \param x  Value.
/// \param y  Power.
//------------------------------------------------------------------------------
U32 power(U32 x, U32 y)
{
    U32 result = 1;
    
    while (y > 0) {

        result *= x;
        y--;
    } 
    return result;
}
