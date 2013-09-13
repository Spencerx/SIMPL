/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "QEbsdReferenceFrameDialog.h"

#include <iostream>


#include <QtCore/QPropertyAnimation>
#include <QtCore/QtEndian>
#include <QtCore/QStateMachine>

#include <QtGui/QPixmap>
#include <QtGui/QGraphicsPixmapItem>
#include <QtGui/QPushButton>
#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QFont>

#include "DREAM3DLib/Common/VolumeDataContainer.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/IOFilters/ReadOrientationData.h"
#include "DREAM3DLib/GenericFilters/GenerateIPFColors.h"
#include "DREAM3DLib/ProcessingFilters/ConvertEulerAngles.h"

#include "QtSupport/DREAM3DHelpUrlGenerator.h"

// This file is generated by Qt's moc program
#include "DREAM3DLib/IOFiltersWidgets/moc_QEbsdReferenceFrameDialog.cxx"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QEbsdReferenceFrameDialog::QEbsdReferenceFrameDialog(QString filename, QWidget *parent) :
  QDialog(parent),
  m_EbsdFileName(filename),
  m_OriginGroup(NULL)
{
  setupUi(this);
  m_OriginGroup = new QButtonGroup(this);
  m_OriginGroup->addButton(m_TSLdefaultBtn);
  m_OriginGroup->addButton(m_HKLdefaultBtn);
  m_OriginGroup->addButton(m_HEDMdefaultBtn);
  m_OriginGroup->addButton(m_NoTransBtn);
  connect(m_TSLdefaultBtn, SIGNAL(toggled(bool)),
          this, SLOT(originChanged(bool)));
  connect(m_HKLdefaultBtn, SIGNAL(toggled(bool)),
          this, SLOT(originChanged(bool)));
  connect(m_HEDMdefaultBtn, SIGNAL(toggled(bool)),
          this, SLOT(originChanged(bool)));
  connect(m_NoTransBtn, SIGNAL(toggled(bool)),
          this, SLOT(originChanged(bool)));

  connect(degToRads, SIGNAL(stateChanged(int)),
          this, SLOT(degToRagsChanged(int)));


  connect(refDir, SIGNAL(currentIndexChanged(int)),
          this, SLOT(referenceDirectionChanged()));

  loadEbsdData();
  updateGraphicsView();
  m_NoTransBtn->setChecked(true);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QEbsdReferenceFrameDialog::~QEbsdReferenceFrameDialog()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::setEbsdFileName(QString filename)
{
  m_EbsdFileName = filename;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::setTSLDefault(bool checked)
{
  m_TSLdefaultBtn->setChecked(checked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::setHKLDefault(bool checked)
{
  m_HKLdefaultBtn->setChecked(checked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::setHEDMDefault(bool checked)
{
  m_HEDMdefaultBtn->setChecked(checked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::setNoTrans(bool checked)
{
  m_NoTransBtn->setChecked(checked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::EbsdToSampleCoordinateMapping QEbsdReferenceFrameDialog::getSelectedOrigin()
{
  if (m_TSLdefaultBtn->isChecked()) { return Ebsd::TSLdefault; }
  if (m_HKLdefaultBtn->isChecked()) { return Ebsd::HKLdefault; }
  if (m_HEDMdefaultBtn->isChecked()) { return Ebsd::HEDMdefault; }
  if (m_NoTransBtn->isChecked()) { return Ebsd::UnknownCoordinateMapping; }
  return Ebsd::UnknownCoordinateMapping;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QEbsdReferenceFrameDialog::getTSLchecked()
{
  return m_TSLdefaultBtn->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QEbsdReferenceFrameDialog::getHKLchecked()
{
  return m_HKLdefaultBtn->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QEbsdReferenceFrameDialog::getHEDMchecked()
{
  return m_HEDMdefaultBtn->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QEbsdReferenceFrameDialog::getNoTranschecked()
{
  return m_NoTransBtn->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::loadEbsdData()
{
  if (m_EbsdFileName.isEmpty() == true) { return; }
  VolumeDataContainer::Pointer m = VolumeDataContainer::New();

  ReadOrientationData::Pointer reader = ReadOrientationData::New();
  reader->setInputFile(m_EbsdFileName);
  reader->setVolumeDataContainer(m.get());
  reader->execute();
  int err = reader->getErrorCondition();
  if (err < 0)
  {
    ipfLabel->setText("Error Reading the file");
    m_BaseImage = QImage();
    m_DisplayedImage = QImage();
  }

  //If they want to convert the Eulers to Radians
  if(degToRads->isChecked() == true)
  {
    ConvertEulerAngles::Pointer convert = ConvertEulerAngles::New();
    convert->setConversionType(DREAM3D::EulerAngleConversionType::DegreesToRadians);
    convert->setVolumeDataContainer(m.get());
    convert->execute();
    err = convert->getErrorCondition();
    if (err < 0)
    {
      ipfLabel->setText("Error converting angles to radians");
      m_BaseImage = QImage();
      m_DisplayedImage = QImage();
    }
  }

  GenerateIPFColors::Pointer ipfColorFilter = GenerateIPFColors::New();
  FloatVec3Widget_t ref;
  ref.x = 0;
  ref.y = 0;
  ref.z = 0;
  if (refDir->currentIndex() == 0)
  {
    ref.x = 1;
  }
  else if(refDir->currentIndex() == 1)
  {
    ref.y = 1;
  }
  else if(refDir->currentIndex() == 2)
  {
    ref.z = 1;
  }
  ipfColorFilter->setReferenceDir(ref);
  ipfColorFilter->setVolumeDataContainer(m.get());
  ipfColorFilter->execute();
  err = ipfColorFilter->getErrorCondition();
  if (err < 0)
  {
    ipfLabel->setText("Error Generating IPF Colors" );
    m_BaseImage = QImage();
    m_DisplayedImage = QImage();
  }

  size_t dims[3] = {0, 0, 0};
  m->getDimensions(dims);
  float res[3] = {0.0f, 0.0f, 0.0f};
  m->getResolution(res);

  m_XDim->setText(QString::number(dims[0]));
  m_YDim->setText(QString::number(dims[1]));
  m_XRes->setText(QString::number(res[0]));
  m_YRes->setText(QString::number(res[1]));

  QImage image(dims[0], dims[1], QImage::Format_ARGB32);
  size_t index = 0;

  IDataArray::Pointer arrayPtr = m->getCellData(ipfColorFilter->getCellIPFColorsArrayName());
  if (NULL == arrayPtr.get())
  {
      ipfLabel->setText("Error Generating IPF Colors from Array in Voxel Data Container" );
    m_BaseImage = QImage();
    m_DisplayedImage = QImage();
    return;
  }

  UInt8ArrayType* rgbArray = UInt8ArrayType::SafePointerDownCast(arrayPtr.get());
  uint8_t* ipfColors = rgbArray->getPointer(0);

  for(size_t y = 0; y < dims[1]; ++y)
  {
    uint8_t* scanLine = image.scanLine(y);
    for(size_t x = 0; x < dims[0]; ++x)
    {
#if defined (CMP_WORDS_BIGENDIAN)
#error
#else
      scanLine[x*4 + 3] = 0xFF;
      index = y * dims[0] * 3 + x * 3;
      scanLine[x*4 + 2] = ipfColors[index + 0];
      scanLine[x*4 + 1] = ipfColors[index + 1];
      scanLine[x*4 + 0] = ipfColors[index + 2];
#endif
    }

  }
  m_BaseImage = image.mirrored(false, true);
  m_DisplayedImage = m_BaseImage;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::originChanged(bool checked)
{
  if (checked == false) { return; }

  updateGraphicsView();

  if (checked == true) {
    updateDisplay();
  }

  // Initialize to "No Transform"
  float sampleTransAngle = 0.0f;
  float sampleTransAxis[3] = { 0.0f, 0.0f, 1.0f};
  float eulerTranAngle = 0.0f;
  float eulerTransAxis[3] = { 0.0f, 0.0f, 1.0f};

  getSampleTranformation(sampleTransAngle, sampleTransAxis[0], sampleTransAxis[1], sampleTransAxis[2]);
  getEulerTranformation(eulerTranAngle, eulerTransAxis[0], eulerTransAxis[1], eulerTransAxis[2]);

  QString sampleTrans = QString::number(sampleTransAngle) + " @ <" + QString::number(sampleTransAxis[0]) + QString::number(sampleTransAxis[1]) + QString::number(sampleTransAxis[2]) + ">";
  QString eulerTrans = QString::number(eulerTranAngle) + " @ <" + QString::number(eulerTransAxis[0]) + QString::number(eulerTransAxis[1]) + QString::number(eulerTransAxis[2]) + ">";
  m_SampleTransformationLabel->setText(sampleTrans);
  m_EulerTransformationLabel->setText(eulerTrans);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::getSampleTranformation(float &sampleTransAngle, float &dir0, float &dir1, float &dir2)
{
  // Initialize to "No Transform"
  sampleTransAngle = 0.0f;
  dir0 = 0.0f;
  dir1 = 0.0f;
  dir2 = 1.0f;


  if (getTSLchecked() == true)
  {
    sampleTransAngle = 180.0;
    dir0 = 0.0;
    dir1 = 1.0;
    dir2 = 0.0;
  }
  else if (getHKLchecked() == true)
  {
    sampleTransAngle = 180.0;
    dir0 = 0.0;
    dir1 = 1.0;
    dir2 = 0.0;
  }
  else if (getHEDMchecked() == true)
  {
    sampleTransAngle = 0.0;
    dir0 = 0.0;
    dir1 = 0.0;
    dir2 = 1.0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::getEulerTranformation(float &eulerTransAngle, float &dir0, float &dir1, float &dir2)
{
  eulerTransAngle = 0.0f;
  dir0 = 0.0f;
  dir1 = 0.0f;
  dir2 = 1.0f;

  if (getTSLchecked() == true)
  {
    eulerTransAngle = 90.0;
    dir0 = 0.0;
    dir1 = 0.0;
    dir2 = 1.0;
  }
  else if (getHKLchecked() == true)
  {
    eulerTransAngle = 0.0;
    dir0 = 0.0;
    dir1 = 0.0;
    dir2 = 1.0;
  }
  else if (getHEDMchecked() == true)
  {
    eulerTransAngle = 0.0;
    dir0 = 0.0;
    dir1 = 0.0;
    dir2 = 1.0;
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::updateGraphicsView()
{
  if (m_TSLdefaultBtn->isChecked()) {
    m_DisplayedImage = m_BaseImage.mirrored(true, false);
  }
  else if (m_HKLdefaultBtn->isChecked()) {
    m_DisplayedImage = m_BaseImage.mirrored(true, false);
  }
  else if (m_HEDMdefaultBtn->isChecked()) {
    m_DisplayedImage = m_BaseImage;
  }
  else if (m_NoTransBtn->isChecked()) {
    m_DisplayedImage = m_BaseImage;
  }


  QSize size = m_DisplayedImage.size();
  // If the EBSD map is larger than 1024 pixels in either height or width then rescale
  // back to 1024 pixels
  if (size.width() > 1024 || size.height() > 1024)
  {
    QSize gvSize(1024,1024);
    m_DisplayedImage = m_DisplayedImage.scaled(gvSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }

  m_DisplayedImage = paintImage(m_DisplayedImage);



  ipfLabel->setText("");
  ipfLabel->setPixmap(QPixmap::fromImage(m_DisplayedImage));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage QEbsdReferenceFrameDialog::paintImage(QImage image)
{
  QSize size = image.size();
  int imageWidth = size.width();
  int imageHeight = size.height();

  int pxHigh = 0;
  int pxWide = 0;

  QFont font("Ariel", 16, QFont::Bold);
  {
    QPainter painter;
    QImage pImage(100, 100, QImage::Format_ARGB32_Premultiplied);
    pImage.fill(0xFFFFFFFF); // All white background
    painter.begin(&pImage);

    painter.setFont(font);
    QFontMetrics metrics = painter.fontMetrics();
    pxHigh = metrics.height();
    pxWide = metrics.width(QString("TD"));
    painter.end();
  }


  int pxOffset = 2 * pxWide;
  int pyOffset = 2 * pxHigh;
  // Get a QPainter object to add some more details to the image


  int pImageWidth = imageWidth + pxOffset * 2;
  int pImageHeight = imageHeight + pyOffset * 2;

  QImage pImage(pImageWidth, pImageHeight, QImage::Format_ARGB32_Premultiplied);
  pImage.fill(0xFFFFFFFF); // All white background

  // Create a Painter backed by a QImage to draw into
  QPainter painter;
  painter.begin(&pImage);
  painter.setRenderHint(QPainter::Antialiasing, true);

  painter.setFont(font);
  QFontMetrics metrics = painter.fontMetrics();
  pxHigh = metrics.height();
  pxWide = metrics.width(QString("TD"));

  QPoint point(pxOffset, pyOffset);
  painter.drawImage(point, image); // Draw the image we just generated into the QPainter's canvas

  qint32 penWidth = 2;
  painter.setPen(QPen(QColor(0, 0, 0, 255), penWidth, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));

//  QPainterPath circle;
//  QPointF center(pImageWidth / 2, pImageHeight / 2);
//  circle.addEllipse(center, imageWidth / 2, imageHeight / 2);
//  painter.drawPath(circle);

  painter.drawText(pxOffset/2, pImageHeight / 2 + pxHigh / 3, "Y");

  pxWide = metrics.width(QString("X"));
  painter.drawText(pImageWidth / 2 - pxWide / 2, pImageHeight - pyOffset + pxHigh + 2, "X");

  painter.drawText(pxOffset/2, pImageHeight - pyOffset + pxHigh + 2, "(0,0)");

//  pxWide = metrics.width(config.label);
//  painter.drawText(2, pxHigh, config.label);

  // Draw slightly transparent lines
  penWidth = 3;
  painter.setPen(QPen(QColor(0, 0, 0, 180), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  // Draw the X-Axis
  painter.drawLine(pxOffset-3, pImageHeight-pyOffset + 3, pImageWidth  / 2, pImageHeight-pyOffset + 3);
  // Draw the Y-Axis
  painter.drawLine(pxOffset-3, pImageHeight-pyOffset + 3, pxOffset-3, pImageHeight / 2);

//    painter.drawLine(pxOffset, pImageHeight / 2, pImageWidth - pxOffset, pImageHeight / 2);
//  painter.drawLine(pImageWidth / 2, pyOffset, pImageWidth / 2, pImageHeight - pyOffset);


  painter.end();
  // Scale the image down to 225 pixels
  return pImage;//.scaled(225, 225, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::updateDisplay()
{
  this->update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::degToRagsChanged(int state)
{
  loadEbsdData();
  originChanged(true);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::referenceDirectionChanged()
{
  loadEbsdData();
  originChanged(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::on_showHelp_clicked()
{
  DREAM3DHelpUrlGenerator::generateAndOpenHTMLUrl("ebsdtoh5ebsd", this);
}

