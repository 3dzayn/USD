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
// matrix3.template.h file to make changes.

#ifndef GF_MATRIX3F_H
#define GF_MATRIX3F_H

/// \file gf/matrix3f.h
/// \ingroup group_gf_LinearAlgebra

#include "pxr/pxr.h"
#include "pxr/base/gf/api.h"
#include "pxr/base/gf/declare.h"
#include "pxr/base/gf/matrixData.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/gf/traits.h"

#include <boost/functional/hash.hpp>

#include <iosfwd>
#include <vector>

PXR_NAMESPACE_OPEN_SCOPE

template <>
struct GfIsGfMatrix<class GfMatrix3f> { static const bool value = true; };

class GfMatrix3d;
class GfMatrix3f;
class GfRotation;
class GfQuaternion;
class GfQuatf;

/// \class GfMatrix3f
/// \ingroup group_gf_LinearAlgebra
///
/// Stores a 3x3 matrix of \c float elements. A basic type.
///
/// Matrices are defined to be in row-major order, so <c>matrix[i][j]</c>
/// indexes the element in the \e i th row and the \e j th column.
///
/// <h3>3D Transformations</h3>
///
/// Three methods, SetRotate(), SetScale(), and ExtractRotation(), interpret
/// a GfMatrix3f as a 3D transformation. By convention, vectors are treated
/// primarily as row vectors, implying the following:
///
/// \li Transformation matrices are organized to deal with row
///        vectors, not column vectors.
/// \li Each of the Set() methods in this class completely rewrites the
///        matrix; for example, SetRotate() yields a matrix
///        which does nothing but rotate.
/// \li When multiplying two transformation matrices, the matrix
///        on the left applies a more local transformation to a row
///        vector. For example, if R represents a rotation
///        matrix and S represents a scale matrix, the
///        product R*S  will rotate a row vector, then scale
///        it.
class GfMatrix3f
{
public:
    typedef float ScalarType;

    static const size_t numRows = 3;
    static const size_t numColumns = 3;

    /// Default constructor. Leaves the matrix component values undefined.
    GfMatrix3f() {}

    /// Constructor. Initializes the matrix from 9 independent
    /// \c float values, specified in row-major order. For example,
    /// parameter \e m10 specifies the value in row 1 and column 0.
    GfMatrix3f(float m00, float m01, float m02, 
               float m10, float m11, float m12, 
               float m20, float m21, float m22) {
        Set(m00, m01, m02, 
            m10, m11, m12, 
            m20, m21, m22);
    }

    /// Constructor. Initializes the matrix from a 3x3 array
    /// of \c float values, specified in row-major order.
    GfMatrix3f(const float m[3][3]) {
        Set(m);
    }

    /// Constructor. Explicitly initializes the matrix to \e s times the
    /// identity matrix.
    explicit GfMatrix3f(float s) {
        SetDiagonal(s);
    }

    /// This explicit constructor initializes the matrix to \p s times
    /// the identity matrix.
    explicit GfMatrix3f(int s) {
        SetDiagonal(s);
    }

    /// Constructor. Explicitly initializes the matrix to diagonal form,
    /// with the \e i th element on the diagonal set to <c>v[i]</c>.
    explicit GfMatrix3f(const GfVec3f& v) {
        SetDiagonal(v);
    }

    /// Constructor.  Initialize the matrix from a vector of vectors of
    /// double. The vector is expected to be 3x3. If it is
    /// too big, only the first 3 rows and/or columns will be used.
    /// If it is too small, uninitialized elements will be filled in with
    /// the corresponding elements from an identity matrix.
    ///
    GF_API
    explicit GfMatrix3f(const std::vector< std::vector<double> >& v);

    /// Constructor.  Initialize the matrix from a vector of vectors of
    /// float. The vector is expected to be 3x3. If it is
    /// too big, only the first 3 rows and/or columns will be used.
    /// If it is too small, uninitialized elements will be filled in with
    /// the corresponding elements from an identity matrix.
    ///
    GF_API
    explicit GfMatrix3f(const std::vector< std::vector<float> >& v);

    /// Constructor. Initialize matrix from rotation.
    GF_API
    GfMatrix3f(const GfRotation& rot);

