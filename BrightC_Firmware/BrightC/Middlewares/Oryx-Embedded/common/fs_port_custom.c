/**
 * @file fs_port_fatfs.c
 * @brief File system abstraction layer (FatFs)
 *
 * @section License
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (C) 2010-2024 Oryx Embedded SARL. All rights reserved.
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
 * @version 2.4.2
 **/

//Dependencies
#include <fs_port_custom.h>
#include <string.h>
#include "fs_port.h"
#include "error.h"
#include "debug.h"


/*Pointer fx media for read */
static FX_MEDIA *fs_media_ptr;



/**
 * @brief File system initialization
 * @return Error code
 **/

error_t fsInit(FX_MEDIA *media)
{

	fs_media_ptr = media;
	//Create a mutex to protect critical sections

	//Mount file system




	//Successful processing
	return NO_ERROR;
}


/**
 * @brief Check whether a file exists
 * @param[in] path NULL-terminated string specifying the filename
 * @return The function returns TRUE if the file exists. Otherwise FALSE is returned
 **/

bool_t fsFileExists(const char_t *path)
{
	FX_FILE file;
	UINT res = FX_SUCCESS;

	res = fx_file_open((FX_MEDIA *)fs_media_ptr, (FX_FILE *)&file, (char *)path, FX_OPEN_FOR_WRITE);
	//Verify result
	if(res == FX_SUCCESS){
		fx_file_close(&file);
		return TRUE;		//file exist
	}
	return FALSE;
}


/**
 * @brief Retrieve the size of the specified file
 * @param[in] path NULL-terminated string specifying the filename
 * @param[out] size Size of the file in bytes
 * @return Error code
 **/

error_t fsGetFileSize(const char_t *path, uint32_t *size)
{
	UINT res;
	FX_FILE file;
	UINT attributes;

	//get attibutes
	res = fx_file_attributes_read(fs_media_ptr, (char *)path, (UINT *)&attributes);
	if(res != FX_SUCCESS){

		return ERROR_FAILURE;
	}
	if(attributes & FX_DIRECTORY){
		return ERROR_FAILURE;
	}
	//read size
	res = fx_file_open(fs_media_ptr,(FX_FILE* ) &file, (char *)path, FX_OPEN_FOR_WRITE);
	if(res != FX_SUCCESS){
		return ERROR_FAILURE;
	}

	*size = file.fx_file_current_file_size;

	fx_file_close(&file);
	//Successful processing
	return NO_ERROR;
}


/**
 * @brief Retrieve the attributes of the specified file
 * @param[in] path NULL-terminated string specifying the filename
 * @param[out] fileStat File attributes
 * @return Error code
 **/

error_t fsGetFileStat(const char_t *path, FsFileStat *fileStat)
{

	UINT res;
	UINT attributes, year, month, day;
	UINT hour, minute, second;
	ULONG size;
	/* Retrieve information about the directory entry
	"myfile.txt".*/

	//Check parameters
	if(path == NULL || fileStat == NULL)
		return ERROR_INVALID_PARAMETER;


	//get attriuutes
	res = fx_directory_information_get(fs_media_ptr, (char *)path,
		&attributes,
		&size, &year, &month, &day,
		&hour, &minute, &second);
	if(res != FX_SUCCESS){
		return ERROR_FAILURE;
	}


	//Clear file attributes
	osMemset(fileStat, 0, sizeof(FsFileStat));

	//File attributes
	fileStat->attributes = attributes;
	//File size
	fileStat->size = size;

	//Time of last modification
	fileStat->modified.year = year;
	fileStat->modified.month = month;
	fileStat->modified.day = day;
	fileStat->modified.dayOfWeek = 0;
	fileStat->modified.hours = hour;
	fileStat->modified.minutes = minute;
	fileStat->modified.seconds = second;
	fileStat->modified.milliseconds = 0;

	//Make sure the date is valid
	fileStat->modified.month = MAX(fileStat->modified.month, 1);
	fileStat->modified.month = MIN(fileStat->modified.month, 12);
	fileStat->modified.day = MAX(fileStat->modified.day, 1);
	fileStat->modified.day = MIN(fileStat->modified.day, 31);

	//Successful processing
	return NO_ERROR;
}


/**
 * @brief Rename the specified file
 * @param[in] oldPath NULL-terminated string specifying the pathname of the file to be renamed
 * @param[in] newPath NULL-terminated string specifying the new filename
 * @return Error code
 **/

