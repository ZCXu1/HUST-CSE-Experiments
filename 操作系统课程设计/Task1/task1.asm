%include	"pm.inc"	; 常量, 宏, 以及一些说明

PageDirBase1	equ	200000h	; 页目录开始地址:	2M
PageTabBase1	equ	201000h	; 页表开始地址:		2M +  4K
PageDirBase2	equ	210000h	; 页目录开始地址:	2M + 64K
PageTabBase2	equ	211000h	; 页表开始地址:		2M + 64K + 4K

LinearAddrDemo	equ	00401000h
TaskHUST		equ	00401000h
TaskIS19		equ	00501000h

org	0100h
	jmp	LABEL_BEGIN

[SECTION .gdt]
; GDT
;                                      段基址,        段界限,            属性
LABEL_GDT:		Descriptor      0,              0,              0				; 空描述符
LABEL_DESC_NORMAL:	Descriptor      0,              0ffffh,         DA_DRW			        ; Normal 描述符
LABEL_DESC_FLAT_C:	Descriptor      0,              0fffffh,        DA_CR | DA_32 | DA_LIMIT_4K     ; 0 ~ 4G
LABEL_DESC_FLAT_RW:	Descriptor      0,              0fffffh,        DA_DRW | DA_LIMIT_4K	        ; 0 ~ 4G
LABEL_DESC_CODE32:	Descriptor      0,              SegCode32Len-1, DA_CR | DA_32		        ; 非一致代码段, 32
LABEL_DESC_CODE16:	Descriptor      0,              0ffffh,         DA_C			        ; 非一致代码段, 16
LABEL_DESC_DATA:	Descriptor      0,              DataLen-1,      DA_DRW			        ; Data
LABEL_DESC_STACK:	Descriptor      0,              TopOfStack,     DA_DRWA | DA_32		        ; Stack, 32 位
LABEL_DESC_LDT_A:       Descriptor      0,              LDTALen-1,      DA_LDT		                ; LDTA
LABEL_DESC_LDT_B:       Descriptor      0,              LDTBLen-1,      DA_LDT                          ; LDTB
LABEL_DESC_TSS_A:       Descriptor      0,              TSSALen-1,      DA_386TSS	                ; TSSA
LABEL_DESC_TSS_B:       Descriptor      0,              TSSBLen-1,      DA_386TSS                       ; TSSB
LABEL_DESC_VIDEO:	Descriptor      0B8000h,        0ffffh,         DA_DRW			        ; 显存首地址
; GDT 结束

GdtLen		equ	$ - LABEL_GDT	; GDT长度
GdtPtr		dw	GdtLen - 1	; GDT界限
		dd	0		; GDT基地址

; GDT 选择子
SelectorNormal	        equ	LABEL_DESC_NORMAL	- LABEL_GDT
SelectorFlatC		equ	LABEL_DESC_FLAT_C	- LABEL_GDT
SelectorFlatRW		equ	LABEL_DESC_FLAT_RW	- LABEL_GDT
SelectorCode32		equ	LABEL_DESC_CODE32	- LABEL_GDT
SelectorCode16		equ	LABEL_DESC_CODE16	- LABEL_GDT
SelectorData		equ	LABEL_DESC_DATA		- LABEL_GDT
SelectorStack		equ	LABEL_DESC_STACK	- LABEL_GDT
SelectorLDTA		equ	LABEL_DESC_LDT_A	- LABEL_GDT
SelectorLDTB		equ	LABEL_DESC_LDT_B	- LABEL_GDT
SelectorTSSA		equ	LABEL_DESC_TSS_A	- LABEL_GDT
SelectorTSSB		equ	LABEL_DESC_TSS_B	- LABEL_GDT
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT
; END of [SECTION .gdt]