    /// Constructor. Initialize matrix from a quaternion.
    GF_API
    explicit GfMatrix3f(const GfQuatf& rot);

    /// This explicit constructor converts a "double" matrix to a "float" matrix.
    GF_API
    explicit GfMatrix3f(const class GfMatrix3d& m);

    /// Sets a row of the matrix from a Vec3.
    void SetRow(int i, const GfVec3f & v) {
        _mtx[i][0] = v[0];
        _mtx[i][1] = v[1];
        _mtx[i][2] = v[2];
    }

    /// Sets a column of the matrix from a Vec3.
    void SetColumn(int i, const GfVec3f & v) {
        _mtx[0][i] = v[0];
        _mtx[1][i] = v[1];
        _mtx[2][i] = v[2];
    }

    /// Gets a row of the matrix as a Vec3.
    GfVec3f GetRow(int i) const {
        return GfVec3f(_mtx[i][0], _mtx[i][1], _mtx[i][2]);
    }

    /// Gets a column of the matrix as a Vec3.
    GfVec3f GetColumn(int i) const {
        return GfVec3f(_mtx[0][i], _mtx[1][i], _mtx[2][i]);
    }

    /// Sets the matrix from 9 independent \c float values,
    /// specified in row-major order. For example, parameter \e m10 specifies
    /// the value in row 1 and column 0.
    GfMatrix3f& Set(float m00, float m01, float m02, 
                    float m10, float m11, float m12, 
                    float m20, float m21, float m22) {
        _mtx[0][0] = m00; _mtx[0][1] = m01; _mtx[0][2] = m02; 
        _mtx[1][0] = m10; _mtx[1][1] = m11; _mtx[1][2] = m12; 
        _mtx[2][0] = m20; _mtx[2][1] = m21; _mtx[2][2] = m22;
        return *this;
    }

    /// Sets the matrix from a 3x3 array of \c float
    /// values, specified in row-major order.
    GfMatrix3f& Set(const float m[3][3]) {
        _mtx[0][0] = m[0][0];
        _mtx[0][1] = m[0][1];
        _mtx[0][2] = m[0][2];
        _mtx[1][0] = m[1][0];
        _mtx[1][1] = m[1][1];
        _mtx[1][2] = m[1][2];
        _mtx[2][0] = m[2][0];
        _mtx[2][1] = m[2][1];
        _mtx[2][2] = m[2][2];
        return *this;
    }

    /// Sets the matrix to the identity matrix.
    GfMatrix3f& SetIdentity() {
        return SetDiagonal(1);
    }

    /// Sets the matrix to zero.
    GfMatrix3f& SetZero() {
        return SetDiagonal(0);
    }

    /// Sets the matrix to \e s times the identity matrix.
    GF_API
    GfMatrix3f& SetDiagonal(float s);

    /// Sets the matrix to have diagonal (<c>v[0], v[1], v[2]</c>).
    GF_API
    GfMatrix3f& SetDiagonal(const GfVec3f&);

    /// Fills a 3x3 array of \c float values with the values in
    /// the matrix, specified in row-major order.
    GF_API
    float* Get(float m[3][3]) const;

    /// Returns vector components as an array of \c float values.
    float* GetArray()  {
        return _mtx.GetData();
    }

    /// Returns vector components as a const array of \c float values.
    const float* GetArray() const {
        return _mtx.GetData();
    }

    /// Accesses an indexed row \e i of the matrix as an array of 3 \c
    /// float values so that standard indexing (such as <c>m[0][1]</c>)
    /// works correctly.
    float* operator [](int i) { return _mtx[i]; }

    /// Accesses an indexed row \e i of the matrix as an array of 3 \c
    /// float values so that standard indexing (such as <c>m[0][1]</c>)
    /// works correctly.
    const float* operator [](int i) const { return _mtx[i]; }

    /// Hash.
    friend inline size_t hash_value(GfMatrix3f const &m) {
        int nElems = 3 * 3;
        size_t h = 0;
        const float *p = m.GetArray();
        while (nElems--)
            boost::hash_combine(h, *p++);
        return h;
    }

    /// Tests for element-wise matrix equality. All elements must match
    /// exactly for matrices to be considered equal.
    GF_API
    bool operator ==(const GfMatrix3d& m) const;