error_t fsRenameFile(const char_t *oldPath, const char_t *newPath)
{

	UINT res;

	//Check parameters
	if(oldPath == NULL || newPath == NULL)
		return ERROR_INVALID_PARAMETER;

	//Rename the specified file
	res = fx_file_rename(fs_media_ptr, (char *)oldPath, (char *)newPath);

	//Any error to report?
	if(res != FX_SUCCESS)
		return ERROR_FAILURE;

	//Successful processing
	return NO_ERROR;

}


/**
 * @brief Delete a file
 * @param[in] path NULL-terminated string specifying the filename
 * @return Error code
 **/

error_t fsDeleteFile(const char_t *path)
{
	UINT res;
	//Make sure the pathname is valid
	if(path == NULL)
		return ERROR_INVALID_PARAMETER;

	//Delete the specified file
	res = fx_file_delete(fs_media_ptr, (char *)path);

	//Any error to report?
	if(res != FX_SUCCESS)
		return ERROR_FAILURE;

	//Successful processing
	return NO_ERROR;

}


/**
 * @brief Open the specified file for reading or writing
 * @param[in] path NULL-terminated string specifying the filename
 * @param[in] mode Type of access permitted (FS_FILE_MODE_READ,
 *   FS_FILE_MODE_WRITE or FS_FILE_MODE_CREATE)
 * @return File handle
 **/

FsFile *fsOpenFile(const char_t *path, uint_t mode)
{
	UINT res;
	UINT flags = 0;
	//File pointer
	FsFile *file = NULL;
	//Make sure the pathname is valid
	if(path == NULL)
		return NULL;

	//alloc memory
	file = osAllocMem(sizeof(FX_FILE));
	if(file == NULL){
		return NULL;
	}
	//Check access mode
	if(mode & FS_FILE_MODE_READ)
		flags = FX_OPEN_FOR_READ;

	if(mode & FS_FILE_MODE_WRITE)
		flags = FX_OPEN_FOR_WRITE;

	if(mode & FS_FILE_MODE_CREATE){
		flags = FX_OPEN_FOR_WRITE;
		res = fx_file_create(fs_media_ptr, (char *)path);
	}
	if(mode & FS_FILE_MODE_TRUNC)
			flags |= FX_OPEN_FOR_WRITE;

	res = fx_file_open(fs_media_ptr, (FX_FILE *)file, (char *)path, flags);
	//verify error
	if(res != FX_SUCCESS){
		osFreeMem(file);
		return NULL;
	}

	//Return a handle to the file
	return file;
}


/**
 * @brief Move to specified position in file
 * @param[in] file Handle that identifies the file
 * @param[in] offset Number of bytes to move from origin
 * @param[in] origin Position used as reference for the offset (FS_SEEK_SET,
 *   FS_SEEK_CUR or FS_SEEK_END)
 * @return Error code
 **/

error_t fsSeekFile(FsFile *file, int_t offset, uint_t origin)
{
	UINT res;

	//Make sure the file pointer is valid
	if(file == NULL)
		return ERROR_INVALID_PARAMETER;

	//The origin is used as reference for the offset
	if(origin == FS_SEEK_CUR)
	{
		//The offset is relative to the current file pointer
		res = fx_file_relative_seek(file, offset, FX_SEEK_BEGIN);

	}
	else if(origin == FS_SEEK_END)
	{
		//The offset is relative to the end of the file
		res = fx_file_relative_seek(file, offset, FX_SEEK_END);

	}
	else
	{
		//The offset is absolute
		res = fx_file_seek(file, offset);

	}
	//Any error to report?
	if(res != FX_SUCCESS)
		return ERROR_FAILURE;

	//Successful processing
	return NO_ERROR;
}


/**
 * @brief Write data to the specified file
 * @param[in] file Handle that identifies the file to be written
 * @param[in] data Pointer to a buffer containing the data to be written
 * @param[in] length Number of data bytes to write
 * @return Error code
 **/

error_t fsWriteFile(FsFile *file, void *data, size_t length)
{
	UINT res;
	if(file == NULL || data == NULL ){
		return ERROR_FAILURE;
	}
	//Write data
	res = fx_file_write(file, data, length);

	//Any error to report?
	if(res != FX_SUCCESS)
		return ERROR_FAILURE;

	//Successful processing
	fx_media_flush(fs_media_ptr);

	return NO_ERROR;
}


