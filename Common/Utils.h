#ifndef UTILS_H
#define UTILS_H
//-------------------------------------------------------------------------------------------------
#include <QTcpSocket>
#include <QHostAddress>

#include <memory>

#include "MsgGameTurn.h"
#include "MsgGameInstanceChanged.h"
#include "MsgIdentifier.h"
#include "MsgStateChanged.h"
//-------------------------------------------------------------------------------------------------
/** Get Message by type */
std::shared_ptr<MsgBase> poDecodeMessage(QByteArray& roMessage) {

  // Check stream data
  int iType = 0;
  QDataStream(roMessage) >> iType;
  EnMessageType enType = static_cast<EnMessageType>(iType);

  std::shared_ptr<MsgBase> poMsg = nullptr;

  switch (enType) {

  case enMTUserInfo:
    //poMsg = std::static_pointer_cast<MsgBase>(std::make_shared<MsgUserInfo>());
    break;

  case enMTIdentifier:
    poMsg = std::static_pointer_cast<MsgBase>(std::make_shared<MsgIdentifier>());
    break;

  case enMTStateChanged:
    poMsg = std::static_pointer_cast<MsgBase>(std::make_shared<MsgStateChanged>());
    break;

  case enMTGameInstanceChanged:
    poMsg = std::static_pointer_cast<MsgBase>(std::make_shared<MsgGameInstanceChanged>());
    break;

  case enMTGameTurn:
    poMsg = std::static_pointer_cast<MsgBase>(std::make_shared<MsgGameTurn>());
    break;

  default:
    break;
  }

  // Decoding
  if (poMsg) {
    QDataStream oDataStream(roMessage);
    poMsg->vDecoding(oDataStream);
  }

  return poMsg;
}
//-------------------------------------------------------------------------------------------------
/** Read message */
bool bReadMessage(QTcpSocket* poSocket, QByteArray& roMessage)
{
  // Check if message is full (bad but keeping this now...)
  unsigned long ulMessageLength = static_cast<unsigned long>(poSocket->bytesAvailable());
  if (ulMessageLength >= cuiMessageSize) {
    poSocket->read(roMessage.data(), roMessage.size());
    return true;
  }
  return false;
}
//-------------------------------------------------------------------------------------------------
/** Send message */
void vSendMessage(QTcpSocket* poSocket, std::shared_ptr<MsgBase> poMessage)
{
  // Coding
  QByteArray oMessage(cuiMessageSize, 0);
  QDataStream oDataStream(&oMessage, QIODevice::WriteOnly);
  poMessage->vCoding(oDataStream);

  // Sending
  poSocket->write(oMessage);
  poSocket->waitForBytesWritten(0);
}
//-------------------------------------------------------------------------------------------------
#endif // UTILS_H
