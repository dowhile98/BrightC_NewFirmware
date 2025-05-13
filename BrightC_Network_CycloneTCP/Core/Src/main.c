/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"
#include "main.h"
#include "crc.h"
#include "memorymap.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lwprintf.h"
#include <stdlib.h>
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "core/net.h"
#include "drivers/mac/stm32h7xx_eth_driver.h"
#include "drivers/phy/lan8742_driver.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "http/http_server.h"
#include "http/mime.h"
#include "hardware/stm32h7xx/stm32h7xx_crypto.h"
#include "rng/trng.h"
#include "path.h"
#include "date_time.h"
#include "resource_manager.h"
#include "debug.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define APP_IF_NAME "eth0"
#define APP_HOST_NAME "http-server-demo"
#define APP_MAC_ADDR "00-AB-CD-EF-07-69"

#define APP_USE_DHCP_CLIENT DISABLED
#define APP_IPV4_HOST_ADDR "192.168.1.86"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.1.1"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_USE_SLAAC ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::769"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::769"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"


#define UPDT_PUBLIC_SIGN_KEY "keys/rsa_public_key.pem"
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
DhcpClientSettings dhcpClientSettings;
DhcpClientContext dhcpClientContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;

//Update signature public key declaration
const uint8_t* pemUpdtSignPublicKey;
size_t pemUpdtSignPublicKeyLen;

//Update mutex handler
OsMutex updateMutex;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief LED task
 * @param[in] param Unused parameter
 **/

void ledTask(void *param)
{
	//Endless loop
	while(1)
	{
		LED_GPIO_Port->BSRR |= LED_Pin;
		osDelayTask(100);
		LED_GPIO_Port->BSRR |= LED_Pin<<16;
		osDelayTask(900);
	}
}

error_t updateUserLoadKeys(void)
{
	error_t error;

	//Get server's private key
	error = resGetData(UPDT_PUBLIC_SIGN_KEY, (const uint8_t **) &pemUpdtSignPublicKey,
			&pemUpdtSignPublicKeyLen);
	//Any error to report?
	if(error)
		return error;

	//Successful process
	return NO_ERROR;
}