[SECTION .data1]	 ; 数据段
ALIGN	32
[BITS	32]
LABEL_DATA:
; 实模式下使用这些符号
; 字符串
_szPMMessage:			db	"It is Protect Mode now.", 0Ah, 0Ah, 0	                        ; 进入保护模式后显示此字符串
_szMemChkTitle:			db	"BaseAddrL BaseAddrH LengthLow LengthHigh   Type", 0Ah, 0	; 进入保护模式后显示此字符串
_szRAMSize			db	"RAM size:", 0
_szReturn			db	0Ah, 0
; 变量
_wSPValueInRealMode		dw	0
_dwMCRNumber:			dd	0	                ; Memory Check Result
_dwDispPos:			dd	(80 * 6 + 0) * 2	; 屏幕第 6 行, 第 0 列。
_dwMemSize:			dd	0
_ARDStruct:		        ; Address Range Descriptor Structure
	_dwBaseAddrLow:		dd	0
	_dwBaseAddrHigh:	dd	0
	_dwLengthLow:		dd	0
	_dwLengthHigh:		dd	0
	_dwType:		dd	0
_PageTableNumber:		dd	0
_SavedIDTR:			dd	0	; 用于保存 IDTR
				dd	0
_SavedIMREG:			db	0	; 中断屏蔽寄存器值
_current:                       db      0       ; # current记录应该执行task0还是task1
_MemChkBuf:	times	256	db	0

; 保护模式下使用这些符号
szPMMessage		equ	_szPMMessage	- $$
szMemChkTitle		equ	_szMemChkTitle	- $$
szRAMSize		equ	_szRAMSize	- $$
szReturn		equ	_szReturn	- $$
dwDispPos		equ	_dwDispPos	- $$
dwMemSize		equ	_dwMemSize	- $$
dwMCRNumber		equ	_dwMCRNumber	- $$
ARDStruct		equ	_ARDStruct	- $$
	dwBaseAddrLow	equ	_dwBaseAddrLow	- $$
	dwBaseAddrHigh	equ	_dwBaseAddrHigh	- $$
	dwLengthLow	equ	_dwLengthLow	- $$
	dwLengthHigh	equ	_dwLengthHigh	- $$
	dwType		equ	_dwType		- $$
MemChkBuf		equ	_MemChkBuf	- $$
SavedIDTR		equ	_SavedIDTR	- $$
SavedIMREG		equ	_SavedIMREG	- $$
PageTableNumber		equ	_PageTableNumber- $$
current                 equ     _current        - $$    ; # current记录应该执行task0还是task1

DataLen			equ	$ - LABEL_DATA
; END of [SECTION .data1]


; IDT
[SECTION .idt]
ALIGN	32
[BITS	32]
LABEL_IDT:
;      门           目标选择子,       偏移,    DCount, 属性
%rep 255
	Gate	SelectorCode32, ClockHandler,   0, DA_386IGate
%endrep

IdtLen	equ	$ - LABEL_IDT
IdtPtr	dw	IdtLen - 1	; 段界限
	dd	0		; 基地址
; END of [SECTION .idt]


; 全局堆栈段
[SECTION .gs]
ALIGN	32
[BITS	32]
LABEL_STACK:
	times 512 db 0

TopOfStack	equ	$ - LABEL_STACK - 1

; END of [SECTION .gs]

; TSS_A --------------------------------------------------------------
[SECTION .tss_a]
ALIGN	32
[BITS	32]
LABEL_TSS_A:
        DD	0			; Back
        DD	0		        ; 0 级堆栈
        DD	0       		; 
        DD	0			; 1 级堆栈
        DD	0			; 
        DD	0			; 2 级堆栈
        DD	0			; 
        DD	PageDirBase1		; CR3，分页
        DD	0			; EIP
        DD	0200h		        ; EFLAGS
        DD	0			; EAX
        DD	0			; ECX
        DD	0			; EDX
        DD	0			; EBX
        DD	TopOfStackA		; ESP
        DD	0			; EBP
        DD	0			; ESI
        DD	PageDirBase1    	; EDI
        DD	SelectorFlatRW 		; ES
        DD	SelectorLDTCodeA	; CS
        DD	SelectorLDTStackA	; SS
        DD	SelectorData		; DS
        DD	0			; FS
        DD	0			; GS
        DD	SelectorLDTA		; LDT
        DW	0			; 调试陷阱标志
        DW	$ - LABEL_TSS_A + 2	; I/O位图基址
        DB	0ffh			; I/O位图结束标志
