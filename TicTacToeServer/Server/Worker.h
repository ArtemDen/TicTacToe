#ifndef TICTACTOESERVER_H
#define TICTACTOESERVER_H
//-------------------------------------------------------------------------------------------------
#include <QTcpServer>
#include <QTcpSocket>

#include <map>
#include <queue>
#include <mutex>

#include "MsgBase.h"
#include "ClientData.h"
//-------------------------------------------------------------------------------------------------
class Worker
{
public:

  /** Constructor */
  Worker();

  /** Destructor */
  ~Worker();

  /** Work function */
  void vWork();

  void vAddCommandToQueue(EnServerCommand enCmd) {
    std::unique_lock<std::mutex> oLock(_oUserCommandMutex);
    _queUserCommands.push(enCmd);
  }

  /** Add message to queue */
  void vAddMessageToQueue(QTcpSocket* poSocket, std::shared_ptr<MsgBase> poMessage) {
    std::unique_lock<std::mutex> oLock(_oSendingMessageMutex);
    _queSendingMessages.push(std::make_pair(poSocket, poMessage));
  }

private:

  /** Open connection and listen for incoming connections */
  void vOpenConnection();

  /** Close connection with sockets */
  void vCloseConnection();

  /** Stop work and exit */
  void vExit() {
    vCloseConnection();
    _bExitSign = true;
  }

  /** Processing client states */
  void vDefaultClientStateProcessing(quint16 uiID, ClientData& roClientData);
  void vSearchingClientStateProcessing(quint16 uiID, ClientData& roClientData);
  void vPlayingClientStateProcessing(quint16 uiID, ClientData& roClientData);

  /** Set ID for client */
  void vIdentifyClient(QTcpSocket* poSocket);

  /** Processing messages */
  void vMsgStateChangedProcessing(std::shared_ptr<MsgBase> poMessage);
  void vMsgGameTurnProcessing(std::shared_ptr<MsgBase> poMessage);

  /** Show help */
  void vHelp();

  /** Create Game Instance Message */
  void vCreateGameInstanceMsg(QTcpSocket* poSocket, EnGameInstanceCommand enCmd, quint16 uiClientID, quint16 uiOpponentID, bool bFirstTurn = true);

  /** User command function ptr's map */
  std::map<EnServerCommand, std::function<void(Worker*)>> _mapUserCommandsFunc;

  /** User command queue */
  std::queue<EnServerCommand> _queUserCommands;

  /** User command Mutex */
  std::mutex _oUserCommandMutex;

  /** Client states control function ptr's map */
  std::map<ClientState::EnClientState, std::function<void(Worker*, quint16, ClientData&)>> _mapClientStatesFunc;

  /** Server */
  QTcpServer* _poServer;

  /** Active connections */
  std::map<quint16, ClientData> _mapClients;

  /** Message processing function ptr's map */
  std::map<EnMessageType, std::function<void(Worker*, std::shared_ptr<MsgBase>)>> _mapMsgProcessingFunc;

  /** Message sending queue */
  std::queue<std::pair<QTcpSocket*, std::shared_ptr<MsgBase>>> _queSendingMessages;

  /** Message sending mutex */
  std::mutex _oSendingMessageMutex;

  /** Client's counter */
  static quint16 _suiClientsCount;

  /** Exit sign */
  bool _bExitSign;
};
//-------------------------------------------------------------------------------------------------
#endif // TICTACTOESERVER_H
