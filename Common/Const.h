#ifndef CONST_H
#define CONST_H
//-------------------------------------------------------------------------------------------------
#include <QObject>

#include <map>
//-------------------------------------------------------------------------------------------------
/** Server Port */
const quint16 cuiPort = 3333;

/** Message size */
const quint16 cuiMessageSize = 64;

/** Message types enumerator */
enum EnMessageType
{
  enMTNone,
  enMTIdentifier,
  enMTStateChanged,
  enMTUserInfo,
  enMTGameInstanceChanged,
  enMTGameTurn
};

/** Client states enumerator (available in QML) */
class ClientState {
  Q_GADGET
public:
  explicit ClientState(){};
  enum EnClientState
  {
    Default,
    Searching,
    Playing
  };
  Q_ENUM(EnClientState)
};

/** Figures enumerator (available in QML) */
class Figure {
  Q_GADGET
public:
  explicit Figure(){};
  enum EnFigure
  {
    None,
    Circle,
    Cross
  };
  Q_ENUM(EnFigure)
};

/** Game instance commands enumerator */
enum EnGameInstanceCommand
{
  enGICStart,
  enGICStop
};

/** Server commands enumerator */
enum EnServerCommand
{
  enSCNone,
  enSCHelp,
  enSCStart,
  enSCStop,
  enSCExit
};

/** Server commads list */
const std::map<std::string, EnServerCommand> cmapCommands {
  std::pair<std::string, EnServerCommand>("help",  enSCHelp),
  std::pair<std::string, EnServerCommand>("start", enSCStart),
  std::pair<std::string, EnServerCommand>("stop",  enSCStop),
  std::pair<std::string, EnServerCommand>("exit",  enSCExit)
};
//-------------------------------------------------------------------------------------------------
#endif // CONST_H
