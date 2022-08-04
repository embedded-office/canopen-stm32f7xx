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
add_executable(QuickstartApp)

#---
# application with object dictionary and driver functions
#
target_sources(QuickstartApp
  PRIVATE
    # application
    app/clock_app.c
    app/clock_spec.c
    # board
    board/bsp_board.c
    board/bsp_cfg.c
    # driver
    driver/drv_can_dummy.c
    driver/drv_nvm_dummy.c
    driver/drv_timer_swcycle.c
  )
target_include_directories(QuickstartApp
  PRIVATE
    app
    board
    driver
)

#---
# specify the dependencies for this application
#
target_link_libraries(QuickstartApp Canopen Bsp)

#---
# specify the linker file and the resulting image
#
setLinkerScript(QuickstartApp "${CMAKE_CURRENT_SOURCE_DIR}/board/basic_flash")
setOutfile(QuickstartApp "quickstart-app")
set(CMAKE_EXECUTABLE_SUFFIX .elf)
