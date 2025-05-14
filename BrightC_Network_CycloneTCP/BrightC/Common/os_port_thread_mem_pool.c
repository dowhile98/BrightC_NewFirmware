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

static TX_MUTEX cyclone_pool_mutex;

void osSetBytePool(void *ptr){
	cyclone_pool = (TX_BYTE_POOL *)ptr;

	/*create mutex for mempool variable*/
	if(tx_mutex_create(&cyclone_pool_mutex, "cyclone pool mutex", TX_INHERIT) != TX_SUCCESS){
		TRACE_DEBUG("Fail to create cyclone pool mutex");
	}
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

#if 0
   //Enter critical section
   osSuspendAllTasks();
#endif
   //take mutex
   tx_mutex_get(&cyclone_pool_mutex, TX_WAIT_FOREVER);
   //Allocate a memory block

   tx_byte_allocate(cyclone_pool, (VOID **)&p, size, TX_NO_WAIT);

   //release mutex
   tx_mutex_put(&cyclone_pool_mutex);

#if 0
   //Leave critical section
   osResumeAllTasks();
#endif
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

#if 0
      //Enter critical section
      osSuspendAllTasks();
#endif
      //take mutex
      tx_mutex_get(&cyclone_pool_mutex, TX_WAIT_FOREVER);
      //Free memory block
      if(tx_byte_release(p) == TX_SUCCESS){

    	  p = NULL;
      }
      //release mutex
      tx_mutex_put(&cyclone_pool_mutex);

#if 0
      //Leave critical section
      osResumeAllTasks();
#endif
   }
}
