/* ----------------------------------------------------------------------------
 *		 
 *        mboot for Lophilo embedded system
 *		
 * ----------------------------------------------------------------------------
 */
 
#include "types.h"
#include "trace.h"
#include "cp15.h"

///////////////////////////////////////////////////////////////////////////////
/// CP15 c1
/// * I cache
/// * D cache
///////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
/// Check Instruction cache
/// \return 0 if I_cache disable, 1 if I_cache enable
//------------------------------------------------------------------------------
U32 CP15_IsIcacheEnabled(void)
{
    U32 control;

    control = CP15_ReadControl();
    return ((control & (1 << CP15_I_BIT)) != 0);
} 

//------------------------------------------------------------------------------
/// Enable Instruction cache
//------------------------------------------------------------------------------
void CP15_EnableIcache(void)
{
    U32 control;

    control = CP15_ReadControl();

    // Check if cache is disabled
    if ((control & (1 << CP15_I_BIT)) == 0) {
        control |= (1 << CP15_I_BIT);
        CP15_WriteControl(control);        
        DEBUG_MSG("I cache enabled.");
    }
#if !defined(OP_BOOTSTRAP_on)
    else {
        DEBUG_MSG("I cache is already enabled.");
    }
#endif
}

//------------------------------------------------------------------------------
/// Disable Instruction cache
//------------------------------------------------------------------------------
void CP15_DisableIcache(void)
{
    U32 control;

    control = CP15_ReadControl();

    // Check if cache is enabled
    if ((control & (1 << CP15_I_BIT)) != 0) {
        control &= ~(1 << CP15_I_BIT);
        CP15_WriteControl(control);        
        DEBUG_MSG("I cache disabled.");
    }
    else {
        DEBUG_MSG("I cache is already disabled.");
    }
} 

//------------------------------------------------------------------------------
/// Check MMU
/// return 0 if MMU disable, 1 if MMU enable
//------------------------------------------------------------------------------
U32 CP15_IsMMUEnabled(void)
{
    U32 control;

    control = CP15_ReadControl();
    return ((control & (1 << CP15_M_BIT)) != 0);
} 

//------------------------------------------------------------------------------
/// Enable MMU
//------------------------------------------------------------------------------
void CP15_EnableMMU(void)
{
    U32 control;

    control = CP15_ReadControl();

    // Check if MMU is disabled
    if ((control & (1 << CP15_M_BIT)) == 0) {

        control |= (1 << CP15_M_BIT);
        CP15_WriteControl(control);        
        DEBUG_MSG("MMU enabled.");
    }
    else {
        DEBUG_MSG("MMU is already enabled.");
    }
}

//------------------------------------------------------------------------------
/// Disable MMU
//------------------------------------------------------------------------------
void CP15_DisableMMU(void)
{
    U32 control;

    control = CP15_ReadControl();

    // Check if MMU is enabled
    if ((control & (1 << CP15_M_BIT)) != 0) {

        control &= ~(1 << CP15_M_BIT);
        control &= ~(1 << CP15_C_BIT);
        CP15_WriteControl(control);        
        DEBUG_MSG("MMU disabled.");
    }
    else {

        DEBUG_MSG("MMU is already disabled.");
    }
}

//------------------------------------------------------------------------------
/// Check D_cache
/// \return 0 if D_cache disable, 1 if D_cache enable (with MMU of course)
//------------------------------------------------------------------------------
U32 CP15_IsDcacheEnabled(void)
{
    U32 control;

    control = CP15_ReadControl();
    return ((control & ((1 << CP15_C_BIT)||(1 << CP15_M_BIT))) != 0);
} 

//------------------------------------------------------------------------------
/// Enable Data cache
//------------------------------------------------------------------------------
void CP15_EnableDcache(void)
{
    U32 control;

    control = CP15_ReadControl();

    if( !CP15_IsMMUEnabled() ) {
        DEBUG_MSG("Do nothing: MMU not enabled");
    }
    else {
        // Check if cache is disabled
        if ((control & (1 << CP15_C_BIT)) == 0) {

            control |= (1 << CP15_C_BIT);
            CP15_WriteControl(control);        
            DEBUG_MSG("D cache enabled.");
        }
        else {
            DEBUG_MSG("D cache is already enabled.");
        }
    }
}

//------------------------------------------------------------------------------
/// Disable Data cache
//------------------------------------------------------------------------------
void CP15_DisableDcache(void)
{
    U32 control;

    control = CP15_ReadControl();

    // Check if cache is enabled
    if ((control & (1 << CP15_C_BIT)) != 0) {

        control &= ~(1 << CP15_C_BIT);
        CP15_WriteControl(control);        
        DEBUG_MSG("D cache disabled.");
    }
    else {
        DEBUG_MSG("D cache is already disabled.");
    }
}

//----------------------------------------------------------------------------
/// Lock I cache
/// \param I cache index
//----------------------------------------------------------------------------
void CP15_LockIcache(U32 index)
{
    U32 victim = 0;

    // invalidate all the cache (4 ways) 
    CP15_InvalidateIcache();
    
    // lockdown all the ways except this in parameter
    victim =  CP15_ReadIcacheLockdown();
    victim = 0;
    victim |= ~index;
    victim &= 0xffff;
    CP15_WriteIcacheLockdown(victim);
}

//----------------------------------------------------------------------------
/// Lock D cache
/// \param D cache way
//----------------------------------------------------------------------------
void CP15_LockDcache(U32 index)
{
    U32 victim = 0;

    // invalidate all the cache (4 ways)    
    CP15_InvalidateDcache();
    
    // lockdown all the ways except this in parameter    
    victim =  CP15_ReadDcacheLockdown();
    victim = 0;
    victim |= ~index;
    victim &= 0xffff;
    CP15_WriteDcacheLockdown(victim);
}

//----------------------------------------------------------------------------
/// Lock D cache
/// \param D cache way
//----------------------------------------------------------------------------
void CP15_ShutdownDcache(void)
{ 
    CP15_TestCleanInvalidateDcache();  
    CP15_DrainWriteBuffer();
    CP15_DisableDcache();
    CP15_InvalidateTLB();      
}
