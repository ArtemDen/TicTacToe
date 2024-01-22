#ifndef QBATTLEFIELDMODEL_H
#define QBATTLEFIELDMODEL_H
//-------------------------------------------------------------------------------------------------
#include <QAbstractTableModel>

#include <array>

#include "Const.h"
//-------------------------------------------------------------------------------------------------
/** Field dimension */
const int ciBattlefieldDim = 3;
//-------------------------------------------------------------------------------------------------
/** Cell struct */
struct UBattlefieldCell
{
  /** Cell Figure */
  Figure::EnFigure _enFigure = Figure::None;

  /** Cell sublighted */
  bool _bSublight = false;
};
//-------------------------------------------------------------------------------------------------
class QBattlefieldModel : public QAbstractTableModel
{
  Q_OBJECT

public:

  /** Constructor */
  explicit QBattlefieldModel(QObject *parent = nullptr);

  /** Count functions */
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  /** Data access functions */
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

  /** Flags */
  Qt::ItemFlags flags(const QModelIndex &index) const override;

  /** Clear field */
  Q_INVOKABLE void clear();

private:

  /** Battlefield */
  UBattlefieldCell _arrField[ciBattlefieldDim][ciBattlefieldDim];

};
//-------------------------------------------------------------------------------------------------
#endif // QBATTLEFIELDMODEL_H
