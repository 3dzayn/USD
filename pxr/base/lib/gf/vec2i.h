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
////////////////////////////////////////////////////////////////////////
// This file is generated by a script.  Do not edit directly.  Edit the
// vec.template.h file to make changes.

#ifndef GF_VEC2I_H
#define GF_VEC2I_H

/// \file gf/vec2i.h
/// \ingroup group_gf_LinearAlgebra

#include "pxr/pxr.h"
#include "pxr/base/tf/diagnostic.h"
#include "pxr/base/gf/api.h"
#include "pxr/base/gf/limits.h"
#include "pxr/base/gf/traits.h"

#include <boost/functional/hash.hpp>

#include <cstddef>

#include <iosfwd>

PXR_NAMESPACE_OPEN_SCOPE

class GfVec2i;

template <>
struct GfIsGfVec<class GfVec2i> { static const bool value = true; };

/// \class GfVec2i
/// \ingroup group_gf_LinearAlgebra
///
/// Basic type for a vector of 2 int components.
///
/// Represents a vector of 2 components of type \c int.
/// It is intended to be fast and simple.
///
class GfVec2i
{
public:
    /// Scalar element type and dimension.
    typedef int ScalarType;
    static const size_t dimension = 2;

    /// Default constructor does no initialization.
    GfVec2i() = default;

    /// Initialize all elements to a single value.
    explicit GfVec2i(int value) {
        _data[0] = value;
        _data[1] = value;
    }

    /// Initialize all elements with explicit arguments.
    GfVec2i(int s0, int s1) {
        Set(s0, s1);
    }

    /// Construct with pointer to values.
    template <class Scl>
    explicit GfVec2i(Scl const *p) { Set(p); }
 
    /// Create a unit vector along the X-axis.
    static GfVec2i XAxis() {
        GfVec2i result(0);
        result[0] = 1;
        return result;
    }
    /// Create a unit vector along the Y-axis.
    static GfVec2i YAxis() {
        GfVec2i result(0);
        result[1] = 1;
        return result;
    }

    /// Create a unit vector along the i-th axis, zero-based.  Return the zero
    /// vector if \p i is greater than or equal to 2.
    static GfVec2i Axis(size_t i) {
        GfVec2i result(0);
        if (i < 2)
            result[i] = 1;
        return result;
    }

    /// Set all elements with passed arguments.
    GfVec2i &Set(int s0, int s1) {
        _data[0] = s0;
        _data[1] = s1;
        return *this;
    }

    /// Set all elements with a pointer to data.
    GfVec2i &Set(int const *a) {
        return Set(a[0], a[1]);
    }

    /// Direct data access.
    int const *data() const { return _data; }
    int *data() { return _data; }
    int const *GetArray() const { return data(); }

    /// Indexing.
    int const &operator[](size_t i) const { return _data[i]; }
    int &operator[](size_t i) { return _data[i]; }

    /// Hash.
    friend inline size_t hash_value(GfVec2i const &vec) {
        size_t h = 0;
        boost::hash_combine(h, vec[0]);
        boost::hash_combine(h, vec[1]);
        return h;
    }

    /// Equality comparison.
    bool operator==(GfVec2i const &other) const {
        return _data[0] == other[0] &&
               _data[1] == other[1];
    }
    bool operator!=(GfVec2i const &other) const {
        return !(*this == other);
    }

    // TODO Add inequality for other vec types...
    /// Equality comparison.
    GF_API
    bool operator==(class GfVec2d const &other) const;
    /// Equality comparison.
    GF_API
    bool operator==(class GfVec2f const &other) const;
    /// Equality comparison.
    GF_API
    bool operator==(class GfVec2h const &other) const;
    
    /// Create a vec with negated elements.
    GfVec2i operator-() const {
        return GfVec2i(-_data[0], -_data[1]);
    }

    /// Addition.
    GfVec2i &operator+=(GfVec2i const &other) {
        _data[0] += other[0];
        _data[1] += other[1];
        return *this;
    }
    friend GfVec2i operator+(GfVec2i const &l, GfVec2i const &r) {
        return GfVec2i(l) += r;
    }

    /// Subtraction.
    GfVec2i &operator-=(GfVec2i const &other) {
        _data[0] -= other[0];
        _data[1] -= other[1];
        return *this;
    }
    friend GfVec2i operator-(GfVec2i const &l, GfVec2i const &r) {
        return GfVec2i(l) -= r;
    }

    /// Multiplication by scalar.
    GfVec2i &operator*=(double s) {
        _data[0] *= s;
        _data[1] *= s;
        return *this;
    }
    GfVec2i operator*(double s) const {
        return GfVec2i(*this) *= s;
    }
    friend GfVec2i operator*(double s, GfVec2i const &v) {
        return v * s;
    }

        /// Division by scalar.
    GfVec2i &operator/=(int s) {
        _data[0] /= s;
        _data[1] /= s;
        return *this;
    }
    GfVec2i operator/(int s) const {
        return GfVec2i(*this) /= s;
    }
    
    /// See GfDot().
    int operator*(GfVec2i const &v) const {
        return _data[0] * v[0] + _data[1] * v[1];
    }

    /// Returns the projection of \p this onto \p v. That is:
    /// \code
    /// v * (*this * v)
    /// \endcode
    GfVec2i GetProjection(GfVec2i const &v) const {
        return v * (*this * v);
    }

    /// Returns the orthogonal complement of \p this->GetProjection(b).
    /// That is:
    /// \code
    ///  *this - this->GetProjection(b)
    /// \endcode
    GfVec2i GetComplement(GfVec2i const &b) const {
        return *this - this->GetProjection(b);
    }

    /// Squared length.
    int GetLengthSq() const {
        return *this * *this;
    }

 
private:
    int _data[2];
};

/// Output a GfVec2i.
/// \ingroup group_gf_DebuggingOutput
GF_API std::ostream& operator<<(std::ostream &, GfVec2i const &);


/// Returns component-wise multiplication of vectors \p v1 and \p v2.
inline GfVec2i
GfCompMult(GfVec2i const &v1, GfVec2i const &v2) {
    return GfVec2i(
        v1[0] * v2[0],
        v1[1] * v2[1]
        );
}

/// Returns component-wise quotient of vectors \p v1 and \p v2.
inline GfVec2i
GfCompDiv(GfVec2i const &v1, GfVec2i const &v2) {
    return GfVec2i(
        v1[0] / v2[0],
        v1[1] / v2[1]
        );
}

/// Returns the dot (inner) product of two vectors.
inline int
GfDot(GfVec2i const &v1, GfVec2i const &v2) {
    return v1 * v2;
}

 
PXR_NAMESPACE_CLOSE_SCOPE

#endif // GF_VEC2I_H
