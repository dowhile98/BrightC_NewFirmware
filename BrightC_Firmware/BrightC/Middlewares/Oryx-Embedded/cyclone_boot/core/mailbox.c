/**
 * @file mailbox.c
 * @brief CycloneBOOT IAP Miscellaneous Functions
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

//Switch to the appropriate trace level
#define TRACE_LEVEL CBOOT_TRACE_LEVEL

//Dependencies
#include "mailbox.h"

// Create a section to place the boot mailbox in RAM
#if defined(__CC_ARM)
BootMailBox bootMailBox __attribute__((__section__(".boot_mailbox"), zero_init));
#elif defined(__GNUC__)
BootMailBox bootMailBox __attribute__((section(".boot_mailbox")));
#elif defined(_MSC_VER)
extern BootMailBox bootMailBox;
#endif

/**
 * @brief Set message into the shared bootloader mailbox.
 * It will be processed by the bootloader at the next reboot.
 * @param[in] mailbox Pointer to the message to be set in the bootloader mailbox
 * @return Status code
 **/

cboot_error_t setBootMailBox(BootMailBox *mailbox)
{
   //Check parameters validity
   if(mailbox == NULL)
      return CBOOT_ERROR_INVALID_PARAMETERS;

   bootMailBox.version = mailbox->version;
   bootMailBox.signature = mailbox->signature;
   bootMailBox.pskSize = mailbox->pskSize;
   memcpy(bootMailBox.psk, (uint8_t*)mailbox->psk, BOOT_MBX_PSK_MAX_SIZE);

   //Successful process
   return CBOOT_NO_ERROR;
}


/**
 * @brief Get message from the shared bootloader mailbox.
 * @param[in,out] mailbox Pointer to the message to retrieved from the bootloader mailbox.
 * @return Status code
 **/

cboot_error_t getBootMailBox(BootMailBox *mailbox)
{
   //Check parameters validity
   if(mailbox == NULL)
      return CBOOT_ERROR_INVALID_PARAMETERS;

   mailbox->version = bootMailBox.version;
   mailbox->signature = bootMailBox.signature;
   mailbox->pskSize = bootMailBox.pskSize;
   memcpy((uint8_t*)mailbox->psk, bootMailBox.psk, BOOT_MBX_PSK_MAX_SIZE);

   //Successful process
   return CBOOT_NO_ERROR;
}

/**
 * @brief Check Bootloader mailbox message validity.
 * @param[in] Pointeur to the message to be checked.
 * @return Status code
 **/

cboot_error_t checkBootMailBox(BootMailBox *mailbox)
{
   cboot_error_t error;

   //Check paramter validity
   if(mailbox == NULL)
      return CBOOT_ERROR_INVALID_PARAMETERS;

   //Initialize error code
   error = CBOOT_ERROR_FAILURE;

   //Check msg isn't null
   if(mailbox != NULL)
   {
      if ((mailbox->version == BOOT_MBX_VERSION) && (mailbox->signature == BOOT_MBX_SIGNATURE))
         error = CBOOT_NO_ERROR;
   }

   //Return error code
   return error;
}
