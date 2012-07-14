; ------------------------------------------------------------------------------
;          Definitions
; ------------------------------------------------------------------------------

		GET		sfr.inc

; Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs

ARM_MODE_USR        EQU     0x10
ARM_MODE_FIQ        EQU     0x11
ARM_MODE_IRQ        EQU     0x12
ARM_MODE_SVC        EQU     0x13
ARM_MODE_ABT        EQU     0x17
ARM_MODE_UND        EQU     0x1B
ARM_MODE_SYS        EQU     0x1F

I_BIT               EQU     0x80            ; when I bit is set, IRQ is disabled
F_BIT               EQU     0x40            ; when F bit is set, FIQ is disabled

AT91C_BASE_AIC      EQU     0xFFFFF000
AIC_IVR             EQU     0x100
AIC_EOICR           EQU     0x130

UND_Stack_Size      EQU     0x00000000
SVC_Stack_Size      EQU     0x00000100
ABT_Stack_Size      EQU     0x00000000
FIQ_Stack_Size      EQU     0x00000000
IRQ_Stack_Size      EQU     0x00000080
USR_Stack_Size      EQU     0x00000400

        PRESERVE8

; ------------------------------------------------------------------------------
;    Area Definition and Entry Point
;    Startup Code must be linked first at Address at which it expects to run.
; ------------------------------------------------------------------------------

        AREA	VECTOR, CODE
        ARM
	
Vectors			; Exception Vectors
		b		HandlerReset	; Handler for Startup
		b		HandlerUndef	; Handler for Undefined mode
		b		HandlerSWI		; Handler for SWI interrupt
		b		HandlerPabort	; Handler for Prefetch Abort
		b		HandlerDabort	; Handler for Data Abort
		DCD		0xFFFF			; Set for SAM9 bootstrap
		b		HandlerIRQ		; Handler for IRQ interrupt
		b		HandlerFIQ		; Handler for FIQ interrupt

;------------------------------------------------------------------------------
;
;------------------------------------------------------------------------------
HandlerUndef
		b		HandlerUndef


;------------------------------------------------------------------------------
;
;------------------------------------------------------------------------------
HandlerSWI
		b		HandlerSWI

;------------------------------------------------------------------------------
;
;------------------------------------------------------------------------------
HandlerPabort
		b		HandlerPabort

;------------------------------------------------------------------------------
;
;------------------------------------------------------------------------------
HandlerDabort
		b		HandlerDabort

