/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Math/Matrix4.h"

namespace DAVA 
{
const Matrix4 Matrix4::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f, 
                          0.0f, 1.0f, 0.0f, 0.0f,
                          0.0f, 0.0f, 1.0f, 0.0f,
                          0.0f, 0.0f, 0.0f, 1.0f);

//inline void Matrix4::glOrthof
void Matrix4::glOrtho(float32 left, float32 right, float32 bottom, float32 top, float32 n, float32 f)
{
    float32 r_l = right - left;
    float32 t_b = top - bottom;
    float32 f_n = f - n;
    float32 tx = - (right + left) / (right - left);
    float32 ty = - (top + bottom) / (top - bottom);
    float32 tz = - (f + n) / (f - n);
    
    data[0] = 2.0f / r_l;
    data[1] = 0.0f;
    data[2] = 0.0f;
    data[3] = 0.0f;
    
    data[4] = 0.0f;
    data[5] = 2.0f / t_b;
    data[6] = 0.0f;
    data[7] = 0.0f;
    
    data[8] = 0.0f;
    data[9] = 0.0f;
    data[10] = -2.0f / f_n;
    data[11] = 0.0f;
    
    data[12] = tx;
    data[13] = ty;
    data[14] = tz;
    data[15] = 1.0f;
}
    
void Matrix4::glFrustum(float32 l, float32 r, float32 b, float32 t, float32 n, float32 f)
{
    float32 r_l = r - l;
    float32 t_b = t - b;
    float32 f_n = f - n;
    
    data[0] = 2.0f * n / r_l;
    data[4] = 0.0f;
    data[8] = (r + l) / (r - l);
    data[12] = 0.0f;
    
    data[1] = 0.0f;
    data[5] = 2.0f * n / t_b;
    data[9] = (t + b) / (t - b);
    data[13] = 0.0f;
    
    data[2] = 0.0f;
    data[6] = 0.0f;
    data[10] = - (f + n) / f_n;
    data[14] = - 2 * f * n / f_n;
    
    data[3] = 0;
    data[7] = 0;
    data[11] = -1;
    data[15] = 0;
}
    
