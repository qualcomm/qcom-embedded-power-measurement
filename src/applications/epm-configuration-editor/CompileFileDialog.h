#ifndef COMPILEFILEDIALOG_H
#define COMPILEFILEDIALOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// Qt
#include <QFileDialog>
class QCheckBox;

class CompileFileDialog :
	public QFileDialog
{
public:
	CompileFileDialog(QWidget* parent, const QString& empFileName);

	bool useRCMMapping()
	{
		return _useRCMMapping;
	}

	bool useKratos()
	{
		return _useKratos;
	}

	QString compileFile()
	{
		return _compileFile;
	}

private slots:
	void on_finsished(int r);

private:
	QString						_compileFile;
	QCheckBox*					_useRCMMapping{Q_NULLPTR};
	QCheckBox*					_useKratos{Q_NULLPTR};
};

#endif // COMPILEFILEDIALOG_H
