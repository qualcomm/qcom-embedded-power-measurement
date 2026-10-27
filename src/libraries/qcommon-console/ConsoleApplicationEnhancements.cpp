// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause


#include "ConsoleApplicationEnhancements.h"
#include "AppCore.h"
#include "Range.h"  
#include "PreferencesBase.h" 
#include "AlpacaSettings.h"

// Qt
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>

#ifdef Q_OS_WINDOWS
	#include <windows.h>
	#include <shellapi.h>
	#include <shlobj.h>
	#pragma comment(lib, "shell32")
#endif
#ifdef Q_OS_LINUX
	#include <pwd.h>
	#include <unistd.h>
#endif


// Returns the folder holding the running executable, with a trailing
// separator so callers can append a program name.
//
// The binaries are installed in a different folder for each product layout,
// so a fixed location is always wrong for one of them. The executable folder
// is right for all of them. The old version also created the fixed folder,
// which a normal user cannot do under Program Files or /opt.
QString applicationBinPath()
{
	// applicationDirPath() needs a QCoreApplication. EPMDev can be loaded by
	// hosts that have none.
	if (QCoreApplication::instance() == Q_NULLPTR)
	{
		qWarning() << "applicationBinPath: no application object, executable folder unknown";
		return QString();
	}

	return QDir::cleanPath(QCoreApplication::applicationDirPath()) + QDir::separator();
}

QString documentsDataPath
(
	const QString& append
)
{
	QString result;
	QString appName;

	appName = QCoreApplication::applicationName();
	if (appName.isEmpty())
		appName = "QEPM";

#ifdef Q_OS_WIN
	// QStandardPaths would return the "One Drive" location. CSV documents don't like living here
	result = QDir::homePath() + QDir::separator() + "Documents";
#else
	result = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#endif

	result = QDir::cleanPath(result + QDir::separator() + appName);
	if (append.isEmpty() == false)
	{
		result = QDir::cleanPath(result + QDir::separator() + append);
	}

	if (QDir(result).exists() == false)
		QDir().mkpath(result);

	return result;
}

QString toCamelCase
(
	const QString& s
)
{
	QStringList parts = s.split('_', Qt::SkipEmptyParts);
	for (auto i: basic_range<int>(1, parts.size()))
		parts[i].replace(0, 1, parts[i][0].toUpper());

	return parts.join("");
}

QString defaultGlobalLoggingPath()
{
	QString result = documentsDataPath("global");
	result = QDir::cleanPath(result);
	if (QDir(result).exists() == false)
		QDir().mkpath(result);

	return result;
}

QString defaultLoggingPath
(
	const QString& appName
)
{
	QString result = documentsDataPath(appName) + QDir::separator() + "logs";
	result = QDir::cleanPath(result);
	if (QDir(result).exists() == false)
		QDir().mkpath(result);

	return result;
}

void renewInstallRegistry
(
	QSettings& settings,
	const QString& newInstallGuid
)
{
	QLibrary cleanLib("InstallFinalize");

	if (cleanLib.load())
	{
		typedef void (*InstallProc)(const char* registryPath);

		InstallProc installProc = reinterpret_cast<InstallProc>(cleanLib.resolve("installProc"));
		if (installProc)
		{
			installProc("HKEY_CURRENT_USER\\Software\\Qualcomm, Inc.");
		}

		cleanLib.unload();
	}

	settings.setValue("Install UUID", newInstallGuid);
	// applicationDirPath() needs a QCoreApplication; without one (EPMDev loaded from
	// Python or another non-Qt host) it only prints a Qt warning and returns nothing.
	if (QCoreApplication::instance() != Q_NULLPTR)
	{
		settings.setValue("Install Path", QCoreApplication::applicationDirPath().toLatin1());
	}
}

bool isNewInstall
(
	QSettings& settings,
	const QString& currentInstallGuid
)
{
	QString uuid = settings.value("Install UUID", QString("xxxx")).toString();
	return uuid != currentInstallGuid;
}

QString createFilenameTimeStamp()
{
	return QDateTime::currentDateTime().toString("_yyyy_dd_MM_HH_mm_ss");
}


QString killOneDrive(const QString& testPath, const QString& revertPath)
{
	QString righteousPath{testPath};

	if (righteousPath.contains("OneDrive -", Qt::CaseInsensitive)) // We don't allow that abomination, OneDrive.  What a piece of crap!
		righteousPath = revertPath;

	return righteousPath;
}

QString getModuleFilePath(const QString &moduleFileName)
{
	QString result;
	QString platformModuleFileName{moduleFileName};

#ifdef Q_OS_WIN
	char path[_MAX_PATH + 1];
	platformModuleFileName += ".dll";
	GetModuleFileNameA(GetModuleHandleA(platformModuleFileName.toLatin1().data()), path, sizeof(path));
	result = QByteArray(path);
#endif

	 return result;
}

void kickIt()
{
	QProcess* kickItProc = new QProcess(Q_NULLPTR);

	QStringList arguments;

	arguments << "upload" << "-telematics";

	if (kickItProc->startDetached("qik.exe", arguments))
	{
		kickItProc->waitForStarted();
	}

	kickItProc->deleteLater();
}

