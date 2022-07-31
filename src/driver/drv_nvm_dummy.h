/******************************************************************************
   Copyright 2020 Embedded Office GmbH & Co. KG

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
******************************************************************************/

/* TODO: rename the include protection to match the naming convention: 
 *   DRV_NVM_<device>_H_
 */
#ifndef DRV_NVM_DUMMY_H_
#define DRV_NVM_DUMMY_H_

#ifdef __cplusplus               /* for compatibility with C++ environments  */
extern "C" {
#endif

/******************************************************************************
* INCLUDES
******************************************************************************/

#include "co_if.h"

/******************************************************************************
* PUBLIC SYMBOLS
******************************************************************************/

/* TODO: rename the extern variable declaration to match the naming convention:
 *   <device-name>NvmDriver
 */
extern const CO_IF_NVM_DRV DummyNvmDriver;

#ifdef __cplusplus               /* for compatibility with C++ environments  */
}
#endif

#endif
