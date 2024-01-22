#include "Worker.h"

#include <thread>

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "Utils.h"
//-------------------------------------------------------------------------------------------------
Worker::Worker():
  _bStopSign(false),
  _oAddress()
{
  _mapMsgProcessingFunc[enMTIdentifier]           = &Worker::vMsgIdentifierProcessing;
  _mapMsgProcessingFunc[enMTGameInstanceChanged]  = &Worker::vMsgGameInstanceChangedProcessing;
  _mapMsgProcessingFunc[enMTGameTurn]             = &Worker::vMsgGameTurnProcessing;

  vLoadConfig();
}
//-------------------------------------------------------------------------------------------------
Worker::~Worker()
{
  vStop();
}
//-------------------------------------------------------------------------------------------------
void Worker::vStart()
{
  std::thread thread(&Worker::vWork, this);
  thread.detach();
}
//-------------------------------------------------------------------------------------------------
void Worker::vChangeState(quint16 uiState)
{
  _oClientData._enState = static_cast<ClientState::EnClientState>(uiState);
  std::shared_ptr<MsgStateChanged> poMessage = std::make_shared<MsgStateChanged>();
  poMessage->_uiID = _oClientData._uiID;
  poMessage->_enState = _oClientData._enState;
  vAddMessageToQueue(poMessage);
}
//-------------------------------------------------------------------------------------------------
void Worker::vMakeTurn(quint16 uiRow, quint16 uiColumn)
{
  std::shared_ptr<MsgGameTurn> poMessage = std::make_shared<MsgGameTurn>();
  poMessage->_uiSenderID     = _oClientData._uiID;
  poMessage->_uiReceiverID   = _oClientData._uiOpponentID;
  poMessage->_enSenderFigure = _oClientData._enFigure;
  poMessage->_uiRow          = uiRow;
  poMessage->_uiColumn       = uiColumn;
  vAddMessageToQueue(poMessage);

  _oClientData._bCanTurn = false;
}
//-------------------------------------------------------------------------------------------------
void Worker::vUpdateServerAddress(const QString &crstrAddress)
{
  // Lock for updating
  _bAddressUpdateRequired.store(true);
  std::unique_lock<std::mutex> oLock(_oAddressMutex);

  if (_oClientData._poSocket->state() != QAbstractSocket::UnconnectedState) {
    _oClientData._poSocket->disconnectFromHost();
    _oClientData._poSocket->waitForDisconnected();
  }
  _oAddress.setAddress(crstrAddress);

  _bAddressUpdateRequired.store(false);
  _oCondition.notify_all();

  vSaveConfig();
}
//-------------------------------------------------------------------------------------------------
void Worker::vWork()
{
  // Create socket object
  _oClientData._poSocket = new QTcpSocket();

  while (!_bStopSign.load()) {

    // Waiting for update
    std::unique_lock<std::mutex> oAddressLock(_oAddressMutex);
    if (_bAddressUpdateRequired.load()) {
      _oCondition.wait(oAddressLock);
    }

    // Connection
    if (_oClientData._poSocket->state() == QAbstractSocket::UnconnectedState) {

      if (_oClientData._enState == ClientState::Playing) {
        emit sigStopGame();
      }
      _oClientData._enState = ClientState::Default;

      // Reconnection
      _oClientData._poSocket->connectToHost(_oAddress, cuiPort);
      emit sigConnecting();

      if (_oClientData._poSocket->waitForConnected()) {
        emit sigConnected();
      }
      else {
        qDebug() << "Is not connected... fucking fuck...";
        continue;
      }
    }

    // Receive message
    if (_oClientData._poSocket->waitForReadyRead(0)) {
       QByteArray oMessage(cuiMessageSize, 0);
       if (bReadMessage(_oClientData._poSocket, oMessage)) {
         std::shared_ptr<MsgBase> poMessage = poDecodeMessage(oMessage);
         if (poMessage) {
           auto itMsgProcessingFunc = _mapMsgProcessingFunc.find(poMessage->_enMessageType);
           if (itMsgProcessingFunc != _mapMsgProcessingFunc.end()) {
             itMsgProcessingFunc->second(this, poMessage);
           }
         }
       }
    }

    // Send message
    std::unique_lock<std::mutex> oLock(_oQueueMutex);
    if (_queMsgSendQueue.size()) {
      vSendMessage(_oClientData._poSocket, _queMsgSendQueue.back());
      _queMsgSendQueue.pop();
    }

  }

  // Delete socket
  if (_oClientData._poSocket && (_oClientData._poSocket->state() != QAbstractSocket::UnconnectedState)) {
    _oClientData._poSocket->disconnectFromHost();
    _oClientData._poSocket->waitForDisconnected();
  }
  delete _oClientData._poSocket;
}
//-------------------------------------------------------------------------------------------------
void Worker::vCheckGameOver()
{
  // Row check
  for (int iRow = 0; iRow < ciBattlefieldDim; iRow++) {
    unsigned int uiFigure = _oBattlefieldModel.data(_oBattlefieldModel.index(iRow, 0)).toUInt();
    bool bSuccess = uiFigure != Figure::None;
    for (int iCol = 1; iCol < ciBattlefieldDim; iCol++) {
      bSuccess &= (uiFigure == _oBattlefieldModel.data(_oBattlefieldModel.index(iRow, iCol)).toUInt());
    }
    if (bSuccess) {
      // Sublight row by decoration role
      for (int iCol = 0; iCol < ciBattlefieldDim; iCol++) {
        _oBattlefieldModel.setData(_oBattlefieldModel.index(iRow, iCol), QVariant(true), Qt::DecorationRole);
      }
      emit sigGameOver(uiFigure);
      return;
    }
  }

  // Column check
  for (int iCol = 0; iCol < ciBattlefieldDim; iCol++) {
    unsigned int uiFigure = _oBattlefieldModel.data(_oBattlefieldModel.index(0,iCol)).toUInt();
    bool bSuccess = uiFigure != Figure::None;
    for (int iRow = 1; iRow < ciBattlefieldDim; iRow++) {
      bSuccess &= (uiFigure == _oBattlefieldModel.data(_oBattlefieldModel.index(iRow, iCol)).toUInt());
    }
    if (bSuccess) {
      // Sublight column by decoration role
      for (int iRow = 0; iRow < ciBattlefieldDim; iRow++) {
        _oBattlefieldModel.setData(_oBattlefieldModel.index(iRow, iCol), QVariant(true), Qt::DecorationRole);
      }
      emit sigGameOver(uiFigure);
      return;
    }
  }

  // First diagonal check
  {
    unsigned int uiFigure = _oBattlefieldModel.data(_oBattlefieldModel.index(0, 0)).toUInt();
    bool bSuccessDir = uiFigure != Figure::None;
    for (int iCell = 1; iCell < ciBattlefieldDim; iCell++) {
      bSuccessDir &= (uiFigure == _oBattlefieldModel.data(_oBattlefieldModel.index(iCell, iCell)).toUInt());
    }
    if (bSuccessDir) {
      // Sublight first diagonal by decoration role
      for (int iCell = 0; iCell < ciBattlefieldDim; iCell++) {
        _oBattlefieldModel.setData(_oBattlefieldModel.index(iCell, iCell), QVariant(true), Qt::DecorationRole);
      }
      emit sigGameOver(uiFigure);
      return;
    }
  }

  // Second diagonal check
  {
    unsigned int uiFigure = _oBattlefieldModel.data(_oBattlefieldModel.index(0, ciBattlefieldDim - 1)).toUInt();
    bool bSuccessRev = uiFigure != Figure::None;
    for (int iCell = 1; iCell < ciBattlefieldDim; iCell++) {
      bSuccessRev &= (uiFigure == _oBattlefieldModel.data(_oBattlefieldModel.index(iCell, ciBattlefieldDim - 1 - iCell)).toUInt());
    }
    if (bSuccessRev) {
      // Sublight second diagonal by decoration role
      for (int iCell = 0; iCell < ciBattlefieldDim; iCell++) {
        _oBattlefieldModel.setData(_oBattlefieldModel.index(iCell, ciBattlefieldDim - 1 - iCell), QVariant(true), Qt::DecorationRole);
      }
      emit sigGameOver(uiFigure);
      return;
    }
  }

  // Draw check
  bool bDraw = true;
  for (int iRow = 0; iRow < ciBattlefieldDim; iRow++) {
    for (int iCol = 0; iCol < ciBattlefieldDim; iCol++) {
      bDraw &= (_oBattlefieldModel.data(_oBattlefieldModel.index(iRow, iCol)).toUInt() != Figure::None);
    }
  }
  if (bDraw) {
    emit sigGameOver(Figure::None);
  }
}
//-------------------------------------------------------------------------------------------------
void Worker::vLoadConfig()
{
  // Load configuration file
  QFile oFile(QDir::currentPath() + "/config.json");
  if(oFile.open(QIODevice::ReadOnly)){

    // Read file
    QByteArray oByteArray = oFile.readAll();
    oFile.close();

    // JSON parsing
    QJsonDocument oJsonDocument(QJsonDocument::fromJson(oByteArray));
    QJsonObject oJsonObject = oJsonDocument.object();

    _oAddress = QHostAddress(oJsonObject.value("server_ip").toString());
  }
}
//------------------------------------------------------------------------------------------------
void Worker::vSaveConfig()
{
  // Save configuration file
  QFile oFile(QDir::currentPath() + "/config.json");
  if(oFile.open(QIODevice::WriteOnly)){

    // Object with values
    QJsonObject oObject;
    oObject.insert("server_ip", _oAddress.toString());

    // JSON doc
    QJsonDocument oJsonDocument;
    oJsonDocument.setObject(oObject);

    oFile.write(oJsonDocument.toJson());
    oFile.close();
  }
}
//------------------------------------------------------------------------------------------------
void Worker::vMsgGameTurnProcessing(std::shared_ptr<MsgBase> poMessage)
{
  std::shared_ptr<MsgGameTurn> poMsgGameTurn = std::dynamic_pointer_cast<MsgGameTurn>(poMessage);
  if (poMsgGameTurn) {
    if (!_oClientData._bCanTurn && (poMsgGameTurn->_uiReceiverID == _oClientData._uiID)) {
      _oClientData._bCanTurn = true;
      _oBattlefieldModel.setData(_oBattlefieldModel.index(poMsgGameTurn->_uiRow, poMsgGameTurn->_uiColumn), QVariant(poMsgGameTurn->_enSenderFigure));
      vCheckGameOver();
    }
  }
}
//------------------------------------------------------------------------------------------------
void Worker::vMsgGameInstanceChangedProcessing(std::shared_ptr<MsgBase> poMessage)
{
  std::shared_ptr<MsgGameInstanceChanged> poMsgGameInstanceChanged = std::dynamic_pointer_cast<MsgGameInstanceChanged>(poMessage);
  if (poMsgGameInstanceChanged && (poMsgGameInstanceChanged->_uiClientID == _oClientData._uiID)) {

    // Data initialization
    _oClientData._enFigure = poMsgGameInstanceChanged->_enFigure;
    _oClientData._uiOpponentID = poMsgGameInstanceChanged->_uiOpponentID;
    _oClientData._bCanTurn = poMsgGameInstanceChanged->_bFirstTurn;

    // Command processing
    if ((poMsgGameInstanceChanged->_enCmd == enGICStart) && (_oClientData._enState == ClientState::Searching)) {
      emit sigStartGame();
      _oClientData._enState = ClientState::Playing;
    }
    else if ((poMsgGameInstanceChanged->_enCmd == enGICStop) && (_oClientData._enState == ClientState::Playing)) {
      emit sigStopGame();
      _oClientData._enState = ClientState::Default;
    }
  }
}
//------------------------------------------------------------------------------------------------
void Worker::vMsgIdentifierProcessing(std::shared_ptr<MsgBase> poMessage)
{
  std::shared_ptr<MsgIdentifier> poMsgIdentifier = std::dynamic_pointer_cast<MsgIdentifier>(poMessage);
  if (poMsgIdentifier) {
    _oClientData._uiID = poMsgIdentifier->_uiID;
    emit sigID(_oClientData._uiID);
  }
}
//-------------------------------------------------------------------------------------------------
