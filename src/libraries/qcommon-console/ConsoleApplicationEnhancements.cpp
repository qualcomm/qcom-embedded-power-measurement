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
	#include <unistd.h>
#endif


QString applicationBinPath()
{
	QString result;
	QString appName{"QEPM"};

#ifdef Q_OS_WIN
	result = "C:/Program Files (x86)/Qualcomm/" + appName + "/";
#endif

#ifdef Q_OS_LINUX
	result = "/opt/qcom/" + appName + "/bin/";
#endif

	result = QDir::cleanPath(result);

	if (QDir(result).exists() == false)
		QDir().mkpath(result);

	return result;
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
	settings.setValue("Install Path", QCoreApplication::applicationDirPath().toLatin1());
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
		if (homePath.compare(QDir::separator()) != 0)
			result.replace("~", homePath);
		else
			result.replace("~", "/root");
	}
#endif

	return result;
}

QString epmConfigRoot()
{
	// QCoreApplication::applicationDirPath() prints "Please instantiate the
	// QApplication object first" (and returns an empty path) when called
	// before a QCoreApplication/QApplication instance exists - which is the
	// normal case for callers using EPMDev.dll directly via Python ctypes
	// (no Qt application object is ever created there). Resolve the binary
	// directory exactly once, only when an application instance actually
	// exists (i.e. real GUI/console apps that link Qt and construct one),
	// so pure-ctypes callers never trigger that warning at all.
	const bool hasAppInstance = (QCoreApplication::instance() != Q_NULLPTR);
	const QString binDirPath = hasAppInstance ? QCoreApplication::applicationDirPath() : QString();

	if (hasAppInstance)
	{
		QDir dir(binDirPath);

		for (int i = 0; i < 5; ++i)
		{
			const QString candidate = dir.absolutePath() + "/configurations";
			if (QDir(candidate).exists())
				return QDir::cleanPath(candidate);

			dir.cdUp();
		}
	}

	// Packaged installs stage EPM's .ccnf files under a shared ProgramData
	// location rather than a bin-relative "configurations" folder, but the
	// exact install folder name and subfolder name vary by install method:
	//   - standalone EPM installer (install.ps1 / package_QEPM.sh):
	//     .../<AppFolder>/configurations
	//   - Alpaca QIK bundle (build-installer.ps1 / QIKproj):
	//     .../<AppFolder>/epm_configs
	// Derive the actual installed folder name ("Alpaca", "QEPM", ...) from
	// where this binary is running (same approach as docsRoot()) rather than
	// hardcoding a single product name, and check both known subfolder
	// naming conventions under it.
	QString appName;
	if (hasAppInstance)
	{
		QDir binDir(binDirPath);
		if (binDir.exists())
		{
			const QString folderName = binDir.dirName();
			if (folderName.isEmpty() == false)
				appName = folderName;
		}
	}

	QString programDataParentRoot;
#ifdef Q_OS_WIN
	programDataParentRoot = "C:/ProgramData/Qualcomm";
#endif
#ifdef Q_OS_LINUX
	programDataParentRoot = "/var/lib/qcom/data";
#endif

	const QStringList subfolderCandidates = { "configurations", "epm_configs" };

	if (appName.isEmpty() == false)
	{
		const QString programDataRoot = programDataParentRoot + "/" + appName;

		// Check for actual .ccnf content, not just directory existence:
		// "configurations" is a shared folder that may also exist for other
		// products (e.g. TAC's .tcnf files, devicelist.json) without
		// containing any EPM .ccnf files, which would otherwise cause this
		// to return the wrong, EPM-config-less folder.
		for (const QString& subfolder : subfolderCandidates)
		{
			const QString candidate = programDataRoot + "/" + subfolder;
			QDir candidateDir(candidate);
			if (candidateDir.exists() &&
				candidateDir.entryList(QStringList() << "*.ccnf", QDir::Files).isEmpty() == false)
				return QDir::cleanPath(candidate);
		}
	}

	// Either there's no application instance to derive the install folder
	// name from (the plain ctypes/Python case, where applicationDirPath()
	// is unusable), or that folder name didn't actually contain any EPM
	// configs (e.g. a stale/partial install). Rather than guessing a single
	// hardcoded product name (which is wrong whenever the real install uses
	// a different folder name, such as "Alpaca" instead of "QEPM"),
	// generically scan every sibling folder under the shared Qualcomm
	// ProgramData root for the one that actually contains EPM .ccnf files.
	if (!programDataParentRoot.isEmpty())
	{
		QDir parentRoot(programDataParentRoot);
		const QStringList subdirs = parentRoot.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for (const QString& subdir : subdirs)
		{
			if (subdir.compare(appName, Qt::CaseInsensitive) == 0)
				continue; // already checked above

			for (const QString& subfolder : subfolderCandidates)
			{
				const QString candidate = programDataParentRoot + "/" + subdir + "/" + subfolder;
				QDir candidateDir(candidate);
				if (candidateDir.exists() &&
					candidateDir.entryList(QStringList() << "*.ccnf", QDir::Files).isEmpty() == false)
					return QDir::cleanPath(candidate);
			}
		}
	}

	if (hasAppInstance)
		return QDir::cleanPath(binDirPath + "/configurations");

	// No application instance and no ProgramData match found anywhere: fall
	// back to the historical default location. This preserves prior
	// behavior for the case where nothing was found at all (e.g. first run
	// before ProgramData is populated) rather than returning an empty path.
	const QString fallbackAppName = appName.isEmpty() ? QString("QEPM") : appName;
	return QDir::cleanPath(programDataParentRoot + "/" + fallbackAppName + "/configurations");
}
