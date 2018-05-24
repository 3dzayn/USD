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
#include "pxr/usd/usdGeom/imagePlane.h"
#include "pxr/usd/usd/schemaRegistry.h"
#include "pxr/usd/usd/typed.h"

#include "pxr/usd/sdf/types.h"
#include "pxr/usd/sdf/assetPath.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(UsdGeomImagePlaneFitTokens,
        USDGEOM_IMAGEPLANE_FIT_TOKENS);

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<UsdGeomImagePlane,
        TfType::Bases< UsdGeomGprim > >();
    
    // Register the usd prim typename as an alias under UsdSchemaBase. This
    // enables one to call
    // TfType::Find<UsdSchemaBase>().FindDerivedByName("ImagePlane")
    // to find TfType<UsdGeomImagePlane>, which is how IsA queries are
    // answered.
    TfType::AddAlias<UsdSchemaBase, UsdGeomImagePlane>("ImagePlane");
}

/* virtual */
UsdGeomImagePlane::~UsdGeomImagePlane()
{
}

/* static */
UsdGeomImagePlane
UsdGeomImagePlane::Get(const UsdStagePtr &stage, const SdfPath &path)
{
    if (!stage) {
        TF_CODING_ERROR("Invalid stage");
        return UsdGeomImagePlane();
    }
    return UsdGeomImagePlane(stage->GetPrimAtPath(path));
}

/* static */
UsdGeomImagePlane
UsdGeomImagePlane::Define(
    const UsdStagePtr &stage, const SdfPath &path)
{
    static TfToken usdPrimTypeName("ImagePlane");
    if (!stage) {
        TF_CODING_ERROR("Invalid stage");
        return UsdGeomImagePlane();
    }
    return UsdGeomImagePlane(
        stage->DefinePrim(path, usdPrimTypeName));
}

/* static */
const TfType &
UsdGeomImagePlane::_GetStaticTfType()
{
    static TfType tfType = TfType::Find<UsdGeomImagePlane>();
    return tfType;
}

/* static */
bool 
UsdGeomImagePlane::_IsTypedSchema()
{
    static bool isTyped = _GetStaticTfType().IsA<UsdTyped>();
    return isTyped;
}

/* virtual */
const TfType &
UsdGeomImagePlane::_GetTfType() const
{
    return _GetStaticTfType();
}

UsdAttribute
UsdGeomImagePlane::GetFilenameAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->infoFilename);
}

UsdAttribute
UsdGeomImagePlane::CreateFilenameAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->infoFilename,
                       SdfValueTypeNames->Asset,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetFrameAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->frame);
}

UsdAttribute
UsdGeomImagePlane::CreateFrameAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->frame,
                       SdfValueTypeNames->Double,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetFitAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->fit);
}

UsdAttribute
UsdGeomImagePlane::CreateFitAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->fit,
                       SdfValueTypeNames->Token,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetOffsetAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->offset);
}

UsdAttribute
UsdGeomImagePlane::CreateOffsetAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->offset,
                       SdfValueTypeNames->Float2,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetSizeAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->size);
}

UsdAttribute
UsdGeomImagePlane::CreateSizeAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->size,
                       SdfValueTypeNames->Float2,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetRotateAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->rotate);
}

UsdAttribute
UsdGeomImagePlane::CreateRotateAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->rotate,
                       SdfValueTypeNames->Float,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetCoverageAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->coverage);
}

UsdAttribute
UsdGeomImagePlane::CreateCoverageAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->coverage,
                       SdfValueTypeNames->Int2,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetCoverageOriginAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->coverageOrigin);
}

UsdAttribute
UsdGeomImagePlane::CreateCoverageOriginAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->coverageOrigin,
                       SdfValueTypeNames->Int2,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetUseFrameExtensionAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->useFrameExtension);
}

UsdAttribute
UsdGeomImagePlane::CreateUseFrameExtensionAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->useFrameExtension,
                       SdfValueTypeNames->Bool,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetFrameOffsetAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->frameOffset);
}

