# Guía de Pruebas y TDD: DAECGE BrightC Controller V3

Este directorio contiene las suites de pruebas automatizadas para el firmware del **DAECGE BrightC Controller V3**, utilizando el framework nativo **Zephyr ZTest**, el orquestador **Twister**, y la plataforma de simulación virtual **Renode**.

---

## 1. Filosofía de Pruebas y Arquitectura

El desarrollo de firmware en este proyecto sigue los principios de **Clean Architecture** (Puertos y Adaptadores) y **Desarrollo Guiado por Pruebas (TDD)**:

```
                  +-----------------------------------+
                  |        Hardware Físico / HIL      |
                  |     (STM32H743ZI BaseBoard V3)     |
                  +-----------------+-----------------+
                                    |
                  +-----------------v-----------------+
                  |   Simulación Virtual con Renode   |
                  |  (Periféricos STM32H7, SPI, UART) |
                  +-----------------+-----------------+
                                    |
                  +-----------------v-----------------+
                  |    Emulación ARM QEMU Cortex-M3   |
                  |   (Primitivas de Kernel, Timers)  |
                  +-----------------+-----------------+
                                    |
                  +-----------------v-----------------+
                  |     Host Nativo (native_sim)      |
                  |  (Lógica de negocio pura, TDD)    |
                  +-----------------------------------+
```

1. **Lógica desacoplada (Dominio y Librerías):** Algoritmos, máquinas de estado, parsers de protocolos y formateo de telemetría no dependen directamente de registros de hardware STM32, permitiendo ejecutarse en milisegundos en la máquina local (`native_sim`).
2. **Drivers y Periféricos:** Aislados detrás de interfaces (APIs de Zephyr o puertos específicos), testeables mediante *Fakes* en memoria o virtualización en *Renode*.
3. **Ciclo TDD (Red -> Green -> Refactor):**
   * **🔴 RED:** Escribir una prueba unitaria que exprese el comportamiento esperado y verificar que falle.
   * **🟢 GREEN:** Implementar el código mínimo indispensable para que la prueba pase.
   * **🔵 REFACTOR:** Mejorar el diseño, nombres y estructura preservando las pruebas en verde.

---

## 2. Estructura Modular del Directorio `tests/`

Las pruebas se organizan por capas y módulos independientes:

```text
tests/
├── README.md               # Esta guía
├── lib/                    # Pruebas de librerías y lógica de negocio pura
│   └── custom/
│       ├── CMakeLists.txt  # Definición CMake del ejecutable de prueba
│       ├── prj.conf        # Configuración Kconfig de la prueba (CONFIG_ZTEST=y)
│       ├── tests.yaml      # Manifiesto Twister (plataformas y escenarios)
│       └── src/
│           └── main.c      # Suite ZTest con casos de prueba
├── drivers/                # Pruebas de drivers con Fakes/Stubs (ej. MCP23S17, relays)
└── services/               # Pruebas de integración de servicios (ej. telemetría, alarmas)
```

Cada subdirectorio de prueba es un proyecto autónomo de Zephyr que incluye:
* `CMakeLists.txt`: Incluye el boilerplate de Zephyr (`find_package(Zephyr REQUIRED)`) y los archivos fuentes bajo prueba.
* `prj.conf`: Activa el subsistema de pruebas (`CONFIG_ZTEST=y`) y las dependencias necesarias.
* `tests.yaml`: Configura las plataformas de integración y variantes de prueba para Twister.
* `src/main.c`: Contiene las suites `ZTEST_SUITE` y funciones `ZTEST`.

---

## 3. Uso del Framework Zephyr ZTest

Zephyr ZTest v3+ organiza las pruebas en **Suites** con un ciclo de vida bien definido:

### Ejemplo de Suite con Ciclo de Vida (`src/main.c`)

