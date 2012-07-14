;=================
; Basic
;=================
REG_WDT_MR				EQU		0xFFFFFD44	; Disable the dame watch dog timer
REG_SHDW_MR				EQU		0xFFFFFD14	; Enable Wake-up button
REG_SYS_RSTC			EQU		0xFFFFFD08	;

;=================
; Clock and PMC
;=================
REG_SCKCR					EQU		0xFFFFFD50 ;
REG_CKGR_MOR			EQU		0xFFFFFC20 ;
REG_CKGR_PLLAR		EQU		0xFFFFFC28 ;
REG_CKGR_UCKR			EQU		0xFFFFFC1C ;

REG_PMC_MCKR			EQU		0xFFFFFC30 ;
REG_PMC_PLLICPR		EQU		0xFFFFFC80 ;
REG_PMC_SR				EQU		0xFFFFFC68 ;
REG_PMC_SCER			EQU		0xFFFFFC00 ;
REG_PMC_SCDR			EQU		0xFFFFFC04 ;
REG_PMC_SCSR			EQU		0xFFFFFC08 ;
REG_PMC_USB				EQU		0xFFFFFC38 ;
REG_PMC_PCER			EQU		0xFFFFFC10 ;
REG_PMC_PCDR			EQU		0xFFFFFC14 ;
REG_PMC_PCSR			EQU		0xFFFFFC18 ;
REG_PMC_PCK0			EQU		0xFFFFFC40 ;
REG_PMC_PCK1			EQU		0xFFFFFC44 ;

;=================
; AIC
;=================
REG_AIC_IDCR			EQU		0xFFFFF124
REG_AIC_SVR0			EQU		0xFFFFF080
REG_AIC_SVR31			EQU		0xFFFFF0FC
REG_AIC_SPU				EQU		0xFFFFF134
REG_AIC_EOICR			EQU		0xFFFFF130

;=================
; Remap
;=================
REG_MATRIX_MRCR		EQU		0xFFFFEB00 ; Master Remap Control Register Read-write 0x00000000

;=================
; RTT and PIT
;=================
REG_RTT_MR				EQU		0xFFFFFD20 ; Mode Register Read-write 0x0000_8000
REG_PIT_MR				EQU		0xFFFFFD30 ; Mode Register Read-write 0x000F_FFFF

;=================
; GPIO
;=================

REG_PIOA_IDR			EQU		0xFFFFF244 ; Interrupt Disable Register Write-only
REG_PIOB_IDR			EQU 	0xFFFFF444 ; Interrupt Disable Register Write-only
REG_PIOC_IDR			EQU 	0xFFFFF644 ; Interrupt Disable Register Write-only
REG_PIOD_IDR			EQU 	0xFFFFF844 ; Interrupt Disable Register Write-only
REG_PIOE_IDR			EQU		0xFFFFFA44 ; Interrupt Disable Register Write-only

REG_PIOA_PDR			EQU		0xFFFFF204 ; PIO Disable Register Write-only
REG_PIOB_PDR			EQU		0xFFFFF404 ; PIO Disable Register Write-only
REG_PIOC_PDR			EQU		0xFFFFF604 ; PIO Disable Register Write-only
REG_PIOD_PDR			EQU		0xFFFFF804 ; PIO Disable Register Write-only
REG_PIOE_PDR			EQU		0xFFFFFA04 ; PIO Disable Register Write-only

REG_PIOA_PER			EQU		0xFFFFF200 ; PIO Disable Register Write-only
REG_PIOB_PER			EQU		0xFFFFF400 ; PIO Disable Register Write-only
REG_PIOC_PER			EQU		0xFFFFF600 ; PIO Disable Register Write-only
REG_PIOD_PER			EQU		0xFFFFF800 ; PIO Disable Register Write-only
REG_PIOE_PER			EQU		0xFFFFFA00 ; PIO Disable Register Write-only

REG_PIOA_ODR			EQU		0xFFFFF214 ; Output Disable Register Write-only
REG_PIOB_ODR			EQU		0xFFFFF414 ; Output Disable Register Write-only
REG_PIOC_ODR			EQU		0xFFFFF614 ; Output Disable Register Write-only
REG_PIOD_ODR			EQU		0xFFFFF814 ; Output Disable Register Write-only
REG_PIOE_ODR			EQU		0xFFFFFA14 ; Output Disable Register Write-only

REG_PIOA_OER			EQU		0xFFFFF210 ; Output Enable Register Write-only
REG_PIOA_SODR			EQU		0xFFFFF230 ; Set Output Data Register Write-only
REG_PIOA_CODR			EQU		0xFFFFF234 ; Clear Output Data Register Write-only
REG_PIOD_ASR			EQU		0xFFFFF870 ; Peripheral A Select Register Write-only
REG_PIOD_BSR			EQU		0xFFFFF874 ; Peripheral B Select Register Write-only

;=================
; DDRSDRC
;=================
DDRSDRC_BASE			EQU		0xFFFFE600

INDEX_DDRSDRC_MR				EQU		0x0		; Mode Register Read-write 0x00000000
INDEX_DDRSDRC_RTR				EQU		0x4		; Refresh Timer Register  Read-write 0x00000000
INDEX_DDRSDRC_CR				EQU		0x8		; Configuration Register  Read-write 0x7024
INDEX_DDRSDRC_TPR0			EQU		0xC		; Timing Parameter 0 Register Read-write 0x20227225
INDEX_DDRSDRC_TPR1			EQU		0x10	; Timing Parameter 1 Register Read-write 0x3c80808
INDEX_DDRSDRC_TPR2			EQU		0x14	; Timing Parameter 2 Register Read-write 0x2062
INDEX_DDRSDRC_LPR				EQU		0x1C	; Low-power Register Read-write 0x10000
INDEX_DDRSDRC_MD				EQU		0x20 	; Memory Device Register Read-write 0x10
INDEX_DDRSDRC_DLL				EQU		0x24 	; DLL Information Register Read-only 0x00000001
INDEX_DDRSDRC_HS				EQU		0x2C 	; High Speed Register Read-write 0x0
INDEX_DDRSDRC_DELAY1		EQU		0x40 	; Delay I/O Register Read-write 0x00000000
INDEX_DDRSDRC_DELAY2		EQU		0x44 	; Delay I/O Register Read-write 0x00000000
INDEX_DDRSDRC_DELAY3		EQU		0x48 	; Delay I/O Register Read-write 0x00000000
INDEX_DDRSDRC_DELAY4		EQU		0x4C 	; Delay I/O Register Read-write 0x00000000
INDEX_DDRSDRC_WPMR			EQU		0xE4 	; Write Protect Mode Register Read-write 0x00000000
INDEX_DDRSDRC_WPSR			EQU		0xE8	; Write Protect Status Register

DDRSD_BASE0				EQU		0x70000000
DDRSD_BASE1				EQU		0x72000000
DDRSD_BASE2				EQU		0x74000000
DDRSD_BASE3				EQU		0x76000000


						END