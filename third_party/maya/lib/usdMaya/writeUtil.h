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

/// \file writeUtil.h

#ifndef PXRUSDMAYA_WRITEUTIL_H
#define PXRUSDMAYA_WRITEUTIL_H

#include "pxr/pxr.h"
#include "usdMaya/api.h"
#include "usdMaya/UserTaggedAttribute.h"

#include "pxr/base/tf/token.h"
#include "pxr/base/vt/types.h"
#include "pxr/usd/sdf/valueTypeName.h"
#include "pxr/usd/usd/attribute.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/timeCode.h"
#include "pxr/usd/usdGeom/imageable.h"
#include "pxr/usd/usdGeom/pointInstancer.h"
#include "pxr/usd/usdGeom/primvar.h"

#include <maya/MDagPath.h>
#include <maya/MFnArrayAttrsData.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#include <maya/MString.h>

#include <string>

PXR_NAMESPACE_OPEN_SCOPE



struct PxrUsdMayaWriteUtil
{
    /// \name Helpers for writing USD
    /// \{

    /// Get the SdfValueTypeName that corresponds to the given plug \p attrPlug.
    /// If \p translateMayaDoubleToUsdSinglePrecision is true, Maya plugs that
    /// contain double data will return the appropriate float-based type.
    /// Otherwise, the type returned will be the appropriate double-based type.
    PXRUSDMAYA_API
    static SdfValueTypeName GetUsdTypeName(
            const MPlug& attrPlug,
            const bool translateMayaDoubleToUsdSinglePrecision =
                PxrUsdMayaUserTaggedAttribute::GetFallbackTranslateMayaDoubleToUsdSinglePrecision());

    /// Given an \p attrPlug, try to create a USD attribute on \p usdPrim with
    /// the name \p attrName. Note, it's value will not be set.
    ///
    /// Attributes that are not part of the primSchema should have \p custom
    /// set to true.
    ///
    /// If \p translateMayaDoubleToUsdSinglePrecision is true, Maya plugs that
    /// contain double data will result in USD attributes of the appropriate
    /// float-based type. Otherwise, their type will be double-based.
    PXRUSDMAYA_API
    static UsdAttribute GetOrCreateUsdAttr(
            const MPlug& attrPlug,
            const UsdPrim& usdPrim,
            const std::string& attrName,
            const bool custom = false,
            const bool translateMayaDoubleToUsdSinglePrecision =
                PxrUsdMayaUserTaggedAttribute::GetFallbackTranslateMayaDoubleToUsdSinglePrecision());

    /// Given an \p attrPlug, try to create a primvar on \p imageable with
    /// the name \p primvarName. Note, it's value will not be set.
    ///
    /// If \p translateMayaDoubleToUsdSinglePrecision is true, Maya plugs that
    /// contain double data will result in primvars of the appropriate
    /// float-based type. Otherwise, their type will be double-based.
    PXRUSDMAYA_API
    static UsdGeomPrimvar GetOrCreatePrimvar(
            const MPlug& attrPlug,
            UsdGeomImageable& imageable,
            const std::string& primvarName,
            const TfToken& interpolation = TfToken(),
            const int elementSize = -1,
            const bool translateMayaDoubleToUsdSinglePrecision =
                PxrUsdMayaUserTaggedAttribute::GetFallbackTranslateMayaDoubleToUsdSinglePrecision());

    /// Given an \p attrPlug, try to create a UsdRi attribute on \p usdPrim with
    /// the name \p attrName. Note, it's value will not be set.
    ///
    /// If \p translateMayaDoubleToUsdSinglePrecision is true, Maya plugs that
    /// contain double data will result in UsdRi attributes of the appropriate
    /// float-based type. Otherwise, their type will be double-based.
    PXRUSDMAYA_API
    static UsdAttribute GetOrCreateUsdRiAttribute(
            const MPlug& attrPlug,
            const UsdPrim& usdPrim,
            const std::string& attrName,
            const std::string& nameSpace = "user",
            const bool translateMayaDoubleToUsdSinglePrecision =
                PxrUsdMayaUserTaggedAttribute::GetFallbackTranslateMayaDoubleToUsdSinglePrecision());

