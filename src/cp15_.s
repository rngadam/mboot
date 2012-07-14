; ------------------------------------------------------------------------------
;          mboot
; ------------------------------------------------------------------------------

        AREA  cp15, CODE

;------------------------------------------------------------------------------
;         Headers
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Functions to access CP15 coprocessor register
;------------------------------------------------------------------------------
        ; c0
        EXPORT  CP15_ReadID
        EXPORT  CP15_ReaDcacheType
        EXPORT  CP15_ReadTCMStatus
        ; c1        
        EXPORT  CP15_ReadControl
        EXPORT  CP15_WriteControl
        ; c2        
        EXPORT  CP15_ReadTTB
        EXPORT  CP15_WriteTTB
        ; c3        
        EXPORT  CP15_ReadDomainAccessControl
        EXPORT  CP15_WriteDomainAccessControl
        ; c7        
        EXPORT  CP15_InvalidateIDcache
        EXPORT  CP15_InvalidateDcache
        EXPORT  CP15_InvalidateIcache
        EXPORT  CP15_PrefetchIcacheLine
        EXPORT  CP15_TestCleanInvalidateDcache
		EXPORT  CP15_CleanInvalidateDcacheIndex
        EXPORT  CP15_DrainWriteBuffer
        EXPORT  CP15_WaitForInterrupt
        ; c8        
        EXPORT  CP15_InvalidateTLB
        EXPORT  CP15_InvalidateTLBMVA
        EXPORT  CP15_InvalidateITLB
        EXPORT  CP15_InvalidateITLBMVA
        EXPORT  CP15_InvalidateDTLB
        EXPORT  CP15_InvalidateDTLBMVA    
        ; c9        
        EXPORT  CP15_ReadDcacheLockdown
        EXPORT  CP15_WriteDcacheLockdown
        EXPORT  CP15_ReadIcacheLockdown 
        EXPORT  CP15_WriteIcacheLockdown
        EXPORT  CP15_ReadTLBLockdown
        EXPORT  CP15_WriteTLBLockdown 
        ; c13        
        EXPORT  CP15_ReadTLBLockdown
        EXPORT  CP15_WriteTLBLockdown        
        ; c13        
        EXPORT  CP15_ReadFCSE_PID
        EXPORT  CP15_WriteFCSE_PID

;------------------------------------------------------------------------------
; c0
; Register c0 accesses the ID Register, Cache Type Register, and TCM Status Registers.
; Reading from this register returns the device ID, the cache type, or the TCM status
; depending on the value of Opcode_2 used:
; Opcode_2 = 0 ID value.
; Opcode_2 = 1 instruction and data cache type.
; Opcode_2 = 2 TCM status.
;------------------------------------------------------------------------------

; C0 read ID
CP15_ReadID
        MOV     r0, #0
        MRC     p15, 0, r0, c0, c0, 0
        BX      lr

; C0 read Cache Type
CP15_ReaDcacheType
        MOV     r0, #0
        MRC     p15, 0, r0, c0, c0, 1
        BX      lr

; C0 read TCM status
CP15_ReadTCMStatus
        MOV     r0, #0
        MRC     p15, 0, r0, c0, c0, 2
        BX      lr

;------------------------------------------------------------------------------
; Control Register c1
; Register c1 is the Control Register for the ARM926EJ-S processor.
; This register specifies the configuration used to enable and disable the
; caches and MMU. It is recommended that you access this register using a
; read-modify-write sequence.
;------------------------------------------------------------------------------
; CP15 Read Control Register
CP15_ReadControl
        MOV     r0, #0
        MRC     p15, 0, r0, c1, c0, 0   
        BX      lr

; CP15 Write Control Register
CP15_WriteControl
        MCR     p15, 0, r0, c1, c0, 0
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP        
        BX      lr

