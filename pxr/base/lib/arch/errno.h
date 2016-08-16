//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef ARCH_ERRNO_H
#define ARCH_ERRNO_H

/*!
 * \file errno.h
 * \brief Functions for dealing with system errors.
 * \ingroup group_arch_SystemFunctions
 */

#include "pxr/base/arch/api.h"
#include <string>

/*!
 * \brief Return the error string for the current value of errno.
 * \ingroup group_arch_SystemFunctions
 *
 * This function provides a thread-safe method of fetching the error string
 * from errno. POSIX.1c defines errno as a macro which provides access to a
 * thread-local integer. This function uses strerror_r, which is thread-safe.
 */
ARCH_API std::string ArchStrerror();

/*!
 * \brief Return the error string for the specified value of errno.
 * \ingroup group_arch_SystemFunctions
 *
 * This function uses strerror_r, which is thread-safe.
 */
ARCH_API std::string ArchStrerror(int errorCode);

/*!
* \brief Return the error string for the current system error.
* \ingroup group_arch_SystemFunctions
*
* This function uses strerror_r, which is thread-safe.
*/
ARCH_API std::string ArchStrSysError(unsigned long errorCode);

#endif // ARCH_ERRNO_H
