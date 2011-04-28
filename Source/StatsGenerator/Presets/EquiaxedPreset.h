/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#ifndef EQUIAXEDPRESETFACTORY_H_
#define EQUIAXEDPRESETFACTORY_H_


//-- C++ includes
#include <string>

//-- MXA Includes
#include <MXA/Common/MXASetGetMacros.h>

#include "AbstractMicrostructurePresetFactory.h"
#include "AbstractMicrostructurePreset.h"


class EquiaxedPreset : public AbstractMicrostructurePreset
{
  public:
    MXA_SHARED_POINTERS(EquiaxedPreset);
    MXA_STATIC_NEW_SUPERCLASS(AbstractMicrostructurePreset, EquiaxedPreset);
    virtual ~EquiaxedPreset();


    void generateOmega3Data(StatsGenPlotWidget* plot, QVector<double> binNumbers);
    void generateBOverAPlotData(StatsGenPlotWidget* plot, QVector<double> binNumbers);
    void generateCOverAPlotData(StatsGenPlotWidget* plot, QVector<double> binNumbers);
    void generateCOverBPlotData(StatsGenPlotWidget* plot, QVector<double> binNumbers);
    void generateNeighborPlotData(StatsGenPlotWidget* plot, QVector<double> binNumbers);

  protected:
    EquiaxedPreset();

  private:
    EquiaxedPreset(const EquiaxedPreset&); // Copy Constructor Not Implemented
    void operator=(const EquiaxedPreset&); // Operator '=' Not Implemented
};

DECLARE_FACTORY_CLASS(EquiaxedPresetFactory, EquiaxedPreset, Equiaxed );

#endif /* EQUIAXEDPRESETFACTORY_H_ */
