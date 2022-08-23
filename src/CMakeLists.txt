#*****************************************************************************
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
add_executable(canopen-stm32f7xx)

#---
# application with object dictionary and driver functions
#
target_sources(canopen-stm32f7xx
  PRIVATE
    # application
    app/clock_app.c
    app/clock_hw.c
    app/clock_spec.c
    # system startup
    config/startup_main.c
    config/startup_stm32f769xx.s
    config/system_stm32f7xx.c
    # driver
    driver/stm32f7xx-hal/drv_can_can1.c
    driver/stm32f7xx-hal/drv_nvm_i2c1_at24c256.c
    driver/stm32f7xx-hal/drv_timer_tim2.c
)

target_include_directories(canopen-stm32f7xx
  PRIVATE
    app
    config
    driver/stm32f7xx-hal
)

#---
# specify the dependencies for this application
#
target_link_libraries(canopen-stm32f7xx canopen-stack stm32f7xx-hal)

#---
# specify the linker file and the resulting image
#
setLinkerScript(canopen-stm32f7xx "${CMAKE_CURRENT_SOURCE_DIR}/config/stm32f769i_discovery")
setOutfile(canopen-stm32f7xx "canopen-stm32f7xx")
