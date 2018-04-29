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
// This file is generated by a script.  Do not edit directly.  Edit the
// wrapMatrix4.template.cpp file to make changes.

#ifndef BOOST_PYTHON_MAX_ARITY
#define BOOST_PYTHON_MAX_ARITY 20
#endif


#include "pxr/pxr.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/matrix4f.h"

#include "pxr/base/gf/pyBufferUtils.h"

#include "pxr/base/gf/matrix3f.h"
#include "pxr/base/gf/quatf.h"
#include "pxr/base/gf/rotation.h"

#include "pxr/base/tf/pyUtils.h"
#include "pxr/base/tf/pyContainerConversions.h"
#include "pxr/base/tf/wrapTypeHelpers.h"

#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <boost/python/detail/api_placeholder.hpp>
#include <boost/python/errors.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/make_constructor.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/return_arg.hpp>
#include <boost/python/tuple.hpp>

#include <string>
#include <vector>

using namespace boost::python;
using std::string;
using std::vector;

PXR_NAMESPACE_USING_DIRECTIVE

namespace {

////////////////////////////////////////////////////////////////////////
// Python buffer protocol support.

// Python's getreadbuf interface function.
static Py_ssize_t
getreadbuf(PyObject *self, Py_ssize_t segment, void **ptrptr) {
    if (segment != 0) {
        // Always one-segment.
        PyErr_SetString(PyExc_ValueError, "accessed non-existent segment");
        return -1;
    }
    GfMatrix4f &mat = extract<GfMatrix4f &>(self);
    *ptrptr = static_cast<void *>(mat.GetArray());
    // Return size in bytes.
    return sizeof(GfMatrix4f);
}

// Python's getwritebuf interface function.
static Py_ssize_t
getwritebuf(PyObject *self, Py_ssize_t segment, void **ptrptr) {
    PyErr_SetString(PyExc_ValueError, "writable buffers supported only with "
                    "new-style buffer protocol.");
    return -1;
}

// Python's getsegcount interface function.
static Py_ssize_t
getsegcount(PyObject *self, Py_ssize_t *lenp) {
    if (lenp)
        *lenp = sizeof(GfMatrix4f);
    return 1; // Always one contiguous segment.
}

// Python's getcharbuf interface function.
static Py_ssize_t
getcharbuf(PyObject *self, Py_ssize_t segment, const char **ptrptr) {
    return getreadbuf(self, segment, (void **) ptrptr);
}

// Python's getbuffer interface function.
static int
getbuffer(PyObject *self, Py_buffer *view, int flags) {
    if (view == NULL) {
        PyErr_SetString(PyExc_ValueError, "NULL view in getbuffer");
        return -1;
    }

    // We don't support fortran order.
    if ((flags & PyBUF_F_CONTIGUOUS) == PyBUF_F_CONTIGUOUS) {
        PyErr_SetString(PyExc_ValueError, "Fortran contiguity unsupported");
        return -1;
    }

    GfMatrix4f &mat = extract<GfMatrix4f &>(self);

    view->obj = self;
    view->buf = static_cast<void *>(mat.GetArray());
    view->len = sizeof(GfMatrix4f);
    view->readonly = 0;
    view->itemsize = sizeof(float);
    if ((flags & PyBUF_FORMAT) == PyBUF_FORMAT) {
        view->format = Gf_GetPyBufferFmtFor<float>();
    } else {
        view->format = NULL;
    }
    if ((flags & PyBUF_ND) == PyBUF_ND) {
        view->ndim = 2;
        static Py_ssize_t shape[] = { 4, 4 };
        view->shape = shape;
    } else {
        view->ndim = 0;
        view->shape = NULL;
    }
    if ((flags & PyBUF_STRIDES) == PyBUF_STRIDES) {
        static Py_ssize_t strides[] = {
            4 * sizeof(float), sizeof(float) };
        view->strides = strides;
    } else {
        view->strides = NULL;
    }
    view->suboffsets = NULL;
    view->internal = NULL;

    Py_INCREF(self); // need to retain a reference to self.
    return 0;
}

// This structure serves to instantiate a PyBufferProcs instance with pointers
// to the right buffer protocol functions.
static PyBufferProcs bufferProcs = {
    (readbufferproc) getreadbuf,   /*bf_getreadbuffer*/
    (writebufferproc) getwritebuf, /*bf_getwritebuffer*/
    (segcountproc) getsegcount,    /*bf_getsegcount*/
    (charbufferproc) getcharbuf,   /*bf_getcharbuffer*/
    (getbufferproc) getbuffer,
    (releasebufferproc) 0,
};

// End python buffer protocol support.
////////////////////////////////////////////////////////////////////////

static string _Repr(GfMatrix4f const &self) {
    static char newline[] = ",\n            ";
    return TF_PY_REPR_PREFIX + "Matrix4f(" +
        TfPyRepr(self[0][0]) + ", " + TfPyRepr(self[0][1]) + ", " + TfPyRepr(self[0][2]) + ", " + TfPyRepr(self[0][3]) + newline +
        TfPyRepr(self[1][0]) + ", " + TfPyRepr(self[1][1]) + ", " + TfPyRepr(self[1][2]) + ", " + TfPyRepr(self[1][3]) + newline +
        TfPyRepr(self[2][0]) + ", " + TfPyRepr(self[2][1]) + ", " + TfPyRepr(self[2][2]) + ", " + TfPyRepr(self[2][3]) + newline +
        TfPyRepr(self[3][0]) + ", " + TfPyRepr(self[3][1]) + ", " + TfPyRepr(self[3][2]) + ", " + TfPyRepr(self[3][3]) + ")";
}

static GfMatrix4f GetInverseWrapper( const GfMatrix4f &self ) {
    return self.GetInverse();
}

static void
throwIndexErr( const char *msg )
{
    PyErr_SetString(PyExc_IndexError, msg);
    boost::python::throw_error_already_set();
}    

static int
normalizeIndex(int index) {
    return TfPyNormalizeIndex(index, 4, true /*throw error*/);
}

// Return number of rows
static int __len__(GfMatrix4f const &self) {
    return 4;
}

static float __getitem__float(GfMatrix4f const &self, tuple index) {
    int i1=0, i2=0;
    if (len(index) == 2) {
        i1 = normalizeIndex(extract<int>(index[0]));
        i2 = normalizeIndex(extract<int>(index[1]));
    } else
        throwIndexErr("Index has incorrect size.");

    return self[i1][i2];
}

static GfVec4f __getitem__vector(GfMatrix4f const &self, int index) {
    return GfVec4f(self[normalizeIndex(index)]);
}

static void __setitem__float(GfMatrix4f &self, tuple index, float value) {
    int i1=0, i2=0;
    if (len(index) == 2) {
        i1 = normalizeIndex(extract<int>(index[0]));
        i2 = normalizeIndex(extract<int>(index[1]));
    } else
        throwIndexErr("Index has incorrect size.");

    self[i1][i2] = value;
}

static void __setitem__vector( GfMatrix4f &self, int index, GfVec4f value ) {
    int ni = normalizeIndex(index);
    self[ni][0] = value[0];
    self[ni][1] = value[1];
    self[ni][2] = value[2];
    self[ni][3] = value[3];
}

static bool __contains__float( const GfMatrix4f &self, float value ) {
    for( int i = 0; i < 4; ++i )
        for( int j = 0; j < 4; ++j )
            if( self[i][j] == value )
                return true;
    return false;
}

// Check rows against GfVec
static bool __contains__vector( const GfMatrix4f &self, GfVec4f value ) {
    for( int i = 0; i < 4; ++i )
	if( self.GetRow(i) == value )
	    return true;
    return false;
}

static GfMatrix4f *__init__() {
    // Default constructor produces identity from python.
    return new GfMatrix4f(1);
}

static tuple FactorWithEpsilon(GfMatrix4f &self, double eps) {
    GfMatrix4f r, u, p;
    GfVec3f s, t;
    bool result = self.Factor(&r, &s, &u, &t, &p, eps);
    return boost::python::make_tuple(result, r, s, u, t, p);
}    

static tuple Factor(GfMatrix4f &self) {
    GfMatrix4f r, u, p;
    GfVec3f s, t;
    bool result = self.Factor(&r, &s, &u, &t, &p);
    return boost::python::make_tuple(result, r, s, u, t, p);
}

static GfMatrix4f RemoveScaleShearWrapper( const GfMatrix4f &self ) {
    return self.RemoveScaleShear();
}

// This adds support for python's builtin pickling library
// This is used by our Shake plugins which need to pickle entire classes
// (including code), which we don't support in pxml.
struct GfMatrix4f_Pickle_Suite : boost::python::pickle_suite
{
    static boost::python::tuple getinitargs(const GfMatrix4f &m)
    {
        return boost::python::make_tuple(
            m[0][0], m[0][1], m[0][2], m[0][3], 
            m[1][0], m[1][1], m[1][2], m[1][3], 
            m[2][0], m[2][1], m[2][2], m[2][3], 
            m[3][0], m[3][1], m[3][2], m[3][3]);
    }
};

static size_t __hash__(GfMatrix4f const &m) { return hash_value(m); }

} // anonymous namespace 

