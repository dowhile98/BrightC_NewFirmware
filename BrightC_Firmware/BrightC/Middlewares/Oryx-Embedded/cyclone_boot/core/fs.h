/**
 * @file fs.h
 * @brief CycloneBOOT File System Layer
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

#ifndef _FS_H
#define _FS_H

//Dependencies
#include <stdlib.h>
#include <stdint.h>
#include "error.h"

//File System Driver Major version
#define FS_DRIVER_VERSION_MAJOR 0x01
//File System Driver Minor version
#define FS_DRIVER_VERSION_MINOR 0x00
//File System Driver Revision version
#define FS_DRIVER_VERSION_PATCH 0x00
//File System Driver version
#define FS_DRIVER_VERSION (uint32_t)(((FS_DRIVER_VERSION_MAJOR & 0xFF) << 16) | \
                               ((FS_DRIVER_VERSION_MINOR & 0xFF) << 8) | \
                               (FS_DRIVER_VERSION_PATCH & 0xFF))

/**
 * @brief File System file handler definition
 **/

typedef void FsFileHandler;


/**
 * @brief File System Status definition
 **/

typedef enum
{
    FS_STATUS_OK = 0,
    FS_STATUS_BUSY,
    FS_STATUS_ERR
} FsStatus;


/**
 * @brief File System Driver Information
 **/

typedef struct
{
   uint32_t version;       ///<File System driver version
   char* fsName;           ///<File System memory name
   char* fsPath;           ///<File System memory volume path
   size_t fsSize;          ///<File System memory size
   uint32_t flags;         ///<File System memory flags
} FsInfo;


/**
 * @brief Fs initialization function
 **/

typedef error_t (*FileSystemInit)(void);


/**
 * @brief Fs de-initialization function
 **/

typedef error_t (*FileSystemDeInit)(void);


/**
 * @brief Fs status function
 **/

typedef error_t (*FileSystemGetInfo)(const FsInfo **fsInfo);


/**
 * @brief Fs status function
 **/

typedef error_t (*FileSystemGetStatus)(FsStatus *fsStatus);


/**
 * @brief Fs open function
 **/

typedef FsFileHandler* (*FileSystemOpen)(const char* path, uint8_t mode);


/**
 * @brief Fs close function
 **/

typedef error_t (*FileSystemClose)(FsFileHandler *fp);


/**
 * @brief Write Data into Fs function
 **/

typedef error_t (*FileSystemWrite)(FsFileHandler *fp, uint8_t offset, uint8_t *data, size_t length);


/**
 * @brief Read Data from Fs function
 **/

typedef error_t (*FileSystemRead)(FsFileHandler *fp, uint8_t offset, uint8_t* data, size_t length);


/**
 * @brief Erase Data from Fs function
 **/

typedef error_t (*FileSystemErase)(FsFileHandler *fp, uint8_t offset, size_t length);


/**
 * @brief Fs Driver definition
 **/

typedef struct
{
    FileSystemInit init;                        ///<Fs Driver init callback function
    FileSystemDeInit deInit;                    ///<Fs Driver deinit callback function
    FileSystemGetInfo getInfo;                ///<Fs Driver get info callback function
    FileSystemGetStatus getStatus;            ///<Fs Driver get status callback function
    FileSystemOpen open;                        ///<Fs Driver open file callback function
    FileSystemClose close;                      ///<Fs Driver close file callback function
    FileSystemWrite write;                      ///<Fs Driver write data callback function
    FileSystemRead read;                        ///<Fs Driver read data callback function
    FileSystemErase erase;                      ///<Fs Driver erase data callback function
} FsDriver;

#endif //_FS_H