UsdAttribute
UsdGeomImagePlane::CreateFrameOffsetAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->frameOffset,
                       SdfValueTypeNames->Int,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetFrameCacheAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->frameCache);
}

UsdAttribute
UsdGeomImagePlane::CreateFrameCacheAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->frameCache,
                       SdfValueTypeNames->Int,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetWidthAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->width);
}

UsdAttribute
UsdGeomImagePlane::CreateWidthAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->width,
                       SdfValueTypeNames->Float,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetHeightAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->height);
}

UsdAttribute
UsdGeomImagePlane::CreateHeightAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->height,
                       SdfValueTypeNames->Float,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetAlphaGainAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->alphaGain);
}

UsdAttribute
UsdGeomImagePlane::CreateAlphaGainAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->alphaGain,
                       SdfValueTypeNames->Float,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetDepthAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->depth);
}

UsdAttribute
UsdGeomImagePlane::CreateDepthAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->depth,
                       SdfValueTypeNames->Float,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdGeomImagePlane::GetSqueezeCorrectionAttr() const
{
    return GetPrim().GetAttribute(UsdGeomTokens->squeezeCorrection);
}

UsdAttribute
UsdGeomImagePlane::CreateSqueezeCorrectionAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdGeomTokens->squeezeCorrection,
                       SdfValueTypeNames->Float,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdRelationship
UsdGeomImagePlane::GetCameraRel() const
{
    return GetPrim().GetRelationship(UsdGeomTokens->camera);
}

UsdRelationship
UsdGeomImagePlane::CreateCameraRel() const
{
    return GetPrim().CreateRelationship(UsdGeomTokens->camera,
                       /* custom = */ false);
}

namespace {
static inline TfTokenVector
_ConcatenateAttributeNames(const TfTokenVector& left,const TfTokenVector& right)
{
    TfTokenVector result;
    result.reserve(left.size() + right.size());
    result.insert(result.end(), left.begin(), left.end());
    result.insert(result.end(), right.begin(), right.end());
    return result;
}
}

/*static*/
const TfTokenVector&
UsdGeomImagePlane::GetSchemaAttributeNames(bool includeInherited)
{
    static TfTokenVector localNames = {
        UsdGeomTokens->infoFilename,
        UsdGeomTokens->frame,
        UsdGeomTokens->fit,
        UsdGeomTokens->offset,
        UsdGeomTokens->size,
        UsdGeomTokens->rotate,
        UsdGeomTokens->coverage,
        UsdGeomTokens->coverageOrigin,
        UsdGeomTokens->useFrameExtension,
        UsdGeomTokens->frameOffset,
        UsdGeomTokens->frameCache,
        UsdGeomTokens->width,
        UsdGeomTokens->height,
        UsdGeomTokens->alphaGain,
        UsdGeomTokens->depth,
        UsdGeomTokens->squeezeCorrection,
    };
    static TfTokenVector allNames =
        _ConcatenateAttributeNames(
            UsdGeomGprim::GetSchemaAttributeNames(true),
            localNames);

    if (includeInherited)
        return allNames;
    else
        return localNames;
}

PXR_NAMESPACE_CLOSE_SCOPE

// ===================================================================== //
// Feel free to add custom code below this line. It will be preserved by
// the code generator.
//
// Just remember to wrap code in the appropriate delimiters:
// 'PXR_NAMESPACE_OPEN_SCOPE', 'PXR_NAMESPACE_CLOSE_SCOPE'.
// ===================================================================== //
// --(BEGIN CUSTOM CODE)--

#include "pxr/usd/usdGeom/camera.h"

PXR_NAMESPACE_OPEN_SCOPE

namespace {

constexpr float inch_to_mm = 25.4f;

template <typename T>
T getAttr(const UsdAttribute& attr, const UsdTimeCode& usdTime, const T& defaultValue) {
    auto r = defaultValue;
    attr.Get(&r, usdTime);
    return r;
}

}

