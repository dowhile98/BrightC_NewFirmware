/*
 * brightc_network.c
 *
 *  Created on: Apr 28, 2025
 *      Author: jeffr
 */

/*Includes --------------------------------------------------------------------------------------------------------------------------*/
#include "app_threadx.h"
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
#include "brightc_typedefs.h"

/*Defines ---------------------------------------------------------------------------------------------------------------------------*/
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

/*Global variables -----------------------------------------------------------------------------------------------------------------*/
DhcpClientSettings dhcpClientSettings;
DhcpClientContext dhcpClientContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;

//Update signature public key declaration
const uint8_t* pemUpdtSignPublicKey;
size_t pemUpdtSignPublicKeyLen;

//Update mutex handler
OsMutex updateMutex;


brightc_network_conf_t brightc_network_conf;
/*Function prototype ---------------------------------------------------------------------------------------------------------------*/

/*Function definition --------------------------------------------------------------------------------------------------------------*/
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


UINT brightc_network_init(void *memory){
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
	/* USER CODE END App_ThreadX_Init */

	return ret;

	return ret;
}

