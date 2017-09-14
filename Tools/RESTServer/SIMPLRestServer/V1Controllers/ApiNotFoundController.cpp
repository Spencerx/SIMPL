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

#include "ApiNotFoundController.h"

#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"

#include <QtCore/QDateTime>
#include <QtCore/QVariant>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ApiNotFoundController::ApiNotFoundController(const QHostAddress &hostAddress, const int hostPort)
{
  setListenHost(hostAddress, hostPort);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ApiNotFoundController::service(HttpRequest& request, HttpResponse& response)
{
  
  QString content_type = request.getHeader(QByteArray("content-type"));
  
  QJsonObject rootObj;
  
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
  
  //   response.setCookie(HttpCookie("firstCookie","hello",600,QByteArray(),QByteArray(),QByteArray(),false,true));
  //   response.setCookie(HttpCookie("secondCookie","world",600));
  
  QByteArray path = request.getPath();
  
  rootObj["ERROR"] = "THIS API IS NOT IMPLEMENTED." + QString(path);
  QJsonDocument jdoc(rootObj);
  
  response.write(jdoc.toJson(), true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ApiNotFoundController::EndPoint()
{
  return QString("ApiNotFound");
}
