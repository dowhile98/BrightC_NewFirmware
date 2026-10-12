# DAECGE BrightC Controller V3 Firmware

Bienvenido a la documentación de la API del firmware para el **DAECGE BrightC Controller V3**, desarrollado sobre **Zephyr RTOS**.

## Descripción del Proyecto

El sistema está diseñado para la automatización, monitoreo ambiental y control industrial sobre la placa base **BrightC-B1 BaseBoard V3**, gobernada por el microcontrolador de alto rendimiento **STM32H743ZIT6** (ARM Cortex-M7 a 480 MHz).

### Módulos Principales
- **Drivers de E/S y Expansores:** Soporte para expansores SPI **MCP23S17** para alarmas digitales y relés HVAC.
- **Comunicaciones:** Ethernet (RMII LAN8742A), doble canal RS-485 aislado (USART2 y USART6), interfaz Wi-Fi (ESP32 via UART4) y pantalla táctil DWIN LCD (USART3).
- **Almacenamiento:** Sistema de archivos LittleFS en memoria Flash QSPI W25Q16 y EEPROM I2C AT24C256.
- **Sensores:** Entradas analógicas multicanal (ADC1, ADC2, ADC3) para sondas de temperatura y presión.
- **Testing:** Suite completa de pruebas unitarias basadas en Zephyr ZTest y orquestadas con Twister.