TSSALen		equ	$ - LABEL_TSS_A
; TSS_A --------------------------------------------------------------


; TSS_B --------------------------------------------------------------
ALIGN	32
[BITS	32]
LABEL_TSS_B:
		DD	0			; Back
		DD	0		        ; 0 级堆栈
		DD	0		        ; 
		DD	0			; 1 级堆栈
		DD	0			; 
		DD	0			; 2 级堆栈
		DD	0			; 
		DD	PageDirBase2		; CR3
		DD	0			; EIP
		DD	0200h			; EFLAGS
		DD	0			; EAX
		DD	0			; ECX
		DD	0			; EDX
		DD	0			; EBX
		DD	0			; ESP
		DD	0			; EBP
		DD	0			; ESI
		DD	PageDirBase2		; EDI
		DD	SelectorFlatRW          ; ES
		DD	SelectorLDTCodeB	; CS
		DD	SelectorLDTStackB	; SS
		DD	SelectorData		; DS
		DD	0			; FS
		DD	0			; GS
		DD	SelectorLDTB		; LDT
		DW	0			; 调试陷阱标志
		DW	$ - LABEL_TSS_B + 2	; I/O位图基址
		DB	0ffh			; I/O位图结束标志
TSSBLen		equ	$ - LABEL_TSS_B
; TSS_B --------------------------------------------------------------


[SECTION .s16]
[BITS	16]
LABEL_BEGIN:
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 0100h

	mov	[LABEL_GO_BACK_TO_REAL+3], ax
	mov	[_wSPValueInRealMode], sp

	; 得到内存数
	mov	ebx, 0
	mov	di, _MemChkBuf
.loop:
	mov	eax, 0E820h
	mov	ecx, 20
	mov	edx, 0534D4150h
	int	15h
	jc	LABEL_MEM_CHK_FAIL
	add	di, 20
	inc	dword [_dwMCRNumber]
	cmp	ebx, 0
	jne	.loop
	jmp	LABEL_MEM_CHK_OK
LABEL_MEM_CHK_FAIL:
	mov	dword [_dwMCRNumber], 0
