# DAECGE BrightC Controller V3 Firmware

<a href="https://github.com/dowhile98/BrightC_NewFirmware/actions/workflows/build.yml">
  <img src="https://github.com/dowhile98/BrightC_NewFirmware/actions/workflows/build.yml/badge.svg" alt="Build Status">
</a>
<a href="https://github.com/dowhile98/BrightC_NewFirmware/actions/workflows/docs.yml">
  <img src="https://github.com/dowhile98/BrightC_NewFirmware/actions/workflows/docs.yml/badge.svg" alt="Documentation Status">
</a>
<a href="https://zephyrproject.org">
  <img src="https://img.shields.io/badge/RTOS-Zephyr-blue.svg" alt="Zephyr RTOS">
</a>
<a href="https://www.st.com/en/microcontrollers-microprocessors/stm32h743zi.html">
  <img src="https://img.shields.io/badge/Target-STM32H743ZI-002050.svg?logo=stmicroelectronics" alt="STM32H743ZI">
</a>

Firmware para el controlador industrial **DAECGE BrightC Controller V3** sobre la placa base **BrightC-B1 BaseBoard V3**, desarrollado con **Zephyr RTOS** (Hardware Model v2) siguiendo principios de **Clean Architecture** y **Test-Driven Development (TDD)**.

---

## 1. Características de Hardware y Arquitectura

El controlador está basado en el microcontrolador de alto rendimiento **STM32H743ZIT6** (empaquetado LQFP144):

* **Núcleo y Frecuencia:** ARM Cortex-M7 con FPU de doble precisión, reloj principal a **480 MHz** (desde cristal HSE de 25 MHz), RTC con LSE de 32.768 kHz.
* **Memoria:** 2 MB Flash dual-bank interna, 1 MB SRAM (incluye DTCM y AXI SRAM).
* **Comunicaciones Industriales:**
  * **Ethernet:** Interfaz RMII nativa con PHY LAN8742A (+ opción SPI con W5500).
  * **Consola de Sistema:** USB-to-Serial vía USART1 (PB14 TX / PB15 RX) a 115200 8N1.
  * **RS-485:** Dos canales diferenciales independientes con control de dirección DE/RE (USART2 en PD5/PD6/PD4 y USART6 en PC6/PC7/PG8).
  * **Pantalla HMI:** Interfaz para pantalla LCD táctil DWIN vía USART3 (PB10/PB11) con control de alimentación y flag de sobrecarga (PD9/PD8).
  * **Conectividad Wi-Fi:** Coprocesador ESP32 vía UART4 (PD1/PD0) con control de reset dedicado (PD14).
* **Expansores de E/S y Relés:**
  * Clúster SPI1 con 4 expansores **Microchip MCP23S17** para:
    * 24 canales de entradas de alarma digital con interrupción por hardware.
    * 18 canales de salida para relés (HVAC, ventiladores inteligentes y alarmas del sistema).
* **Adquisición Analógica:**
  * ADC1 (9 canales analógicos para sensores de temperatura y presión).
  * ADC2 (2 canales analógicos).
  * ADC3 (4 canales analógicos).
* **Almacenamiento y Archivos:**
  * Memoria Flash externa QSPI **W25Q16** integrada con sistema de archivos **LittleFS** (`/lfs`).
  * Ranura MicroSD sobre bus **SDMMC1** de 4 bits.
  * Memoria EEPROM I2C2 **AT24C256** (256 Kbit).
* **Indicadores y Actuadores:**
  * Buzzer con control PWM sobre TIM15 Canal 1 (PE5).
  * LED de usuario de alta visibilidad (PF2) y Botón de usuario (PC13).

---

## 2. Estructura del Repositorio

El proyecto implementa separación de responsabilidades por capas:

