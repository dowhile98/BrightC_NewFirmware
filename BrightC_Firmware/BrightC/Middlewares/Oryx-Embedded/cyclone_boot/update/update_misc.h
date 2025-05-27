/**
 * @file update_misc.h
 * @brief CycloneBOOT IAP Miscellaneous Functions API
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

#ifndef _UPDATE_MISC_H
#define _UPDATE_MISC_H

//Dependancies
#include "update/update.h"
#include "error.h"

//CycloneBOOT IAP miscellaneous related functions
//error_t updateInitPrimaryMemory(UpdateSettings *settings, UpdateContext *context);
//error_t updateInitSecondaryMemory(UpdateSettings *settings, UpdateContext *context);
cboot_error_t updateInitInputImage(UpdateSettings *settings, UpdateContext *context);
cboot_error_t updateInitOutputImage(UpdateSettings *settings, UpdateContext *context);
//error_t updateWrite(ImageContextTemp *context, const uint8_t *data, size_t length, uint8_t flag);
cboot_error_t updateGetImageHeaderFromSlot(Slot *slot, ImageHeader *header);

#endif // !_UPDATE_MISC_H