LABEL_MEM_CHK_OK:

	; # 段描述符结构如下：
        ; # |----------------------------64bits-----------------------------|
        ; # | BYTE7 | BYTE6 | BYTE5 | BYTE4 | BYTE3 | BYTE2 | BYTE1 | BYTE0 |
        ; # |段基址2|      属性     |        段基址1        |     段界限1   |

	; 初始化 16 位代码段描述符
	mov	ax, cs                                  ; # 段基址给ax
	movzx	eax, ax                                 ; # 零位扩展ax
	shl	eax, 4                                  ; # 段基址左移4位
	add	eax, LABEL_SEG_CODE16                   ; # 逻辑地址 = 段基址左移4位 + 段内偏移地址
	mov	word [LABEL_DESC_CODE16 + 2], ax        ; # 段基址低16位存在段基址1中
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE16 + 4], al        ; # 段基址第16至23位存在段基址1高位中
	mov	byte [LABEL_DESC_CODE16 + 7], ah        ; # 段基址第24至31位存在段基址2中

	; 初始化 32 位代码段描述符
	xor	eax, eax
	mov	ax, cs
	shl	eax, 4
	add	eax, LABEL_SEG_CODE32
	mov	word [LABEL_DESC_CODE32 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE32 + 4], al
	mov	byte [LABEL_DESC_CODE32 + 7], ah

	; 初始化数据段描述符
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_DATA
	mov	word [LABEL_DESC_DATA + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_DATA + 4], al
	mov	byte [LABEL_DESC_DATA + 7], ah

	; 初始化堆栈段描述符
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_STACK
	mov	word [LABEL_DESC_STACK + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_STACK + 4], al
	mov	byte [LABEL_DESC_STACK + 7], ah

        ; 初始化 LDT_A 在 GDT 中的描述符
        ; code
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_LDT_A
	mov	word [LABEL_DESC_LDT_A + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_LDT_A + 4], al
	mov	byte [LABEL_DESC_LDT_A + 7], ah
        ; stack
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_LDT_STACK_A
	mov	word [LABEL_LDT_DESC_STACK_A + 2], ax
	shr	eax, 16
	mov	byte [LABEL_LDT_DESC_STACK_A + 4], al
	mov	byte [LABEL_LDT_DESC_STACK_A + 7], ah

	; 初始化 LDT_A 中的描述符
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_CODE_A
	mov	word [LABEL_LDT_DESC_CODE_A + 2], ax
	shr	eax, 16
	mov	byte [LABEL_LDT_DESC_CODE_A + 4], al
	mov	byte [LABEL_LDT_DESC_CODE_A + 7], ah

        ; 初始化 LDT_B 在 GDT 中的描述符
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_LDT_B
	mov	word [LABEL_DESC_LDT_B + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_LDT_B + 4], al
	mov	byte [LABEL_DESC_LDT_B + 7], ah

	; 初始化 LDT_B 中的描述符
        ; code
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_CODE_B
	mov	word [LABEL_LDT_DESC_CODE_B + 2], ax
	shr	eax, 16
	mov	byte [LABEL_LDT_DESC_CODE_B + 4], al
	mov	byte [LABEL_LDT_DESC_CODE_B + 7], ah
        ; stack        
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_LDT_STACK_B
	mov	word [LABEL_LDT_DESC_STACK_B + 2], ax
	shr	eax, 16
	mov	byte [LABEL_LDT_DESC_STACK_B + 4], al
	mov	byte [LABEL_LDT_DESC_STACK_B + 7], ah

        ; 初始化 TSS_A 描述符
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_TSS_A
	mov	word [LABEL_DESC_TSS_A + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_TSS_A + 4], al
	mov	byte [LABEL_DESC_TSS_A + 7], ah

        ; 初始化 TSS_B 描述符
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_TSS_B
	mov	word [LABEL_DESC_TSS_B + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_TSS_B + 4], al
	mov	byte [LABEL_DESC_TSS_B + 7], ah

	; 为加载 GDTR 作准备
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_GDT		; eax <- gdt 基地址
	mov	dword [GdtPtr + 2], eax	; [GdtPtr + 2] <- gdt 基地址

	; 为加载 IDTR 作准备
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_IDT		; eax <- idt 基地址
	mov	dword [IdtPtr + 2], eax	; [IdtPtr + 2] <- idt 基地址

	; 保存 IDTR
	sidt	[_SavedIDTR]

	; 保存中断屏蔽寄存器(IMREG)值
	in	al, 21h
	mov	[_SavedIMREG], al

	; 加载 GDTR
	lgdt	[GdtPtr]

	; 关中断
	cli

	; 加载 IDTR
	lidt	[IdtPtr]

	; 打开地址线A20
	in	al, 92h
	or	al, 00000010b
	out	92h, al

	; 准备切换到保护模式
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

	; 真正进入保护模式
	jmp	dword SelectorCode32:0	; 执行这一句会把 SelectorCode32 装入 cs, 并跳转到 Code32Selector:0  处

LABEL_REAL_ENTRY:		; 从保护模式跳回到实模式就到了这里
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, [_wSPValueInRealMode]

	lidt	[_SavedIDTR]	; 恢复 IDTR 的原值

	mov	al, [_SavedIMREG]	; ┓恢复中断屏蔽寄存器(IMREG)的原值
	out	21h, al			; ┛

	in	al, 92h		; ┓
	and	al, 11111101b	; ┣ 关闭 A20 地址线
	out	92h, al		; ┛

	sti			; 开中断

	mov	ax, 4c00h	; ┓
	int	21h		; ┛回到 DOS
; END of [SECTION .s16]


