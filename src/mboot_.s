        AREA  bootm_init, CODE
		
		EXPORT BootmSet_ParamAddr
		EXPORT BootmSet_MachineID
		
BootmSet_ParamAddr
		;r0= paddr
		MOV		r2, r0		
        BX      lr
		
BootmSet_MachineID
		;r0= mid
		MOV		r1, r0
		
		MOV		r0, #0
		MOV		ip, #0
		MCR		p15, 0, ip, c13, c0, 0
		MCR		p15, 0, ip, c7, c7, 0
		MCR		p15, 0, ip, c7, c10, 4
		MCR		p15, 0, ip, c8, c7, 0
		MRC		p15, 0, ip, c1, c0, 0
		BIC		ip, ip, #0x0001
		MCR		p15, 0, ip, c1, c0, 0
		BX      lr

        END
