#include "Worker.h"
//-------------------------------------------------------------------------------------------------
#include <QTcpSocket>
#include <QDataStream>
#include <QRandomGenerator>

#include <iostream>

#include "Utils.h"
//-------------------------------------------------------------------------------------------------
quint16 Worker::_suiClientsCount = 0;
//-------------------------------------------------------------------------------------------------
Worker::Worker():
  _poServer(nullptr),
  _bExitSign(false)
{
  _mapUserCommandsFunc[enSCHelp]  = &Worker::vHelp;
  _mapUserCommandsFunc[enSCStart] = &Worker::vOpenConnection;
  _mapUserCommandsFunc[enSCStop]  = &Worker::vCloseConnection;
  _mapUserCommandsFunc[enSCExit]  = &Worker::vExit;

  _mapClientStatesFunc[ClientState::Default]   = &Worker::vDefaultClientStateProcessing;
  _mapClientStatesFunc[ClientState::Searching] = &Worker::vSearchingClientStateProcessing;
  _mapClientStatesFunc[ClientState::Playing]   = &Worker::vPlayingClientStateProcessing;

  _mapMsgProcessingFunc[enMTStateChanged] = &Worker::vMsgStateChangedProcessing;
  _mapMsgProcessingFunc[enMTGameTurn]     = &Worker::vMsgGameTurnProcessing;

  vAddCommandToQueue(enSCStart);
}
//-------------------------------------------------------------------------------------------------
Worker::~Worker()
{
  vAddCommandToQueue(enSCExit);
}
//-------------------------------------------------------------------------------------------------
void Worker::vWork()
{
  // Create server object
  _poServer = new QTcpServer();

  while (!_bExitSign) {

    // User command logic
    {
      std::unique_lock<std::mutex> oLock(_oUserCommandMutex);
      if (_queUserCommands.size()) {
        auto itUserCmd = _mapUserCommandsFunc.find(_queUserCommands.front());
        if (itUserCmd != _mapUserCommandsFunc.end()) {
          itUserCmd->second(this);
        }
        _queUserCommands.pop();
      }
    }

    if (!_poServer->isListening()) {
      continue;
    }

    // Incoming connection
    if (_poServer->waitForNewConnection()) {

      QTcpSocket* poSocket = _poServer->nextPendingConnection();
      vIdentifyClient(poSocket);

      std::cout << "Connected: " << poSocket->peerAddress().toString().toStdString()
                << " Port: "     << poSocket->peerPort()
                << " ID: "       << _suiClientsCount << std::endl;

      _mapClients.emplace(_suiClientsCount, ClientData(poSocket));
    }

    // Sockets processing
    for (std::map<quint16, ClientData>::iterator itClientData = _mapClients.begin(); itClientData != _mapClients.end();) {

      QTcpSocket* poSocket = itClientData->second._poSocket;

      // Erase closed conncetions
      if (!poSocket || (poSocket->state() == QAbstractSocket::UnconnectedState)) {
        std::cout << "Disconnected: ID: " << itClientData->first << std::endl;
        itClientData = _mapClients.erase(itClientData);
        continue;
      }

      // Receive message
      if (poSocket->waitForReadyRead(0)) {
        QByteArray oMessage(cuiMessageSize, 0);
        if (bReadMessage(poSocket, oMessage)) {
          std::shared_ptr<MsgBase> poMessage = poDecodeMessage(oMessage);
          if (poMessage) {
            std::cout << "Message Received: Type: " << poMessage->_enMessageType << std::endl;
            auto itMsgProcessingFunc = _mapMsgProcessingFunc.find(poMessage->_enMessageType);
            if (itMsgProcessingFunc != _mapMsgProcessingFunc.end()) {
              itMsgProcessingFunc->second(this, poMessage);
            }
          }
        }
      }

      // Client states processing
      auto itClientStateProcessing = _mapClientStatesFunc.find(itClientData->second._enState);
      if (itClientStateProcessing != _mapClientStatesFunc.end()) {
        itClientStateProcessing->second(this, itClientData->first, itClientData->second);
      }

      itClientData++;
    }

    // Sending messages
    std::unique_lock<std::mutex> oLock(_oSendingMessageMutex);
    while (_queSendingMessages.size()) {
      auto oSendingData = _queSendingMessages.front();
      vSendMessage(oSendingData.first, oSendingData.second);
      _queSendingMessages.pop();
    }
  }

  delete _poServer;
  std::cout << "Application has stopped work, press Enter to close window" << std::endl;
}
//-------------------------------------------------------------------------------------------------
void Worker::vOpenConnection()
{
  if (!_poServer->isListening()) {
    _poServer->listen(QHostAddress::AnyIPv4, cuiPort);
    std::cout << "Server started" << std::endl;
  }
}
//-------------------------------------------------------------------------------------------------
void Worker::vCloseConnection()
{
  // Close connections
  for (auto oClientData: _mapClients) {
    oClientData.second._poSocket->disconnectFromHost();
    oClientData.second._poSocket->waitForDisconnected();
  }
  _mapClients.clear();

  // Stop listenning
  if (_poServer->isListening()) {
    _poServer->close();
    std::cout << "Server closed" << std::endl;
  }
}
//-------------------------------------------------------------------------------------------------
void Worker::vDefaultClientStateProcessing(quint16 uiID, ClientData& roClientData)
{
  Q_UNUSED(uiID)
  auto itOpponentData = _mapClients.find(roClientData._uiOpponentID);
  if (itOpponentData != _mapClients.end()) {
    if (itOpponentData->second._enState == ClientState::Playing) {
      // Stop play for opponent
      itOpponentData->second._uiOpponentID = 0;
      itOpponentData->second._enState = ClientState::Default;
      itOpponentData->second._bCanTurn = false;
      vCreateGameInstanceMsg(itOpponentData->second._poSocket, enGICStop, itOpponentData->first, 0);
    }
    roClientData._uiOpponentID = 0;
  }
}
//-------------------------------------------------------------------------------------------------
void Worker::vSearchingClientStateProcessing(quint16 uiID, ClientData& roClientData)
{
  // Start play for clients
  for (std::map<quint16, ClientData>::iterator itOpponentData = _mapClients.begin(); itOpponentData != _mapClients.end(); itOpponentData++) {
    if ((itOpponentData->first != uiID) && (itOpponentData->second._enState == ClientState::Searching)) {

      roClientData._enState = ClientState::Playing;
      roClientData._uiOpponentID = itOpponentData->first;
      roClientData._bCanTurn = QRandomGenerator::global()->bounded(2); // Randomize first player

      itOpponentData->second._enState = ClientState::Playing;
      itOpponentData->second._uiOpponentID = uiID;
      itOpponentData->second._bCanTurn = !roClientData._bCanTurn;

      vCreateGameInstanceMsg(roClientData._poSocket, enGICStart, uiID, itOpponentData->first, roClientData._bCanTurn);
      vCreateGameInstanceMsg(itOpponentData->second._poSocket, enGICStart, itOpponentData->first, uiID, itOpponentData->second._bCanTurn);
    }
  }
}
//-------------------------------------------------------------------------------------------------
void Worker::vPlayingClientStateProcessing(quint16 uiID, ClientData& roClientData)
{
  auto itOpponent = _mapClients.find(roClientData._uiOpponentID);
  if ((itOpponent == _mapClients.end()) || (itOpponent->second._enState != ClientState::Playing)) {
    // Stop play for client
    roClientData._enState = ClientState::Default;
    roClientData._uiOpponentID = 0;
    roClientData._bCanTurn = false;
    vCreateGameInstanceMsg(roClientData._poSocket, enGICStop, uiID, 0);
  }
}
//-------------------------------------------------------------------------------------------------
void Worker::vIdentifyClient(QTcpSocket* poSocket)
{
  // Create a reply with ID
  std::shared_ptr<MsgIdentifier> poMessage = std::make_shared<MsgIdentifier>();
  poMessage->_uiID = ++_suiClientsCount;
  vAddMessageToQueue(poSocket, poMessage);
}
//-------------------------------------------------------------------------------------------------
void Worker::vMsgStateChangedProcessing(std::shared_ptr<MsgBase> poMessage)
{
  std::shared_ptr<MsgStateChanged> poMsgStateChanged = std::dynamic_pointer_cast<MsgStateChanged>(poMessage);
  if (poMsgStateChanged) {
    auto itClientData = _mapClients.find(poMsgStateChanged->_uiID);
    if (itClientData != _mapClients.end()) {
      itClientData->second._enState = poMsgStateChanged->_enState;
      std::cout << "Player " << itClientData->first << " has entered state " << itClientData->second._enState  << std::endl;
    }
  }
}
//-------------------------------------------------------------------------------------------------
void Worker::vMsgGameTurnProcessing(std::shared_ptr<MsgBase> poMessage)
{
  std::shared_ptr<MsgGameTurn> poMsgGameTurn = std::dynamic_pointer_cast<MsgGameTurn>(poMessage);
  if (poMsgGameTurn) {

    auto itSenderData = _mapClients.find(poMsgGameTurn->_uiSenderID);
    if (itSenderData != _mapClients.end()) {
      itSenderData->second._bCanTurn = false;
    }

    auto itReceiverData = _mapClients.find(poMsgGameTurn->_uiReceiverID);
    if (itReceiverData != _mapClients.end()) {
      if ((itReceiverData->second._enState == ClientState::Playing) && (itReceiverData->second._uiOpponentID == poMsgGameTurn->_uiSenderID)) {
        itReceiverData->second._bCanTurn = true;
        vAddMessageToQueue(itReceiverData->second._poSocket, poMsgGameTurn);
      }
    }
  }
}
//-------------------------------------------------------------------------------------------------
void Worker::vHelp()
{
  std::cout << "help  -> Show command's list" << std::endl
            << "start -> Start server"        << std::endl
            << "stop  -> Stop server"         << std::endl
            << "exit  -> Exit"                << std::endl;
}
//-------------------------------------------------------------------------------------------------
void Worker::vCreateGameInstanceMsg(QTcpSocket* poSocket, EnGameInstanceCommand enCmd, quint16 uiClientID, quint16 uiOpponentID, bool bFirstTurn)
{
  if (poSocket) {
    std::shared_ptr<MsgGameInstanceChanged> poMessage = std::make_shared<MsgGameInstanceChanged>();
    poMessage->_enCmd = enCmd;
    poMessage->_enFigure = bFirstTurn ? Figure::Cross : Figure::Circle;
    poMessage->_bFirstTurn = bFirstTurn;
    poMessage->_uiClientID = uiClientID;
    poMessage->_uiOpponentID = uiOpponentID;
    vAddMessageToQueue(poSocket, poMessage);
  }
}
//-------------------------------------------------------------------------------------------------
