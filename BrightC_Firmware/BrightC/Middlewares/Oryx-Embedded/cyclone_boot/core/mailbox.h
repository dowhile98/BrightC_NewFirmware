/**
 * @file mailbox.h
 * @brief Boot Mailbox mangement
 *
 * @section License
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (C) 2010-2025 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneBOOT Open.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 2.5.0
 **/

#ifndef _MAILBOX_H
#define _MAILBOX_H

//Dependencies
#include "os_port.h"
#include "core/cboot_error.h"

//Bootloader mailbox version
#define BOOT_MBX_VERS_MAJOR 1
#define BOOT_MBX_VERS_MINOR 0
#define BOOT_MBX_VERS_PATCH 0
#define BOOT_MBX_VERSION (uint32_t)(      \
   ((BOOT_MBX_VERS_MAJOR & 0xFF) << 16) | \
   ((BOOT_MBX_VERS_MINOR & 0xFF) << 8)  | \
   (BOOT_MBX_VERS_MAJOR & 0xFF))

//Bootloader mailbox signture
#define BOOT_MBX_SIGNATURE 0x1b241671

//Bootloader mailbox maximum PSK size
#define BOOT_MBX_PSK_MAX_SIZE 32

#if ((defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) || \
   defined(__GNUC__) || defined(__CC_ARM) || defined(__IAR_SYSTEMS_ICC__) || \
   defined(__TASKING__) || defined(__CWCC__) || defined(__TI_ARM__))

/**
 * @brief Bootloader shared RAM mailbox structure
 **/

typedef __packed_struct
{
   uint32_t version;          ///<Bootloader Mailbox version
   uint32_t signature;        ///<Bootloader Mailbox signature
   uint32_t pskSize;          ///<Bootloader Mailbox PSK size
   uint8_t psk[32];           ///<Bootloader Mailbox PSK key
   uint8_t reserved[84];      ///<Reserved
} BootMailBox;

#else

#undef interface
#undef __start_packed
#define __start_packed __pragma( pack(push, 1) )
#undef __end_packed
#define __end_packed __pragma( pack(pop) )
#define __weak

/**
 * @brief Bootloader shared RAM mailbox structure
 **/

__start_packed typedef struct
{
   uint32_t version;          ///<Bootloader Mailbox version
   uint32_t signature;        ///<Bootloader Mailbox signature
   uint32_t pskSize;          ///<Bootloader Mailbox PSK size
   uint8_t psk[32];           ///<Bootloader Mailbox PSK key
   uint8_t reserved[84];      ///<Reserved
} BootMailBox __end_packed;

#endif


//CycloneBOOT IAP Mailbox functions
cboot_error_t setBootMailBox(BootMailBox *mailbox);
cboot_error_t getBootMailBox(BootMailBox *mailbox);
cboot_error_t checkBootMailBox(BootMailBox *mailbox);

#endif //!_MAILBOX_H
