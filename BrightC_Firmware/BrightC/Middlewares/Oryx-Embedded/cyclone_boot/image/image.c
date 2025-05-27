/**
 * @file image.c
 * @brief CycloneBOOT Image managment
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
#include <stdlib.h>
#include "core/crc32.h"
#include "core/crypto.h"
#include "memory/memory.h"
#include "image.h"
#include "debug.h"


/**
 * @brief Check Internal Image header validity
 * @param[in] header Pointer to the internal image header to be checked
 * @return Status code
 **/

cboot_error_t imageCheckHeader(ImageHeader *header)
{
#ifndef SIMULATOR
   error_t error;
   uint32_t computedCrc;

   //Check parameter vailidty
   if(header == NULL)
      return CBOOT_ERROR_INVALID_PARAMETERS;

   //Compute image header crc
   error = CRC32_HASH_ALGO->compute((uint8_t*)header, sizeof(ImageHeader) - CRC32_DIGEST_SIZE, (uint8_t*)&computedCrc);
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to compute image header crc!\r\n");
      return CBOOT_ERROR_FAILURE;
   }

   //Check image header integrity
   if(header->headCrc != computedCrc)
      return CBOOT_ERROR_INVALID_IMAGE_HEADER;

   //Check image header version
   if(header->headVers != IMAGE_HEADER_VERSION)
      return CBOOT_ERROR_INVALID_IMAGE_HEADER_VERSION;

   //Successful process
   return CBOOT_NO_ERROR;
#else
   return CBOOT_NO_ERROR;
#endif
}


/**
 * @brief Get image header from data buffer
 * @param[in] buffer Data buffer containing the image header
 * @param[in] bufferLen Data buffer length
 * @param[out] header Pointer to the header structure to be returned
 * @return Status code
 **/

cboot_error_t imageGetHeader(uint8_t *buffer, size_t bufferLen, ImageHeader **header)
{
   cboot_error_t cerror;
   ImageHeader *tempHeader;

   //Check parameters validity
   if(buffer == NULL || bufferLen == 0 || header == NULL)
      return CBOOT_ERROR_INVALID_PARAMETERS;

   if(bufferLen < sizeof(ImageHeader))
      return CBOOT_ERROR_INVALID_LENGTH;

   //Point to the image header
   tempHeader = (ImageHeader *)buffer;

   //Check image header
   cerror = imageCheckHeader(tempHeader);
   //Is any error?
   if(cerror)
      return cerror;

   //Save image header
   *header = tempHeader;

   //Successful process
   return CBOOT_NO_ERROR;
}


/**
 * @brief Compute image header crc and store it in the image header crc field.
 * The given input header will also be the outout image header.
 * @param[in/out] header Pointer the header on which to calculate the crc.
 * @return Error code
 **/

cboot_error_t imageComputeHeaderCrc(ImageHeader *header)
{
    error_t error;

    //Check parameters validity
    if(header == NULL)
        return CBOOT_ERROR_INVALID_PARAMETERS;

    //Compute image header crc
    error = CRC32_HASH_ALGO->compute((uint8_t*)header, sizeof(ImageHeader) - CRC32_DIGEST_SIZE, (uint8_t*)&header->headCrc);
    //Is any error?
    if(error)
        return CBOOT_ERROR_FAILURE;

    //Successfull process
    return CBOOT_NO_ERROR;
}