;------------------------------------------------------------------------------
; CP15 Translation Table Base Register c2
; Register c2 is the Translation Table Base Register (TTBR), for the base
; address of the first-level translation table.
; Reading from c2 returns the pointer to the currently active first-level
; translation table in bits [31:14] and an Unpredictable value in bits [13:0].
; Writing to register c2 updates the pointer to the first-level translation
; table from the value in bits [31:14] of the written value. Bits [13:0]
; Should Be Zero.
; You can use the following instructions to access the TTBR:
; Read TTBR  : MRC p15, 0, <Rd>, c2, c0, 0
; Write TTBR : MCR p15, 0, <Rd>, c2, c0, 0
;------------------------------------------------------------------------------
CP15_ReadTTB
        MOV     r0, #0
        MRC     p15, 0, r0, c2, c0, 0
        BX      lr

CP15_WriteTTB
        MCR     p15, 0, r0, c2, c0, 0
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        BX      lr

;------------------------------------------------------------------------------
; Domain Access Control Register c3
; Read domain access permissions  : MRC p15, 0, <Rd>, c3, c0, 0
; Write domain access permissions : MCR p15, 0, <Rd>, c3, c0, 0
;------------------------------------------------------------------------------
CP15_ReadDomainAccessControl
        MOV     r0, #0
        MRC     p15, 0, r0, c3, c0, 0
        BX      lr

CP15_WriteDomainAccessControl
        MCR     p15, 0, r0, c3, c0, 0
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        BX      lr

;------------------------------------------------------------------------------
; Fault Status Registers Register c5
; Register c5 accesses the Fault Status Registers (FSRs). The FSRs contain the source of
; the last instruction or data fault. The instruction-side FSR is intended for debug
; purposes only. The FSR is updated for alignment faults, and external aborts that occur
; while the MMU is disabled.
; The FSR accessed is determined by the value of the Opcode_2 field:
; Opcode_2 = 0  Data Fault Status Register (DFSR).
; Opcode_2 = 1  Instruction Fault Status Register (IFSR).
; The fault type encoding is listed in Table 3-9 on page 3-22.
; You can access the FSRs using the following instructions:
; MRC p15, 0, <Rd>, c5, c0, 0 ;read DFSR
; MCR p15, 0, <Rd>, c5, c0, 0 ;write DFSR
; MRC p15, 0, <Rd>, c5, c0, 1 ;read IFSR
; MCR p15, 0, <Rd>, c5, c0, 1 ;write IFSR
;------------------------------------------------------------------------------

CP15_ReadDFSR
        MOV     r0, #0
        MRC     p15, 0, r0, c5, c0, 0
        BX      lr

CP15_writeDFSR
        MCR     p15, 0, r0, c5, c0, 0
        BX      lr

CP15_ReadIFSR
        MOV     r0, #0
        MRC     p15, 0, r0, c5, c0, 1
        BX      lr

CP15_WriteIFSR
        MCR     p15, 0, r0, c5, c0, 1
        BX      lr

;------------------------------------------------------------------------------
; Fault Address Register c6
; Register c6 accesses the Fault Address Register (FAR). The FAR contains the Modified
; Virtual Address of the access being attempted when a Data Abort occurred. The FAR is
; only updated for Data Aborts, not for Prefetch Aborts. The FAR is updated for
; alignment faults, and external aborts that occur while the MMU is disabled.
; You can use the following instructions to access the FAR:
; MRC p15, 0, <Rd>, c6, c0, 0 ; read FAR
; MCR p15, 0, <Rd>, c6, c0, 0 ; write FAR
;------------------------------------------------------------------------------
CP15_ReadFAR
        MOV     r0, #0
        MRC     p15, 0, r0, c6, c0, 0
        BX      lr

CP15_writeFAR
        MCR     p15, 0, r0, c6, c0, 0
        BX      lr

