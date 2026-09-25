#ifndef APPLICATIONENHANCEMENTS_H
#define APPLICATIONENHANCEMENTS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "QCommonGlobal.h"
#include "RecentFiles.h"

// QT
class QMdiSubWindow;
class QMenu;
#include <QSettings>
#include <QString>
class QTableWidget;
class QWidget;

QString QCOMMON_EXPORT createFilenameTimeStamp();

void QCOMMON_EXPORT adjustMdiSubWindow(QWidget* childWindow, QMdiSubWindow* subMdiWindow);
void QCOMMON_EXPORT buildRecentsMenu(QMenu* recentsMenu, RecentFiles& recentFiles, const QObject* receiver, const char* member);
void QCOMMON_EXPORT setupApplicationStyle();

void QCOMMON_EXPORT resizeTableColumns(QTableWidget* setMeUp);
void QCOMMON_EXPORT startLocalBrowser(const QString& filePath);
void launchFolder(const QString &folderName);

QString QCOMMON_EXPORT docsRoot();

// Resolve a documentation page by its descriptive stem, e.g.
// docPage("getting-started", "EPM-Scope"). The "NN-" ordering prefix and the
// file extension are both ignored, because each product (standalone QEPM,
// standalone QTAC, bundled Alpaca) numbers its documentation set
// independently and may ship either built .html or the .md sources. Prefer
// this over concatenating a hardcoded filename onto docsRoot().
QString QCOMMON_EXPORT docPage(const QString& section, const QString& stem);

#endif
