// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	$Id: //depot/QDL/QCommon/CommListModel.cpp#3 $
	$Header: //depot/QDL/QCommon/CommListModel.cpp#3 $
	$Date: 2016/07/06 $
	$DateTime: 2016/07/06 12:52:24 $
	$Change: 889602 $
	$File: //depot/QDL/QCommon/CommListModel.cpp $
	$Revision: #3 $
	%Author: msimpson %
*/

#include "CommListModel.h"

// Qt
#include <QtCore/QSize>
#include <QtGui/QColor>

CommListModel::CommListModel
(
	QObject* parent // = NULL
) :
	QStandardItemModel(parent)
{
	QStringList headers;

	headers << tr("Port Number") << tr("Port Description") ;

	setHorizontalHeaderLabels(headers);

//	if (_deviceServer.isNull() == false)
//		Refresh();
}

void CommListModel::Refresh()
{
	removeRows(0, rowCount());

/*	SerialPorts serialPorts;

	serialPorts = _deviceServer->GetSerialPorts();

	if (serialPorts.empty() == false)
	{
		SerialPortIter serialPort = serialPorts.begin();
		while (serialPort != serialPorts.end())
		{	
			QStandardItem* standardItem;
		
			QList<QStandardItem*> standardItems;

			standardItem = new QStandardItem(serialPort->_portLocation);
			standardItem->setData(serialPort->_portLocation);
			standardItems.push_back(standardItem);

			standardItem = new QStandardItem(serialPort->_description);
			standardItems.push_back(standardItem);

			appendRow(standardItems);

			emit ModelUpdated();

			serialPort++;
		}
	}
*/
}
