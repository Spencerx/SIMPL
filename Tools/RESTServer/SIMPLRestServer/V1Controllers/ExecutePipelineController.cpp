/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Softwae, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ExecutePipelineController.h"


#include <QtCore/QDateTime>
#include <QtCore/QVariant>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QDir>

#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/InputPathFilterParameter.h"


#include "SIMPLRestServer/V1Controllers/SIMPLStaticFileController.h"
#include "SIMPLRestServer/PipelineListener.h"
#include "QtWebApp/httpserver/httpsessionstore.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExecutePipelineController::ExecutePipelineController()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineController::service(HttpRequest& request, HttpResponse& response)
{
  
    // Get current session, or create a new one
  HttpSessionStore* sessionStore = HttpSessionStore::Instance();
  HttpSession session = sessionStore->getSession(request, response);
  
  QString content_type = request.getHeader(QByteArray("content-type"));
  
  QJsonObject rootObj;
  rootObj["SessionID"] = QString(session.getId());
  response.setHeader("Content-Type", "application/json");
  
  if(content_type.compare("application/json") != 0)
  {
    // Form Error response
    rootObj["ErrorMessage"] = EndPoint() + ": Content Type is not application/json";
    rootObj["ErrorCode"]= -20;
    QJsonDocument jdoc(rootObj);
    
    response.write(jdoc.toJson(), true);
    return;
  }


    
  QString requestBody = request.getBody();
  QJsonDocument requestDoc = QJsonDocument::fromJson(requestBody.toUtf8());
  QJsonObject requestObj = requestDoc.object();

  QJsonObject pipelineObj = requestObj["Pipeline"].toObject();
  FilterPipeline::Pointer pipeline = FilterPipeline::FromJson(pipelineObj);

  // Look through the pipeline to find any input or output filter parameters.  Replace
  // the file paths in these filter parameters with session-id specific paths.
  QList<AbstractFilter::Pointer> filters = pipeline->getFilterContainer();
  for (int i = 0; i < filters.size(); i++)
  {
    AbstractFilter::Pointer filter = filters[i];
    QVector<FilterParameter::Pointer> filterParams = filter->getFilterParameters();

    for(QVector<FilterParameter::Pointer>::iterator iter = filterParams.begin(); iter != filterParams.end(); ++iter)
    {
      FilterParameter* parameter = (*iter).get();
      OutputFileFilterParameter* outFileParam = dynamic_cast<OutputFileFilterParameter*>(parameter);
      OutputPathFilterParameter* outPathParam = dynamic_cast<OutputPathFilterParameter*>(parameter);
      InputFileFilterParameter* inFileParam = dynamic_cast<InputFileFilterParameter*>(parameter);
      InputPathFilterParameter* inPathParam = dynamic_cast<InputPathFilterParameter*>(parameter);

      QString newFilePath = "Foo";
      SIMPLStaticFileController* staticFileController = SIMPLStaticFileController::Instance();
      QString docRoot = staticFileController->getDocRoot();
      if(outFileParam != nullptr)
      {
        QString existingPath = outFileParam->getGetterCallback()();
        existingPath = docRoot + QDir::separator() + QString(session.getId()) + QDir::separator() + existingPath; 
        outFileParam->getSetterCallback()(existingPath);
      }
      else if(outPathParam != nullptr)
      {
        outPathParam->getSetterCallback()(newFilePath);
      }
      else if(inFileParam != nullptr)
      {
        inFileParam->getSetterCallback()(newFilePath);
      }
      else if(inPathParam != nullptr)
      {
        inPathParam->getSetterCallback()(newFilePath);
      }
    }
  }

  PipelineListener* listener = new PipelineListener(nullptr);
  pipeline->addMessageReceiver(listener);
  pipeline->execute();
  
   //  response.setCookie(HttpCookie("firstCookie","hello",600,QByteArray(),QByteArray(),QByteArray(),false,true));
  //   response.setCookie(HttpCookie("secondCookie","world",600));

  std::vector<PipelineMessage> errorMessages = listener->getErrorMessages();
  bool completed = (errorMessages.size() == 0);
  if(!completed)
  {
      QJsonArray errors;
      int numErrors = errorMessages.size();
      for(int i = 0; i < numErrors; i++)
      {
          QJsonObject error;
          error["Code"] = errorMessages[i].generateErrorString();
          error["Message"] = errorMessages[i].getText();
          error["FilterHumanLabel"] = errorMessages[i].getFilterHumanLabel();
          error["FilterIndex"] = errorMessages[i].getPipelineIndex();

          errors.push_back(error);
      }

      rootObj["Errors"] = errors;
  }

  std::vector<PipelineMessage> warningMessages = listener->getWarningMessages();
  QJsonArray warnings;
  int numWarnings = warningMessages.size();
  for(int i = 0; i < numWarnings; i++)
  {
    QJsonObject warning;
    warning["Code"] = warningMessages[i].generateWarningString();
    warning["Message"] = warningMessages[i].getText();
    warning["FilterHumanLabel"] = warningMessages[i].getFilterHumanLabel();
    warning["FilterIndex"] = warningMessages[i].getPipelineIndex();

    warnings.push_back(warning);
  }
  rootObj["Warnings"] = warnings;

  delete listener;

  rootObj["Completed"] = completed;
  
  
  QJsonDocument jdoc(rootObj);
  
  response.write(jdoc.toJson(), true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExecutePipelineController::EndPoint()
{
  return QString("ExecutePipeline");
}
