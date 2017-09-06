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

#include "pxr/pxr.h"
#include "pxr/base/tf/envSetting.h"

#include "pxr/usd/usd/common.h"

PXR_NAMESPACE_OPEN_SCOPE


TF_DEFINE_ENV_SETTING(
    USD_RETIRE_LUMOS, true,
    "Set to true when tools should no longer use lumos/2x.");

TF_DEFINE_ENV_SETTING(
    USD_SHADING_MODEL, "usdRi",
    "Set to usdRi when models can interchange UsdShade prims.");

TF_DEFINE_ENV_SETTING(
    USD_AUTHOR_APPEND_AS_ADD, true,
    "Set true if USD Append() API's should author Add operations instead of "
    "Append, to mimic their historical behavior.");

bool UsdIsRetireLumosEnabled()
{
    return TfGetEnvSetting(USD_RETIRE_LUMOS);
}

bool UsdAuthorAppendAsAdd()
{
    return TfGetEnvSetting(USD_AUTHOR_APPEND_AS_ADD);
}

PXR_NAMESPACE_CLOSE_SCOPE

