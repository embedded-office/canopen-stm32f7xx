#/*****************************************************************************
#   Copyright 2020 Embedded Office GmbH & Co. KG
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#******************************************************************************

#---
# define the application executable
#
add_executable(canopen-stm32f769)

#---
# application with object dictionary and driver functions
#
target_sources(canopen-stm32f769
  PRIVATE
    # application
    app/clock_app.c
    app/clock_spec.c
    # board configuration
    config/main.c
    config/startup_stm32f769xx.s
    config/stm32f7xx_hal_msp.c
    config/stm32f7xx_it.c
    config/system_stm32f7xx.c
    # driver
    driver/drv_can_dummy.c
    driver/drv_nvm_dummy.c
    driver/drv_timer_swcycle.c
  )
target_include_directories(canopen-stm32f769
  PRIVATE
    app
    config
    driver
)

#---
# specify the dependencies for this application
#
target_link_libraries(canopen-stm32f769 canopen-stack stm32f7xx-hal)

#---
# specify the linker file and the resulting image
#
setLinkerScript(canopen-stm32f769 "${CMAKE_CURRENT_SOURCE_DIR}/config/stm32f769i_discovery")
setOutfile(canopen-stm32f769 "quickstart-app")
