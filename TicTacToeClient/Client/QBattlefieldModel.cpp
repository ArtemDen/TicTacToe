#include "QBattlefieldModel.h"
//-------------------------------------------------------------------------------------------------
QBattlefieldModel::QBattlefieldModel(QObject *parent) : QAbstractTableModel(parent)
{
  clear();
}
//-------------------------------------------------------------------------------------------------
int QBattlefieldModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent)
  return ciBattlefieldDim;
}
//-------------------------------------------------------------------------------------------------
int QBattlefieldModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent)
  return ciBattlefieldDim;
}
//-------------------------------------------------------------------------------------------------
QVariant QBattlefieldModel::data(const QModelIndex& index, int role) const
{
  const UBattlefieldCell& croBattlefieldCell = _arrField[index.row()][index.column()];
  if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {
    return QVariant(croBattlefieldCell._enFigure);
  }
  else if (role == Qt::DecorationRole) {
    return QVariant(croBattlefieldCell._bSublight);
  }
  return QVariant();
}
//-------------------------------------------------------------------------------------------------
bool QBattlefieldModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  UBattlefieldCell& roCell = _arrField[index.row()][index.column()];
  if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {
    roCell._enFigure = static_cast<Figure::EnFigure>(value.toInt());
    emit dataChanged(index, index);
    return true;
  }
  else if (role == Qt::DecorationRole) {
    roCell._bSublight = value.toBool();
    emit dataChanged(index, index);
    return true;
  }
  return false;
}
//-------------------------------------------------------------------------------------------------
Qt::ItemFlags QBattlefieldModel::flags(const QModelIndex& index) const
{
  Q_UNUSED(index)
  return Qt::ItemIsEnabled;
}
//-------------------------------------------------------------------------------------------------
void QBattlefieldModel::clear()
{
  // Empty field
  for (int iRow = 0; iRow < ciBattlefieldDim; iRow++) {
    for (int iCol = 0; iCol < ciBattlefieldDim; iCol++) {
      _arrField[iRow][iCol] = UBattlefieldCell();
    }
  }
}
//-------------------------------------------------------------------------------------------------