[SECTION .s32]; 32 位代码段. 由实模式跳入.
[BITS	32]
LABEL_SEG_CODE32:
	mov	ax, SelectorData
	mov	ds, ax			; 数据段选择子
	mov	es, ax
	mov	ax, SelectorVideo
	mov	gs, ax			; 视频段选择子

	mov	ax, SelectorStack
	mov	ss, ax			; 堆栈段选择子
	mov	esp, TopOfStack        

	; 显示进入保护模式的提示字符串
	push	szPMMessage
	call	DispStr
	add	esp, 4

	; # 显示内存信息表
	push	szMemChkTitle
	call	DispStr                 ; # 显示内存信息表表头
	add	esp, 4
	call	DispMemSize		; 显示内存信息        

        ; 复制代码到指定线性地址
        mov	ax, cs
	mov	ds, ax
	mov	ax, SelectorFlatRW
	mov	es, ax

	push	LenHUST          ; # 长度 Counter
	push	OffsetHUST       ; # 偏移 Source
	push	TaskHUST         ; # 页起始地址 Destination
	call	MemCpy
	add	esp, 12

	push	LenIS19
	push	OffsetIS19
	push	TaskIS19
	call	MemCpy
	add	esp, 12
        
    mov	ax, SelectorData
	mov	ds, ax			; 数据段选择子
	mov	es, ax

	; 打开分页
	call    SetupPaging

	; # 加载LDT
	mov     ax, SelectorLDTB
	lldt    ax

	; # 配置TR
	; # TR存储的是当前任务，即执行完调用任务后接着继续执行的任务；
	; # 而当两个任务都是死循环时，会发生任务1未执行完而发生中断去执行任务2，则最终会跳回来继续执行任务1
	; # 而由于任务2也是死循环，会导致该任务也没有执行完，中断发生后跳进任务1，最终又会跳回来继续执行任务2
	; # 因此不但要配置TR，还要进入任务使得TSS被保存
	; # 但该任务之前若有其他任务也无妨，因为由于死循环特性最终还是会成为两个任务相互切换的局面
	mov	ax, SelectorTSSB        
	ltr	ax

        ; # 配置可编程控制中断器
        call	Init8259A
        sti
	jmp	SelectorLDTCodeB:0      ; # 进入任务2打印'IS19 '

	;call	SetRealmode8259A

	; 到此停止
	jmp	SelectorCode16:0

; Init8259A ---------------------------------------------------------------------------------------------
Init8259A:
	mov	al, 011h
	out	020h, al	; 主8259, ICW1.
	call	io_delay

	out	0A0h, al	; 从8259, ICW1.
	call	io_delay

	mov	al, 020h	; IRQ0 对应中断向量 0x20
	out	021h, al	; 主8259, ICW2.
	call	io_delay

	mov	al, 028h	; IRQ8 对应中断向量 0x28
	out	0A1h, al	; 从8259, ICW2.
	call	io_delay

	mov	al, 004h	; IR2 对应从8259
	out	021h, al	; 主8259, ICW3.
	call	io_delay

	mov	al, 002h	; 对应主8259的 IR2
	out	0A1h, al	; 从8259, ICW3.
	call	io_delay

	mov	al, 001h
	out	021h, al	; 主8259, ICW4.
	call	io_delay

	out	0A1h, al	; 从8259, ICW4.
	call	io_delay

	;mov	al, 11111111b	; 屏蔽主8259所有中断
	mov	al, 11111110b	; 仅仅开启定时器中断
	out	021h, al	; 主8259, OCW1.
	call	io_delay

	mov	al, 11111111b	; 屏蔽从8259所有中断
	out	0A1h, al	; 从8259, OCW1.
	call	io_delay

	ret
; Init8259A ---------------------------------------------------------------------------------------------


; SetRealmode8259A ---------------------------------------------------------------------------------------------
SetRealmode8259A:
	mov	ax, SelectorData
	mov	fs, ax

	mov	al, 017h
	out	020h, al	; 主8259, ICW1.
	call	io_delay

	mov	al, 008h	; IRQ0 对应中断向量 0x8
	out	021h, al	; 主8259, ICW2.
	call	io_delay

	mov	al, 001h
	out	021h, al	; 主8259, ICW4.
	call	io_delay

	mov	al, [fs:SavedIMREG]	; ┓恢复中断屏蔽寄存器(IMREG)的原值
	out	021h, al		; ┛
	call	io_delay

	ret