    /// Tests for element-wise matrix equality. All elements must match
    /// exactly for matrices to be considered equal.
    GF_API
    bool operator ==(const GfMatrix3f& m) const;

    /// Tests for element-wise matrix inequality. All elements must match
    /// exactly for matrices to be considered equal.
    bool operator !=(const GfMatrix3d& m) const {
        return !(*this == m);
    }

    /// Tests for element-wise matrix inequality. All elements must match
    /// exactly for matrices to be considered equal.
    bool operator !=(const GfMatrix3f& m) const {
        return !(*this == m);
    }

    /// Returns the transpose of the matrix.
    GF_API
    GfMatrix3f GetTranspose() const;

    /// Returns the inverse of the matrix, or FLT_MAX * SetIdentity() if the
    /// matrix is singular. (FLT_MAX is the largest value a \c float can have,
    /// as defined by the system.) The matrix is considered singular if the
    /// determinant is less than or equal to the optional parameter \e eps. If
    /// \e det is non-null, <c>*det</c> is set to the determinant.
    GF_API
    GfMatrix3f GetInverse(double* det = NULL, double eps = 0) const;

    /// Returns the determinant of the matrix.
    GF_API
    double GetDeterminant() const;

    /// Makes the matrix orthonormal in place. This is an iterative method that
    /// is much more stable than the previous cross/cross method.  If the
    /// iterative method does not converge, a warning is issued.
    ///
    /// Returns true if the iteration converged, false otherwise.  Leaves any
    /// translation part of the matrix unchanged.  If \a issueWarning is true,
    /// this method will issue a warning if the iteration does not converge,
    /// otherwise it will be silent.
    GF_API
    bool Orthonormalize(bool issueWarning=true);

    /// Returns an orthonormalized copy of the matrix.
    GF_API
    GfMatrix3f GetOrthonormalized(bool issueWarning=true) const;

    /// Returns the sign of the determinant of the matrix, i.e. 1 for a
    /// right-handed matrix, -1 for a left-handed matrix, and 0 for a
    /// singular matrix.
    GF_API
    double GetHandedness() const;

    /// Returns true if the vectors in the matrix form a right-handed
    /// coordinate system.
    bool IsRightHanded() const {
        return GetHandedness() == 1.0;
    }

    /// Returns true if the vectors in matrix form a left-handed
    /// coordinate system.
    bool IsLeftHanded() const {
        return GetHandedness() == -1.0;
    }

    /// Post-multiplies matrix \e m into this matrix.
    GF_API
    GfMatrix3f& operator *=(const GfMatrix3f& m);

    /// Multiplies the matrix by a float.
    GF_API
    GfMatrix3f& operator *=(double);

    /// Returns the product of a matrix and a float.
    friend GfMatrix3f operator *(const GfMatrix3f& m1, double d)
    {
        GfMatrix3f m = m1;
        return m *= d;
    }

    ///
    // Returns the product of a matrix and a float.
    friend GfMatrix3f operator *(double d, const GfMatrix3f& m)
    {
        return m * d;
    }

    /// Adds matrix \e m to this matrix.
    GF_API
    GfMatrix3f& operator +=(const GfMatrix3f& m);

    /// Subtracts matrix \e m from this matrix.
    GF_API
    GfMatrix3f& operator -=(const GfMatrix3f& m);

    /// Returns the unary negation of matrix \e m.
    GF_API
    friend GfMatrix3f operator -(const GfMatrix3f& m);

    /// Adds matrix \e m2 to \e m1
    friend GfMatrix3f operator +(const GfMatrix3f& m1, const GfMatrix3f& m2)
    {
        GfMatrix3f tmp(m1);
        tmp += m2;
        return tmp;
    }

    /// Subtracts matrix \e m2 from \e m1.
    friend GfMatrix3f operator -(const GfMatrix3f& m1, const GfMatrix3f& m2)
    {
        GfMatrix3f tmp(m1);
        tmp -= m2;
        return tmp;
    }

    /// Multiplies matrix \e m1 by \e m2.
    friend GfMatrix3f operator *(const GfMatrix3f& m1, const GfMatrix3f& m2)
    {
        GfMatrix3f tmp(m1);
        tmp *= m2;
        return tmp;
    }