    /// Given an \p attrPlug, determine it's value and set it on \p usdAttr at
    /// \p usdTime.
    ///
    /// If \p translateMayaDoubleToUsdSinglePrecision is true, Maya plugs that
    /// contain double data will be set on \p usdAttr as the appropriate
    /// float-based type. Otherwise, their data will be set as the appropriate
    /// double-based type.
    PXRUSDMAYA_API
    static bool SetUsdAttr(
            const MPlug& attrPlug,
            const UsdAttribute& usdAttr,
            const UsdTimeCode& usdTime,
            const bool writeIfConstant,
            const bool translateMayaDoubleToUsdSinglePrecision =
                PxrUsdMayaUserTaggedAttribute::GetFallbackTranslateMayaDoubleToUsdSinglePrecision());

    /// Given a Maya node at \p dagPath, inspect it for attributes tagged by
    /// the user for export to USD and write them onto \p usdPrim at time
    /// \p usdTime.
    PXRUSDMAYA_API
    static bool WriteUserExportedAttributes(
            const MDagPath& dagPath,
            const UsdPrim& usdPrim,
            const UsdTimeCode& usdTime,
            const bool writeIfConstant);

    /// Authors class inherits on \p usdPrim.  \p inheritClassNames are
    /// specified as names (not paths).  For example, they should be
    /// ["_class_Special", ...].
    PXRUSDMAYA_API
    static bool WriteClassInherits(
            const UsdPrim& usdPrim,
            const std::vector<std::string>& inheritClassNames);

    /// Given \p inputPointsData (native Maya particle data), writes the
    /// arrays as point-instancer attributes on the given \p instancer
    /// schema object.
    /// Returns true if successful.
    PXRUSDMAYA_API
    static bool WriteArrayAttrsToInstancer(
            MFnArrayAttrsData& inputPointsData,
            const UsdGeomPointInstancer& instancer,
            const size_t numPrototypes,
            const UsdTimeCode& usdTime);

    /// \}

    /// \name Helpers for reading Maya data
    /// \{

    /// \brief Reads attribute \p name on \p depNode into \p val.
    PXRUSDMAYA_API
    static bool ReadMayaAttribute(
            const MFnDependencyNode& depNode,
            const MString& name, 
            std::string* val);

    PXRUSDMAYA_API
    static bool ReadMayaAttribute(
            const MFnDependencyNode& depNode,
            const MString& name, 
            std::vector<std::string>* val);

    /// \brief Reads attribute \p name on \p depNode into \p val.
    PXRUSDMAYA_API
    static bool ReadMayaAttribute(
            const MFnDependencyNode& depNode,
            const MString& name, 
            VtIntArray* val);

    /// \brief Reads attribute \p name on \p depNode into \p val.
    PXRUSDMAYA_API
    static bool ReadMayaAttribute(
            const MFnDependencyNode& depNode,
            const MString& name, 
            VtFloatArray* val);

    /// \brief Reads attribute \p name on \p depNode into \p val.
    PXRUSDMAYA_API
    static bool ReadMayaAttribute(
            const MFnDependencyNode& depNode,
            const MString& name, 
            VtVec3fArray* val);
    /// \}

    /// \brief Cleans up duplicate keys on \p attribute based on \p parameterInterpolation.
    /// If \p keepSingleSample is true, will stop short of converting a time sampled attr
    /// to a constant one.
    PXRUSDMAYA_API
    static void CleanupAttributeKeys(
        UsdAttribute attribute,
        bool keepSingleSample = false,
        UsdInterpolationType parameterInterpolation = UsdInterpolationTypeLinear);

    /// \brief Cleans up duplicate keys on \p primvar based on \p parameterInterpolation.
    PXRUSDMAYA_API
    static void CleanupPrimvarKeys(
        UsdGeomPrimvar primvar,
        bool keepSingleSample = false,
        UsdInterpolationType parameterInterpolation = UsdInterpolationTypeLinear);

    /// \brief Appends new VtValue to a USDAttribute. The function does cleanup and
    /// removes extra keys on the fly, so it's useful to decrease memory usage during export.
    PXRUSDMAYA_API
    static void SetAttributeKey(
            UsdAttribute attribute,
            const VtValue& value,
            const UsdTimeCode& usdTime);

    /// \brief Appends new VtValue and indices to a USDGeomPrimvar. The function does cleanup and
    ///  removes extra keys on the fly, so it's useful to decrease memory usage during export.
    PXRUSDMAYA_API
    static void SetPrimvarKey(
            UsdGeomPrimvar primvar,
            const VtValue& value,
            const VtValue& indices,
            const UsdTimeCode& usdTime);
};


PXR_NAMESPACE_CLOSE_SCOPE

#endif // PXRUSDMAYA_WRITEUTIL_H
