/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __CP15_H__
#define __CP15_H__

#include "types.h"

//-----------------------------------------------------------------------------
//         Defines
//-----------------------------------------------------------------------------

#define CP15_L4_BIT 15 // Determines if the T bit is set when load instructions 
                       // change the PC: 
                       // 0 = loads to PC set the T bit 
                       // 1 = loads to PC do not set T bit

#define CP15_RR_BIT 14 // RR bit Replacement strategy for Icache and Dcache: 
                       // 0 = Random replacement 
                       // 1 = Round-robin replacement.
                      
#define CP15_V_BIT  13 // V bit Location of exception vectors: 
                       // 0 = Normal exception vectors selected address range = 0x0000 0000 to 0x0000 001C 
                       // 1 = High exception vect selected, address range = 0xFFFF 0000 to 0xFFFF 001C
                       
#define CP15_I_BIT  12 // I bit Icache enable/disable: 
                       // 0 = Icache disabled 
                       // 1 = Icache enabled
                       
#define CP15_R_BIT   9 // R bit ROM protection

#define CP15_S_BIT   8 // S bit System protection
                  
#define CP15_B_BIT   7 // B bit Endianness: 
                       // 0 = Little-endian operation 
                       // 1 = Big-endian operation.                  
                     
#define CP15_C_BIT   2 // C bit Dcache enable/disable: 
                       // 0 = cache disabled 
                       // 1 = cache enabled

#define CP15_A_BIT   1 // A bit Alignment fault enable/disable:
                       // 0 = Data address alignment fault checking disabled
                       // 1 = Data address alignment fault checking enabled

#define CP15_M_BIT   0 // M bit MMU enable/disable: 0 = disabled 1 = enabled.
                       // 0 = disabled 
                       // 1 = enabled

// No access Any access generates a domain fault.
#define CP15_DOMAIN_NO_ACCESS      0x00  
// Client Accesses are checked against the access permission bits in the section or page descriptor.
#define CP15_DOMAIN_CLIENT_ACCESS  0x01  
// Manager Accesses are not checked against the access permission bits so a permission fault cannot be generated.
#define CP15_DOMAIN_MANAGER_ACCESS 0x03  

//-----------------------------------------------------------------------------
//         External functions defined in cp15_asm.S
//-----------------------------------------------------------------------------

// c0
extern U32 CP15_ReadID(void);
extern U32 CP15_ReadCacheType(void);
extern U32 CP15_ReadTCMStatus(void);

// c1
extern U32 CP15_ReadControl(void);
extern void CP15_WriteControl(U32 value);

// c2
extern U32 CP15_ReadTTB(void);
extern void CP15_WriteTTB(U32 value);

// c3
extern U32 CP15_ReadDomainAccessControl(void);
extern void CP15_WriteDomainAccessControl(U32 value);

// c5
// CP15_ReadDFSR
// CP15_writeDFSR
// CP15_ReadIFSR
// CP15_WriteIFSR

// c6
// CP15_ReadFAR
// CP15_writeFAR

// c7  
extern void	CP15_InvalidateIDcache(void);
extern void CP15_InvalidateDcache(void);
extern void CP15_InvalidateIcache(void);
extern void CP15_PrefetchIcacheLine(U32 value);
extern void CP15_TestCleanInvalidateDcache(void);
extern void CP15_CleanInvalidateDcacheIndex(U32 index);	
extern void CP15_DrainWriteBuffer(void);
extern void CP15_WaitForInterrupt(void);

// c8
extern void CP15_InvalidateTLB(void);
extern void CP15_InvalidateTLBMVA(U32 mva);
extern void CP15_InvalidateITLB(void);
extern void CP15_InvalidateITLBMVA(U32 mva);
extern void CP15_InvalidateDTLB(void);
extern void CP15_InvalidateDTLBMVA(U32 mva);

// c9
extern U32 CP15_ReadDcacheLockdown(void);
extern void CP15_WriteDcacheLockdown(U32 value);
extern U32 CP15_ReadIcacheLockdown(void);
extern void CP15_WriteIcacheLockdown(U32 value);

// c10
// CP15_ReadTLBLockdown:
// CP15_WriteTLBLockdown:

// c13
// CP15_ReadFCSE_PID
// CP15_WriteFCSE_PID

//-----------------------------------------------------------------------------
//         Exported functions from CP15.c
//-----------------------------------------------------------------------------

// MMU (Status/Enable/Disable)
extern U32 CP15_IsMMUEnabled(void);
extern void CP15_EnableMMU(void);
extern void CP15_DisableMMU(void);

// I cache (Status/Enable/Disable)
extern U32 CP15_IsIcacheEnabled(void);
extern void CP15_EnableIcache(void);
extern void CP15_DisableIcache(void);

// D cache (Status/Enable/Disable)
extern U32 CP15_IsDcacheEnabled(void);
extern void CP15_EnableDcache(void);
extern void CP15_DisableDcache(void);

// complex functions
extern void CP15_LockIcache(U32 way);
extern void CP15_LockDcache(U32 way);

extern void CP15_ShutdownDcache(void);

#endif /* __CP15_H__ */