void wrapMatrix4f()
{    
    typedef GfMatrix4f This;

    static const tuple _dimension = make_tuple(4, 4);

    def("IsClose", (bool (*)(const GfMatrix4f &m1, const GfMatrix4f &m2, double))
        GfIsClose);
    
    class_<This> cls( "Matrix4f", no_init);
    cls
        .def_pickle(GfMatrix4f_Pickle_Suite())
	.def("__init__", make_constructor(__init__))
        .def(init< const GfMatrix4d & >())
        .def(init< const GfMatrix4f & >())
        .def(init< int >())
        .def(init< float >())
        .def(init<
             float, float, float, float, 
             float, float, float, float, 
             float, float, float, float, 
             float, float, float, float 
             >())
        .def(init< const GfVec4f & >())
        .def(init< const vector< vector<float> >& >())
        .def(init< const vector< vector<double> >& >())
        .def(init< const vector<float>&,
                   const vector<float>&,
                   const vector<float>&,
                   const vector<float>& >())
        .def(init< const vector<double>&,
                   const vector<double>&,
                   const vector<double>&,
                   const vector<double>& >())
        .def(init< const GfMatrix3f &, const GfVec3f >())
        .def(init< const GfRotation &, const GfVec3f >())

        .def( TfTypePythonClass() )

        .def_readonly( "dimension", _dimension )
        .def( "__len__", __len__, "Return number of rows" )

        .def( "__getitem__", __getitem__float )
        .def( "__getitem__", __getitem__vector )
        .def( "__setitem__", __setitem__float )
        .def( "__setitem__", __setitem__vector )
        .def( "__contains__", __contains__float )
        .def( "__contains__", __contains__vector, "Check rows against GfVec"  )

        .def("Set", (This &(This::*)(float, float, float, float, 
                                     float, float, float, float, 
                                     float, float, float, float, 
                                     float, float, float, float))&This::Set,
             return_self<>())
        
        .def("SetIdentity", &This::SetIdentity, return_self<>())
        .def("SetZero", &This::SetZero, return_self<>())

        .def("SetDiagonal", 
             (This & (This::*)(float))&This::SetDiagonal, 
             return_self<>())
        .def("SetDiagonal", 
             (This & (This::*)(const GfVec4f &))&This::SetDiagonal, 
             return_self<>())

        .def("SetRow", &This::SetRow)
        .def("SetColumn", &This::SetColumn)
        .def("GetRow", &This::GetRow)
        .def("GetColumn", &This::GetColumn)

        .def("GetTranspose", &This::GetTranspose)
        .def("GetInverse", GetInverseWrapper)

        .def("GetDeterminant", &This::GetDeterminant)
        .def("GetRow3", &This::GetRow3)
        .def("SetRow3", &This::SetRow3)
        .def("GetDeterminant3", &This::GetDeterminant3)
        .def("HasOrthogonalRows3", &This::HasOrthogonalRows3)

        .def("GetHandedness", &This::GetHandedness)
        .def("IsLeftHanded", &This::IsLeftHanded)
        .def("IsRightHanded", &This::IsRightHanded)

        .def("Orthonormalize", &This::Orthonormalize,
             (arg("issueWarning") = true))
        .def("GetOrthonormalized", &This::GetOrthonormalized,
             (arg("issueWarning") = true))
        
        .def( str(self) )
        .def( self == self )
        .def( self == GfMatrix4d() )
        .def( self != self )
        .def( self != GfMatrix4d() )
        .def( self *= self )
        .def( self * self )
        .def( self *= double() )
        .def( self * double() )
        .def( double() * self )
        .def( self += self )
        .def( self + self )
        .def( self -= self )
        .def( self - self )
        .def( -self )
        .def( self / self )
        .def( self * GfVec4f() )
        .def( GfVec4f() * self )

        .def("SetTransform",
	     (This & (This::*)( const GfRotation &,
				const GfVec3f & ))&This::SetTransform,
	     return_self<>())	
        .def("SetTransform",
	     (This & (This::*)( const GfMatrix3f&,
				const GfVec3f & ))&This::SetTransform,
	     return_self<>())

        .def("SetScale", (This & (This::*)( const GfVec3f& ))&This::SetScale,
	     return_self<>())

        .def("SetTranslate", &This::SetTranslate, return_self<>())
        .def("SetTranslateOnly", &This::SetTranslateOnly, return_self<>())

        .def("SetRotate",
	     (This & (This::*)( const GfQuatf & )) &This::SetRotate,
	     return_self<>())
        .def("SetRotateOnly",
	     (This & (This::*)( const GfQuatf & )) &This::SetRotateOnly,
	     return_self<>())

        .def("SetRotate",
	     (This & (This::*)( const GfRotation & )) &This::SetRotate,
	     return_self<>())
        .def("SetRotateOnly",
	     (This & (This::*)( const GfRotation & )) &This::SetRotateOnly,
	     return_self<>())

        .def("SetRotate",
	     (This & (This::*)( const GfMatrix3f& )) &This::SetRotate,
	     return_self<>())
        .def("SetRotateOnly",
	     (This & (This::*)( const GfMatrix3f& )) &This::SetRotateOnly,
	     return_self<>())

        .def("SetLookAt", (This & (This::*)( const GfVec3f &,
                                             const GfVec3f &,
                                             const GfVec3f & ))&This::SetLookAt,
	     return_self<>())

        .def("SetLookAt",
             (This & (This::*)( const GfVec3f &,
                                const GfRotation & ))&This::SetLookAt,
             return_self<>())

        .def("ExtractTranslation", &This::ExtractTranslation)
        .def("ExtractRotation", &This::ExtractRotation)
        .def("ExtractRotationMatrix", &This::ExtractRotationMatrix)

        .def("Factor", FactorWithEpsilon)
        .def("Factor", Factor)
        .def("RemoveScaleShear", RemoveScaleShearWrapper)
        
        .def("Transform",
	     (GfVec3f (This::*)(const GfVec3f &) const)&This::Transform)
        .def("Transform",
	     (GfVec3d (This::*)(const GfVec3d &) const)&This::Transform)

        .def("TransformDir",
	     (GfVec3f (This::*)(const GfVec3f &) const)&This::TransformDir)
        .def("TransformDir",
	     (GfVec3d (This::*)(const GfVec3d &) const)&This::TransformDir)

        .def("TransformAffine",
	     (GfVec3f (This::*)(const GfVec3f &) const)&This::TransformAffine)
        .def("TransformAffine",
	     (GfVec3d (This::*)(const GfVec3d &) const)&This::TransformAffine)
        .def("SetScale", (This & (This::*)( float ))&This::SetScale,
	     return_self<>())

        .def("__repr__", _Repr)
        .def("__hash__", __hash__)

        ;
    to_python_converter<std::vector<This>,
        TfPySequenceToPython<std::vector<This> > >();
    
    // Install buffer protocol: set the tp_as_buffer slot to point to a
    // structure of function pointers that implement the buffer protocol for
    // this type, and set the type flags to indicate that this type supports the
    // buffer protocol.
    auto *typeObj = reinterpret_cast<PyTypeObject *>(cls.ptr());
    typeObj->tp_as_buffer = &bufferProcs;
    typeObj->tp_flags |= (Py_TPFLAGS_HAVE_NEWBUFFER |
                          Py_TPFLAGS_HAVE_GETCHARBUFFER);

}