;------------------------------------------------------------------------------
; Control functions caches and the write buffer c7
; Register c7 controls the caches and the write buffer. The function of each cache
; operation is selected by the Opcode_2 and CRm fields in the MCR instruction used to
; write to CP15 c7. Writing other Opcode_2 or CRm values is Unpredictable.
; Reading from CP15 c7 is Unpredictable, with the exception of the two test and clean
; operations (see Table 2-18 on page 2-21 and Test and clean operations on page 2-23).
; You can use the following instruction to write to c7:
; MCR p15, <Opcode_1>, <Rd>, <CRn>, <CRm>, <Opcode_2>
;------------------------------------------------------------------------------
; Invalidate Icache and Dcache                        MCR p15, 0, <Rd>, c7, c7, 0
; Invalidate Icache                                   MCR p15, 0, <Rd>, c7, c5, 0
; Invalidate Icache single entry (MVA) MVA            MCR p15, 0, <Rd>, c7, c5, 1
; Invalidate Icache single entry (Set/Way) Set/Way    MCR p15, 0, <Rd>, c7, c5, 2
; Prefetch Icache line (MVA) MVA                      MCR p15, 0, <Rd>, c7, c13, 1
; Invalidate Dcache                                   MCR p15, 0, <Rd>, c7, c6, 0
; Invalidate Dcache single entry (MVA) MVA            MCR p15, 0, <Rd>, c7, c6, 1
; Invalidate Dcache single entry (Set/Way) Set/Way    MCR p15, 0, <Rd>, c7, c6, 2
; Clean Dcache single entry (MVA) MVA                 MCR p15, 0, <Rd>, c7, c10, 1
; Clean Dcache single entry (Set/Way) Set/Way         MCR p15, 0, <Rd>, c7, c10, 2
; Test and clean Dcache -                             MRC p15, 0, <Rd>, c7, c10, 3
; Clean and invalidate Dcache entry (MVA)  MVA        MCR p15, 0, <Rd>, c7, c14, 1
; Clean and invalidate Dcache entry (Set/Way) Set/Way MCR p15, 0, <Rd>, c7, c14, 2
; Test, clean, and invalidate Dcache -                MRC p15, 0, <Rd>, c7, c14, 3
; Drain write buffer SBZ                              MCR p15, 0, <Rd>, c7, c10, 4
; Wait for interrupt SBZ                              MCR p15, 0, <Rd>, c7, c0, 4
;------------------------------------------------------------------------------

; Invalidate Icache and Dcache
CP15_InvalidateIDcache
        MOV     r0, #0
        MCR     p15, 0, r0, c7, c7, 0        
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP          
        BX      lr
        
; Invalidate Icache
CP15_InvalidateIcache
        MOV     r0, #0
        MCR     p15, 0, r0, c7, c5, 0     
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP          
        BX      lr

; Invalidate Dcache
CP15_InvalidateDcache
        MOV     r0, #0
        MCR     p15, 0, r0, c7, c6, 0
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP          
        BX      lr

;------------------------------------------------------------------------------
; CP15 Prefetch Icache line c7
; Performs an Icache lookup of the specified modified virtual address.
; If the cache misses, and the region is cacheable, a linefill is performed.
; Prefetch Icache line (MVA): MCR p15, 0, <Rd>, c7, c13, 1
;------------------------------------------------------------------------------
CP15_PrefetchIcacheLine
        MCR     p15, 0, r0, c7, c13, 1
        BX      lr

;------------------------------------------------------------------------------
; CP15 Test, clean, and invalidate Dcache c7
; As for test and clean, except that when the entire cache has 
; been tested and cleaned, it is invalidated.
;------------------------------------------------------------------------------
CP15_TestCleanInvalidateDcache
        MRC     p15, 0, r0, c7, c14, 3
        BNE     CP15_TestCleanInvalidateDcache
        BX      lr

CP15_CleanInvalidateDcacheIndex
		;r0=index
		MCR p15,0,r0,c7,c14,2
        BX      lr

;------------------------------------------------------------------------------
; CP15 Drain write buffer c7
; This instruction acts as an explicit memory barrier. It drains 
; the contents of the write buffers of all memory stores 
; occurring in program order before this instruction is 
; completed. No instructions occurring in program order 
; after this instruction are executed until it completes. This 
; can be used when timing of specific stores to the level two 
; memory system has to be controlled (for example, when a 
; store to an interrupt acknowledge location has to complete 
; before interrupts are enabled).
;------------------------------------------------------------------------------
CP15_DrainWriteBuffer
        MOV     r0, #0
        MCR     p15, 0, r0, c7, c10, 4
        BX      lr

