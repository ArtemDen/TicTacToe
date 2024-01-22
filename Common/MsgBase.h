#ifndef MSGBASE_H
#define MSGBASE_H
//-------------------------------------------------------------------------------------------------
#include <QDataStream>

#include "Const.h"
//-------------------------------------------------------------------------------------------------
class MsgBase
{
public:

  /** Constructor */
  MsgBase() {
    _enMessageType = enMTNone;
  }

  /** Destructor */
  virtual ~MsgBase() {}

  /** Coding */
  virtual void vCoding(QDataStream& roDataStream) {
    roDataStream << _enMessageType;
  }

  /** Decoding */
  virtual void vDecoding(QDataStream& roDataStream) {
    roDataStream >> _enMessageType;
  }

  EnMessageType _enMessageType;
};
//-------------------------------------------------------------------------------------------------
#endif // MSGBASE_H