/**
 * @brief Read data from the specified file
 * @param[in] file Handle that identifies the file to be read
 * @param[in] data Pointer to the buffer where to copy the data
 * @param[in] size Size of the buffer, in bytes
 * @param[out] length Number of data bytes that have been read
 * @return Error code
 **/

error_t fsReadFile(FsFile *file, void *data, size_t size, size_t *length)
{
	ULONG n = 0;
	UINT res = 0;

	//Check parameters
	if(file == NULL || length == NULL)
		return ERROR_INVALID_PARAMETER;

	//No data has been read yet
	*length = 0;

	//Read data
	res = fx_file_read(file, data, size, &n);

	//Any error to report?
	if(res != FX_SUCCESS)
		return ERROR_FAILURE;

	//End of file?
	if(!n)
		return ERROR_END_OF_FILE;

	//Total number of data that have been read
	*length = n;
	//Successful processing
	return NO_ERROR;
}


/**
 * @brief Close a file
 * @param[in] file Handle that identifies the file to be closed
 **/

void fsCloseFile(FsFile *file)
{
	//Make sure the file pointer is valid
	if(file != NULL)
	{
		//Close the specified file
		fx_file_close(file);
		//Mark the corresponding entry as free
		osFreeMem(file);
	}
	return;
}


/**
 * @brief Check whether a directory exists
 * @param[in] path NULL-terminated string specifying the directory path
 * @return The function returns TRUE if the directory exists. Otherwise FALSE is returned
 **/

bool_t fsDirExists(const char_t *path)
{
	UINT res;
	UINT attributes, year, month, day;
	UINT hour, minute, second;
	ULONG size;
	/* Retrieve information about the directory entry
	"myfile.txt".*/

	//Check parameters
	if(path == NULL )
		return ERROR_INVALID_PARAMETER;

	//Root directory?
		if(!osStrcmp(path, "/"))
			return TRUE;
	//get attriuutes
	res = fx_directory_information_get(fs_media_ptr, (char *)path,
		&attributes,
		&size, &year, &month, &day,
		&hour, &minute, &second);
	//Any error to report?
	if(res != FX_SUCCESS){
		return ERROR_FAILURE;
	}

	//Valid directory?
	if(attributes & FX_DIRECTORY)
		return TRUE;
	else
		return FALSE;
}


/**
 * @brief Create a directory
 * @param[in] path NULL-terminated string specifying the directory path
 * @return Error code
 **/

error_t fsCreateDir(const char_t *path)
{

	UINT res;

	//Make sure the pathname is valid
	if(path == NULL)
		return ERROR_INVALID_PARAMETER;

	res = fx_directory_create(fs_media_ptr, (char *)path);

	//Any error to report?
	if(res != FX_SUCCESS)
		return ERROR_FAILURE;

	//Successful processing
	return NO_ERROR;

}


/**
 * @brief Remove a directory
 * @param[in] path NULL-terminated string specifying the directory path
 * @return Error code
 **/

error_t fsRemoveDir(const char_t *path)
{

	UINT res;

	//Make sure the pathname is valid
	if(path == NULL)
		return ERROR_INVALID_PARAMETER;

	res = fx_directory_delete(fs_media_ptr, (char *)path);

	//Any error to report?
	if(res != FX_SUCCESS)
		return ERROR_FAILURE;

	//Successful processing
	return NO_ERROR;

}


/**
 * @brief Open a directory stream
 * @param[in] path NULL-terminated string specifying the directory path
 * @return Directory handle
 **/

FsDir *fsOpenDir(const char_t *path)
{



	//Directory pointer
	FsDir *dir = NULL;


	//Return a handle to the directory
	return dir;
}


/**
 * @brief Read an entry from the specified directory stream
 * @param[in] dir Handle that identifies the directory
 * @param[out] dirEntry Pointer to a directory entry
 * @return Error code
 **/

error_t fsReadDir(FsDir *dir, FsDirEntry *dirEntry)
{

	//Successful processing
	return NO_ERROR;
}


/**
 * @brief Close a directory stream
 * @param[in] dir Handle that identifies the directory to be closed
 **/

void fsCloseDir(FsDir *dir)
{
	//Make sure the directory pointer is valid
	if(dir != NULL)
	{

	}
}
