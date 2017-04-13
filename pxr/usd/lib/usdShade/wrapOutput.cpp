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
#include "pxr/usd/usdShade/output.h"
#include "pxr/usd/usdShade/parameter.h"
#include "pxr/usd/usdShade/connectableAPI.h"

#include "pxr/usd/usd/conversions.h"

#include "pxr/base/tf/pyContainerConversions.h"
#include "pxr/base/tf/pyResultConversions.h"

#include <boost/python/class.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/tuple.hpp>

#include <vector>

using std::vector;
using namespace boost::python;

PXR_NAMESPACE_USING_DIRECTIVE

namespace {

static bool
_Set(const UsdShadeOutput &self, object val, const UsdTimeCode &time) 
{
    return self.Set(UsdPythonToSdfType(val, self.GetTypeName()), time);
}

} // anonymous namespace 

void wrapUsdShadeOutput()
{
    typedef UsdShadeOutput Output;

    class_<Output>("Output")
        .def(init<UsdAttribute>(arg("attr")))
        .def(!self)

        .def("GetFullName", &Output::GetFullName,
                return_value_policy<return_by_value>())
        .def("GetBaseName", &Output::GetBaseName)
        .def("GetPrim", &Output::GetPrim)
        .def("GetTypeName", &Output::GetTypeName)
        .def("Set", _Set, (arg("value"), arg("time")=UsdTimeCode::Default()))
        .def("SetRenderType", &Output::SetRenderType,
             (arg("renderType")))
        .def("GetRenderType", &Output::GetRenderType)
        .def("HasRenderType", &Output::HasRenderType)

        .def("GetAttr", &Output::GetAttr)
        .def("GetRel", &Output::GetRel)
        .def("GetProperty", &Output::GetProperty,
                return_value_policy<return_by_value>())
        ;

    implicitly_convertible<Output, UsdAttribute>();
    implicitly_convertible<Output, UsdProperty>();

    to_python_converter<
        std::vector<Output>,
        TfPySequenceToPython<std::vector<Output> > >();
}


