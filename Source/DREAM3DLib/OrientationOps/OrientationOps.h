/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Michael A. Groeber (US Air Force Research Laboratory)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _OrientationOps_H_
#define _OrientationOps_H_

#include <vector>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/Math/QuaternionMath.hpp"
#include "DREAM3DLib/Math/MatrixMath.h"


/*
 * @class OrientationOps OrientationOps.h DREAM3DLib/Common/OrientationOps.h
 * @brief This class performs Crystallographic Misorientation Calculations
 * @author Michael A. Jackson (BlueQuartz Software)
 * @author Michael A. Groeber (US Air Force Research Laboratory)
 * @date Feb 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT OrientationOps
{
  public:
    DREAM3D_SHARED_POINTERS(OrientationOps)
    DREAM3D_TYPE_MACRO(OrientationOps)

    virtual ~OrientationOps();



    static std::vector<OrientationOps::Pointer> getOrientationOpsVector();

    virtual int getODFSize() = 0;
    virtual int getMDFSize() = 0;
    virtual int getNumSymOps() = 0;

    virtual float getMisoQuat(QuatF &q1, QuatF &q2, float &n1, float &n2, float &n3) = 0;
    virtual void getQuatSymOp(int i, QuatF &q) = 0;
    virtual void getRodSymOp(int i, float *r) = 0;
    virtual void getMatSymOp(int i, float g[3][3]) = 0;
    virtual void getODFFZRod(float &r1, float &r2, float &r3) = 0;
    virtual void getMDFFZRod(float &r1, float &r2, float &r3) = 0;
    virtual void getNearestQuat(QuatF &q1, QuatF &q2) = 0;
    virtual int getMisoBin(float r1, float r2, float r3) = 0;
    virtual void determineEulerAngles(int choose, float &synea1, float &synea2, float &synea3) = 0;
    virtual void determineRodriguesVector(int choose, float &r1, float &r2, float &r3) = 0;
    virtual int getOdfBin(float r1, float r2, float r3) = 0;
    virtual void getSchmidFactorAndSS(float loadx, float loady, float loadz, float &schmidfactor, int &slipsys) = 0;
    virtual void getmPrime(QuatF &q1, QuatF &q2, float LD[3], float &mPrime) = 0;
    virtual void getF1(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F1) = 0;
    virtual void getF1spt(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F1spt) = 0;
    virtual void getF7(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F7) = 0;

    virtual void generateSphereCoordsFromEulers(FloatArrayType *eulers, FloatArrayType *c1, FloatArrayType *c2, FloatArrayType *c3) = 0;

    virtual void generateIPFColor(double* eulers, double* refDir, uint8_t* rgb, bool convertDegrees) = 0;
    virtual void generateIPFColor(double e0, double e1, double e2, double dir0, double dir1, double dir2, uint8_t* rgb, bool convertDegrees) = 0;
    virtual void generateRodriguesColor(float r1, float r2, float r3, unsigned char* rgb) = 0;

  protected:
    OrientationOps();

    float _calcMisoQuat(const QuatF quatsym[24], int numsym,
                  QuatF &q1, QuatF &q2,
                  float &n1, float &n2, float &n3);

    void _calcRodNearestOrigin(const float rodsym[24][3], int numsym, float &r1,float &r2, float &r3);
    void _calcNearestQuat(const QuatF quatsym[24], int numsym, QuatF &q1, QuatF &q2);
    void _calcQuatNearestOrigin(const QuatF quatsym[24], int numsym, QuatF &qr);

    int _calcMisoBin(float dim[3], float bins[3], float step[3], float r1, float r2, float r3);
    void _calcDetermineHomochoricValues(float init[3], float step[3], float phi[3], int choose, float &r1, float &r2, float &r3);
    int _calcODFBin(float dim[3], float bins[3], float step[3], float r1, float r2, float r3);

  private:
    OrientationOps(const OrientationOps&); // Copy Constructor Not Implemented
    void operator=(const OrientationOps&); // Operator '=' Not Implemented
};

#endif /* _OrientationOps_H_ */