```text
BrightC_NewFirmware/
├── app/                              # Aplicación de firmware (código ejecutable)
│   ├── src/main.c                    # Punto de entrada y tareas del kernel Zephyr
│   ├── CMakeLists.txt                # Configuración de compilación del firmware
│   ├── prj.conf                      # Kconfig base del producto
│   ├── debug.conf                    # Configuración overlay para depuración
│   └── tests.yaml                    # Especificación de smoke test para Twister
├── boards/                           # Definiciones de hardware out-of-tree
│   └── daecge/
│       └── brightc-controllerv3/     # Soporte completo para BrightC Controller V3
│           ├── board.yml             # Metadatos del Hardware Model v2
│           ├── brightc-controllerv3.dts # Devicetree con el mapeo de pines y periféricos
│           ├── brightc-controllerv3_defconfig
│           └── doc/                  # Documentación de hardware y esquema de pines
├── drivers/                          # Drivers específicos de periféricos externos
├── lib/                              # Librerías de dominio y lógica pura (desacopladas)
│   └── custom/                       # Módulos de lógica reutilizables
├── tests/                            # Suites de pruebas automatizadas (ZTest / Twister)
│   ├── README.md                     # Guía de pruebas unitarias y TDD
│   └── lib/custom/                   # Pruebas unitarias de librerías
├── doc/                              # Documentación del proyecto (Doxygen + Sphinx)
└── west.yml                          # Manifiesto de dependencias de Zephyr y módulos
```

---

## 3. Preparación del Entorno de Desarrollo

Para compilar y trabajar con este repositorio se utiliza la herramienta oficial **West** de Zephyr.

### Inicialización del Workspace

1. Clona el repositorio e inicializa el espacio de trabajo de West:
   ```bash
   west init -m https://github.com/dowhile98/BrightC_NewFirmware --mr main brightc_workspace
   cd brightc_workspace
   ```

2. Descarga Zephyr RTOS y todos los módulos requeridos por el manifiesto:
   ```bash
   west update
   ```

3. Instala las dependencias de Python:
   ```bash
   pip install -r zephyr/scripts/requirements.txt
   ```

---

## 4. Compilación y Grabación del Firmware

### Compilación Estándar (Modo Producción)

Para compilar el firmware dirigido a la placa **DAECGE BrightC Controller V3**:

```bash
west build -b brightc-controllerv3 app
```

El binario resultante se generará en `build/zephyr/zephyr.bin` (y `zephyr.elf`).

### Compilación con Modo Depuración

Para habilitar logging extendido e información de depuración:

```bash
west build -b brightc-controllerv3 app -- -DEXTRA_CONF_FILE=debug.conf
```

### Grabación (Flashing) vía SWD

El flasheo se realiza a través de la interfaz SWD mediante ST-Link, OpenOCD o J-Link:

```bash
# Flasheo predeterminado con STM32CubeProgrammer
west flash

# Alternativa mediante OpenOCD
west flash --runner openocd

# Alternativa mediante J-Link
west flash --runner jlink
```

---

## 5. Pruebas Automatizadas y TDD

El proyecto cuenta con una estrategia de pruebas multinivel orquestada por **Twister**:

```bash
# 1. Smoke Test de compilación del firmware de producción
west twister -T app -v --inline-logs --integration

# 2. Ejecutar la suite completa de pruebas unitarias y de integración
west twister -T tests -v --inline-logs --integration
```

### Ciclo TDD Rápido en Host (`native_sim` / QEMU)

Durante el ciclo diario de desarrollo (Red -> Green -> Refactor), las pruebas de lógica se ejecutan localmente en 1 a 2 segundos sin necesidad de hardware:

```bash
# Ejecución nativa en el host
west build -b native_sim tests/lib/custom -p always -t run

# Ejecución emulada en QEMU ARM Cortex-M3
west build -b qemu_cortex_m3 tests/lib/custom -p always -t run
```

### Simulación Virtual de Periféricos con Renode

Para pruebas de integración donde se requiere emular periféricos específicos del **STM32H743ZI** (como los expansores SPI **MCP23S17**, memorias QSPI o transceptores RS-485), se cuenta con soporte para simulación virtual mediante **Renode** (Antmicro), permitiendo validar el comportamiento del sistema sin hardware físico conectado.

Para más detalles, consulta la [Guía de Pruebas](tests/README.md).

---

## 6. Generación de Documentación

El proyecto incluye configuración para generar documentación de API con **Doxygen** y manuales técnicos con **Sphinx**:

```bash
cd doc

# Instalar dependencias de documentación
pip install -r requirements.txt

# Generar documentación de la API con Doxygen
doxygen

# Generar documentación en HTML con Sphinx
make html
```

Los documentos generados se ubicarán en `doc/_build_sphinx/html/index.html`.
