// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef UDASBLOCKMODEL_H
#define UDASBLOCKMODEL_H

// libEPM
#include "UDASBlock.h"

// Qt
#include <QAbstractTableModel>

class UDASBlockModel :
	public QAbstractTableModel
{
	Q_OBJECT

public:
	explicit UDASBlockModel(QObject *parent = nullptr);

	void setModelData(UDASBlock udasBlock);

	// QAbstractTableModel
	Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
	QStringList					_columnHeaders;
	UDASBlock					_udasBlock;
};

#endif // UDASBLOCKMODEL_H
