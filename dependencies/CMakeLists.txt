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
# Note: With the default presets, the downloaded components are cached in
#       `/build/_deps`. You may add a `CMakeUserPresets.json` and set the
#       variable `CPM_SOURCE_CACHE` to a location on your host for global
#       component caching.

#---
# declare the dependency to external projects
#
CPMAddPackage(
  NAME               canopen-stack
  GITHUB_REPOSITORY  embedded-office/canopen-stack
  VERSION            4.4.0
  GIT_TAG            develop-type-modules
)

CPMAddPackage(
  NAME               stm32f7xx-hal
  GITHUB_REPOSITORY  embedded-office/STM32CubeF7
  VERSION            1.17.0
  GIT_TAG            924cc1b6e48c08738d25b89d21629d369613bb58
  OPTIONS
    "STM_USE_HAL_DRIVER true"
    "STM_USE_FULL_LL_DRIVER true"
    "STM_HAL_DEVICE STM32F769xx"
    "STM_HAL_CONF_DIR ${CMAKE_SOURCE_DIR}/src/config"
)
