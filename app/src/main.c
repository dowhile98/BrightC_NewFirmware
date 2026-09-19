/*
 * Copyright (c) 2026 DAECGE
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/app_version.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define SLEEP_TIME_MS 500

/* Devicetree aliases */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0});
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios, {0});

int main(void)
{
	int ret;

	LOG_INF("========================================");
	LOG_INF("  DAECGE BrightC Controller V3 Firmware");
	LOG_INF("  Versión: %s", APP_VERSION_STRING);
	LOG_INF("========================================");

	/* Configuración del LED de usuario (PF2) */
	if (!gpio_is_ready_dt(&led)) {
		LOG_ERR("Dispositivo GPIO para LED no está listo");
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		LOG_ERR("Fallo al configurar pin LED (%d)", ret);
		return 0;
	}
	LOG_INF("LED de usuario (PF2) configurado.");

	/* Configuración del Botón de usuario (PC13) */
	if (gpio_is_ready_dt(&button)) {
		ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
		if (ret < 0) {
			LOG_WRN("Fallo al configurar pin de botón (%d)", ret);
		} else {
			LOG_INF("Botón de usuario (PC13) configurado.");
		}
	}

	LOG_INF("Sistema inicializado. Iniciando heartbeat...");

	while (1) {
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			LOG_ERR("Fallo al alternar LED (%d)", ret);
		}

		if (gpio_is_ready_dt(&button)) {
			int val = gpio_pin_get_dt(&button);
			if (val > 0) {
				LOG_INF("¡Botón de usuario presionado!");
			}
		}

		k_msleep(SLEEP_TIME_MS);
	}

	return 0;
}