; SetRealmode8259A ---------------------------------------------------------------------------------------------

io_delay:
	nop
	nop
	nop
	nop
	ret

; int handler ---------------------------------------------------------------
_ClockHandler:
ClockHandler	equ	_ClockHandler - $$
        push    eax
        mov     ax, [current]
        cmp     ax, 0
        jz      .hust   

        mov     byte [current], 0               ; # 当current为1时输出'IS19 '
        mov	al, 20h
	out	20h, al				; 发送 EOI
        jmp     SelectorTSSB:0
        pop     eax
        iretd
.hust:                                          ; # 当current为0时输出'HUST'
        mov     byte [current], 1
        mov	al, 20h
	out	20h, al				; 发送 EOI
        jmp     SelectorTSSA:0
        pop     eax        
	iretd
; ---------------------------------------------------------------------------

; 启动分页机制 --------------------------------------------------------------
SetupPaging:
	; 根据内存大小计算应初始化多少PDE以及多少页表
	xor	edx, edx
	mov	eax, [dwMemSize]
	mov	ebx, 400000h	; 400000h = 4M = 4096 * 1024, 一个页表对应的内存大小
	div	ebx
	mov	ecx, eax	; 此时 ecx 为页表的个数，也即 PDE 应该的个数
	test	edx, edx        ; # 测试页表除后是否有余数
	jz	.no_remainder   ; # 除尽了
	inc	ecx		; 如果余数不为 0 就需增加一个页表
.no_remainder:
	mov	[PageTableNumber], ecx	; 暂存页表个数

	; 为简化处理, 所有线性地址对应相等的物理地址. 并且不考虑内存空洞.

	; 首先初始化页目录
	mov	ax, SelectorFlatRW
	mov	es, ax
	mov	edi, PageDirBase1	; 此段首地址为 PageDirBase1
	xor	eax, eax
	mov	eax, PageTabBase1 | PG_P  | PG_USU | PG_RWW
.1:
	stosd
	add	eax, 4096		; 为了简化, 所有页表在内存中是连续的.
	loop	.1

	; 再初始化所有页表
	mov	eax, [PageTableNumber]	; 页表个数
	mov	ebx, 1024		; 每个页表 1024 个 PTE
	mul	ebx
	mov	ecx, eax		; PTE个数 = 页表个数 * 1024
	mov	edi, PageTabBase1	; 此段首地址为 PageTabBase1
	xor	eax, eax
	mov	eax, PG_P  | PG_USU | PG_RWW
.2:
	stosd
	add	eax, 4096		; 每一页指向 4K 的空间
	loop	.2

	mov	eax, PageDirBase1
	mov	cr3, eax
	mov	eax, cr0
	or	eax, 80000000h
	mov	cr0, eax
	jmp	short .3
.3:
	nop
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; 初始化页目录(第二个)
	mov	ax, SelectorFlatRW
	mov	es, ax
	mov	edi, PageDirBase2	; 此段首地址为 PageDirBase2
	xor	eax, eax
	mov	eax, PageTabBase2 | PG_P  | PG_USU | PG_RWW
	mov	ecx, [PageTableNumber]
.4:
	stosd
	add	eax, 4096		; 为了简化, 所有页表在内存中是连续的.
	loop	.4

	; 再初始化所有页表
	mov	eax, [PageTableNumber]	; 页表个数
	mov	ebx, 1024		; 每个页表 1024 个 PTE
	mul	ebx
	mov	ecx, eax		; PTE个数 = 页表个数 * 1024
	mov	edi, PageTabBase2	; 此段首地址为 PageTabBase2
	xor	eax, eax
	mov	eax, PG_P  | PG_USU | PG_RWW
