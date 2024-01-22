#ifndef MSGSTATECHANGED_H
#define MSGSTATECHANGED_H
//-------------------------------------------------------------------------------------------------
#include "MsgBase.h"
//-------------------------------------------------------------------------------------------------
class MsgStateChanged: public MsgBase
{
public:

  /** Constructor */
  MsgStateChanged() {
    _enMessageType = enMTStateChanged;
  }

  /** Coding */
  void vCoding(QDataStream& roDataStream) override {
    roDataStream << _enMessageType
                 << _uiID
                 << _enState;
  }

  /** Decoding */
  void vDecoding(QDataStream& roDataStream) override {
    roDataStream >> _enMessageType
                 >> _uiID
                 >> _enState;
  }

  quint16 _uiID;
  ClientState::EnClientState _enState;
};
//-------------------------------------------------------------------------------------------------
#endif // MSGSTATECHANGED_H
