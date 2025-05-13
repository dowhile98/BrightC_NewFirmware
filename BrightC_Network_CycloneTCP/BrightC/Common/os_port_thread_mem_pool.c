/*
 * os_mem_pool.c
 *
 *  Created on: Sep 15, 2024
 *      Author: jeffr
 */


//Switch to the appropriate trace level
#define TRACE_LEVEL TRACE_LEVEL_OFF

//Dependencies
#include <stdio.h>
#include <stdlib.h>
#include "os_port.h"
#include "os_port_threadx.h"
#include "debug.h"


static TX_BYTE_POOL *cyclone_pool;

void osSetBytePool(void *ptr){
	cyclone_pool = (TX_BYTE_POOL *)ptr;
	return;
}

/**
 * @brief Allocate a memory block
 * @param[in] size Bytes to allocate
 * @return A pointer to the allocated memory block or NULL if
 *   there is insufficient memory available
 **/

void *osAllocMem(size_t size)
{
   void *p;

   //Enter critical section
   osSuspendAllTasks();
   //Allocate a memory block
   tx_byte_allocate(cyclone_pool, (VOID **)&p, size, TX_NO_WAIT);
  // p = malloc(size);
   //Leave critical section
   osResumeAllTasks();

   //Debug message
   TRACE_DEBUG("Allocating %" PRIuSIZE " bytes at 0x%08" PRIXPTR "\r\n",
      size, (uintptr_t) p);

   //Return a pointer to the newly allocated memory block
   return p;
}


/**
 * @brief Release a previously allocated memory block
 * @param[in] p Previously allocated memory block to be freed
 **/

void osFreeMem(void *p)
{
   //Make sure the pointer is valid
   if(p != NULL)
   {
      //Debug message
      TRACE_DEBUG("Freeing memory at 0x%08" PRIXPTR "\r\n", (uintptr_t) p);

      //Enter critical section
      osSuspendAllTasks();
      //Free memory block
      tx_byte_release(p);
      //free(p);
      //Leave critical section
      osResumeAllTasks();
   }
}
