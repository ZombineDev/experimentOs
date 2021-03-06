[BITS 32]

extern _kmain
extern _idtp
extern _irq_handler
extern _gdt_pointer

global _gdt_load
global _idt_load
global _tss_flush
global _start

section .text

_start:

   ; before jump to kernel, we have to setup a basic paging
   ; in order to map the kernel from 0x100000 to 0xC0000000 (+3 GB)
   
   ; let's put the page directory at 0x1000 (+ 4 KB)
   
   mov edi, 0x1000

   .l1:  
   mov dword [edi], 0
   add edi, 4
   cmp edi, 0x2000
   jne .l1

   ; let's put the first page table at 0x2000 (+ 8 KB)
   mov eax, 0
   .l2:
   
   mov ebx, eax
   or ebx, 3  ; present, rw
   mov [edi], ebx
   
   add eax, 0x1000 ; += 4K
   add edi, 4
   
   cmp edi, 0x3000
   jne .l2
   
   ; xchg bx, bx ; bochs magic break
   
   mov eax, 0x2003    ; = 0x2000 | preset,rw
   
   ; identity map the first low 4 MB 
   ; this is necessary for executing the jmp far eax below)
   ; otherwise, just after 'mov cr0, eax', EIP will point to an invalid address
   
   mov [0x1000], eax
   mov [0x1C00], eax  ; map them also to 0xC0000000
   
   mov ebx, 0x1000
   mov cr3, ebx     ; set page dir physical address
   
   mov eax, cr0
   or eax, 0x80000000 ; paging ON
   or eax, 0x10000    ; WP ON (write protect for supervisor)
   mov cr0, eax       ; enable paging!
  
   mov eax, 0xC0100400
   jmp far eax        ; jump to next instruction using the high virtual address

   times 1024-($-$$) db 0
   
   ; this is 0xC0100400
   mov esp, 0xC01FFFF0
   jmp _kmain        ; now, really jump to kernel's code which uses 0xC0100000 as ORG
   
_gdt_load:
   lgdt [_gdt_pointer]
   ret

_idt_load:
    lidt [_idtp]
    ret
    
_tss_flush:
   mov ax, 0x2B      ; Load the index of our TSS structure - The index is
                     ; 0x28, as it is the 5th selector and each is 8 bytes
                     ; long, but we set the bottom two bits (making 0x2B)
                     ; so that it has an RPL of 3, not zero.
   ltr ax            ; Load 0x2B into the task state register.
   ret

global _switch_to_usermode_asm
   
_switch_to_usermode_asm:
     mov ax,0x23
     mov ds,ax
     mov es,ax 
     mov fs,ax 
     mov gs,ax ; we don't need to worry about SS. it's handled by iret
     
     mov ebx, [esp + 4]  ; first arg, the usermode entry point
     mov eax, [esp + 8]  ; second arg, the usermode stack ptr
     
     push 0x23 ; user data segment with bottom 2 bits set for ring 3
     push eax  ; push the stack pointer
     pushf
     push 0x1B ; user code segment with bottom 2 bits set for ring 3
     push ebx
     iret
   
   

; In just a few pages in this tutorial, we will add our Interrupt
; Service Routines (ISRs) right here!
global _isr0
global _isr1
global _isr2
global _isr3
global _isr4
global _isr5
global _isr6
global _isr7
global _isr8
global _isr9
global _isr10
global _isr11
global _isr12
global _isr13
global _isr14
global _isr15
global _isr16
global _isr17
global _isr18
global _isr19
global _isr20
global _isr21
global _isr22
global _isr23
global _isr24
global _isr25
global _isr26
global _isr27
global _isr28
global _isr29
global _isr30
global _isr31

global _isr128

;  0: Divide By Zero Exception
_isr0:
    ; cli
    push byte 0
    push byte 0
    jmp isr_common_stub

;  1: Debug Exception
_isr1:
    ; cli
    push byte 0
    push byte 1
    jmp isr_common_stub

;  2: Non Maskable Interrupt Exception
_isr2:
    ; cli
    push byte 0
    push byte 2
    jmp isr_common_stub

;  3: Int 3 Exception
_isr3:
    ; cli
    push byte 0
    push byte 3
    jmp isr_common_stub

;  4: INTO Exception
_isr4:
    ; cli
    push byte 0
    push byte 4
    jmp isr_common_stub

;  5: Out of Bounds Exception
_isr5:
    ; cli
    push byte 0
    push byte 5
    jmp isr_common_stub

;  6: Invalid Opcode Exception
_isr6:
    ; cli
    push byte 0
    push byte 6
    jmp isr_common_stub

;  7: Coprocessor Not Available Exception
_isr7:
    ; cli
    push byte 0
    push byte 7
    jmp isr_common_stub

;  8: Double Fault Exception (With Error Code!)
_isr8:
    ; cli
    push byte 8
    jmp isr_common_stub

;  9: Coprocessor Segment Overrun Exception
_isr9:
    ; cli
    push byte 0
    push byte 9
    jmp isr_common_stub

; 10: Bad TSS Exception (With Error Code!)
_isr10:
    ; cli
    push byte 10
    jmp isr_common_stub

