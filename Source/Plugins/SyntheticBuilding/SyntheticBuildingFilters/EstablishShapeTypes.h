/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _EstablishShapeTypes_H_
#define _EstablishShapeTypes_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"
/**
 * @class EstablishShapeTypes EstablishShapeTypes.h DREAM3DLib/SyntheticBuildingFilters/EstablishShapeTypes.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class EstablishShapeTypes : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(EstablishShapeTypes)
    DREAM3D_STATIC_NEW_MACRO(EstablishShapeTypes)
    DREAM3D_TYPE_MACRO_SUPER(EstablishShapeTypes, AbstractFilter)


    virtual ~EstablishShapeTypes();

    // inputs needed by this filter
    DREAM3D_FILTER_PARAMETER(DataArrayPath, InputPhaseTypesArrayPath)
    Q_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)

    // Outputs generated by this filter
    DREAM3D_FILTER_PARAMETER(QString, ShapeTypesArrayName)
    Q_PROPERTY(QString ShapeTypesArrayName READ getShapeTypesArrayName WRITE setShapeTypesArrayName)

    DREAM3D_FILTER_PARAMETER(UInt32Vector_t, ShapeTypeData)
    Q_PROPERTY(UInt32Vector_t ShapeTypeData READ getShapeTypeData WRITE setShapeTypeData)

    int getPhaseCount();
    Q_PROPERTY(int PhaseCount READ getPhaseCount)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();
    virtual const QString getBrandingString() { return SyntheticBuilding::SyntheticBuildingPluginDisplayName + " Filter"; }

    /**
     * @brief setupFilterParameters
     */
    virtual void setupFilterParameters();

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    virtual void preflight();

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    EstablishShapeTypes();

  private:
    // Cell Data - make sure these are all initialized to NULL in the constructor
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(unsigned int, PhaseTypes)
    DEFINE_CREATED_DATAARRAY_VARIABLE(unsigned int, ShapeTypes)

    void dataCheck();

    EstablishShapeTypes(const EstablishShapeTypes&); // Copy Constructor Not Implemented
    void operator=(const EstablishShapeTypes&); // Operator '=' Not Implemented
};

#endif /* _EstablishShapeTypes_H_ */