;------------------------------------------------------------------------------
; Handles incoming interrupt requests by branching to the corresponding
; handler, as defined in the AIC. Supports interrupt nesting.
;------------------------------------------------------------------------------
HandlerIRQ
        ;  Save interrupt context on the stack to allow nesting */
        SUB     lr, lr, #4
        STMFD   sp!, {lr}
        MRS     lr, SPSR
        STMFD   sp!, {r0,r1,lr}

        ; Write in the IVR to support Protect Mode */
        LDR     lr, =AT91C_BASE_AIC
        LDR     r0, [r14, #AIC_IVR]
        STR     lr, [r14, #AIC_IVR]

        ; Branch to interrupt handler in Supervisor mode */
        MSR     CPSR_c, #ARM_MODE_SVC
        STMFD   sp!, {r1-r4, r12, lr}
        MOV     lr, pc
        BX      r0
        LDMIA   sp!, {r1-r4, r12, lr}
        MSR     CPSR_c, #ARM_MODE_IRQ :OR: I_BIT

        ; Acknowledge interrupt */
        LDR     lr, =AT91C_BASE_AIC
        STR     lr, [r14, #AIC_EOICR]

        ; Restore interrupt context and branch back to calling code
        LDMIA   sp!, {r0,r1,lr}
        MSR     SPSR_cxsf, lr
        LDMIA   sp!, {pc}^
        
;------------------------------------------------------------------------------
;
;------------------------------------------------------------------------------
HandlerFIQ
		b		HandlerFIQ
		
;------------------------------------------------------------------------------
; After a reset, execution starts here, the mode is ARM, supervisor
; with interrupts disabled.
; Initializes the chip and branches to the main() function.
;------------------------------------------------------------------------------

   		AREA	STARTUP, CODE
   		ENTRY        ; Entry point for the application

        EXPORT  HandlerReset
        IMPORT	|Image$$Fixed_region$$Limit|
        IMPORT  |Image$$Relocate_region$$Base|
        IMPORT  |Image$$Relocate_region$$ZI$$Base|
        IMPORT  |Image$$Relocate_region$$ZI$$Limit|
        IMPORT  |Image$$ARM_LIB_STACK$$Base|
        IMPORT  |Image$$ARM_LIB_STACK$$ZI$$Limit|
		
HandlerReset  
        
        ; Set pc to actual code location (i.e. not in remap zone)
	    LDR     pc, =JumpStart
		
JumpStart	    
		; Set up temporary stack (Top of the SRAM)
		LDR     r0, = |Image$$ARM_LIB_STACK$$ZI$$Limit|
        MOV     sp, r0
		; Call Low level init
	    LDR     r0, =InitLowLevel
        MOV     lr, pc
        BX      r0

;		LDR		r0, =0x0
;		LDR		r1, =|Image$$ARM_LIB_STACK$$ZI$$Limit|	
;		LDR		r3, =0x77EFF000
;LoopStackInit
;		STR		r0, [r1, #0x0]
;		SUB		r1, r1, #4
;		CMP		r1, r3
;		BNE     LoopStackInit

;Initialize the Relocate_region segment 
		LDR 	r0, =|Image$$Fixed_region$$Limit|
		LDR 	r1, =|Image$$Relocate_region$$Base|
		LDR 	r3, =|Image$$Relocate_region$$ZI$$Base|
	    CMP     r0, r1                 
     	BEQ     %1

; Copy init data
0       CMP     r1, r3         
        LDRCC   r2, [r0], #4   
        STRCC   r2, [r1], #4
        BCC     %0
1       LDR     r1, =|Image$$Relocate_region$$ZI$$Limit|
        MOV     r2, #0
2       CMP     r3, r1                  
        STRCC   r2, [r3], #4
        BCC     %2
     
; Setup Stack for each mode

        LDR     R0, = |Image$$ARM_LIB_STACK$$ZI$$Limit|

;  Enter IRQ Mode and set its Stack Pointer
        MSR     CPSR_c, #ARM_MODE_IRQ:OR:I_BIT:OR:F_BIT
        MOV     SP, R0
        SUB     R4, SP, #IRQ_Stack_Size

; Supervisor mode (interrupts enabled) 
        MSR     CPSR_c, #ARM_MODE_SVC:OR:F_BIT
        MOV     SP, R4         

; Enter the C code

        IMPORT  __main
        LDR     R0, =__main
        BX      R0

InitLowLevel
; Init the basic settings

		; Disable the watch dog
		LDR		r1, =0x3EFF8FFF
		LDR		r2, =REG_WDT_MR		
		STR		r1, [r2, #0x0]

		LDR		r1, =0x1
		LDR		r2, =REG_SHDW_MR		
		STR		r1, [r2, #0x0]
		
		LDR		r1, =0xA5000001
		LDR		r2, =REG_SYS_RSTC		
		STR		r1, [r2, #0x0]
		
; Init the system clock
		LDR		r1, =0x00008001
		LDR		r2, =REG_CKGR_MOR		
		STR		r1, [r2, #0x0]
		
		; Wait the 12 MHz to be stable.
Clock_Init_Wait_1
		LDR		r2, =REG_PMC_SR
		LDR		r1, [r2, #0x0]
		AND		r1, r1, #0x1
		CMP		r1, #0x1
		BNE		Clock_Init_Wait_1
				
		LDR		r1, =0x00001301
		LDR		r2, =REG_PMC_MCKR		
		STR		r1, [r2, #0x0]

		; DIV = 3, MUL = 199 (MAIN_CLOCK = 12MHz, APLL = 800MHz, SYS_CLK = 400MHz)
		LDR		r1, =0x0
		LDR		r2, =REG_PMC_PLLICPR		
		STR		r1, [r2, #0x0]
		LDR		r1, =0x20C73F03
		LDR		r2, =REG_CKGR_PLLAR		
		STR		r1, [r2, #0x0]
	
Clock_Init_Wait_2
		LDR		r2, =REG_PMC_SR
		LDR		r1, [r2, #0x0]
		AND		r1, r1, #0x3
		CMP		r1, #0x3
		BNE		Clock_Init_Wait_2
	
		; MCLK = 133MHz, DDR_CLK = 133MHz, DDR_Rate = 266MHz */
		LDR		r1, =0x00001302
		LDR		r2, =REG_PMC_MCKR		
		STR		r1, [r2, #0x0]	

Clock_Init_Wait_3
		LDR		r2, =REG_PMC_SR
		LDR		r1, [r2, #0x0]
		AND		r1, r1, #0xF
		;LSL		r1, r1, #28
		;LSR		r1, r1, #28
		CMP		r1, #0xB
		BNE		Clock_Init_Wait_3
		
		; Enable UPLL
		LDR		r1, =0xF1F10000		
		LDR		r2, =REG_CKGR_UCKR		
		STR		r1, [r2, #0x0]

Clock_Init_Wait_4
		LDR		r2, =REG_PMC_SR
		LDR		r1, [r2, #0x0]
		LSR		r1, r1, #6
		AND		r1, r1, #0x1
		CMP		r1, #0x1
		BNE		Clock_Init_Wait_4
		
		; Enable the 32k Quartz Crystal
		LDR		r2, =REG_SCKCR
		LDR		r1, [r2, #0x0]
		CMP		r1, #0xA
		BEQ		Clock_Init_NoSCKCR
	
		LDR		r1, =0x3		
		LDR		r2, =REG_SCKCR		
		STR		r1, [r2, #0x0]

		LDR		r3, =0xD8000
		MOV		r4, #0
Init_Delay_1
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_1
		
		LDR		r1, =0xB	
		LDR		r2, =REG_SCKCR		
		STR		r1, [r2, #0x0]
		
		LDR		r3, =0xD8000
		MOV		r4, #0
Init_Delay_2
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_2
		
		LDR		r1, =0xA		
		LDR		r2, =REG_SCKCR		
		STR		r1, [r2, #0x0]
		
		LDR		r3, =0xD8000
		MOV		r4, #0
Init_Delay_3
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_3
		
Clock_Init_NoSCKCR
		LDR		r1, =0x344		
		LDR		r2, =REG_PMC_SCER		
		STR		r1, [r2, #0x0]	

		LDR		r1, =0x2C003C		
		LDR		r2, =REG_PMC_PCER		
		STR		r1, [r2, #0x0]

		LDR		r1, =0x6FD3FFC0	
		LDR		r2, =REG_PMC_PCDR		
		STR		r1, [r2, #0x0]
					
		LDR		r1, =0x106		
		LDR		r2, =REG_PMC_PCK0	; Set the PCK0 = 66.66 MHz
		STR		r1, [r2, #0x0]

		LDR		r1, =0x106	
		LDR		r2, =REG_PMC_PCK1	; Set the PCK1 = 66.66 MHz	
		STR		r1, [r2, #0x0]
	
Clock_Init_Wait_5
		LDR		r2, =REG_PMC_SR
		LDR		r1, [r2, #0x0]
		LSR		r1, r1, #8
		AND		r1, r1, #0x3
		CMP		r1, #0x3
		BNE		Clock_Init_Wait_5

; Disable All Interrupt
		LDR		r1, =0xFFFFFFFF		
		LDR		r2, =REG_AIC_IDCR		
		STR		r1, [r2, #0x0]
		
; rAIC_SVR(0) = (U32)defaultFiqHandler;
	    LDR     r1, =defaultFiqHandler
		LDR		r2, =REG_AIC_SVR0		
		STR		r1, [r2, #0x0]		
	
; for (i = 1; i < 31; i++) rAIC_SVR(i) = (U32)defaultIrqHandler;
		LDR     r1, =defaultIrqHandler
		LDR		r3, =REG_AIC_SVR31
Init_Set_AIC_SVR
		ADD		r2, r2, #4
		STR		r1, [r2, #0x0]
		CMP		r2, r3
		BNE		Init_Set_AIC_SVR

; rAIC_SPU = (U32)defaultSpuriousHandler;
		LDR     r1, =defaultSpuriousHandler
		LDR		r2, =REG_AIC_SPU			
		STR		r1, [r2, #0x0]			
		
; for (i = 0; i < 8; i++) rAIC_EOICR = 0;
		MOV		r4, #0
Init_Set_AIC_EOICR
		LDR		r1, =0x0
		LDR		r2, =REG_AIC_EOICR			
		STR		r1, [r2, #0x0]
		ADD		r4, r4, #1
		CMP		r4, #8
		BNE		Init_Set_AIC_EOICR

; Memory Remap
		LDR		r1, =((0x1 << 0) :OR: (0x1 << 1))		
		LDR		r2, =REG_MATRIX_MRCR		
		STR		r1, [r2, #0x0]

; RTT and PIT
		LDR		r1, =0x0		
		LDR		r2, =REG_RTT_MR		
		STR		r1, [r2, #0x0]
		
		LDR		r1, =0x0		
		LDR		r2, =REG_PIT_MR		
		STR		r1, [r2, #0x0]
	
; Simple GPIO init, set all GPIO as Hi-Z input
		LDR		r1, =0xFFFFFFFF
		
		LDR		r2, =REG_PIOA_IDR		
		STR		r1, [r2, #0x0]
		LDR		r2, =REG_PIOB_IDR		
		STR		r1, [r2, #0x0]
		LDR		r2, =REG_PIOC_IDR		
		STR		r1, [r2, #0x0]
		LDR		r2, =REG_PIOD_IDR		
		STR		r1, [r2, #0x0]
		LDR		r2, =REG_PIOE_IDR		
		STR		r1, [r2, #0x0]
		
		LDR		r2, =REG_PIOA_ODR		
		STR		r1, [r2, #0x0]
		LDR		r2, =REG_PIOB_ODR		
		STR		r1, [r2, #0x0]
		LDR		r2, =REG_PIOC_ODR		
		STR		r1, [r2, #0x0]
		LDR		r2, =REG_PIOD_ODR		
		STR		r1, [r2, #0x0]
		LDR		r2, =REG_PIOE_ODR		
		STR		r1, [r2, #0x0]

		LDR		r2, =REG_PIOA_PER		
		STR		r1, [r2, #0x0]
		LDR		r2, =REG_PIOB_PER		
		STR		r1, [r2, #0x0]
		LDR		r2, =REG_PIOC_PER		
		STR		r1, [r2, #0x0]
		LDR		r2, =REG_PIOD_PER		
		STR		r1, [r2, #0x0]
		LDR		r2, =REG_PIOE_PER		
		STR		r1, [r2, #0x0]
		
		; Reset the peripherals.
		LDR		r1, =0x20000000
		LDR		r2, =REG_PIOA_OER		
		STR		r1, [r2, #0x0]
		LDR		r2, =REG_PIOA_CODR		
		STR		r1, [r2, #0x0]

; Init the DDR Controller
		LDR		r0, =DDRSDRC_BASE
		LDR		r1, =0x16		
		STR		r1, [r0, #INDEX_DDRSDRC_MD]
		LDR		r1, =0x3D		
		STR		r1, [r0, #INDEX_DDRSDRC_CR]	
		LDR		r1, =0x21128226		
		STR		r1, [r0, #INDEX_DDRSDRC_TPR0]
		LDR		r1, =0x02C8100E		
		STR		r1, [r0, #INDEX_DDRSDRC_TPR1]	
		LDR		r1, =0x2072		
		STR		r1, [r0, #INDEX_DDRSDRC_TPR2]	
		LDR		r1, =0x1		
		STR		r1, [r0, #INDEX_DDRSDRC_MR]	
		LDR		r1, =0x0
		LDR		r2, =DDRSD_BASE0		
		STR		r1, [r2, #0x0]
		
		LDR		r3, =0x80000
		MOV		r4, #0
Init_Delay_4
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_4

		LDR		r1, =0x1		
		STR		r1, [r0, #INDEX_DDRSDRC_MR]	
		LDR		r1, =0x0
		LDR		r2, =DDRSD_BASE0		
		STR		r1, [r2, #0x0]		
	
		LDR		r3, =0x40000
		MOV		r4, #0
Init_Delay_5
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_5
	
		LDR		r1, =0x2		
		STR		r1, [r0, #INDEX_DDRSDRC_MR]	
		LDR		r1, =0x0
		LDR		r2, =DDRSD_BASE0		
		STR		r1, [r2, #0x0]	

		LDR		r3, =0x40000
		MOV		r4, #0
Init_Delay_6
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_6	

		LDR		r1, =0x5		
		STR		r1, [r0, #INDEX_DDRSDRC_MR]	
		LDR		r1, =0x0
		LDR		r2, =DDRSD_BASE2		
		STR		r1, [r2, #0x0]	

		LDR		r3, =0x40000
		MOV		r4, #0
Init_Delay_7
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_7	

		LDR		r1, =0x5		
		STR		r1, [r0, #INDEX_DDRSDRC_MR]	
		LDR		r1, =0x0
		LDR		r2, =DDRSD_BASE3		
		STR		r1, [r2, #0x0]	

		LDR		r3, =0x40000
		MOV		r4, #0
Init_Delay_8
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_8

		LDR		r1, =0x5		
		STR		r1, [r0, #INDEX_DDRSDRC_MR]	
		LDR		r1, =0x0
		LDR		r2, =DDRSD_BASE1		
		STR		r1, [r2, #0x0]	

		LDR		r3, =0x80000
		MOV		r4, #0
Init_Delay_9
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_9
		
		LDR		r1, [r0, #INDEX_DDRSDRC_CR]
		MOV		r4, #0xBD
		ORR		r1, r4
		STR		r1, [r0, #INDEX_DDRSDRC_CR]	
		LDR		r1, =0x3		
		STR		r1, [r0, #INDEX_DDRSDRC_MR]	
		LDR		r1, =0x0
		LDR		r2, =DDRSD_BASE0		
		STR		r1, [r2, #0x0]

		LDR		r3, =0x40000
		MOV		r4, #0
Init_Delay_10
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_10	

		LDR		r1, =0x2		
		STR		r1, [r0, #INDEX_DDRSDRC_MR]	
		LDR		r1, =0x0
		LDR		r2, =DDRSD_BASE0		
		STR		r1, [r2, #0x0]
		
		LDR		r3, =0x40000
		MOV		r4, #0
Init_Delay_11
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_11
		
		LDR		r1, =0x4		
		STR		r1, [r0, #INDEX_DDRSDRC_MR]	
		LDR		r1, =0x0
		LDR		r2, =DDRSD_BASE0		
		STR		r1, [r2, #0x0]
		
		LDR		r3, =0x40000
		MOV		r4, #0
Init_Delay_12
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_12	

		LDR		r1, =0x4		
		STR		r1, [r0, #INDEX_DDRSDRC_MR]	
		LDR		r1, =0x0
		LDR		r2, =DDRSD_BASE0		
		STR		r1, [r2, #0x0]
		
		LDR		r3, =0x40000
		MOV		r4, #0
Init_Delay_13
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_13
		
		LDR		r1, [r0, #INDEX_DDRSDRC_CR]
		MOV		r4, #0x80
		BIC		r1, r4
		STR		r1, [r0, #INDEX_DDRSDRC_CR]
		LDR		r1, =0x3		
		STR		r1, [r0, #INDEX_DDRSDRC_MR]	
		LDR		r1, =0x0
		LDR		r2, =DDRSD_BASE0		
		STR		r1, [r2, #0x0]
				
		LDR		r3, =0x40000
		MOV		r4, #0
Init_Delay_14
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_14
		
		LDR		r1, [r0, #INDEX_DDRSDRC_CR]
		LDR		r4, =0x7000
		ORR		r1, r4
		STR		r1, [r0, #INDEX_DDRSDRC_CR]
		LDR		r1, =0x5		
		STR		r1, [r0, #INDEX_DDRSDRC_MR]	
		LDR		r1, =0x0
		LDR		r2, =DDRSD_BASE1		
		STR		r1, [r2, #0x0]
		
		LDR		r3, =0x40000
		MOV		r4, #0
Init_Delay_15
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_15
		
		LDR		r1, [r0, #INDEX_DDRSDRC_CR]
		LDR		r4, =0x7000
		BIC		r1, r4
		STR		r1, [r0, #INDEX_DDRSDRC_CR]
		LDR		r1, =0x5		
		STR		r1, [r0, #INDEX_DDRSDRC_MR]	
		LDR		r1, =0x0
		LDR		r2, =DDRSD_BASE3		
		STR		r1, [r2, #0x0]		
		
		LDR		r3, =0x40000
		MOV		r4, #0
Init_Delay_16
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_16

		LDR		r1, =0x0		
		STR		r1, [r0, #INDEX_DDRSDRC_MR]	
		LDR		r1, =0x0
		LDR		r2, =DDRSD_BASE0		
		STR		r1, [r2, #0x0]
		
		LDR		r3, =0x80000
		MOV		r4, #0
Init_Delay_17
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_17
		
		LDR		r1, =0x00000411		
		STR		r1, [r0, #INDEX_DDRSDRC_RTR]	
		LDR		r1, =0x4		
		STR		r1, [r0, #INDEX_DDRSDRC_HS]	

		LDR		r3, =0x40000
		MOV		r4, #0
Init_Delay_18
		ADD		r4, r4, #1
		CMP		r4, r3
		BNE		Init_Delay_18		

; Init SMC

		BX      lr

JumpLoop
        B       JumpLoop
defaultSpuriousHandler
		B       defaultSpuriousHandler
defaultFiqHandler
		B       defaultFiqHandler
defaultIrqHandler
		B       defaultIrqHandler

        END
