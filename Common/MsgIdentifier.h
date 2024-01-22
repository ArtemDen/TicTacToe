#ifndef MSGIDENTIFIER_H
#define MSGIDENTIFIER_H
//-------------------------------------------------------------------------------------------------
#include "MsgBase.h"
//-------------------------------------------------------------------------------------------------
class MsgIdentifier: public MsgBase
{
public:

  /** Constructor */
  MsgIdentifier() {
    _enMessageType = enMTIdentifier;
    _uiID = 0;
  }

  /** Coding */
  void vCoding(QDataStream& roDataStream) override {
    roDataStream << _enMessageType
                 << _uiID;
  }

  /** Decoding */
  void vDecoding(QDataStream& roDataStream) override {
    roDataStream >> _enMessageType
                 >> _uiID;
  }

  /** Player ID */
  quint16 _uiID;
};
//-------------------------------------------------------------------------------------------------
#endif // MSGIDENTIFIER_H