.5:
	stosd
	add	eax, 4096		; 每一页指向 4K 的空间
	loop	.5

        ; 在此假设内存是大于 8M 的
	mov	eax, LinearAddrDemo
	shr	eax, 22
	mov	ebx, 4096
	mul	ebx
	mov	ecx, eax
	mov	eax, LinearAddrDemo
	shr	eax, 12
	and	eax, 03FFh	; 1111111111b (10 bits)
	mov	ebx, 4
	mul	ebx
	add	eax, ecx
	add	eax, PageTabBase2
	mov	dword [es:eax], TaskIS19 | PG_P | PG_USU | PG_RWW

        nop

	ret
; 分页机制启动完毕 ----------------------------------------------------------


; HUST -----------------------------------------------------------------------
HUST:
OffsetHUST	equ	HUST - $$
	mov	ax, SelectorVideo
	mov	gs, ax			; 视频段选择子(目的)

        mov	ah, 0Ch				; 0000: 黑底    1100: 红字
	mov	al, 'H'
	mov	[gs:((80 * 3 + 0) * 2)], ax	; 屏幕第 3 行, 第 0 列。
	mov	al, 'U'
	mov	[gs:((80 * 3 + 1) * 2)], ax	; 屏幕第 3 行, 第 1 列。
    mov al, 'S'
	mov	[gs:((80 * 3 + 2) * 2)], ax	; 屏幕第 3 行, 第 2 列。
    mov al, 'T'
	mov	[gs:((80 * 3 + 3) * 2)], ax	; 屏幕第 3 行, 第 3 列

        retf
LenHUST	equ	$ - HUST
; ---------------------------------------------------------------------------


; IS19 -----------------------------------------------------------------------
IS19:
OffsetIS19	equ	IS19 - $$
	mov	ax, SelectorVideo
	mov	gs, ax			; 视频段选择子(目的)
    mov	ah, 0Ch				; 0000: 黑底    1100: 红字
	mov	al, 'I'
	mov	[gs:((80 * 3 + 0) * 2)], ax	; 屏幕第 3 行, 第 0 列。
	mov	al, 'S'
	mov	[gs:((80 * 3 + 1) * 2)], ax	; 屏幕第 3 行, 第 1 列。
	mov	al, '1'
	mov	[gs:((80 * 3 + 2) * 2)], ax	; 屏幕第 3 行, 第 2 列。
    mov	al, '9'
	mov	[gs:((80 * 3 + 3) * 2)], ax	; 屏幕第 3 行, 第 3 列。 
    retf
LenIS19	equ	$ - IS19
; ---------------------------------------------------------------------------


; 显示内存信息 --------------------------------------------------------------
DispMemSize:
	push	esi
	push	edi
	push	ecx

	mov	esi, MemChkBuf
	mov	ecx, [dwMCRNumber]	;for(int i=0;i<[MCRNumber];i++) // 每次得到一个ARDS(Address Range Descriptor Structure)结构
.loop:					;{
	mov	edx, 5			;	for(int j=0;j<5;j++)	// 每次得到一个ARDS中的成员，共5个成员
	mov	edi, ARDStruct		;	{			// 依次显示：BaseAddrLow，BaseAddrHigh，LengthLow，LengthHigh，Type
.1:					;
	push	dword [esi]		;
	call	DispInt			;		DispInt(MemChkBuf[j*4]); // 显示一个成员
	pop	eax			;
	stosd				;		ARDStruct[j*4] = MemChkBuf[j*4];
	add	esi, 4			;
	dec	edx			;
	cmp	edx, 0			;
	jnz	.1			;	}
	call	DispReturn		;	printf("\n");
	cmp	dword [dwType], 1	;	if(Type == AddressRangeMemory) // AddressRangeMemory : 1, AddressRangeReserved : 2
	jne	.2			;	{
	mov	eax, [dwBaseAddrLow]	;
	add	eax, [dwLengthLow]	;
	cmp	eax, [dwMemSize]	;		if(BaseAddrLow + LengthLow > MemSize)
	jb	.2			;
	mov	[dwMemSize], eax	;			MemSize = BaseAddrLow + LengthLow;
.2:					;	}
	loop	.loop			;}
					;
	call	DispReturn		;printf("\n");
	push	szRAMSize		;
	call	DispStr			;printf("RAM size:");
	add	esp, 4			;
					;
	push	dword [dwMemSize]	;
	call	DispInt			;DispInt(MemSize);
	add	esp, 4			;

	pop	ecx
	pop	edi
	pop	esi
	ret
