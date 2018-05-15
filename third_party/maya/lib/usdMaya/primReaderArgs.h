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
#ifndef PXRUSDMAYA_PRIMREADERARGS_H
#define PXRUSDMAYA_PRIMREADERARGS_H

/// \file primReaderArgs.h

#include "pxr/pxr.h"
#include "usdMaya/api.h"
#include "pxr/usd/usd/prim.h"

PXR_NAMESPACE_OPEN_SCOPE


/// \class PxrUsdMayaPrimReaderArgs
/// \brief This class holds read-only arguments that are passed into reader plugins for
/// the usdMaya library.
/// 
/// \sa PxrUsdMayaPrimReaderContext
class PxrUsdMayaPrimReaderArgs
{
public:
    PXRUSDMAYA_API
    PxrUsdMayaPrimReaderArgs(
            const UsdPrim& prim,
            const TfToken& shadingMode,
            const bool readAnimData,
            const bool useCustomFrameRange,
            const double startTime,
            const double endTime,
            const TfToken::Set& includeMetadataKeys,
            const TfToken::Set& includeAPINames);

    /// \brief return the usd prim that should be read.
    PXRUSDMAYA_API
    const UsdPrim& GetUsdPrim() const;

    PXRUSDMAYA_API
    const TfToken& GetShadingMode() const;

    PXRUSDMAYA_API
    const bool& GetReadAnimData() const;

    PXRUSDMAYA_API
    bool HasCustomFrameRange() const;
    PXRUSDMAYA_API
    double GetStartTime() const;
    PXRUSDMAYA_API
    double GetEndTime() const;

    PXRUSDMAYA_API
    const TfToken::Set& GetIncludeMetadataKeys() const;
    PXRUSDMAYA_API
    const TfToken::Set& GetIncludeAPINames() const;

    bool ShouldImportUnboundShaders() const {
        // currently this is disabled.
        return false;
    }

private:
    const UsdPrim& _prim;
    const TfToken& _shadingMode;
    const bool _readAnimData;
    const bool _useCustomFrameRange;
    const double _startTime;
    const double _endTime;
    const TfToken::Set& _includeMetadataKeys;
    const TfToken::Set& _includeAPINames;
};


PXR_NAMESPACE_CLOSE_SCOPE

#endif // PXRUSDMAYA_PRIMREADERARGS_H
