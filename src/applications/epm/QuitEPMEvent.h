#ifndef QUITEPMEVENT_H
#define QUITEPMEVENT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <QEvent>

const QEvent::Type kQuitEPMEvent(static_cast<QEvent::Type>(QEvent::User + 1));

class QuitEPMEvent :
	public QEvent
{
public:
	QuitEPMEvent();
};

#endif // QUITEPMEVENT_H