; ---------------------------------------------------------------------------
%include	"lib.inc"	; 库函数
SegCode32Len	equ	$ - LABEL_SEG_CODE32
; END of [SECTION .s32]


; 16 位代码段. 由 32 位代码段跳入, 跳出后到实模式
[SECTION .s16code]
ALIGN	32
[BITS	16]
LABEL_SEG_CODE16:
	; 跳回实模式:
	mov	ax, SelectorNormal
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	mov	eax, cr0
	and	al, 11111110b
	mov	cr0, eax

LABEL_GO_BACK_TO_REAL:
	jmp	0:LABEL_REAL_ENTRY	; 段地址会在程序开始处被设置成正确的值

Code16Len	equ	$ - LABEL_SEG_CODE16

; END of [SECTION .s16code]


; LDT_A
[SECTION .ldt_a]
ALIGN	32
LABEL_LDT_A:
;                                   段基址,         段界限,      属性
LABEL_LDT_DESC_CODE_A:	Descriptor      0,      CodeALen-1,     DA_C + DA_32	; Code, 32 位
LABEL_LDT_DESC_STACK_A: Descriptor      0,      TopOfStackA,    DA_DRWA | DA_32 ; Stack
; LDT_A 结束
LDTALen		equ	$ - LABEL_LDT_A

; LDT_A 选择子
SelectorLDTCodeA	equ	LABEL_LDT_DESC_CODE_A - LABEL_LDT_A + SA_TIL
SelectorLDTStackA       equ     LABEL_LDT_DESC_STACK_A - LABEL_LDT_A + SA_TIL
; END of [SECTION .ldt_a]

; StackA 堆栈段
[SECTION .lstack_a]
ALIGN	32
[BITS	32]
LABEL_LDT_STACK_A:
	times 512 db 0
TopOfStackA	equ	$ - LABEL_LDT_STACK_A - 1
; END of [SECTION .lstack_a]

; CodeA (LDT, 32 位代码段)
[SECTION .lcode_a]
ALIGN	32
[BITS	32]
LABEL_CODE_A:
.loop:
        call	SelectorFlatC:LinearAddrDemo
        jmp     .loop
CodeALen	equ	$ - LABEL_CODE_A
; END of [SECTION .lcode_a]


; LDT_B
[SECTION .ldt_b]
ALIGN	32
LABEL_LDT_B:
;                                   段基址,         段界限,      属性
LABEL_LDT_DESC_CODE_B:	Descriptor      0,      CodeBLen-1,     DA_C + DA_32	; Code, 32 位
LABEL_LDT_DESC_STACK_B: Descriptor      0,      TopOfStackB,    DA_DRWA | DA_32 ; Stack 
; LDT_B 结束 
LDTBLen		equ	$ - LABEL_LDT_B

; LDT_B 选择子
SelectorLDTCodeB	equ	LABEL_LDT_DESC_CODE_B - LABEL_LDT_B + SA_TIL
SelectorLDTStackB       equ     LABEL_LDT_DESC_STACK_B - LABEL_LDT_B + SA_TIL
; END of [SECTION .ldt_b]

; StackA 堆栈段
[SECTION .lstack_b]
ALIGN	32
[BITS	32]
LABEL_LDT_STACK_B:
	times 512 db 0
TopOfStackB	equ	$ - LABEL_LDT_STACK_B - 1
; END of [SECTION .lstack_b]

; CodeB (LDT, 32 位代码段)
[SECTION .lcode_b]
ALIGN	32
[BITS	32]
LABEL_CODE_B:
.loop:
        call	SelectorFlatC:LinearAddrDemo
        jmp     .loop
CodeBLen	equ	$ - LABEL_CODE_B
; END of [SECTION .lcode_b]