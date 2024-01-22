#ifndef CLIENTDATA_H
#define CLIENTDATA_H
//-------------------------------------------------------------------------------------------------
#include "Const.h"

class QTcpSocket;
//-------------------------------------------------------------------------------------------------
/** Client data */
struct ClientData {

  /** Default constructor */
  ClientData():
    _poSocket(nullptr),
    _enState(ClientState::Default),
    _enFigure(Figure::None),
    _uiID(0),
    _uiOpponentID(0),
    _bCanTurn(false)
  {}

  /** Constructor with socket */
  ClientData(QTcpSocket* poSocket):
    _poSocket(poSocket),
    _enState(ClientState::Default),
    _enFigure(Figure::None),
    _uiID(0),
    _uiOpponentID(0),
    _bCanTurn(false)
  {}

  /** Client socket */
  QTcpSocket* _poSocket;

  /** Client state */
  ClientState::EnClientState _enState;

  /** Client figure */
  Figure::EnFigure _enFigure;

  /** Client ID */
  quint16 _uiID;

  /** Opponent ID */
  quint16 _uiOpponentID;

  /** Can make turn */
  bool _bCanTurn;
};
//-------------------------------------------------------------------------------------------------
#endif // CLIENTDATA_H