void Matrix4::glRotate(float32 angle, float32 x, float32 y, float32 z)
{
    float32 xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c, s, c;
    //float32 m[16];
    bool optimized;
    
    s = (float32) sinf( DegToRad(angle) );
    c = (float32) cosf( DegToRad(angle) );
    
    memcpy(this->data, IDENTITY.data, sizeof(float32)*16);
    optimized = false;
    
#define M(row,col)  this->data[col*4+row]
    
    if (x == 0.0F) {
        if (y == 0.0F) {
            if (z != 0.0F) {
                optimized = true;
                /* rotate only around z-axis */
                M(0,0) = c;
                M(1,1) = c;
                if (z < 0.0F) {
                    M(0,1) = s;
                    M(1,0) = -s;
                }
                else {
                    M(0,1) = -s;
                    M(1,0) = s;
                }
            }
        }
        else if (z == 0.0F) {
            optimized = true;
            /* rotate only around y-axis */
            M(0,0) = c;
            M(2,2) = c;
            if (y < 0.0F) {
                M(0,2) = -s;
                M(2,0) = s;
            }
            else {
                M(0,2) = s;
                M(2,0) = -s;
            }
        }
    }
    else if (y == 0.0F) {
        if (z == 0.0F) {
            optimized = true;
            /* rotate only around x-axis */
            M(1,1) = c;
            M(2,2) = c;
            if (x < 0.0F) {
                M(1,2) = s;
                M(2,1) = -s;
            }
            else {
                M(1,2) = -s;
                M(2,1) = s;
            }
        }
    }
    
    if (!optimized) {
        const float32 mag = sqrtf(x * x + y * y + z * z);
        
        if (mag <= 1.0e-4) {
            /* no rotation, leave mat as-is */
            return;
        }
        
        x /= mag;
        y /= mag;
        z /= mag;
        
        
        /*
         *     Arbitrary axis rotation matrix.
         *
         *  This is composed of 5 matrices, Rz, Ry, T, Ry', Rz', multiplied
         *  like so:  Rz * Ry * T * Ry' * Rz'.  T is the final rotation
         *  (which is about the X-axis), and the two composite transforms
         *  Ry' * Rz' and Rz * Ry are (respectively) the rotations necessary
         *  from the arbitrary axis to the X-axis then back.  They are
         *  all elementary rotations.
         *
         *  Rz' is a rotation about the Z-axis, to bring the axis vector
         *  into the x-z plane.  Then Ry' is applied, rotating about the
         *  Y-axis to bring the axis vector parallel with the X-axis.  The
         *  rotation about the X-axis is then performed.  Ry and Rz are
         *  simply the respective inverse transforms to bring the arbitrary
         *  axis back to its original orientation.  The first transforms
         *  Rz' and Ry' are considered inverses, since the data from the
         *  arbitrary axis gives you info on how to get to it, not how
         *  to get away from it, and an inverse must be applied.
         *
         *  The basic calculation used is to recognize that the arbitrary
         *  axis vector (x, y, z), since it is of unit length, actually
         *  represents the sines and cosines of the angles to rotate the
         *  X-axis to the same orientation, with theta being the angle about
         *  Z and phi the angle about Y (in the order described above)
         *  as follows:
         *
         *  cos ( theta ) = x / sqrt ( 1 - z^2 )
         *  sin ( theta ) = y / sqrt ( 1 - z^2 )
         *
         *  cos ( phi ) = sqrt ( 1 - z^2 )
         *  sin ( phi ) = z
         *
         *  Note that cos ( phi ) can further be inserted to the above
         *  formulas:
         *
         *  cos ( theta ) = x / cos ( phi )
         *  sin ( theta ) = y / sin ( phi )
         *
         *  ...etc.  Because of those relations and the standard trigonometric
         *  relations, it is pssible to reduce the transforms down to what
         *  is used below.  It may be that any primary axis chosen will give the
         *  same results (modulo a sign convention) using thie method.
         *
         *  Particularly nice is to notice that all divisions that might
         *  have caused trouble when parallel to certain planes or
         *  axis go away with care paid to reducing the expressions.
         *  After checking, it does perform correctly under all cases, since
         *  in all the cases of division where the denominator would have
         *  been zero, the numerator would have been zero as well, giving
         *  the expected result.
         */
        
        xx = x * x;
        yy = y * y;
        zz = z * z;
        xy = x * y;
        yz = y * z;
        zx = z * x;
        xs = x * s;
        ys = y * s;
        zs = z * s;
        one_c = 1.0F - c;
        
        /* We already hold the identity-matrix so we can skip some statements */
        M(0,0) = (one_c * xx) + c;
        M(0,1) = (one_c * xy) - zs;
        M(0,2) = (one_c * zx) + ys;
        /*    M(0,3) = 0.0F; */
        
        M(1,0) = (one_c * xy) + zs;
        M(1,1) = (one_c * yy) + c;
        M(1,2) = (one_c * yz) - xs;
        /*    M(1,3) = 0.0F; */
        
        M(2,0) = (one_c * zx) - ys;
        M(2,1) = (one_c * yz) + xs;
        M(2,2) = (one_c * zz) + c;
        /*    M(2,3) = 0.0F; */
        
        /*
         M(3,0) = 0.0F;
         M(3,1) = 0.0F;
         M(3,2) = 0.0F;
         M(3,3) = 1.0F;
         */
    }
#undef M
    
    //matrix_multf( mat, m, MAT_FLAG_ROTATION );
}
    
void Matrix4::glTranslate(float32 x, float32 y, float32 z)
{
    CreateTranslation(Vector3(x, y, z));
}
    
void  Matrix4::glScale(float32 x, float32 y, float32 z)
{
    CreateScale(Vector3(x, y, z));
}



};