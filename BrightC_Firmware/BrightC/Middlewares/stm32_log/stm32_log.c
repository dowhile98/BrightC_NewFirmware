/*
 * stm32_log.c
 *
 *  Created on: Nov 11, 2023
 *      Author: eplim
 */

#include "stm32_log.h"
#include "main.h"
#include "lwprintf.h"
// Función para obtener la marca de tiempo (adapta según sea necesario)
uint32_t stm32_log_timestamp(void) {
    // Implementa la obtención de la marca de tiempo aquí (puede ser un contador o un reloj en tiempo real)
    return HAL_GetTick() ;
}

// Función para escribir un mensaje de registro (adapta según sea necesario)
void stm32_log_write(int level, const char* tag, const char* format, ...) {
    if (level <= STM32_LOG_LEVEL) {
        va_list args;
        va_start(args, format);
        lwprintf_vprintf(format, args);
        va_end(args);
    }
    return;
}
