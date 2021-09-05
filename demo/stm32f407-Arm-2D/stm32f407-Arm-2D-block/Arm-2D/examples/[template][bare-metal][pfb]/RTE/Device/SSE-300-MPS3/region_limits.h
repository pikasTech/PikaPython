/*
 * Copyright (c) 2018-2020 Arm Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __REGION_LIMITS_H__
#define __REGION_LIMITS_H__

/* **************************************************************
 * WARNING: this file is parsed both by the C/C++ compiler
 * and the linker. As a result the syntax must be valid not only
 * for C/C++ but for the linker scripts too.
 * Beware of the following limitations:
 *   - LD (GCC linker) requires white space around operators.
 *   - UL postfix for macros is not suported by the linker script
 ****************************************************************/

/* Secure Code */
#define S_ROM_ALIAS               (0x10000000) /* ITCM_BASE_S */
#define TOTAL_S_ROM_SIZE          (0x00040000) /* 256 kB */

/* Secure Data */
#define S_RAM_ALIAS               (0x30000000) /* DTCM_BASE_S */
#define TOTAL_S_RAM_SIZE          (0x00040000) /* 256 kB */


/* Non-Secure Code */
#define NS_ROM_ALIAS              (0x01000000) /* SRAM_BASE_NS */
#define TOTAL_NS_ROM_SIZE         (0x00040000) /* 256 kB */

/* Non-Secure Data */
#define NS_RAM_ALIAS              (0x21000000) /* ISRAM0_BASE_NS */
#define TOTAL_NS_RAM_SIZE         (0x00040000) /* 256 kB */


/* Heap and Stack sizes for secure and nonsecure applications */
#define HEAP_SIZE                 (0x00000400) /* 1 KiB */
#define STACK_SIZE                (0x00000400) /* 1 KiB */

#endif /* __REGION_LIMITS_H__ */
