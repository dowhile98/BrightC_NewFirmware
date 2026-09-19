# Install script for directory: /media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/zephyr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/home/tecna/zephyr-sdk-1.0.1/gnu/arm-zephyr-eabi/bin/arm-zephyr-eabi-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/zephyr/arch/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/zephyr/lib/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/zephyr/soc/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/zephyr/boards/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/zephyr/subsys/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/zephyr/drivers/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/modules/BrightC_NewFirmware/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/modules/microconf/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/modules/cmsis-dsp/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/modules/cmsis-nn/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/modules/cmsis_6/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/modules/fatfs/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/modules/hal_nordic/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/modules/hal_stm32/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/modules/lvgl/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/modules/mbedtls/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/modules/mipi-sys-t/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/modules/percepio/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/modules/segger/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/modules/zcbor/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/zephyr/kernel/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/zephyr/cmake/flash/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/zephyr/cmake/usage/cmake_install.cmake")
  include("/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/zephyr/cmake/reports/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/media/tecna/disk11/linux/embedded-mcu/BrightC_Workspace/BrightC_NewFirmware/app/build/primary/zephyr/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
