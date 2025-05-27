/*
 * brightc_typedefs.h
 *
 *  Created on: Apr 29, 2025
 *      Author: jeffr
 */

#ifndef APP_INCLUDE_BRIGHTC_TYPEDEFS_H_
#define APP_INCLUDE_BRIGHTC_TYPEDEFS_H_

/*Includes ---------------------------------------------------------------------------------------------------------------------------*/
#include "tx_api.h"
#include "lwprintf.h"
#include <stdlib.h>
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "core/net.h"
#include "drivers/mac/stm32h7xx_eth_driver.h"
#include "drivers/phy/lan8742_driver.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"

/*Defines ----------------------------------------------------------------------------------------------------------------------------*/

#ifndef BRIGHTC_IP_SIZE_LENGTH
#define BRIGHTC_IP_SIZE_LENGTH	32
#endif

#ifndef BRIGHTC_SNMP_MAX_MANAGER
#define BRIGHTC_SNMP_MAX_MANAGER		4
#endif


/*typedefs ---------------------------------------------------------------------------------------------------------------------------*/
typedef struct brigthc_network_conf{
	/*address*/
	Ipv4Addr ip;						//device ip
	Ipv4Addr gateway;					//device gateway
	Ipv4Addr netmask;
	MacAddr macAddr;
	/*DHCP client enable*/
	uint8_t dhcp_client;
	/*snmp manager address*/
	Ipv4Addr manager[BRIGHTC_SNMP_MAX_MANAGER];
	/*Thingsboard IP address*/
	Ipv4Addr thingsboard_ip;
	/*CRC*/
	uint32_t crc;
}brightc_network_conf_t;


typedef struct brightc_network{
	NetInterface *interface;

	DhcpClientSettings dhcpClientSettings;
	DhcpClientContext dhcpClientContext;
}brightc_network_t;

#endif /* APP_INCLUDE_BRIGHTC_TYPEDEFS_H_ */
