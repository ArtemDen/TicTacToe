#ifndef TICTACTOECLIENT_H
#define TICTACTOECLIENT_H
//-------------------------------------------------------------------------------------------------
#include <QTcpSocket>
#include <QHostAddress>

#include <queue>
#include <map>
#include <mutex>
#include <condition_variable>

#include "MsgBase.h"
#include "ClientData.h"
#include "QBattlefieldModel.h"
//-------------------------------------------------------------------------------------------------
class Worker: public QObject
{
  Q_OBJECT

public:

  /** Constructor */
  Worker();

  /** Destructor */
  ~Worker();

  /** Start Work */
  Q_INVOKABLE void vStart();

  /** Change client state */
  Q_INVOKABLE void vChangeState(quint16 uiState);

  /** Get model data pointer */
  Q_INVOKABLE QBattlefieldModel* poGetModel() {
    return &_oBattlefieldModel;
  }

  /** Get opponent ID */
  Q_INVOKABLE quint16 uiGetOpponentID() {
    return _oClientData._uiOpponentID;
  }

  Q_INVOKABLE QString crstrGetServerAddress() {
    return _oAddress.toString();
  }

  /** Get turn permission */
  Q_INVOKABLE bool bCanTurn() {
    return _oClientData._bCanTurn;
  }

  /** Get client figure */
  Q_INVOKABLE quint16 uiGetFigure() {
    return _oClientData._enFigure;
  }

  /** Make Turn */
  Q_INVOKABLE void vMakeTurn(quint16 uiRow, quint16 uiColumn);

  /** Change server IP */
  Q_INVOKABLE void vUpdateServerAddress(const QString& crstrAddress);


  /** Work function */
  void vWork();

  /** Add message to queue */
  void vAddMessageToQueue(std::shared_ptr<MsgBase> poMessage) {
    std::unique_lock<std::mutex> oLock(_oQueueMutex);
    _queMsgSendQueue.push(poMessage);
  }

  /** Stop work */
  void vStop() {
    _bStopSign.store(true);
  }

  /** Check end game */
  Q_INVOKABLE void vCheckGameOver();

signals:

  void sigConnecting();
  void sigConnected();
  void sigID(quint16 ID);
  void sigStartGame();
  void sigStopGame();
  void sigGameOver(unsigned int uiFigure);

private:

  /** Network configuration file */
  void vLoadConfig();
  void vSaveConfig();

  /** Processing messages */
  void vMsgGameTurnProcessing(std::shared_ptr<MsgBase> poMessage);
  void vMsgGameInstanceChangedProcessing(std::shared_ptr<MsgBase> poMessage);
  void vMsgIdentifierProcessing(std::shared_ptr<MsgBase> poMessage);

  /** Message processing function ptr's map */
  std::map<EnMessageType, std::function<void(Worker*, std::shared_ptr<MsgBase>)>> _mapMsgProcessingFunc;

  /** Messages for sending */
  std::queue<std::shared_ptr<MsgBase>> _queMsgSendQueue;

  /** Stop sign */
  std::atomic<bool> _bStopSign;

  /** Sending queue utex */
  std::mutex _oQueueMutex;

  /** Address updating control */
  std::atomic<bool> _bAddressUpdateRequired;
  std::mutex _oAddressMutex;
  std::condition_variable _oCondition;

  /** Server's IP */
  QHostAddress _oAddress;

  /** Client data */
  ClientData _oClientData;

  /** Battlefield model */
  QBattlefieldModel _oBattlefieldModel;
};
//-------------------------------------------------------------------------------------------------
#endif // TICTACTOECLIENT_H
