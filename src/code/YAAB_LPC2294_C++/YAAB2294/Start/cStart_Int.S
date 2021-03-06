///
/// Constants
///
.EQU SYSTEM_MODE,0x13
.EQU IRQ_MODE, 0x12
.EQU IRQ_STACK_SIZE,1024
.EQU USER_STACK_SIZE, 1024 + IRQ_STACK_SIZE





///
/// Set the startup code in the .startup section
///
.section .startup, "ax"
.global _start

_start:
///
/// Exception table
///
    b .reset_handler /* Reset */
    ldr pc, .undefined_handler_address /* Undefined instructions */
    ldr pc, .swi_handler_address /* Software interrupt (SWI/SVC) */
    ldr pc, .prefetch_abort_handler_address /* Prefetch abort */
    ldr pc, .data_abort_handler_address /* Data abort */
    .word 0xb5403fba /* Reserved */
    ldr pc, [pc, #-0xFF0] /* IRQ */
    ldr pc, [pc, #-0xFF0] /* FIQ */

///
/// Exception handlers addresses
///
.undefined_handler_address:
    .word .undefined_handler
.swi_handler_address:
    .word .swi_handler
.prefetch_abort_handler_address:
    .word .prefetch_abort_handler
.data_abort_handler_address:
    .word .data_abort_handler


///
/// Exception handlers
///
.reset_handler:
    b _system_start
.undefined_handler:
    b .
.swi_handler:
    b .
.prefetch_abort_handler:
    b .
.data_abort_handler:
    b .

///
/// System initialization
///

_system_start:


	ldr r0, =0xE01FC040
	mov r1,#0x2
	str r1,[r0]
    ///
    /// Change to IRQ mode to configure its stack
    ///
    msr cpsr,#IRQ_MODE
    ldr sp, =__stack_end__
    mov r0,sp
    ///
    /// Set the user mode base stack pointer on r0
    ///
    sub r0,r0,#IRQ_STACK_SIZE

    ///
    /// Change to system mode
    ///
    msr cpsr,#SYSTEM_MODE

    ///
    /// Initialize Program Stack
    ///
    mov sp, r0
    mov fp,sp

    ///
    /// Run target configuration
    ///
    //bl target_config

    ///
    /// Initialize BSS
    ///
    ldr r0,=__bss_start__
    ldr r1,=__bss_end__
    bl init_bss

    ///
    /// Call global instances ctors
    ///
    ldr r0,=__ctors_init__
    ldr r1,=__ctors_end__
    bl init_constructors

    ///
    /// Call system main
    ///
    bl main
    b .


.section .stack
.space USER_STACK_SIZE

