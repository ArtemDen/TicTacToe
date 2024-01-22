#ifndef MSGGAMETURN_H
#define MSGGAMETURN_H
//-------------------------------------------------------------------------------------------------
#include "MsgBase.h"
//-------------------------------------------------------------------------------------------------
class MsgGameTurn: public MsgBase
{
public:

  /** Constructor */
  MsgGameTurn() {
    _enMessageType = enMTGameTurn;
  }

  /** Coding */
  void vCoding(QDataStream& roDataStream) override {
    roDataStream << _enMessageType
                 << _uiSenderID
                 << _uiReceiverID
                 << _enSenderFigure
                 << _uiRow
                 << _uiColumn;
  }

  /** Decoding */
  void vDecoding(QDataStream& roDataStream) override {
    roDataStream >> _enMessageType
                 >> _uiSenderID
                 >> _uiReceiverID
                 >> _enSenderFigure
                 >> _uiRow
                 >> _uiColumn;
  }

  quint16 _uiSenderID;
  quint16 _uiReceiverID;
  Figure::EnFigure _enSenderFigure;
  quint16 _uiRow;
  quint16 _uiColumn;
};
//-------------------------------------------------------------------------------------------------
#endif // MSGGAMETURN_H
