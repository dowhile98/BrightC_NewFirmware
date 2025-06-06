/*
 * stm32_log.h
 *
 *  Created on: Nov 11, 2023
 *      Author: eplim
 */

#ifndef STM32_LOG_STM32_LOG_H_
#define STM32_LOG_STM32_LOG_H_
#ifdef __cplusplus
extern "C" {
#endif
/*Includes ----------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include "stm32_log_config.h"
/*Defines -----------------------------------------------------------------------*/
#define STM32_LOG_NONE      0
#define STM32_LOG_ERROR     1
#define STM32_LOG_WARN      2
#define STM32_LOG_INFO      3
#define STM32_LOG_DEBUG     4
#define STM32_LOG_VERBOSE   5

#ifndef STM32_LOG_LEVEL
#define STM32_LOG_LEVEL    STM32_LOG_DEBUG // Establece el nivel de registro deseado
#endif

#ifndef CONFIG_LOG_COLORS
#define CONFIG_LOG_COLORS	0
#endif

#if (CONFIG_LOG_COLORS == 1)
#define STM32_LOG_COLOR_BLACK   "30"
#define STM32_LOG_COLOR_RED     "31"
#define STM32_LOG_COLOR_GREEN   "32"
#define STM32_LOG_COLOR_BROWN   "33"
#define STM32_LOG_COLOR_BLUE    "34"
#define STM32_LOG_COLOR_PURPLE  "35"
#define STM32_LOG_COLOR_CYAN    "36"
#define STM32_LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define STM32_LOG_BOLD(COLOR)   "\033[1;" COLOR "m"
#define STM32_LOG_RESET_COLOR   "\033[0m"
#define STM32_LOG_COLOR_E       STM32_LOG_COLOR(STM32_LOG_COLOR_RED)
#define STM32_LOG_COLOR_W       STM32_LOG_COLOR(STM32_LOG_COLOR_BROWN)
#define STM32_LOG_COLOR_I       STM32_LOG_COLOR(STM32_LOG_COLOR_GREEN)
#define STM32_LOG_COLOR_D
#define STM32_LOG_COLOR_V

#else //CONFIG_LOG_COLORS
#define STM32_LOG_COLOR_E
#define STM32_LOG_COLOR_W
#define STM32_LOG_COLOR_I
#define STM32_LOG_COLOR_D
#define STM32_LOG_COLOR_V
#define STM32_LOG_RESET_COLOR
#endif //CONFIG_LOG_COLORS


#define STM32_LOG_FORMAT(level, format)  STM32_LOG_COLOR_ ## level #level " (%u) %s: " format STM32_LOG_RESET_COLOR "\r\n"


#if STM32_LOG_LEVEL >= STM32_LOG_ERROR
#define STM32_LOGE(tag, format, ...)  {stm32_log_write(STM32_LOG_ERROR,   tag, STM32_LOG_FORMAT(E, format), stm32_log_timestamp(), tag, ##__VA_ARGS__);}
#else
#define STM32_LOGE(tag, format, ...)	{(void)tag;}
#endif

#if STM32_LOG_LEVEL >= STM32_LOG_WARN
#define STM32_LOGW(tag, format, ...)  {stm32_log_write(STM32_LOG_WARN,    tag, STM32_LOG_FORMAT(W, format), stm32_log_timestamp(), tag, ##__VA_ARGS__);}
#else
#define STM32_LOGW(tag, format, ...)	{(void)tag;}
#endif

#if STM32_LOG_LEVEL >= STM32_LOG_INFO
#define STM32_LOGI(tag, format, ...)  {stm32_log_write(STM32_LOG_INFO,    tag, STM32_LOG_FORMAT(I, format), stm32_log_timestamp(), tag, ##__VA_ARGS__);}
#else
#define STM32_LOGI(tag, format, ...)	{(void)tag;}
#endif

#if STM32_LOG_LEVEL >= STM32_LOG_DEBUG
#define STM32_LOGD(tag, format, ...)  {stm32_log_write(STM32_LOG_DEBUG,   tag, STM32_LOG_FORMAT(D, format), stm32_log_timestamp(), tag, ##__VA_ARGS__);}
#else
#define STM32_LOGD(tag, format, ...) {(void)tag;}
#endif

#if STM32_LOG_LEVEL >= STM32_LOG_VERBOSE
#define STM32_LOGV(tag, format, ...)  {stm32_log_write(STM32_LOG_VERBOSE, tag, STM32_LOG_FORMAT(V, format), stm32_log_timestamp(), tag, ##__VA_ARGS__);}
#else
#define STM32_LOGV(tag, format, ...)  {(void)tag;}
#endif

/*Function prototype -----------------------------------------------------------*/
uint32_t stm32_log_timestamp(void);
void stm32_log_write(int level, const char* tag, const char* format, ...);

#ifdef __cplusplus
}
#endif
#endif /* STM32_LOG_STM32_LOG_H_ */