void
UsdGeomImagePlane::CalculateGeometryForViewport(
    VtVec3fArray* vertices,
    VtVec2fArray* uvs,
    const UsdTimeCode& usdTime) const {
    if (ARCH_UNLIKELY(vertices == nullptr)) { return; }
    float depth = 100.0f;
    GetDepthAttr().Get(&depth, usdTime);

    SdfPathVector cameras;
    GetCameraRel().GetTargets(&cameras);

    if (cameras.size() != 1) { return; }
    UsdGeomCamera usdCamera(this->GetPrim().GetStage()->GetPrimAtPath(cameras[0]));
    if (!usdCamera) { return; }

    GfVec2f aperture {1.0f, 1.0f};
    usdCamera.GetHorizontalApertureAttr().Get(&aperture[0], usdTime);
    usdCamera.GetVerticalApertureAttr().Get(&aperture[1], usdTime);
    const auto focalLength = getAttr(usdCamera.GetFocalLengthAttr(), usdTime, 1.0f);

    // The trick here is to take the image plane size (if not valid the camera aperture),
    // and try to fit the aperture to the image ratio, based on the fit parameter on the image plane.
    // We don't need the viewport aspect ratio / size, because it's already affecting the image by
    // affecting the projection matrix.

    // Size is exported in inches
    // while aperture is in millimeters.
    // TODO: fix this in the maya image plane writer / translator.
    auto size = getAttr(GetSizeAttr(), usdTime, GfVec2f(-1.0f, -1.0f)) * inch_to_mm;
    if (size[0] <= 0.0f) {
        size[0] = aperture[0];
    }
    if (size[1] <= 0.0f) {
        size[1] = aperture[1];
    }

    // TODO: get the image's size
    GfVec2f imageSize {400.0f, 400.0f};
    const auto imageRatio = imageSize[0] / imageSize[1];
    const auto sizeRatio = size[0] / size[1];

    const auto fit = getAttr(GetFitAttr(), usdTime, UsdGeomImagePlaneFitTokens->best);
    if (fit == UsdGeomImagePlaneFitTokens->fill) {

    } else if (fit == UsdGeomImagePlaneFitTokens->best) {
        if (imageRatio > sizeRatio) {
            size[1] = size[0] / imageRatio;
        } else {
            size[0] = size[1] * imageRatio;
        }
    } else if (fit == UsdGeomImagePlaneFitTokens->horizontal) {
    } else if (fit == UsdGeomImagePlaneFitTokens->vertical) {
    } else if (fit == UsdGeomImagePlaneFitTokens->toSize) {
    } else { assert("Invalid value passed to UsdGeomImagePlane.fit!"); }

    // Both aperture and focal length should be in millimeters.
    auto calculateFOV = [] (const float fl, const float ap) -> float {
        return atanf(ap / (2.0f * fl));
    };

    const auto hFov = calculateFOV(focalLength, size[0]);
    const auto vFov = calculateFOV(focalLength, size[1]);
    const auto hEnd = static_cast<float>(sin(hFov)) * depth;
    const auto vEnd = static_cast<float>(sin(vFov)) * depth;
    vertices->resize(4);
    vertices->operator[](0) = GfVec3f(-hEnd ,  vEnd , -depth);
    vertices->operator[](1) = GfVec3f( hEnd ,  vEnd , -depth);
    vertices->operator[](2) = GfVec3f( hEnd , -vEnd , -depth);
    vertices->operator[](3) = GfVec3f(-hEnd , -vEnd , -depth);

    if (ARCH_UNLIKELY(uvs == nullptr)) { return; }
    uvs->resize(4);
    uvs->operator[](0) = GfVec2f(0.0f, 0.0f);
    uvs->operator[](1) = GfVec2f(1.0f, 0.0f);
    uvs->operator[](2) = GfVec2f(1.0f, 1.0f);
    uvs->operator[](3) = GfVec2f(0.0f, 1.0f);
}

PXR_NAMESPACE_CLOSE_SCOPE