;------------------------------------------------------------------------------
; CP15 Wait For Interrupt operation c7
; The purpose of the Wait For Interrupt operation is to put the processor in
; to a low power state.
; This puts the processor into a low-power state and stops it executing more
; instructions until an interrupt, or debug request occurs, regardless of
; whether the interrupts are disabled by the masks in the CPSR.
; When an interrupt does occur, the MCR instruction completes and the IRQ or
; FIQ handler is entered as normal. The return link in r14_irq or r14_fiq
; contains the address of the MCR instruction plus 8, so that the normal
; instruction used for interrupt return (SUBS PC,R14,#4) returns to the
; instruction following the MCR.
; Wait For Interrupt : MCR p15, 0, <Rd>, c7, c0, 4
;------------------------------------------------------------------------------
CP15_WaitForInterrupt
        MOV     r0, #0
        MCR     p15, 0, r0, c7, c0, 4
        BX      lr

;------------------------------------------------------------------------------
; Translation Lookaside Buffer (TLB) : c8
; This is a write-only register used to control the Translation Lookaside Buffer (TLB).
; There is a single TLB used to hold entries for both data and instructions. The TLB is
; divided into two parts:
; • a set-associative part
; • a fully-associative part.
; The fully-associative part (also referred to as the lockdown part of the TLB) is used to
; store entries to be locked down. Entries held in the lockdown part of the TLB are
; preserved during an invalidate TLB operation. Entries can be reMOVed from the
; lockdown TLB using an invalidate TLB single entry operation.
; Six TLB operations are defined, and the function to be performed is selected by the
; Opcode_2 and CRm fields in the MCR instruction used to write CP15 c8. Writing other
; Opcode_2 or CRm values is Unpredictable. Reading from this register is Unpredictable.
;------------------------------------------------------------------------------
; Invalidate TLB                                 MCR p15, 0, <Rd>, c8, c7, 0
; Invalidate TLB single entry (MVA)              MCR p15, 0, <Rd>, c8, c7, 1
; Invalidate instruction TLB                     MCR p15, 0, <Rd>, c8, c5, 0
; Invalidate instruction TLB single entry (MVA)  MCR p15, 0, <Rd>, c8, c5, 1
; Invalidate data TLB                            MCR p15, 0, <Rd>, c8, c6, 0
; Invalidate data TLB single entry (MVA)         MCR p15, 0, <Rd>, c8, c6, 1
;------------------------------------------------------------------------------

CP15_InvalidateTLB
        MOV     r0, #0
        MCR     p15, 0, r0, c8, c7, 0
        BX      lr

CP15_InvalidateTLBMVA
        MCR     p15, 0, r0, c8, c7, 1
        BX      lr

CP15_InvalidateITLB
        MOV     r0, #0
        MCR     p15, 0, r0, c8, c5, 0
        BX      lr

CP15_InvalidateITLBMVA
        MCR     p15, 0, r0, c8, c5, 1
        BX      lr

CP15_InvalidateDTLB
        MOV     r0, #0
        MCR     p15, 0, r0, c8, c6, 0
        BX      lr

CP15_InvalidateDTLBMVA
        MCR     p15, 0, r0, c8, c6, 1
        BX      lr

;------------------------------------------------------------------------------
; Cache Lockdown Register c9
; The Cache Lockdown Register uses a cache-way-based locking scheme (Format C) that
; enables you to control each cache way independently.
; These registers enable you to control which cache ways of the four-way cache are used
; for the allocation on a linefill. When the registers are defined, subsequent linefills are
; only placed in the specified target cache way. This gives you some control over the
; cache pollution caused by particular applications, and provides a traditional lockdown
; operation for locking critical code into the cache.
;------------------------------------------------------------------------------
; Read Dcache Lockdown Register   MRC p15,0,<Rd>,c9,c0,0
; Write Dcache Lockdown Register  MCR p15,0,<Rd>,c9,c0,0
; Read Icache Lockdown Register   MRC p15,0,<Rd>,c9,c0,1
; Write Icache Lockdown Register  MCR p15,0,<Rd>,c9,c0,1
;------------------------------------------------------------------------------

CP15_ReadDcacheLockdown
        MOV     r0, #0
        MRC     p15, 0, r0, c9, c0, 0
        BX      lr

CP15_WriteDcacheLockdown
        MCR     p15, 0, r0, c9, c0, 0
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP          
        BX      lr

CP15_ReadIcacheLockdown
        MOV     r0, #0
        MRC     p15, 0, r0, c9, c0, 1
        BX      lr

CP15_WriteIcacheLockdown
        MCR     p15, 0, r0, c9, c0, 1
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP          
        BX      lr

;------------------------------------------------------------------------------
; TLB Lockdown Register c10
; The TLB Lockdown Register controls where hardware page table walks place the
; TLB entry, in the set associative region or the lockdown region of the TLB,
; and if in the lockdown region, which entry is written. The lockdown region
; of the TLB contains eight entries. See TLB structure for a description of
; the structure of the TLB.
;------------------------------------------------------------------------------
; Read data TLB lockdown victim    MRC p15,0,<Rd>,c10,c0,0
; Write data TLB lockdown victim   MCR p15,0,<Rd>,c10,c0,0
;------------------------------------------------------------------------------
CP15_ReadTLBLockdown
        MOV     r0, #0
        MRC     p15, 0, r0, c10, c0, 0
        BX      lr

CP15_WriteTLBLockdown
        MCR     p15, 0, r0, c10, c0, 0
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        BX      lr

;------------------------------------------------------------------------------
; Register c13 accesses the process identifier registers. The register accessed depends on
; the value of the Opcode_2 field:
; Opcode_2 = 0  Selects the Fast Context Switch Extension (FCSE) Process Identifier (PID) Register.
; Opcode_2 = 1  Selects the Context ID Register.
;------------------------------------------------------------------------------
; FCSE PID Register
; Addresses issued by the ARM9EJ-S core in the range 0 to 32MB are translated in
; accordance with the value contained in this register. Address A becomes A + (FCS
; PID x 32MB). It is this modified address that is seen by the caches, MMU, and TC
; interface. Addresses above 32MB are not modified. The FCSE PID is a seven-bit fie
; enabling 128 x 32MB processes to be mapped.
; If the FCSE PID is 0, there is a flat mapping between the virtual addresses output by
; ARM9EJ-S core and the modified virtual addresses used by the caches, MMU, and
; TCM interface. The FCSE PID is set to 0 at system reset.
; If the MMU is disabled, then no FCSE address translation occurs.
; FCSE translation is not applied for addresses used for entry based cache or TLB
; maintenance operations. For these operations VA = MVA.
;------------------------------------------------------------------------------
; Read FCSE PID  MRC p15,0,<Rd>,c13,c0, 0
; Write FCSE PID MCR p15,0,<Rd>,c13,c0, 0
;------------------------------------------------------------------------------
; Context ID Register
; The Context ID Register provides a mechanism to allow real-time trace tools to identify
; the currently executing process in multi-tasking environments.
; The contents of this register are replicated on the ETMPROCID pins of the
; ARM926EJ-S processor. ETMPROCIDWR is pulsed when a write occurs to the
; Context ID Register.
;------------------------------------------------------------------------------
; Read context ID  MRC p15,0,<Rd>,c13,c0, 1
; Write context ID MCR p15,0,<Rd>,c13,c0, 1
;------------------------------------------------------------------------------
CP15_ReadFCSE_PID
        MOV     r0, #0
        MRC     p15, 0, r0, c13, c0, 0
        BX      lr

CP15_WriteFCSE_PID
        MCR     p15, 0, r0, c13, c0, 0
        BX      lr
    END
