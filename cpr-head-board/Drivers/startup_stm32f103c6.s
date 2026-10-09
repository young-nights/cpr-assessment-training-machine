/**
 * @file    startup_stm32f103c6.s
 * @brief   STM32F103C6T6 startup code (ARM assembly, Cortex-M3)
 *
 * Minimal startup: vector table, Reset_Handler, default handler
 * Change Logs:
 * Date           Author      Notes
 * 2026-09-27     coder       the first version
 */
    .syntax unified
    .cpu cortex-m3
    .thumb

/* ====== Stack and Heap ====== */
    .equ _estack, 0x20002800        /* Top of SRAM (10KB = 0x20000000 + 0x2800) */
    .equ _Min_Heap_Size, 0x200
    .equ _Min_Stack_Size, 0x400

/* ====== Vector Table ====== */
    .section .isr_vector, "a", %progbits
    .type g_pfnVectors, %object
    .size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
    .word   _estack                 /* Initial stack pointer */
    .word   Reset_Handler           /* Reset handler */
    .word   NMI_Handler
    .word   HardFault_Handler
    .word   MemManage_Handler
    .word   BusFault_Handler
    .word   UsageFault_Handler
    .word   0                       /* Reserved */
    .word   0
    .word   0
    .word   0
    .word   SVC_Handler
    .word   DebugMon_Handler
    .word   0
    .word   PendSV_Handler
    .word   SysTick_Handler

    /* External interrupts (STM32F103C6) */
    .word   WWDG_IRQHandler
    .word   PVD_IRQHandler
    .word   TAMPER_IRQHandler
    .word   RTC_IRQHandler
    .word   FLASH_IRQHandler
    .word   RCC_IRQHandler
    .word   EXTI0_IRQHandler
    .word   EXTI1_IRQHandler
    .word   EXTI2_IRQHandler
    .word   EXTI3_IRQHandler
    .word   EXTI4_IRQHandler
    .word   DMA1_Channel1_IRQHandler
    .word   DMA1_Channel2_IRQHandler
    .word   DMA1_Channel3_IRQHandler
    .word   DMA1_Channel4_IRQHandler
    .word   DMA1_Channel5_IRQHandler
    .word   DMA1_Channel6_IRQHandler
    .word   DMA1_Channel7_IRQHandler
    .word   ADC1_IRQHandler
    .word   USB_HP_CAN_TX_IRQHandler
    .word   USB_LP_CAN_RX0_IRQHandler
    .word   CAN_RX1_IRQHandler
    .word   CAN_SCE_IRQHandler
    .word   EXTI9_5_IRQHandler
    .word   TIM1_BRK_IRQHandler
    .word   TIM1_UP_IRQHandler
    .word   TIM1_TRG_COM_IRQHandler
    .word   TIM1_CC_IRQHandler
    .word   TIM2_IRQHandler
    .word   TIM3_IRQHandler
    .word   TIM4_IRQHandler
    .word   I2C1_EV_IRQHandler
    .word   I2C1_ER_IRQHandler
    .word   I2C2_EV_IRQHandler
    .word   I2C2_ER_IRQHandler
    .word   SPI1_IRQHandler
    .word   SPI2_IRQHandler
    .word   USART1_IRQHandler
    .word   USART2_IRQHandler
    .word   USART3_IRQHandler
    .word   EXTI15_10_IRQHandler

/* ====== Reset Handler ====== */
    .section .text.Reset_Handler
    .weak Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:
    ldr   r0, =_estack
    mov   sp, r0

    /* Copy .data from flash to SRAM */
    ldr   r0, =_sdata
    ldr   r1, =_edata
    ldr   r2, =_sidata
    movs  r3, #0
    b     LoopCopyDataInit

CopyDataInit:
    ldr   r4, [r2, r3]
    str   r4, [r0, r3]
    adds  r3, r3, #4

LoopCopyDataInit:
    adds  r4, r0, r3
    cmp   r4, r1
    bcc   CopyDataInit

    /* Zero fill .bss */
    ldr   r2, =_sbss
    ldr   r4, =_ebss
    movs  r3, #0
    b     LoopFillZerobss

FillZerobss:
    str   r3, [r2]
    adds  r2, r2, #4

LoopFillZerobss:
    cmp   r2, r4
    bcc   FillZerobss

    /* Call main */
    bl    main

    /* Infinite loop if main returns */
    b     .

    .size Reset_Handler, .-Reset_Handler

/* ====== Default Handler ====== */
    .section .text.Default_Handler, "ax", %progbits
Default_Handler:
    b     Default_Handler

/* ====== Weak aliases for all handlers ====== */
    .macro def_irq handler
    .weak \handler
    .thumb_set \handler, Default_Handler
    .endm

    def_irq NMI_Handler
    def_irq HardFault_Handler
    def_irq MemManage_Handler
    def_irq BusFault_Handler
    def_irq UsageFault_Handler
    def_irq SVC_Handler
    def_irq DebugMon_Handler
    def_irq PendSV_Handler
    def_irq SysTick_Handler
    def_irq WWDG_IRQHandler
    def_irq PVD_IRQHandler
    def_irq TAMPER_IRQHandler
    def_irq RTC_IRQHandler
    def_irq FLASH_IRQHandler
    def_irq RCC_IRQHandler
    def_irq EXTI0_IRQHandler
    def_irq EXTI1_IRQHandler
    def_irq EXTI2_IRQHandler
    def_irq EXTI3_IRQHandler
    def_irq EXTI4_IRQHandler
    def_irq DMA1_Channel1_IRQHandler
    def_irq DMA1_Channel2_IRQHandler
    def_irq DMA1_Channel3_IRQHandler
    def_irq DMA1_Channel4_IRQHandler
    def_irq DMA1_Channel5_IRQHandler
    def_irq DMA1_Channel6_IRQHandler
    def_irq DMA1_Channel7_IRQHandler
    def_irq ADC1_IRQHandler
    def_irq USB_HP_CAN_TX_IRQHandler
    def_irq USB_LP_CAN_RX0_IRQHandler
    def_irq CAN_RX1_IRQHandler
    def_irq CAN_SCE_IRQHandler
    def_irq EXTI9_5_IRQHandler
    def_irq TIM1_BRK_IRQHandler
    def_irq TIM1_UP_IRQHandler
    def_irq TIM1_TRG_COM_IRQHandler
    def_irq TIM1_CC_IRQHandler
    def_irq TIM2_IRQHandler
    def_irq TIM3_IRQHandler
    def_irq TIM4_IRQHandler
    def_irq I2C1_EV_IRQHandler
    def_irq I2C1_ER_IRQHandler
    def_irq I2C2_EV_IRQHandler
    def_irq I2C2_ER_IRQHandler
    def_irq SPI1_IRQHandler
    def_irq SPI2_IRQHandler
    def_irq USART1_IRQHandler
    def_irq USART2_IRQHandler
    def_irq USART3_IRQHandler
    def_irq EXTI15_10_IRQHandler
