/* start.S - start, bzero */

#include <armv7a.h>

#define NULLSTK 8192
/*------------------------------------------------------------------------
 * start  -  Initial entry point for a Xinu image (ARM)
 *------------------------------------------------------------------------
 */
	.text
	.globl	start			/* Declare entry point global	*/

	.func start
start:
	/* Save the pointer to the atags (ARM boot tags).  The bootloader should
	 * pass this in r2, but override 0 with 0x100 to deal with old
	 * bootloaders that assume Linux's default behavior.  */
	ldr r3, =atags_ptr
	cmp r2, #0
	moveq r2, #0x100
	str r2, [r3]

	/* Continue execution at reset_handler.  */
	b reset_handler
	.endfunc

/* ARM exception vector table.  This is copied to address 0.  See A2.6
 * "Exceptions" of the ARM Architecture Reference Manual.  */
_vectors:
	ldr pc, reset_addr    /* Reset handler */
	ldr pc, undef_addr	  /* Undefined instruction handler */
	ldr pc, swi_addr	  /* Software interrupt handler */
	ldr pc, prefetch_addr /* Prefetch abort handler */
	ldr pc, abort_addr	  /* Data abort handler */
	ldr pc, reserved_addr /* Reserved */
	ldr pc, irq_addr	  /* IRQ (Interrupt request) handler */
	ldr pc, fiq_addr	  /* FIQ (Fast interrupt request) handler */

reset_addr:     .word reset_handler
undef_addr:     .word reset_handler
swi_addr:       .word reset_handler
prefetch_addr:  .word reset_handler
abort_addr:     .word page_fault
reserved_addr:  .word reset_handler
irq_addr:       .word irq_handler
fiq_addr:       .word reset_handler

_endvectors:

.section .text

	/* reset_handler: Reset handler routine executed to start up the kernel,
	 * when the ARM processor is reset, or (currently) when an unhandled
	 * exception occurs.  */
	.func reset_handler
reset_handler:

	/* Enter SPR mode with all interrupts disabled so the ARM processor is
	 * in a known state. */
	cpsid if, #ARMV7A_CPSR_SPR

	/* Copy the ARM exception table from .init section to address 0,
	 * including the absolute address table.  Here we assume it is exactly
	 * 16 words.  */
	mov r0, #0
	ldr r1, =_vectors
	ldmia r1!, {r2-r9}
	stmia r0!, {r2-r9}
	ldmia r1!, {r2-r9}
	stmia r0!, {r2-r9}

	/* Enable the following features by modifying the ARM processor's
	 * Control Register:
	 *
	 * - unaligned memory accesses   (bit 22)
	 *
	 * Note: Xinu shouldn't do any unaligned memory accesses on purpose, but
	 * we found that gcc can still generate unaligned memory accesses for
	 * tasks such as copying character strings.  (An alternative would be to
	 * pass -mno-unaligned-access to gcc to prevent it from doing so.)  */
	mrc p15, 0, r0, c1, c0, 0
	orr r0, #1 << 22
	mcr p15, 0, r0, c1, c0, 0

	/* Clear the .bss section of the kernel.  */
	ldr r0, =edata
	ldr r1, =_end
	mov r2, #0
	mov r3, #0
	mov r4, #0
	mov r5, #0
	b bssloopa
bssloopb:
	stmia r0!, {r2-r5}
bssloopa:
	cmp r0, r1 	/* check that we still haven't hit the end of bss yet */
	blo bssloopb	/* if still below, go backwards and loop */

	/* Put the null thread's stack directly after the kernel image.  */
	add sp, r1, #NULLSTK

	/* The remaining memory available to the ARM will be Xinu's "memheap"
	 * region, which is used for dynamic memory allocation.  Set its
	 * starting address.  */
	ldr r0, =MAXADDR
	str sp, [r0]

	/* Branch to the platform-independent C startup code, which takes no
	 * parameters and never returns.  */
	b nulluser
	.endfunc