```c
#include <zephyr/ztest.h>
#include <app/lib/custom.h>

/* Setup ejecutado antes de CADA caso de prueba (equivalente a TEST_SETUP) */
static void custom_before(void *fixture)
{
    /* Inicializar variables o resetear contadores */
}

/* Teardown ejecutado después de CADA caso de prueba (equivalente a TEST_TEAR_DOWN) */
static void custom_after(void *fixture)
{
    /* Limpiar memoria o liberar recursos */
}

/* Registro de la suite: (nombre, predicado, setup_suite, before_each, after_each, teardown_suite) */
ZTEST_SUITE(custom_lib, NULL, NULL, custom_before, custom_after, NULL);

/* Caso de prueba */
ZTEST(custom_lib, test_comportamiento_base)
{
    zassert_equal(custom_get_value(10), 10, "El valor obtenido no coincide con el esperado");
    zassert_not_null(custom_get_ptr(), "El puntero no debe ser nulo");
}
```

### Aserciones Comunes

* `zassert_equal(actual, expected, "mensaje")`: Comprueba igualdad escalar.
* `zassert_true(condicion, "mensaje")`: Comprueba que una expresión sea verdadera.
* `zassert_false(condicion, "mensaje")`: Comprueba que una expresión sea falsa.
* `zassert_not_null(puntero, "mensaje")`: Comprueba puntero no nulo.
* `zassert_mem_equal(buf1, buf2, longitud, "mensaje")`: Comprueba igualdad de buffers de memoria.
* `zassert_str_equal(str1, str2, "mensaje")`: Comprueba igualdad de strings terminados en null.

---

## 4. Manifiesto Twister (`tests.yaml`)

El archivo `tests.yaml` le indica a Twister qué plataformas compilar y ejecutar, permitiendo probar la misma librería con diferentes configuraciones Kconfig:

```yaml
common:
  tags: unit_test
  integration_platforms:
    - native_sim            # Ejecución nativa ultra rápida en Host
    - qemu_cortex_m3        # Emulación de arquitectura ARM Cortex-M
    - brightc-controllerv3  # Verificación de compilación para la placa física
tests:
  lib.custom: {}
  lib.custom.non_default:
    extra_configs:
      - CONFIG_CUSTOM_GET_VALUE_DEFAULT=42  # Sobrescribe Kconfig para este escenario
```

---

## 5. Simulación Virtual con Renode

Para pruebas de integración donde se requiere emular periféricos específicos del microcontrolador **STM32H743ZIT6** (como los expansores SPI **MCP23S17**, memoria flash **QSPI W25Q16**, transceptores **RS-485** o el coprocesador **ESP32**), se utiliza **Renode**:

1. **Archivo de Plataforma (`.repl`):** Describe los periféricos de la placa base BrightC-B1 V3 conectados al bus del STM32H7.
2. **Script de Inicialización (`.resc`):** Crea la máquina virtual, carga la plataforma, inyecta el binario ELF compilado y arranca la CPU.
3. **Automatización Headless en CI:** Permite arrancar la simulación desde scripts de Python (`renode --console --plain`) e inspeccionar el estado de los pines GPIO, buffers UART o tráfico de red virtual para validar aserciones a nivel de sistema.

---

## 6. Comandos de Ejecución

### A. Ciclo TDD Rápido en Host (`native_sim` / `qemu_cortex_m3`)
Compila y corre directamente en la consola en 1 a 2 segundos:
```bash
# Ejecución nativa (requiere gcc-multilib en Linux)
west build -b native_sim tests/lib/custom -p always -t run

# Ejecución emulada en QEMU ARM Cortex-M3 (usa arm-zephyr-eabi)
west build -b qemu_cortex_m3 tests/lib/custom -p always -t run
```

### B. Ejecutar un Módulo con Twister
```bash
west twister -T tests/lib/custom -v --inline-logs --integration
```

### C. Ejecutar la Suite Completa en CI
```bash
# 1. Smoke test de compilación del firmware de producción
west twister -T app -v --inline-logs --integration

# 2. Todas las suites de pruebas unitarias y funcionales
west twister -T tests -v --inline-logs --integration
```
