// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// Author: msimpson

#include "TACPsocProtocol.h"

// Alpaca Server
#include "TACCommands.h"
#include "TACPsocCommand.h"
#include "TACDriveThread.h"
#include "TACPsocCoder.h"

// QCommon
#include "TickCount.h"

TacPsocProtocol::TacPsocProtocol() :
	_tacDriveTrain(Q_NULLPTR),
	_tickCount(0)
{
	setFrameCoder(new TacPsocCoder);
}

TacPsocProtocol::~TacPsocProtocol()
{
	if (_frameCoder != Q_NULLPTR)
	{
		delete _frameCoder;
		_frameCoder = Q_NULLPTR;
	}
}

void TacPsocProtocol::setTacDriveTrain
(
	TACDriveThread* tacDriveTrain
)
{
	Q_ASSERT(tacDriveTrain != Q_NULLPTR);

	_tacDriveTrain = tacDriveTrain;
}

quint32 TacPsocProtocol::sendCommand
(
	const QByteArray& command,
	const Arguments& arguments,
	bool console,
	ReceiveInterface* receiveInterface,
	bool shouldStore
)
{
	quint32 result(kBadQueueValue);

	if (command.isEmpty() == false)
	{
		result = getNextSendID();

		FramePackage framePackage = FramePackage(new _FramePackage);

		framePackage->_packetID = result;
		framePackage->_request = command;
		framePackage->_arguments = arguments;
		framePackage->_requestHash = CommandStringToHash(command);
		framePackage->_console = console;
		framePackage->_shouldStore = shouldStore;
		framePackage->_recieveInterface = receiveInterface;
		if (_frameCoder == Q_NULLPTR)
			framePackage->_codedRequest = command;
		else
			framePackage->_codedRequest = _frameCoder->encode(command, arguments);

		framePackage->_tickcount = tickCount();

		ProtocolInterface::queueCommand(framePackage);
	}

	return result;
}

void TacPsocProtocol::endTransaction(ReceiveInterface* receiveInterface)
{
	ProtocolInterface::queueEndTransaction(receiveInterface);
}

void TacPsocProtocol::receive
(
	FramePackage& framePackage
)
{
	_tacDriveTrain->receive(framePackage);
}

void TacPsocProtocol::idle()
{
}

void TacPsocProtocol::frameComplete
(
	const QByteArray& completedFrame
)
{
	static QList<QByteArray> frames;

	if (completedFrame != QByteArray())
	{
		if (completedFrame.contains(kCommand) == false)
		{
			frames.push_back(completedFrame);
		}
	}
	else
	{
		FramePackage& framePackage = pendingFramePackage();
		if (framePackage.isNull() == false)
		{
			framePackage->_responses = frames;
			framePackage->_valid = frames.last().trimmed().compare("ok", Qt::CaseInsensitive) == 0;

			if (framePackage->_recieveInterface != Q_NULLPTR)
			{
				framePackage->_recieveInterface->receive(framePackage);
			}
			else
			{
				_tacDriveTrain->receive(framePackage);
			}
		}

		clearPendingFrame();

		frames.clear();
	}
}

void TacPsocProtocol::badFrame
(
	const QByteArray& completedFrame
)
{
	Q_UNUSED(completedFrame);
}

void TacPsocProtocol::triggerElapsed()
{
	_tickCount = 0;
}
