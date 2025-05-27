/**
 * @file boot_config.h
 * @brief CycloneBOOT configuration file
 *
 * @section License
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (C) 2010-2025 Oryx Embedded SARL. All rights reserved.
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

#ifndef _BOOT_CONFIG_H
#define _BOOT_CONFIG_H

//Trace level for CycloneBOOT stack debugging
#define CBOOT_TRACE_LEVEL TRACE_LEVEL_INFO
#define CBOOT_DRIVER_TRACE_LEVEL TRACE_LEVEL_INFO

//Number of memories used
#define NB_MEMORIES 1
//Update Single Bank Mode support
#define UPDATE_SINGLE_BANK_SUPPORT DISABLED
//Update Dual Bank Mode support
#define UPDATE_DUAL_BANK_SUPPORT ENABLED
//Update Anti-Rollback support
#define UPDATE_ANTI_ROLLBACK_SUPPORT DISABLED

//Cipher support
#define CIPHER_SUPPORT ENABLED
//Image input encrypted
#define IMAGE_INPUT_ENCRYPTED ENABLED
//Verification Integrity support
#define VERIFY_INTEGRITY_SUPPORT ENABLED
//Verification Signture support
#define VERIFY_SIGNATURE_SUPPORT ENABLED
//Verification RSA signture algo support
#define VERIFY_RSA_SUPPORT ENABLED


#endif //!_BOOT_CONFIG_H
