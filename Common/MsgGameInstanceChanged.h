#ifndef MSGGAMEINSTANCE_H
#define MSGGAMEINSTANCE_H
//-------------------------------------------------------------------------------------------------
#include "MsgBase.h"
//-------------------------------------------------------------------------------------------------
class MsgGameInstanceChanged: public MsgBase
{
public:

  /** Constructor */
  MsgGameInstanceChanged() {
    _enMessageType = enMTGameInstanceChanged;
  }

  /** Coding */
  void vCoding(QDataStream& roDataStream) override {
    roDataStream << _enMessageType
                 << _enCmd
                 << _enFigure
                 << _bFirstTurn
                 << _uiClientID
                 << _uiOpponentID;
  }

  /** Decoding */
  void vDecoding(QDataStream& roDataStream) override {
    roDataStream >> _enMessageType
                 >> _enCmd
                 >> _enFigure
                 >> _bFirstTurn
                 >> _uiClientID
                 >> _uiOpponentID;
  }

  EnGameInstanceCommand _enCmd;
  Figure::EnFigure _enFigure;
  bool _bFirstTurn;
  quint16 _uiClientID;
  quint16 _uiOpponentID;
};
//-------------------------------------------------------------------------------------------------
#endif // MSGGAMEINSTANCE_H