void cleanIt(const QString &pathToClean)
{
	QProcess* cleanItProc = new QProcess(Q_NULLPTR);

	QStringList arguments;

	arguments << pathToClean;

	if (cleanItProc->startDetached("LogCleanup.exe", arguments))
	{
		cleanItProc->waitForStarted();
	}

	cleanItProc->deleteLater();
}

bool isUserPrivileged()
{
	bool privileged{false};

#ifdef Q_OS_WINDOWS
	privileged = IsUserAnAdmin();
#endif
#ifdef Q_OS_LINUX
	privileged = (geteuid() == 0);
#endif

	return privileged;
}

bool executeBinaryAsAdministrator(const QString &binary, const QString &cmdArgs)
{
	bool result{false};

#ifdef Q_OS_WINDOWS
	SHELLEXECUTEINFO shExecInfo{0};

	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExecInfo.hwnd = NULL;
	shExecInfo.lpVerb = L"runas";
	shExecInfo.lpFile = (LPCWSTR)binary.utf16();
	shExecInfo.lpParameters = (LPCWSTR)cmdArgs.utf16();
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = SW_SHOW;
	shExecInfo.hInstApp = NULL;

	if (ShellExecuteEx(&shExecInfo))
	{
		WaitForSingleObject(shExecInfo.hProcess, INFINITE);

		DWORD exitCode(0);
		GetExitCodeProcess(shExecInfo.hProcess, &exitCode);

		if(exitCode == 0)
			result = true;

		CloseHandle(shExecInfo.hProcess);
	}

#else
	QProcess process;

	process.start("sudo", {binary, cmdArgs});
	if (process.waitForFinished())
	{
		if(process.exitCode() == 0)
			result = true;
	}
#endif

	return result;
}

QString expandPath(const QString &filePath)
{
	QString result{filePath};

#ifdef Q_OS_LINUX
	// On Linux machines, QDir::homePath() is returned as / if $HOME is not set.
	// Alpaca applications work fine as they are triggered from the shell which has $HOME set.
	// On implementations which does not involve script execution from shell, the expandPath API may return invalid path.

	if (result.startsWith("~"))
	{
		QString homePath = QDir::homePath();
		if (homePath.compare(QDir::separator()) == 0)
		{
			// HOME is not set. Ask the password database for the real home of
			// the current user instead of assuming /root, which is wrong for
			// every non-root user.
			const struct passwd* pw = getpwuid(getuid());
			if (pw != Q_NULLPTR && pw->pw_dir != Q_NULLPTR && pw->pw_dir[0] != '\0')
			{
				homePath = QString::fromLocal8Bit(pw->pw_dir);
			}
			qWarning() << "expandPath: HOME is not set, using" << homePath << "for ~";
		}
		result.replace(0, 1, homePath);
	}
#endif

	return result;
}

QString epmConfigRoot()
{
	// applicationDirPath() needs a QCoreApplication. EPMDev is also loaded by hosts that
	// have none (Python, C#, plain C++), where calling it prints "Please instantiate the
	// QApplication object first". In that case skip the search next to the executable
	// and use the installed configuration folder.
	const bool haveApplication = QCoreApplication::instance() != Q_NULLPTR;

	if (haveApplication)
	{
		QDir dir(QCoreApplication::applicationDirPath());

		for (int i = 0; i < 5; ++i)
		{
			const QString candidate = dir.absolutePath() + "/configurations";
			if (QDir(candidate).exists())
			{
				return QDir::cleanPath(candidate);
			}

			dir.cdUp();
		}
	}

	QString result;

#ifdef Q_OS_WIN
	result = "C:/ProgramData/Qualcomm/QEPM/configurations";
#endif
#ifdef Q_OS_LINUX
	result = "/var/lib/qcom/data/QEPM/configurations";
#endif

	if (!result.isEmpty() && QDir(result).exists())
		return QDir::cleanPath(result);

	static bool reportedMissing{false};
	if (reportedMissing == false)
	{
		reportedMissing = true;
		qWarning() << "epmConfigRoot: no configurations folder found above the executable or in"
			<< result << "- configurations will not load";
	}

	if (haveApplication == false)
	{
		return QDir::cleanPath(result);
	}

	return QDir::cleanPath(QCoreApplication::applicationDirPath() + "/configurations");
}

// Writes the resolved install and data locations to the debug output once at
// startup, so a wrong path in the field can be diagnosed from the log alone.
void logInstallLayout()
{
	const QString exeDir = QCoreApplication::instance() != Q_NULLPTR
		? QDir::cleanPath(QCoreApplication::applicationDirPath())
		: QStringLiteral("<no application>");

	qInfo().noquote() << "Install layout: executable folder =" << exeDir;
	qInfo().noquote() << "Install layout: configurations    =" << epmConfigRoot();
	qInfo().noquote() << "Install layout: documents folder  =" << documentsDataPath(QString());
}
