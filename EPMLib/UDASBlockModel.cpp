// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "UDASBlockModel.h"

const int kIndex(0);
const int kTimeStamp(1);
const int kYSeriesValue(2);

UDASBlockModel::UDASBlockModel(QObject *parent)
	: QAbstractTableModel(parent)
{
	_columnHeaders << tr("Index") << tr("Time Stamp") << tr("Y Value");
}

void UDASBlockModel::setModelData(UDASBlock udasBlock)
{
	_udasBlock = udasBlock;

	beginInsertRows(QModelIndex(), 0, _udasBlock->_ySeries.count() - 1);
	endInsertRows();
}

int UDASBlockModel::rowCount
(
	const QModelIndex& parent
) const
{
	Q_UNUSED(parent);

	if (_udasBlock.isNull())
		return 0;

	return _udasBlock->_ySeries.count();
}

int UDASBlockModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return _columnHeaders.count();
}

QVariant UDASBlockModel::headerData
(
	int section,
	Qt::Orientation orientation,
	int role
) const
{
	Q_UNUSED(orientation);

	switch (role)
	{
	case Qt::DisplayRole:
		return _columnHeaders.at(section);

	default:
		break;
	}

	return QVariant();
}

QVariant UDASBlockModel::data
(
	const QModelIndex &index,
	int role
) const
{
	if (!index.isValid() | _udasBlock.isNull())
		return QVariant();

	switch (role)
	{
	case Qt::DisplayRole:
		switch (index.column())
		{
		case kIndex: return QString::number(index.row());
		case kTimeStamp:
		{
			return QString::number(index.row() * _udasBlock->_timeBetweenSamples);
		}
		case kYSeriesValue:
			return QString::number(_udasBlock->_ySeries[index.row()]);

		default:
			return QString("-");
		}

	case Qt::DecorationRole:

		switch (index.column())
		{
		case kIndex: return QString::number(index.row());
		case kTimeStamp:
		{
			return QString::number(index.row() * _udasBlock->_timeBetweenSamples);
		}
		case kYSeriesValue:
			return QString::number(_udasBlock->_ySeries[index.row()]);

		default:
			return QString("-");
		}

		break;

	default:
		break;
	}

	return QVariant();
}
