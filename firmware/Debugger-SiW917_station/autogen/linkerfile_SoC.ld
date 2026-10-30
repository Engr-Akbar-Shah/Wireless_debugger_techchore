/***************************************************************************//**
 * GCC/CMake Linker script for Silicon Labs devices
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/


 MEMORY
 {
   rom   (rx)  : ORIGIN = 0x8202000, LENGTH = 0x1fe000

   ram   (rwx) : ORIGIN = 0xc, LENGTH = 0x2fbf4
 }
 MEMORY
 {
   udma0   (rwx)  : ORIGIN = 0x2fc00, LENGTH = 0x400
   udma1   (rwx)  : ORIGIN = 0x24061c00, LENGTH = 0x400
 }

ENTRY(Reset_Handler)
 
SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
		. = ALIGN(32);
			
		  *(EXCLUDE_FILE(*/UDMA.* */tasks.*).text*)
			 KEEP(*(.init))
			 KEEP(*(.fini))
				
		
				
        	     	  	

		
		/* Align before rodata for flash line efficiency */
		. = ALIGN(32);
		*(.rodata*)
		*(.rodata.*)
		
        /* .ctors */
		*/crtbegin.*(.ctors)
		*/crtbegin?.*(.ctors)
		*(EXCLUDE_FILE(*/crtend?.* */crtend.*) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)
		
		/* .dtors */
		*/crtbegin.*(.dtors)
		*/crtbegin?.*(.dtors)
		*(EXCLUDE_FILE(*/crtend?.* */crtend.*) .dtors)
		*(SORT(.dtors.*))
		*(.dtors)

		KEEP(*(.eh_frame*))			
	} > rom	
	
	.ARM.extab : 
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)			
	} > rom
	
	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)			
	} > rom 
	__exidx_end = .;
	__etext = .;
	
	__rom_start  = ORIGIN(rom);
	__rom_length = LENGTH(rom);	

	/* _sidata is used in code startup code */
	_sidata = __etext;

	

	
	
	.data :

	{
		__data_start__ = .;
		
		/* _sdata is used in startup code */
		_sdata = __data_start__;
		*(.data*)
		*/UDMA.*(.text*)
		
		*/tasks.*(.text*)
		
		
		
		/* ipmu calibration data */
		*(.common_ipmu_ram*)
		
		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);
		
		. = ALIGN(4);
		/* init data */
		
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);
		
		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);
		
		KEEP(*(.jcr*))
		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;
		
		/* _edata is used in startup code */
		_edata = __data_end__; 
	} > ram AT> rom
	
	.bss (NOLOAD) :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
	} > ram 

	
	.stack (NOLOAD):
	{
		__StackLimit = .;
		KEEP(*(.stack*))
		. = ALIGN(4);
		__StackTop = .;
		PROVIDE(__stack = __StackTop);			
	} > ram
	/* RAM overflow detection */
	__current_ram_used = . - ORIGIN(ram);
	__ram_total = LENGTH(ram);
	__ram_remaining = __ram_total - __current_ram_used;
	
	/* Check for RAM overflow */
	ASSERT((. <= (ORIGIN(ram) + LENGTH(ram))), "RAM OVERFLOW: RAM usage exceeds available memory region")
	
	/* Check minimum heap space */
	ASSERT((__ram_remaining >= 1024), "WARNING: Less than 1KB RAM left for heap - application may run out of dynamic memory")
	
	/* Check stack overlap */
	ASSERT((__StackLimit >= __bss_end__), "CRITICAL: Stack overlaps with BSS section - increase RAM or reduce static allocation")
	
	/* End of RAM overflow detection section */
	/* Overflow guard symbols derived from MEMORY (no hard-coding) */
    __ram_start__ = ORIGIN(ram);
    __ram_end__   = ORIGIN(ram) + LENGTH(ram);
  	.heap (COPY):
  	{
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		_end = __end__;
		KEEP(*(.heap*))
		__HeapLimit = ORIGIN(ram) + LENGTH(ram);   /* end of RAM */
  	} > ram
 
	__heap_size = __HeapLimit - __HeapBase;

	/* Final heap validation */
	ASSERT((__heap_size >= 0), "CRITICAL: Heap region is invalid (negative size) - stack and heap are overlapping")
	.udma_addr0 :
	{
		*(.udma_addr0*)
	} > udma0 AT> rom

	.udma_addr1 :
	{
		*(.udma_addr1*)		
	} > udma1 AT> rom	
  /* nvm3 enable*/  /* Flash Present*/  /* littlefs_enable */  /* Flash Present */

ASSERT(__etext <= (ORIGIN(rom) + LENGTH(rom)), "ERROR: Flash overflow code/data exceed ROM region - code and constants will not fit in flash")
}