/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FilterPipeline.h"

#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/CoreFilters/EmptyFilter.h"

#include "SIMPLib/CoreFilters/DataContainerReader.h"

#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "moc_FilterPipeline.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::FilterPipeline()
: QObject()
, m_ErrorCondition(0)
, m_Cancel(false)
, m_PipelineName("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::~FilterPipeline()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterPipeline::getName()
{
  return m_PipelineName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::setName(QString newName)
{
  QString oldName = m_PipelineName;
  m_PipelineName = newName;

  emit pipelineNameChanged(oldName, newName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer FilterPipeline::deepCopy()
{
  // This is not the fastest way to perform a deep copy, but it does avoid 
  // issues with newFilterInstance retaining bindings to the original filter
  // that may no longer exist.

  // Convert to JSon
  QJsonObject json = toJson();

  // Convert from JSon
  FilterPipeline::Pointer copy = FilterPipeline::New();
  copy->fromJson(json);

  return copy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject FilterPipeline::toJson()
{
  QJsonObject json;

  FilterContainerType container = getFilterContainer();
  int count = container.size();
  int offset = 0;

  for(qint32 i = 0; i < count; i++)
  {
    AbstractFilter::Pointer filter = container.at(i);
    if(nullptr != filter.get())
    {
      DataContainerReader::Pointer reader = std::dynamic_pointer_cast<DataContainerReader>(filter);
      if(reader.get())
      {
        offset = reader->writeExistingPipelineToFile(json, i);
      }

      json.insert(QString::number(i + offset), filter->toJson());
    }
    else
    {
      AbstractFilter::Pointer badFilter = AbstractFilter::New();

      QJsonObject badFilterJson = badFilter->toJson();
      badFilterJson["Unknown Filter"] = "ERROR: Filter instance was nullptr within the FilterPipeline instance. Report this error to the DREAM3D Developers";
      
      json.insert(QString::number(i + offset), badFilterJson);
    }
  }

  // Add meta data about the pipeline
  {
    QJsonObject meta;
    meta[SIMPL::Settings::PipelineName] = m_PipelineName;
    meta[SIMPL::Settings::Version] = SIMPL::PipelineVersionNumbers::CurrentVersion;

    if(json.size() > 0)
    {
      meta[SIMPL::Settings::NumFilters] = count;
    }
    else
    {
      meta[SIMPL::Settings::NumFilters] = 0;
    }

    json[SIMPL::Settings::PipelineBuilderGroup] = meta;
  }

  return json;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer FilterPipeline::FromJson(const QJsonObject& json, IObserver* obs)
{
  QJsonObject builderObj = json[SIMPL::Settings::PipelineBuilderGroup].toObject();
  int filterCount = builderObj[SIMPL::Settings::NumFilters].toInt();

  FilterPipeline::Pointer pipeline;
  if(filterCount >= 0)
  {
    pipeline = FilterPipeline::New();
  }
  else
  {
    pipeline = FilterPipeline::NullPointer();
  }

  pipeline->fromJson(json, obs);

  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::fromJson(const QJsonObject& json, IObserver* obs)
{
  // Clear the pipeline first
  clear();

  // Store FilterManager
  FilterManager* filtManager = FilterManager::Instance();
  FilterFactory<EmptyFilter>::Pointer emptyFilterFactory = FilterFactory<EmptyFilter>::New();
  filtManager->addFilterFactory("EmptyFilter", emptyFilterFactory);

  QJsonObject builderObj = json[SIMPL::Settings::PipelineBuilderGroup].toObject();
  int filterCount = builderObj[SIMPL::Settings::NumFilters].toInt();
  int maxFilterIndex = filterCount - 1; // Zero based indexing
  
  setName(builderObj[SIMPL::Settings::PipelineName].toString());

  for(int i = 0; i < filterCount; ++i)
  {
    QJsonObject currentFilterIndex = json[QString::number(i)].toObject();

    QString filterName = currentFilterIndex[SIMPL::Settings::FilterName].toString();

    if(filterName.isEmpty() == false)
    {
      IFilterFactory::Pointer factory = filtManager->getFactoryForFilter(filterName);
      if(factory.get() != nullptr)
      {
        AbstractFilter::Pointer filter = factory->create();

        if(nullptr != filter.get())
        {
          filter->readFilterParameters(currentFilterIndex);
          this->pushBack(filter);
        }
      }
      else // Could not find the filter because the specific name has not been registered. This could
           // be due to a name change for the filter.
      {
        EmptyFilter::Pointer filter = EmptyFilter::New();
        QString humanLabel = QString("UNKNOWN FILTER: ") + filterName;
        filter->setHumanLabel(humanLabel);
        filter->setOriginalFilterName(filterName);
        this->pushBack(filter);

        if(nullptr != obs)
        {
          QString ss = QObject::tr("An implementation for filter '%1' could not be located. Possible reasons include a name change of the filter, plugin not loading or a simple spelling mistake? A "
            "blank filter has been inserted in its place.")
            .arg(filterName);
          PipelineMessage pm(filterName, ss, -66066, PipelineMessage::MessageType::Error);
          pm.setPrefix("JsonFilterParametersReader::ReadPipelineFromFile()");
          obs->processPipelineMessage(pm);
        }
      }
    }
    else
    {
      EmptyFilter::Pointer filter = EmptyFilter::New();
      QString humanLabel = QString("MISSING FILTER: ") + filterName;
      filter->setHumanLabel(humanLabel);
      filter->setOriginalFilterName(filterName);
      this->pushBack(filter);

      if(nullptr != obs)
      {
        QString gName = QString::number(i);
        QString ss = QObject::tr("A filter for index '%1' is missing in the file. Is the numbering of the filters correct in the pipeline file?").arg(gName);
        PipelineMessage pm(filterName, ss, -66067, PipelineMessage::MessageType::Error);
        pm.setPrefix("JsonFilterParametersReader::ReadPipelineFromFile()");
        obs->processPipelineMessage(pm);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::setCancel(bool value)
{
  this->m_Cancel = value;
  if(nullptr != m_CurrentFilter.get())
  {
    m_CurrentFilter->setCancel(value);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::getCancel()
{
  return m_Cancel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::cancelPipeline()
{
  setCancel(true);
  emit pipelineCanceled();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::run()
{
  execute();
  pipelineFinished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::pushFront(AbstractFilter::Pointer f)
{
  m_Pipeline.push_front(f);
  updatePrevNextFilters();
  emit pipelineWasEdited();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::popFront()
{
  m_Pipeline.pop_front();
  updatePrevNextFilters();
  emit pipelineWasEdited();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::pushBack(AbstractFilter::Pointer f)
{
  m_Pipeline.push_back(f);
  updatePrevNextFilters();
  emit pipelineWasEdited();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::popBack()
{
  m_Pipeline.pop_back();
  updatePrevNextFilters();
  emit pipelineWasEdited();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::insert(size_t index, AbstractFilter::Pointer f)
{
  FilterContainerType::iterator it = m_Pipeline.begin();
  for(size_t i = 0; i < index; ++i)
  {
    ++it;
  }
  m_Pipeline.insert(it, f);
  updatePrevNextFilters();
  emit pipelineWasEdited();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::erase(size_t index)
{
  FilterContainerType::iterator it = m_Pipeline.begin();
  for(size_t i = 0; i < index; ++i)
  {
    ++it;
  }
  m_Pipeline.erase(it);
  updatePrevNextFilters();
  emit pipelineWasEdited();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::clear()
{
  for(FilterContainerType::iterator iter = m_Pipeline.begin(); iter != m_Pipeline.end(); ++iter)
  {
    (*iter)->setPreviousFilter(AbstractFilter::NullPointer());
    (*iter)->setNextFilter(AbstractFilter::NullPointer());
  }
  m_Pipeline.clear();
  emit pipelineWasEdited();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t FilterPipeline::size()
{
  return m_Pipeline.size();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::empty()
{
  return m_Pipeline.isEmpty();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FilterPipeline::removeFirstFilterByName(const QString& name)
{
  AbstractFilter::Pointer f = AbstractFilter::NullPointer();
  for(FilterContainerType::iterator it = m_Pipeline.begin(); it != m_Pipeline.end(); ++it)
  {
    if((*it)->getHumanLabel().compare(name) == 0)
    {
      f = *it;
      m_Pipeline.erase(it);
      break;
    }
  }
  updatePrevNextFilters();
  emit pipelineWasEdited();

  return f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::FilterContainerType& FilterPipeline::getFilterContainer()
{
  return m_Pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::updatePrevNextFilters()
{
  FilterContainerType::iterator prev;
  FilterContainerType::iterator next;

  for(FilterContainerType::iterator iter = m_Pipeline.begin(); iter != m_Pipeline.end(); ++iter)
  {
    (*iter)->setPreviousFilter(AbstractFilter::NullPointer());
    (*iter)->setNextFilter(AbstractFilter::NullPointer());
    if(iter != m_Pipeline.begin())
    {
      prev = iter;
      prev--;
      // prevFilt = *prev;
      (*iter)->setPreviousFilter(*prev);
    }

    if(iter != m_Pipeline.end())
    {
      next = iter;
      next++;
      //  nextFilt = *next;
      if(next != m_Pipeline.end())
      {
        (*iter)->setNextFilter(*next);
      }
    }
  }
  int index = 0;
  for(FilterContainerType::iterator filter = m_Pipeline.begin(); filter != m_Pipeline.end(); ++filter)
  {
    (*filter)->setPipelineIndex(index++);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::addMessageReceiver(QObject* obj)
{
  m_MessageReceivers.push_back(obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::connectFilterNotifications(QObject* filter)
{
  for(int i = 0; i < m_MessageReceivers.size(); i++)
  {
    connect(filter, SIGNAL(filterGeneratedMessage(const PipelineMessage&)), m_MessageReceivers.at(i), SLOT(processPipelineMessage(const PipelineMessage&)));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::disconnectFilterNotifications(QObject* filter)
{
  for(int i = 0; i < m_MessageReceivers.size(); i++)
  {
    disconnect(filter, SIGNAL(filterGeneratedMessage(const PipelineMessage&)), m_MessageReceivers.at(i), SLOT(processPipelineMessage(const PipelineMessage&)));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FilterPipeline::preflightPipeline()
{
  // Create the DataContainer object
  DataContainerArray::Pointer dca = DataContainerArray::New();

  setErrorCondition(0);
  int preflightError = 0;

  // Start looping through each filter in the Pipeline and preflight everything
  for(FilterContainerType::iterator filter = m_Pipeline.begin(); filter != m_Pipeline.end(); ++filter)
  {
    (*filter)->setDataContainerArray(dca);
    setCurrentFilter(*filter);
    connectFilterNotifications((*filter).get());
    (*filter)->preflight();
    disconnectFilterNotifications((*filter).get());

    (*filter)->setDataContainerArray(dca->deepCopy(false));
    (*filter)->setCancel(false); // Reset the cancel flag
    preflightError |= (*filter)->getErrorCondition();
  }
  setCurrentFilter(AbstractFilter::NullPointer());
  return preflightError;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::execute()
{
  int err = 0;

  connectSignalsSlots();

  DataContainerArray::Pointer dca = DataContainerArray::New();

  // Start looping through the Pipeline
  float progress = 0.0f;

  // Connect this object to anything that wants to know about PipelineMessages
  for(int i = 0; i < m_MessageReceivers.size(); i++)
  {
    connect(this, SIGNAL(pipelineGeneratedMessage(const PipelineMessage&)), m_MessageReceivers.at(i), SLOT(processPipelineMessage(const PipelineMessage&)));
  }

  PipelineMessage progValue("", "", 0, PipelineMessage::MessageType::ProgressValue, -1);
  for(FilterContainerType::iterator filter = m_Pipeline.begin(); filter != m_Pipeline.end(); ++filter)
  {
    AbstractFilter::Pointer filt = *filter;
    progress = progress + 1.0f;
    progValue.setType(PipelineMessage::MessageType::ProgressValue);
    progValue.setProgressValue(static_cast<int>(progress / (m_Pipeline.size() + 1) * 100.0f));
    emit pipelineGeneratedMessage(progValue);

    QString ss = QObject::tr("[%1/%2] %3 ").arg(progress).arg(m_Pipeline.size()).arg(filt->getHumanLabel());

    progValue.setType(PipelineMessage::MessageType::StatusMessage);
    progValue.setText(ss);
    emit pipelineGeneratedMessage(progValue);

    filt->setMessagePrefix(ss);
    connectFilterNotifications(filt.get());
    filt->setDataContainerArray(dca);
    setCurrentFilter(*filter);
    emit filt->filterInProgress();
    filt->execute();
    disconnectFilterNotifications((*filter).get());
    filt->setDataContainerArray(DataContainerArray::NullPointer());
    err = filt->getErrorCondition();
    if(err < 0)
    {
      setErrorCondition(err);
      progValue.setFilterClassName(filt->getNameOfClass());
      progValue.setFilterHumanLabel(filt->getHumanLabel());
      progValue.setType(PipelineMessage::MessageType::Error);
      progValue.setProgressValue(100);
      ss = QObject::tr("[%1/%2] %3 caused an error during execution.").arg(progress).arg(m_Pipeline.size()).arg(filt->getHumanLabel());
      progValue.setText(ss);
      progValue.setPipelineIndex((*filter)->getPipelineIndex());
      progValue.setCode(filt->getErrorCondition());
      emit pipelineGeneratedMessage(progValue);
      emit filt->filterCompleted();
      emit pipelineFinished();
      disconnectSignalsSlots();

      return;
    }
    if(this->getCancel() == true)
    {
      break;
    }
    // ss = QObject::tr("%1 Filter Complete").arg(filt->getNameOfClass());
    emit filt->filterCompleted();
  }

  emit pipelineFinished();

  disconnectSignalsSlots();

  PipelineMessage completMessage("", "Pipeline Complete", 0, PipelineMessage::MessageType::StatusMessage, -1);
  emit pipelineGeneratedMessage(completMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::printFilterNames(QTextStream& out)
{
  out << "---------------------------------------------------------------------";
  for(FilterContainerType::iterator iter = m_Pipeline.begin(); iter != m_Pipeline.end(); ++iter)
  {
    out << (*iter)->getNameOfClass() << "\n";
  }
  out << "---------------------------------------------------------------------";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::connectSignalsSlots()
{
  for(int i = 0; i < m_Pipeline.size(); i++)
  {
    AbstractFilter::Pointer filter = m_Pipeline[i];
    connect(this, SIGNAL(pipelineFinished()), filter.get(), SLOT(cleanupFilter()));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::disconnectSignalsSlots()
{
  for(int i = 0; i < m_Pipeline.size(); i++)
  {
    AbstractFilter::Pointer filter = m_Pipeline[i];
    disconnect(this, SIGNAL(pipelineFinished()), filter.get(), SLOT(cleanupFilter()));
  }
}