    /// Divides matrix \e m1 by \e m2 (that is, <c>m1 * inv(m2)</c>).
    friend GfMatrix3f operator /(const GfMatrix3f& m1, const GfMatrix3f& m2)
    {
        return(m1 * m2.GetInverse());
    }

    /// Returns the product of a matrix \e m and a column vector \e vec.
    friend inline GfVec3f operator *(const GfMatrix3f& m, const GfVec3f& vec) {
        return GfVec3f(vec[0] * m._mtx[0][0] + vec[1] * m._mtx[0][1] + vec[2] * m._mtx[0][2],
                       vec[0] * m._mtx[1][0] + vec[1] * m._mtx[1][1] + vec[2] * m._mtx[1][2],
                       vec[0] * m._mtx[2][0] + vec[1] * m._mtx[2][1] + vec[2] * m._mtx[2][2]);
    }

    /// Returns the product of row vector \e vec and a matrix \e m.
    friend inline GfVec3f operator *(const GfVec3f &vec, const GfMatrix3f& m) {
        return GfVec3f(vec[0] * m._mtx[0][0] + vec[1] * m._mtx[1][0] + vec[2] * m._mtx[2][0],
                       vec[0] * m._mtx[0][1] + vec[1] * m._mtx[1][1] + vec[2] * m._mtx[2][1],
                       vec[0] * m._mtx[0][2] + vec[1] * m._mtx[1][2] + vec[2] * m._mtx[2][2]);
    }

    /// Sets matrix to specify a uniform scaling by \e scaleFactor.
    GF_API
    GfMatrix3f& SetScale(float scaleFactor);

    /// \name 3D Transformation Utilities
    /// @{

    /// Sets the matrix to specify a rotation equivalent to \e rot.
    GF_API
    GfMatrix3f& SetRotate(const GfQuatf &rot);

    /// Sets the matrix to specify a rotation equivalent to \e rot.
    GF_API
    GfMatrix3f& SetRotate(const GfRotation &rot);

    /// Sets the matrix to specify a nonuniform scaling in x, y, and z by
    /// the factors in vector \e scaleFactors.
    GF_API
    GfMatrix3f& SetScale(const GfVec3f &scaleFactors);

    /// Returns the rotation corresponding to this matrix. This works
    /// well only if the matrix represents a rotation.
    ///
    /// For good results, consider calling Orthonormalize() before calling
    /// this method.
    GF_API
    GfRotation ExtractRotation() const;

    /// Decompose the rotation corresponding to this matrix about 3
    /// orthogonal axes.  If the axes are not orthogonal, warnings
    /// will be spewed.
    ///
    /// This is a convenience method that is equivalent to calling
    /// ExtractRotation().Decompose().
    GF_API
    GfVec3f DecomposeRotation(const GfVec3f &axis0,
                              const GfVec3f &axis1,
                              const GfVec3f &axis2 ) const;

    /// Returns the quaternion corresponding to this matrix. This works
    /// well only if the matrix represents a rotation.
    ///
    /// For good results, consider calling Orthonormalize() before calling
    /// this method.
    GF_API
    GfQuaternion ExtractRotationQuaternion() const;

    /// @}

private:
    /// Set the matrix to the rotation given by a quaternion,
    /// defined by the real component \p r and imaginary components \p i.
    void _SetRotateFromQuat(float r, const GfVec3f& i);


private:
    /// Matrix storage, in row-major order.
    GfMatrixData<float, 3, 3> _mtx;

    // Friend declarations
    friend class GfMatrix3d;
};


/// Tests for equality within a given tolerance, returning \c true if the
/// difference between each component of the matrix is less than or equal
/// to \p tolerance, or false otherwise.
GF_API 
bool GfIsClose(GfMatrix3f const &m1, GfMatrix3f const &m2, double tolerance);

/// Output a GfMatrix3f
/// \ingroup group_gf_DebuggingOutput
GF_API std::ostream& operator<<(std::ostream &, GfMatrix3f const &);

PXR_NAMESPACE_CLOSE_SCOPE

#endif // GF_MATRIX3F_H
