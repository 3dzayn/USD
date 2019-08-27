//
// Copyright 2019 Pixar
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
#ifndef USDIMAGINGGL_DOME_LIGHT_ADAPTER_H
#define USDIMAGINGGL_DOME_LIGHT_ADAPTER_H

/// \file usdImagingGL/domeLightAdapter.h

#include "pxr/pxr.h"
#include "pxr/usdImaging/usdImagingGL/api.h"
#include "pxr/usdImaging/usdImaging/domeLightAdapter.h"

PXR_NAMESPACE_OPEN_SCOPE


class UsdPrim;

/// \class UsdImagingGLDomeLightAdapter
///
/// Adapter class for lights of type DomeLight, allows for loading of 
/// textures attached to DomeLights 
///
class UsdImagingGLDomeLightAdapter : public UsdImagingDomeLightAdapter {
public:
    typedef UsdImagingDomeLightAdapter BaseAdapter;

    UsdImagingGLDomeLightAdapter()
        : UsdImagingDomeLightAdapter()
    {}

    USDIMAGINGGL_API
    virtual ~UsdImagingGLDomeLightAdapter();
    
    // ---------------------------------------------------------------------- //
    /// \name Texture resources
    // ---------------------------------------------------------------------- //

    USDIMAGINGGL_API
    virtual HdTextureResourceSharedPtr
    GetTextureResource(UsdPrim const& usdPrim, 
                       SdfPath const &id, 
                       UsdTimeCode time) const override;
};


PXR_NAMESPACE_CLOSE_SCOPE

#endif // USDIMAGINGGL_DOME_LIGHT_ADAPTER_H
