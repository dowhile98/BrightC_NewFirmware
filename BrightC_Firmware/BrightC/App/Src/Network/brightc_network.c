/*
 * brightc_network.c
 *
 *  Created on: May 26, 2025
 *      Author: tecna-smart-lab
 */


/*Includes ----------------------------------------------------------------------------------------------------------------*/
#include "brightc.h"


/*Defines -----------------------------------------------------------------------------------------------------------------*/
#ifndef APP_IF_NAME
#define APP_IF_NAME "eth0"
#endif

#ifndef APP_HOST_NAME
#define APP_HOST_NAME "brightc"
#endif

#ifndef APP_MAC_ADDR
#define APP_MAC_ADDR "00-AB-CD-EF-07-69"
#endif

/*global variables --------------------------------------------------------------------------------------------------------*/
static const char *TAG = "NETWORK";
/*Private variables -------------------------------------------------------------------------------------------------------*/

/*private function prototype ----------------------------------------------------------------------------------------------*/


/*function definition -----------------------------------------------------------------------------------------------------*/
UINT brightc_network_init(void *memory){
	UINT ret = TX_SUCCESS;

	error_t error;
	NetInterface *interface;
	MacAddr macAddr;
	DhcpClientSettings dhcpClientSettings;

	/*Init ---------------------------------------------------------------------------------------------------------------*/
	STM32_LOGI(TAG, "****************************************************\r\n");
	STM32_LOGI(TAG, "***                  CycloneTCP                  ***\r\n");
	STM32_LOGI(TAG, "****************************************************\r\n");
	/*todo: Reset ETH interface*/

	/*Initialize hardware cryptographic accelerator*/
	error = stm32h7xxCryptoInit();

	if(error){
		STM32_LOGE(TAG, "¡Error al inicializar el acelerador de cifrado de hardware!");
	}

	/*TCP/IP stack initialization*/
	error = netInit();
	if(error){
		STM32_LOGE(TAG, "¡Error al inicializar la pila TCP/IP!");
	}
	/*Configure the first Ethernet interface*/
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
		STM32_LOGE(TAG, "No se pudo configurar la interfaz %s!\r\n", interface->name);
	}
	/*Configure DHCP -----------------------------------------------------------------------------------------------------*/


	return ret;
}

/*Private function definition ---------------------------------------------------------------------------------------------*/
