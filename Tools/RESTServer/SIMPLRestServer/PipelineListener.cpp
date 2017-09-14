#include "PipelineListener.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineListener::PipelineListener(QObject* parent)
    : QObject(parent)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineListener::~PipelineListener()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<PipelineMessage> PipelineListener::getMessages()
{
    return m_Messages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<PipelineMessage> PipelineListener::getErrorMessages()
{
    std::vector<PipelineMessage> errorMessages;

    size_t numMessages = m_Messages.size();
    for(size_t i = 0; i < numMessages; i++)
    {
        if(m_Messages[i].getType() == PipelineMessage::MessageType::Error)
        {
            errorMessages.push_back(m_Messages[i]);
        }
    }

    return errorMessages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<PipelineMessage> PipelineListener::getWarningMessages()
{
    std::vector<PipelineMessage> warningMessages;

    size_t numMessages = m_Messages.size();
    for(size_t i = 0; i < numMessages; i++)
    {
        if(m_Messages[i].getType() == PipelineMessage::MessageType::Warning)
        {
            warningMessages.push_back(m_Messages[i]);
        }
    }

    return warningMessages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<PipelineMessage> PipelineListener::getStatusMessages()
{
  std::vector<PipelineMessage> statusMessages;

  size_t numMessages = m_Messages.size();
  for(size_t i = 0; i < numMessages; i++)
  {
    if(m_Messages[i].getType() == PipelineMessage::MessageType::StatusMessage)
    {
      statusMessages.push_back(m_Messages[i]);
    }
  }

  return statusMessages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<PipelineMessage> PipelineListener::getStandardOutputMessages()
{
  std::vector<PipelineMessage> stdOutMessages;

  size_t numMessages = m_Messages.size();
  for(size_t i = 0; i < numMessages; i++)
  {
    if(m_Messages[i].getType() == PipelineMessage::MessageType::StandardOutputMessage)
    {
      stdOutMessages.push_back(m_Messages[i]);
    }
  }

  return stdOutMessages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListener::processPipelineMessage(const PipelineMessage& pm)
{
    m_Messages.push_back(pm);

    if(pm.getType() == PipelineMessage::MessageType::StandardOutputMessage)
    {
        qDebug() << pm.getText();
    }
}