UINT App_cyclone_tcp_Init(VOID *memory_ptr)
{
	UINT ret = TX_SUCCESS;
	CHAR *pointer;
	/* USER CODE BEGIN App_ThreadX_MEM_POOL */

	/* USER CODE END App_ThreadX_MEM_POOL */

	/* USER CODE BEGIN App_ThreadX_Init */
	error_t error;
	OsTaskId taskId;
	OsTaskParameters taskParams;
	NetInterface *interface;
	MacAddr macAddr;
#if (APP_USE_DHCP_CLIENT == DISABLED)
	Ipv4Addr ipv4Addr;
#endif
#if (APP_USE_SLAAC == DISABLED)
	Ipv6Addr ipv6Addr;
#endif
	//Configure debug UART
	debugInit(115200);

	__HAL_RCC_GPIOG_CLK_ENABLE();

	GPIOG->MODER &=~ GPIO_MODER_MODE4;
	GPIOG->MODER |= GPIO_MODER_MODE4_0;

	GPIOG->ODR |= GPIO_ODR_OD4;
	//Start-up message
	TRACE_INFO("\r\n");
	TRACE_INFO("****************************************************\r\n");
	TRACE_INFO("*** CycloneBOOT IAP Single-Bank HTTP Server Demo ***\r\n");
	TRACE_INFO("****************************************************\r\n");
	TRACE_INFO("Copyright: 2010-2023 Oryx Embedded SARL\r\n");
	TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
	TRACE_INFO("Target: STM32H743\r\n");
	TRACE_INFO("\r\n");


	//Initialize hardware cryptographic accelerator
	error = stm32h7xxCryptoInit();
	//Any error to report?
	if(error)
	{
		//Debug message
		TRACE_ERROR("Failed to initialize hardware crypto accelerator!\r\n");
	}

	//TCP/IP stack initialization
	error = netInit();
	//Any error to report?
	if(error)
	{
		//Debug message
		TRACE_ERROR("Failed to initialize TCP/IP stack!\r\n");
	}

	//Configure the first Ethernet interface
	interface = &netInterface[0];

	//Set interface name
	netSetInterfaceName(interface, APP_IF_NAME);
	//Set host name
	netSetHostname(interface, APP_HOST_NAME);
	//Set host MAC address
	macStringToAddr(APP_MAC_ADDR, &macAddr);
	netSetMacAddr(interface, &macAddr);
	//Select the relevant network adapter
	netSetDriver(interface, &stm32h7xxEthDriver);
	netSetPhyDriver(interface, &lan8742PhyDriver);

	//Initialize network interface
	error = netConfigInterface(interface);
	//Any error to report?
	if(error)
	{
		//Debug message
		TRACE_ERROR("Failed to configure interface %s!\r\n", interface->name);
	}

#if (IPV4_SUPPORT == ENABLED)
#if (APP_USE_DHCP_CLIENT == ENABLED)
	//Get default settings
	dhcpClientGetDefaultSettings(&dhcpClientSettings);
	//Set the network interface to be configured by DHCP
	dhcpClientSettings.interface = interface;
	//Disable rapid commit option
	dhcpClientSettings.rapidCommit = FALSE;

	//DHCP client initialization
	error = dhcpClientInit(&dhcpClientContext, &dhcpClientSettings);
	//Failed to initialize DHCP client?
	if(error)
	{
		//Debug message
		TRACE_ERROR("Failed to initialize DHCP client!\r\n");
	}

	//Start DHCP client
	error = dhcpClientStart(&dhcpClientContext);
	//Failed to start DHCP client?
	if(error)
	{
		//Debug message
		TRACE_ERROR("Failed to start DHCP client!\r\n");
	}
#else
	//Set IPv4 host address
	ipv4StringToAddr(APP_IPV4_HOST_ADDR, &ipv4Addr);
	ipv4SetHostAddr(interface, ipv4Addr);

	//Set subnet mask
	ipv4StringToAddr(APP_IPV4_SUBNET_MASK, &ipv4Addr);
	ipv4SetSubnetMask(interface, ipv4Addr);

	//Set default gateway
	ipv4StringToAddr(APP_IPV4_DEFAULT_GATEWAY, &ipv4Addr);
	ipv4SetDefaultGateway(interface, ipv4Addr);

	//Set primary and secondary DNS servers
	ipv4StringToAddr(APP_IPV4_PRIMARY_DNS, &ipv4Addr);
	ipv4SetDnsServer(interface, 0, ipv4Addr);
	ipv4StringToAddr(APP_IPV4_SECONDARY_DNS, &ipv4Addr);
	ipv4SetDnsServer(interface, 1, ipv4Addr);
#endif
#endif

#if (IPV6_SUPPORT == ENABLED)
#if (APP_USE_SLAAC == ENABLED)
	//Get default settings
	slaacGetDefaultSettings(&slaacSettings);
	//Set the network interface to be configured
	slaacSettings.interface = interface;

	//SLAAC initialization
	error = slaacInit(&slaacContext, &slaacSettings);
	//Failed to initialize SLAAC?
	if(error)
	{
		//Debug message
		TRACE_ERROR("Failed to initialize SLAAC!\r\n");
	}

	//Start IPv6 address autoconfiguration process
	error = slaacStart(&slaacContext);
	//Failed to start SLAAC process?
	if(error)
	{
		//Debug message
		TRACE_ERROR("Failed to start SLAAC!\r\n");
	}
#else
	//Set link-local address
	ipv6StringToAddr(APP_IPV6_LINK_LOCAL_ADDR, &ipv6Addr);
	ipv6SetLinkLocalAddr(interface, &ipv6Addr);

	//Set IPv6 prefix
	ipv6StringToAddr(APP_IPV6_PREFIX, &ipv6Addr);
	ipv6SetPrefix(interface, 0, &ipv6Addr, APP_IPV6_PREFIX_LENGTH);

	//Set global address
	ipv6StringToAddr(APP_IPV6_GLOBAL_ADDR, &ipv6Addr);
	ipv6SetGlobalAddr(interface, 0, &ipv6Addr);

	//Set default router
	ipv6StringToAddr(APP_IPV6_ROUTER, &ipv6Addr);
	ipv6SetDefaultRouter(interface, 0, &ipv6Addr);

	//Set primary and secondary DNS servers
	ipv6StringToAddr(APP_IPV6_PRIMARY_DNS, &ipv6Addr);
	ipv6SetDnsServer(interface, 0, &ipv6Addr);
	ipv6StringToAddr(APP_IPV6_SECONDARY_DNS, &ipv6Addr);
	ipv6SetDnsServer(interface, 1, &ipv6Addr);
#endif
#endif

/*
	//Load Update public signature key
	error = updateUserLoadKeys();
	if (error)
	{
		//Debug message
		TRACE_ERROR("Failed to load Update user keys!\r\n");
		return error;
	}

	//Create IAP mutex
	if(!osCreateMutex(&updateMutex))
	{
		//Debug message
		TRACE_ERROR("Failed to create IAP mutex!\r\n");
	}
*/
	//Set LED task parameters
	taskParams = OS_TASK_DEFAULT_PARAMS;
	taskParams.stackSize = 200;
	taskParams.priority = OS_TASK_PRIORITY_NORMAL;
	//Create a task to blink the LED
	taskId = osCreateTask("LED", ledTask, NULL, &taskParams);
	//Failed to create the task?
	if(taskId == OS_INVALID_TASK_ID)
	{
		//Debug message
		TRACE_ERROR("Failed to create task!\r\n");
	}

	/* USER CODE END App_ThreadX_Init */

	return ret;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  MX_ThreadX_Init();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x20000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x30000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
