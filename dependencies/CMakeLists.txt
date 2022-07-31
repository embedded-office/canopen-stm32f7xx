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
include(FetchContent)

#---
# declare the dependency to external projects
#
FetchContent_Declare(Canopen
  GIT_REPOSITORY  git@github.com:embedded-office/canopen-stack.git
  GIT_TAG         develop-type-modules
)
FetchContent_Declare(Bsp
  GIT_REPOSITORY  git@github.com:embedded-office/bsp-stm32f769.git
  GIT_TAG         v2.1.3
)

#---
# add external projects to the build
#
FetchContent_MakeAvailable(Canopen)
FetchContent_MakeAvailable(Bsp)