; 11: Segment Not Present Exception (With Error Code!)
_isr11:
    ; cli
    push byte 11
    jmp isr_common_stub

; 12: Stack Fault Exception (With Error Code!)
_isr12:
    ; cli
    push byte 12
    jmp isr_common_stub

; 13: General Protection Fault Exception (With Error Code!)
_isr13:
    ; cli
    push byte 13
    jmp isr_common_stub

; 14: Page Fault Exception (With Error Code!)
_isr14:
    ; cli
    push byte 14
    jmp isr_common_stub

; 15: Reserved Exception
_isr15:
    ; cli
    push byte 0
    push byte 15
    jmp isr_common_stub

; 16: Floating Point Exception
_isr16:
    ; cli
    push byte 0
    push byte 16
    jmp isr_common_stub

; 17: Alignment Check Exception
_isr17:
    ; cli
    push byte 0
    push byte 17
    jmp isr_common_stub

; 18: Machine Check Exception
_isr18:
    ; cli
    push byte 0
    push byte 18
    jmp isr_common_stub

; 19: Reserved
_isr19:
    ; cli
    push byte 0
    push byte 19
    jmp isr_common_stub

; 20: Reserved
_isr20:
    ; cli
    push byte 0
    push byte 20
    jmp isr_common_stub

; 21: Reserved
_isr21:
    ; cli
    push byte 0
    push byte 21
    jmp isr_common_stub

; 22: Reserved
_isr22:
    ; cli
    push byte 0
    push byte 22
    jmp isr_common_stub

; 23: Reserved
_isr23:
    ; cli
    push byte 0
    push byte 23
    jmp isr_common_stub

; 24: Reserved
_isr24:
    ; cli
    push byte 0
    push byte 24
    jmp isr_common_stub

; 25: Reserved
_isr25:
    ; cli
    push byte 0
    push byte 25
    jmp isr_common_stub

; 26: Reserved
_isr26:
    ; cli
    push byte 0
    push byte 26
    jmp isr_common_stub

; 27: Reserved
_isr27:
    ; cli
    push byte 0
    push byte 27
    jmp isr_common_stub

; 28: Reserved
_isr28:
    ; cli
    push byte 0
    push byte 28
    jmp isr_common_stub

; 29: Reserved
_isr29:
    ; cli
    push byte 0
    push byte 29
    jmp isr_common_stub

; 30: Reserved
_isr30:
    ; cli
    push byte 0
    push byte 30
    jmp isr_common_stub

; 31: Reserved
_isr31:
    ; cli
    push byte 0
    push byte 31
    jmp isr_common_stub

_isr128:
    push byte 0
    push 0x80
    jmp isr_common_stub


extern _generic_interrupt_handler

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
    pusha          ;  Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, _generic_interrupt_handler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa          ; Pops edi,esi,ebp...
    add esp, 8    ; Cleans up the pushed error code and pushed ISR number
    iret

    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _irq0
global _irq1
global _irq2
global _irq3
global _irq4
global _irq5
global _irq6
global _irq7
global _irq8
global _irq9
global _irq10
global _irq11
global _irq12
global _irq13
global _irq14
global _irq15

; 32: IRQ0
_irq0:
    ; cli
    push byte 0
    push byte 32
    jmp irq_common_stub

; 33: IRQ1
_irq1:
    ; cli
    push byte 0
    push byte 33
    jmp irq_common_stub

; 34: IRQ2
_irq2:
    ; cli
    push byte 0
    push byte 34
    jmp irq_common_stub

; 35: IRQ3
_irq3:
    ; cli
    push byte 0
    push byte 35
    jmp irq_common_stub

; 36: IRQ4
_irq4:
    ; cli
    push byte 0
    push byte 36
    jmp irq_common_stub

; 37: IRQ5
_irq5:
    ; cli
    push byte 0
    push byte 37
    jmp irq_common_stub

; 38: IRQ6
_irq6:
    ; cli
    push byte 0
    push byte 38
    jmp irq_common_stub

; 39: IRQ7
_irq7:
    ; cli
    push byte 0
    push byte 39
    jmp irq_common_stub

; 40: IRQ8
_irq8:
    ; cli
    push byte 0
    push byte 40
    jmp irq_common_stub

; 41: IRQ9
_irq9:
    ; cli
    push byte 0
    push byte 41
    jmp irq_common_stub

; 42: IRQ10
_irq10:
    ; cli
    push byte 0
    push byte 42
    jmp irq_common_stub

; 43: IRQ11
_irq11:
    ; cli
    push byte 0
    push byte 43
    jmp irq_common_stub

; 44: IRQ12
_irq12:
    ; cli
    push byte 0
    push byte 44
    jmp irq_common_stub

; 45: IRQ13
_irq13:
    ; cli
    push byte 0
    push byte 45
    jmp irq_common_stub

; 46: IRQ14
_irq14:
    ; cli
    push byte 0
    push byte 46
    jmp irq_common_stub

; 47: IRQ15
_irq15:
    ; cli
    push byte 0
    push byte 47
    jmp irq_common_stub



irq_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp

    push eax
    mov eax, _irq_handler
    call eax
    pop eax

    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

