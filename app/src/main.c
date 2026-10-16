/*
 * Copyright (c) 2026 DAECGE
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/app_version.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_context.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/wifi_mgmt.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define SLEEP_TIME_MS 500

#define NET_EVENT_WIFI_MASK \
	(NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT)

#define NET_EVENT_IPV4_MASK \
	(NET_EVENT_IPV4_ADDR_ADD | NET_EVENT_IPV4_DHCP_BOUND)

/* Devicetree aliases */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0});
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios, {0});

static struct net_if *sta_iface;
static struct wifi_connect_req_params sta_config;
static struct net_mgmt_event_callback wifi_cb;
static struct net_mgmt_event_callback ipv4_cb;

static void wifi_event_handler(struct net_mgmt_event_callback *cb, uint64_t mgmt_event,
							   struct net_if *iface)
{
	const struct wifi_status *status = (const struct wifi_status *)cb->info;

	switch (mgmt_event)
	{
	case NET_EVENT_WIFI_CONNECT_RESULT:
		if (status && status->status)
		{
			LOG_ERR("Wi-Fi connection request failed (%d)", status->status);
		}
		else
		{
			LOG_INF("Wi-Fi STA connected! Waiting for DHCPv4 lease...");
		}
		break;
	case NET_EVENT_WIFI_DISCONNECT_RESULT:
		if (status && status->status)
		{
			LOG_WRN("Wi-Fi disconnect request status (%d)", status->status);
		}
		else
		{
			LOG_INF("Wi-Fi STA disconnected");
		}
		break;
	default:
		break;
	}
}

static void ipv4_event_handler(struct net_mgmt_event_callback *cb, uint64_t mgmt_event,
							   struct net_if *iface)
{
	switch (mgmt_event)
	{
	case NET_EVENT_IPV4_ADDR_ADD:
	{
		for (int i = 0; i < NET_IF_MAX_IPV4_ADDR; i++)
		{
			char ip_buf[NET_IPV4_ADDR_LEN];
			char mask_buf[NET_IPV4_ADDR_LEN];
			char gw_buf[NET_IPV4_ADDR_LEN];

			if (!iface->config.ip.ipv4)
			{
				continue;
			}

			if (!iface->config.ip.ipv4->unicast[i].ipv4.is_used)
			{
				continue;
			}

			enum net_addr_type addr_type =
				iface->config.ip.ipv4->unicast[i].ipv4.addr_type;

			LOG_INF("IPv4 Address assigned on interface %d (%s):",
					net_if_get_by_iface(iface),
					addr_type == NET_ADDR_DHCP ? "DHCP" : "Static/Manual");
			LOG_INF("  IP Address : %s",
					net_addr_ntop(
						NET_AF_INET,
						&iface->config.ip.ipv4->unicast[i].ipv4.address.in_addr,
						ip_buf, sizeof(ip_buf)));
			LOG_INF("  Netmask    : %s",
					net_addr_ntop(NET_AF_INET,
								  &iface->config.ip.ipv4->unicast[i].netmask,
								  mask_buf, sizeof(mask_buf)));
			LOG_INF("  Gateway    : %s",
					net_addr_ntop(NET_AF_INET, &iface->config.ip.ipv4->gw, gw_buf,
								  sizeof(gw_buf)));
		}
		break;
	}
	case NET_EVENT_IPV4_DHCP_BOUND:
		if (iface->config.ip.ipv4)
		{
			LOG_INF("DHCPv4 lease bound (lease time: %u seconds)",
					iface->config.dhcpv4.lease_time);
		}
		break;
	default:
		break;
	}
}

static int connect_to_wifi(void)
{
	if (!sta_iface)
	{
		LOG_ERR("Wi-Fi STA interface is not initialized");
		return -EIO;
	}

	sta_config.ssid = (const uint8_t *)CONFIG_WIFI_SAMPLE_SSID;
	sta_config.ssid_length = strlen(CONFIG_WIFI_SAMPLE_SSID);
	sta_config.psk = (const uint8_t *)CONFIG_WIFI_SAMPLE_PSK;
	sta_config.psk_length = strlen(CONFIG_WIFI_SAMPLE_PSK);
	sta_config.security = (sta_config.psk_length > 0) ? WIFI_SECURITY_TYPE_PSK
													  : WIFI_SECURITY_TYPE_NONE;
	sta_config.channel = WIFI_CHANNEL_ANY;
	sta_config.band = WIFI_FREQ_BAND_2_4_GHZ;

	LOG_INF("Connecting automatically to Wi-Fi SSID: %s", CONFIG_WIFI_SAMPLE_SSID);

	int ret = net_mgmt(NET_REQUEST_WIFI_CONNECT, sta_iface, &sta_config,
					   sizeof(struct wifi_connect_req_params));
	if (ret)
	{
		LOG_ERR("Unable to request connection to (%s), err: %d",
				CONFIG_WIFI_SAMPLE_SSID, ret);
	}

	return ret;
}

int main(void)
{
	int ret;

	LOG_INF("========================================");
	LOG_INF("  DAECGE BrightC Controller V3 Firmware");
	LOG_INF("  Versión: %s", APP_VERSION_STRING);
	LOG_INF("========================================");

	/* Configuración del LED de usuario (PF2) */
	if (!gpio_is_ready_dt(&led))
	{
		LOG_ERR("Dispositivo GPIO para LED no está listo");
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		LOG_ERR("Fallo al configurar pin LED (%d)", ret);
		return 0;
	}
	LOG_INF("LED de usuario (PF2) configurado.");

	/* Configuración del Botón de usuario (PC13) */
	if (gpio_is_ready_dt(&button))
	{
		ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
		if (ret < 0)
		{
			LOG_WRN("Fallo al configurar pin de botón (%d)", ret);
		}
		else
		{
			LOG_INF("Botón de usuario (PC13) configurado.");
		}
	}

	/* Register Wi-Fi and IPv4 management callbacks */
	net_mgmt_init_event_callback(&wifi_cb, wifi_event_handler, NET_EVENT_WIFI_MASK);
	net_mgmt_add_event_callback(&wifi_cb);

	net_mgmt_init_event_callback(&ipv4_cb, ipv4_event_handler, NET_EVENT_IPV4_MASK);
	net_mgmt_add_event_callback(&ipv4_cb);

	sta_iface = net_if_get_wifi_sta();
	if (!sta_iface)
	{
		LOG_WRN("Wi-Fi STA network interface not found");
	}
	else
	{
		LOG_INF("Wi-Fi STA interface ready (iface %d)", net_if_get_by_iface(sta_iface));
		if (strlen(CONFIG_WIFI_SAMPLE_SSID) > 0)
		{
			connect_to_wifi();
		}
		else
		{
			LOG_INF("No automatic SSID configured (CONFIG_WIFI_SAMPLE_SSID is empty).");
			LOG_INF("Use the serial shell to connect interactively:");
			LOG_INF("  wifi scan");
			LOG_INF("  wifi connect -s \"<SSID>\" -k 1 -p \"<PSK>\"");
			LOG_INF("Or set CONFIG_WIFI_SAMPLE_SSID in prj.conf.");
		}
	}

	LOG_INF("Sistema inicializado. Iniciando heartbeat...");

	while (1)
	{
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0)
		{
			LOG_ERR("Fallo al alternar LED (%d)", ret);
		}

		if (gpio_is_ready_dt(&button))
		{
			int val = gpio_pin_get_dt(&button);
			if (val > 0)
			{
				LOG_INF("¡Botón de usuario presionado!");
			}
		}

		k_msleep(SLEEP_TIME_MS);
	}

	return 0;
}
