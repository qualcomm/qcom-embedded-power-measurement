//The MIT License
//
//Copyright (c) 2008-2018 Michael Simpson
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

#include "QTExcel.h"

#include "Range.h"

// Qt
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

// Excel.h and Excel.cpp are created with this command
// dumpcpp {00020813-0000-0000-C000-000000000046} -o excel
// run dumpcpp from a QT environment command line in the generated folders directory
#ifdef Q_OS_WIN32
#include "excel.h"

// Windows
#include <combaseapi.h>
#endif

struct ExcelContext
{
#ifdef Q_OS_WIN32
	Excel::Application*			_application{Q_NULLPTR};
	Excel::Workbook*			_workbook{Q_NULLPTR};
	Excel::Worksheet*			_workSheet{Q_NULLPTR};
#else
	void*						_application{Q_NULLPTR};
#endif
};

#ifdef Q_OS_WIN32
using namespace Excel;
#endif

static void NullHandler(QtMsgType, const QMessageLogContext&, const QString&)
{
}

QTExcel::QTExcel() :
	_excelContext(new ExcelContext)
{
}

QTExcel::~QTExcel()
{
	if (_excelContext != Q_NULLPTR)
	{
#ifdef Q_OS_WIN32
		if (_excelContext->_workSheet != Q_NULLPTR)
		{
			delete _excelContext->_workSheet;
			_excelContext->_workSheet = Q_NULLPTR;
		}
		if (_excelContext->_workbook != Q_NULLPTR)
		{
			delete _excelContext->_workbook;
			_excelContext->_workbook = Q_NULLPTR;
		}
		if (_excelContext->_application != Q_NULLPTR)
		{
			_excelContext->_application->Quit();

			delete _excelContext->_application;
			_excelContext->_application = Q_NULLPTR;
		}
#endif
		delete _excelContext;
		_excelContext = Q_NULLPTR;
	}
}

bool QTExcel::excelAvailable()
{
	bool result{false};

#ifdef Q_OS_WIN
	QSettings settings("HKEY_CLASSES_ROOT\\Excel.Application", QSettings::NativeFormat);

	auto childGroups = settings.childGroups().count();
	if (childGroups > 0)
		result = true;
#endif

	return result;
}

bool QTExcel::create
(
	bool visible
)
{
	bool result(false);

#ifdef Q_OS_WIN
	if (openExcel(visible))
	{
		QtMessageHandler oldHandler = qInstallMessageHandler(NullHandler);
		_excelContext->_workbook = _excelContext->_application->Workbooks()->Add();
		qInstallMessageHandler(oldHandler);

		if (_excelContext->_workbook != Q_NULLPTR)
		{
			result = true;
		}
	}
#endif

	return result;
}

bool QTExcel::open
(
	const QString& excelXLSFile,
	bool visible
)
{
	bool result(false);

#ifdef Q_OS_WIN
	if (openExcel(visible))
	{
		_excelContext->_workbook = _excelContext->_application->Workbooks()->Open(excelXLSFile);
		if (_excelContext->_workbook != Q_NULLPTR)
		{
			_excelContext->_workSheet = new Excel::Worksheet(new _Worksheet(_excelContext->_workbook->Sheets()->Item(QVariant(1))));

			result = true;
		}
	}
#endif
	return result;
}

bool QTExcel::save()
{
	bool result(false);

#ifdef Q_OS_WIN
	if (_excelContext->_workbook != Q_NULLPTR)
	{
		_excelContext->_workbook->Save();
		result = true;
	}
#endif

	return result;
}

bool QTExcel::saveAs
(
	const QString &xlsFile
)
{
	bool result(false);

#ifdef Q_OS_WIN
	if (_excelContext->_workbook != Q_NULLPTR)
	{
		QFileInfo fileInfo(xlsFile);

		QString path = fileInfo.absolutePath();
		if (QFileInfo::exists(path) == false)
		{
			result = QDir().mkpath(path);
		}
		else
			result = true;

		if (result)
			_excelContext->_workbook->SaveCopyAs(QVariant(path));
		result = true;
	}
#endif

	return result;
}

void QTExcel::screenUpdating
(
	bool updateScreen
)
{
#ifdef Q_OS_WIN
	if (_excelContext->_application != Q_NULLPTR)
	{
		_excelContext->_application->SetScreenUpdating(updateScreen);
	}
#else
	Q_UNUSED(updateScreen)
#endif
}

void QTExcel::displayAlerts
(
	bool displayAlerts
)
{
#ifdef Q_OS_WIN
	if (_excelContext->_application != Q_NULLPTR)
	{
		_excelContext->_application->SetDisplayAlerts(displayAlerts);
	}
#else
	Q_UNUSED(displayAlerts)
#endif
}

void QTExcel::setVisible
(
	bool visible
)
{
#ifdef Q_OS_WIN
	if (_excelContext->_application != Q_NULLPTR)
		_excelContext->_application->SetVisible(visible);
#else
	Q_UNUSED(visible)
#endif
}

bool QTExcel::version
(
	quint16& major,
	quint16& minor
)
{
	bool result(false);

#ifdef Q_OS_WIN
	major = 0;
	minor = 0;

	if (_excelContext->_application != NULL)
	{
		QString versionStr;

		versionStr =_excelContext->_application->Version();
		if (versionStr.isEmpty() == false)
		{
			result = true;

			QStringList parts = versionStr.split(".");
			if (parts.size() >= 1)
			{
				major = parts.at(0).toInt();

				if (parts.size() >= 2)
				{
					minor = parts.at(1).toInt();
				}
			}
		}
	}

#else
	Q_UNUSED(major)
	Q_UNUSED(minor)
#endif

	return result;
}

bool QTExcel::openExcel
(
	bool visible
)
{
	bool result(false);

#ifdef Q_OS_WIN
	if (_excelContext->_application == Q_NULLPTR)
	{
		_excelContext->_application = new Excel::Application;
		if (_excelContext->_application != Q_NULLPTR)
		{
			if (_excelContext->_application->isNull())
			{
				delete _excelContext->_application;
				_excelContext->_application = Q_NULLPTR;
			}
			else
			{
				_excelContext->_application->SetVisible(visible);
				result = true;
			}
		}
	}
#else
	Q_UNUSED(visible)
#endif

	return result;
}

void QTExcel::quit()
{
#ifdef Q_OS_WIN
	if (_excelContext != Q_NULLPTR)
	{
		if (_excelContext->_workbook != Q_NULLPTR)
		{
			delete _excelContext->_workbook;
			_excelContext->_workbook = Q_NULLPTR;
		}

		if (_excelContext->_workSheet != Q_NULLPTR)
		{
			delete _excelContext->_workSheet;
			_excelContext->_workSheet = Q_NULLPTR;
		}

		if (_excelContext->_application != Q_NULLPTR)
		{
			_excelContext->_application->Quit();
			delete _excelContext->_application;
			_excelContext->_application = Q_NULLPTR;
		}
	}
#endif

}

bool QTExcel::activateWorkbook()
{
	bool result(false);

#ifdef Q_OS_WIN
	if (_excelContext->_workbook != Q_NULLPTR)
	{
		_excelContext->_workbook->Activate();
		result = true;
	}
#endif

	return result;
}

QStringList QTExcel::workBookNames()
{	
	QStringList result;

#ifdef Q_OS_WIN
	if (_excelContext->_application != Q_NULLPTR)
	{
		Excel::Workbooks* workbooks = _excelContext->_application->Workbooks();
		if (workbooks != Q_NULLPTR)
		{
			for (auto index: range(1, workbooks->Count() + 1))
			{
				Excel::Workbook* workbook = workbooks->Item(QVariant(index));
				if (workbook != Q_NULLPTR)
				{
					QString name = workbook->Name();
					
					result.append(name);
				}
			}

			delete workbooks;
		}
	}
#endif

	return result;
}

int QTExcel::workBookCount()
{	
	int result(0);

#ifdef Q_OS_WIN
	if (_excelContext->_application != Q_NULLPTR)
	{
		Excel::Workbooks* workbooks = _excelContext->_application->Workbooks();
		if (workbooks != Q_NULLPTR)
		{
			result = workbooks->Count();
			delete workbooks;
		}
	}
#endif

	return result;
}

bool QTExcel::selectWorkbook
(
	int index
)
{
	bool result(false);

#ifdef Q_OS_WIN
	if (_excelContext->_application != Q_NULLPTR)
	{
		Excel::Workbooks* workbooks = _excelContext->_application->Workbooks();
		if (workbooks != Q_NULLPTR)
		{
			Excel::Workbook* workbook = workbooks->Item(QVariant(index));
			if (workbook != Q_NULLPTR)
			{
				if (_excelContext->_workbook != Q_NULLPTR)
					delete _excelContext->_workbook;

				_excelContext->_workbook = workbook;
				result = true;
			}

			delete workbooks;
		}
	}
#endif

	return result;
}

bool QTExcel::selectWorkbook
(
	const QString& sheetName
)
{
	bool result(false);

#ifdef Q_OS_WIN
	if (_excelContext->_application != Q_NULLPTR)
	{
		Excel::Workbooks* workbooks = _excelContext->_application->Workbooks();
		if (workbooks != Q_NULLPTR)
		{
			Excel::Workbook* workbook = workbooks->Item(QVariant(sheetName));
			if (workbook != Q_NULLPTR)
			{
				if (_excelContext->_workbook != Q_NULLPTR)
					delete _excelContext->_workbook;

				_excelContext->_workbook = workbook;
				result = true;
			}

			delete workbooks;
		}
	}
#else
	Q_UNUSED(sheetName)
#endif

	return result;
}

bool QTExcel::activateWorksheet()
{	
	bool result(false);

#ifdef Q_OS_WIN
	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		_excelContext->_workSheet->Activate();
		result = true;
	}
#endif

	return result;
}

bool QTExcel::renameSheet
(
	const QString& newName
)
{
	bool result(false);

#ifdef Q_OS_WIN
	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		_excelContext->_workSheet->SetName(newName);
		result = true;
	}	
#else
	Q_UNUSED(newName)
#endif

	return result;
}

void QTExcel::setWorksheetGridLines
(
	bool showGridlines
)
{
#ifdef Q_OS_WIN
	if (_excelContext->_application != Q_NULLPTR)
	{
		Excel::Window* activeWindow = _excelContext->_application->ActiveWindow();
		if (activeWindow != Q_NULLPTR)
		{
			activeWindow->SetDisplayGridlines(showGridlines);
			delete activeWindow;
		}
	}
#else
	Q_UNUSED(showGridlines)
#endif
}

bool QTExcel::deleteWorksheet
(
	quint32 index
)
{
	bool result(false);

#ifdef Q_OS_WIN
	if (_excelContext->_workbook != Q_NULLPTR)
	{
		Excel::Sheets* sheets = _excelContext->_workbook->Sheets();
		if (sheets != Q_NULLPTR)
		{
			quint32 sheetCount = sheets->Count();

			if (index <= sheetCount)
			{
				IDispatch* sheetItem = sheets->Item(QVariant(index));
				if (sheetItem != Q_NULLPTR)
				{
					Excel::Worksheet* workSheet = new Excel::Worksheet(new Excel::_Worksheet(sheetItem));
					if (workSheet != Q_NULLPTR)
					{
						workSheet->Delete();

						delete workSheet;

						result = true;
					}
				}
			}

			delete sheets;
		}
	}
#else
	Q_UNUSED(index)
#endif

	return result;
}

void QTExcel::setColumnWidth
(
	const QString& cellReferece,
	quint32 width
)
{
#ifdef Q_OS_WIN
	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReferece));

		if (range != Q_NULLPTR)
		{
			Excel::Range* columns = range->Columns();
			if (columns != Q_NULLPTR)
			{
				columns->SetColumnWidth(width);

				delete columns;
			}

			delete range;
		}
	}
#else
	Q_UNUSED(cellReferece)
	Q_UNUSED(width)
#endif
}

QStringList QTExcel::workSheetNames()
{
	QStringList result;

#ifdef Q_OS_WIN
	if (_excelContext->_workbook != Q_NULLPTR)
	{
		Excel::Sheets* sheets = _excelContext->_workbook->Sheets();
		if (sheets != Q_NULLPTR)
		{
			for (auto index: range(1, sheets->Count() + 1))
			{
				IDispatch* sheetItem = sheets->Item(QVariant(index));
				if (sheetItem != Q_NULLPTR)
				{
					Excel::Worksheet* workSheet = new Excel::Worksheet(new Excel::_Worksheet(sheetItem));
					if (workSheet != Q_NULLPTR)
					{
						result.append(workSheet->Name());
						delete workSheet;
					}
				}
			}

			delete sheets;
		}
	}
#endif

	return result;
}

int QTExcel::workSheetCount()
{
	int result(0);

#ifdef Q_OS_WIN
	if (_excelContext->_workbook != Q_NULLPTR)
	{
		Excel::Sheets* sheets = _excelContext->_workbook->Sheets();
		if (sheets != Q_NULLPTR)
		{
			result = sheets->Count();
			
			delete sheets;
		}
	}
#endif

	return result;
}

bool QTExcel::selectWorksheet
(
	quint32 index
)
{
	bool result(false);

#ifdef Q_OS_WIN
	if (_excelContext->_workbook != NULL)
	{
		Excel::Sheets* sheets = _excelContext->_workbook->Sheets();
		if (sheets != NULL)
		{
			quint32 sheetCount = sheets->Count();

			if (index <= sheetCount)
			{
				IDispatch* sheetItem = sheets->Item(QVariant(index));
				if (sheetItem != NULL)
				{
					Excel::Worksheet* workSheet = new Excel::Worksheet(new Excel::_Worksheet(sheetItem));
					if (workSheet != NULL)
					{
						if (_excelContext->_workSheet != NULL)
							delete _excelContext->_workSheet;

						_excelContext->_workSheet = workSheet;

						result = true;
					}
				}
			}

			delete sheets;
		}
	}
#else
	Q_UNUSED(index)
#endif

	return result;
}

bool QTExcel::selectWorksheet
(
	const QString& sheetName
)
{
	bool result(false);

#ifdef Q_OS_WIN
	if (_excelContext->_workbook != Q_NULLPTR)
	{
		Excel::Sheets* sheets = _excelContext->_workbook->Sheets();
		if (sheets != Q_NULLPTR)
		{
			for (auto index: range(1, sheets->Count() + 1))
			{
				IDispatch* sheetItem = sheets->Item(index);
				if (sheetItem != Q_NULLPTR)
				{
					Excel::Worksheet* workSheet = new Excel::Worksheet(new Excel::_Worksheet(sheetItem));
					if (workSheet != Q_NULLPTR)
					{
						if (workSheet->Name() == sheetName)
						{
							if (_excelContext->_workSheet != Q_NULLPTR)
								delete _excelContext->_workSheet;

							_excelContext->_workSheet = workSheet;

							return true;
						}
					}
				}
			}

			delete sheets;
		}
	}
#else
	Q_UNUSED(sheetName)
#endif

	return result;
}

#ifdef Q_OS_WIN
static QVariant getCellValue
(
	Excel::Range* range,
	quint32 row,
	quint32 col
)
{
	QVariant result;

	QVariant item = range->Item(row, col);

	if (item.isValid())
	{
		QAxObject* cellObject = item.value<QAxObject*>();
		if (cellObject != Q_NULLPTR)
		{
			cellObject->disableMetaObject();
			result = cellObject->dynamicCall("Value");

			if (!result.isValid())
				result = QString("");

			delete cellObject;
		}
	}

	return result;
}
#endif

QVariant QTExcel::getCellValue
(
	const QString& cellReference
)
{
	QVariant result;

#ifdef Q_OS_WIN
	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != Q_NULLPTR)
		{
			result = ::getCellValue(range, 1, 1);

			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
#endif

	return result;
}

quint32 QTExcel::getCellValues
(
	const QString& cellReference,
	QList<QVariant>& results
)
{
	results.clear();

#ifdef Q_OS_WIN
	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != Q_NULLPTR)
		{
			quint32 rowBounds = range->Rows()->Count();
			quint32 colBounds = range->Columns()->Count();
			if (rowBounds == 1 &&  colBounds == 1)
			{
				results.push_back(::getCellValue(range, 1, 1));
			}
			else
			{
				for (quint32 row = 1; row <= rowBounds; row++)
				{
					for (quint32 col = 1; col <= colBounds; col++)
					{
						results.push_back(::getCellValue(range, row, col));
					}
				}
			}

			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
#endif

	return results.size();
}

#ifdef Q_OS_WIN
static void setCellValue
(
	Excel::Range* range,
	quint32 row,
	quint32 col,
	const QVariant& value
)
{
	QVariant item = range->Item(row, col);

	if (item.isValid())
	{
		QAxObject* cellObject = item.value<QAxObject*>();
		if (cellObject != NULL)
		{
			cellObject->disableMetaObject();
			cellObject->dynamicCall("Value", value);

			delete cellObject;
		}
	}
}
#endif

void QTExcel::setCellValue
(
	const QString& cellReference, 
	const QString& cellValue
)
{
	setCellValue(cellReference, QVariant(cellValue));
}

void QTExcel::setCellValue
(
	const QString& cellReference, 
	const QVariant& cellValue
)
{
#ifdef Q_OS_WIN
	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != NULL)
		{
			::setCellValue(range, 1, 1, cellValue);

			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
	Q_UNUSED(cellValue)
#endif
}

void QTExcel::setCellValues
(
	const QString& cellReference, 
	const QStringList& cellValues
)
{
	QList<QVariant> cellValuesV;

	QList<QString>::const_iterator cellValue = cellValues.begin();

	while (cellValue != cellValues.end())
	{
		cellValuesV.push_back(QVariant(*cellValue));
		cellValue++;
	}

	setCellValues(cellReference, cellValuesV);
}

void QTExcel::setCellValues
(
	const QString& cellReference, 
	const QList<QVariant>& cellValues
)
{
#ifdef Q_OS_WIN
	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != NULL)
		{
			quint32 rowBounds = range->Rows()->Count();
			quint32 colBounds = range->Columns()->Count();

			QList<QVariant>::const_iterator cellValue = cellValues.begin();

			while (cellValue != cellValues.end())
			{
				if (rowBounds == 1 &&  colBounds == 1)
				{
					::setCellValue(range, 1, 1, *cellValue);
					cellValue = cellValues.end();
				}
				else
				{
					quint32 row = 1;
					quint32 col = 1;

					while (cellValue != cellValues.end())
					{
						::setCellValue(range, row, col, *cellValue);

						row++;
						if (row > rowBounds)
						{
							row = 1;
							col++;
						}

						if (col > colBounds)
						{
							cellValue = cellValues.end();
						}
						else
							cellValue++;
					}
				}
			}

			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
	Q_UNUSED(cellValues)
#endif
}

void QTExcel::formatCharacterColor
(
	const QString& cellReference,
	quint32 start, 
	quint32 end,
	const sRGB& color
)
{
#ifdef Q_OS_WIN
	QColor characterColor(color._red, color._green, color._blue);

	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != Q_NULLPTR)
		{
			QList<QVariant> args;
			
			args.push_back(QVariant(start));
			args.push_back(QVariant(end));

			QVariant charsVariant = range->dynamicCall("Characters(QVariant, QVariant)", args);
			if (charsVariant.isValid())
			{
				IDispatch* charsItem = charsVariant.value<IDispatch*>();
				if (charsItem != Q_NULLPTR)
				{
					Excel::Characters* characters = new Excel::Characters(charsItem);
					if (characters != Q_NULLPTR)
					{
						characters->Font()->SetColor(QVariant(characterColor));
						delete characters;
					}
					else
					{
						charsItem->Release();
					}
				}
			}

			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
	Q_UNUSED(start)
	Q_UNUSED(end)
	Q_UNUSED(color)
#endif
}

sRGB QTExcel::getCellInteriorColor
(
	const QString& cellReference
)
{
	sRGB result;

#ifdef Q_OS_WIN
	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != Q_NULLPTR)
		{
			QColor interiorColor = range->Interior()->Color().value<QColor>();
			result = sRGB(interiorColor.red(), interiorColor.green(), interiorColor.blue());
			delete range;
		}
	}
#endif

	return result;
}

void QTExcel::autoFitColumns
(
	const QString& cellReference
)
{	
#ifdef Q_OS_WIN
	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != NULL)
		{
			Excel::Range* columns = range->EntireColumn();
			if (columns != NULL)
			{
				columns->AutoFit();
				delete columns;
			}

			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
#endif
}

void QTExcel::setColumnWidths
(
	int firstColumn,
	int lastColumn,
	qreal columnWidth
)
{

#ifdef Q_OS_WIN
	QString cellReference = produceRange(stringFromColumnIndex(firstColumn), 1, stringFromColumnIndex(lastColumn), 1);

	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != NULL)
		{
			Excel::Range* columns = range->EntireColumn();
			if (columns != NULL)
			{
				columns->SetColumnWidth(columnWidth);
				delete columns;
			}

			delete range;
		}
	}
#endif
}

void QTExcel::setCellFont
(
	const QString& cellReference, 
	const QString& fontName
)
{
#ifdef Q_OS_WIN
	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != NULL)
		{
			Excel::Font* font = range->Font();
			if (font != Q_NULLPTR)
			{
				font->SetName(QVariant(fontName));

				delete font;
			}

			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
	Q_UNUSED(fontName)
#endif
}

void QTExcel::setCellBold
(
	const QString& cellReference, 
	bool bold
)
{	
#ifdef Q_OS_WIN
	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != Q_NULLPTR)
		{
			Excel::Font* font = range->Font();
			if (font != Q_NULLPTR)
			{
				font->SetBold(bold);

				delete font;
			}

			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
	Q_UNUSED(bold)
#endif
}

void QTExcel::setCellFontSize
(
	const QString& cellReference, 
	int fontSize
)
{	
#ifdef Q_OS_WIN
	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != NULL)
		{
			Excel::Font* font = range->Font();
			if (font != Q_NULLPTR)
			{
				font->SetSize(fontSize);

				delete font;
			}

			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
	Q_UNUSED(fontSize)
#endif
}

void QTExcel::setCellFontColor
(
	const QString& cellReference, 
	const sRGB& color
)
{
#ifdef Q_OS_WIN
	QColor fontColor(color._red, color._green, color._blue);

	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != Q_NULLPTR)
		{
			Excel::Font* font = range->Font();
			if (font != Q_NULLPTR)
			{
				font->SetColor(fontColor);

				delete font;
			}

			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
	Q_UNUSED(color)
#endif
}

void QTExcel::setCellInteriorColor
(
	const QString& cellReference, 
	const sRGB& color
)
{
#ifdef Q_OS_WIN
	QColor interiorColor(color._red, color._green, color._blue);

	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != Q_NULLPTR)
		{
			range->Interior()->SetColor(interiorColor);
			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
	Q_UNUSED(color)
#endif
}

void QTExcel::setCellBorderStyle
(
	const QString& cellReference, 
	ExcelLineStyle lineStyle,
	ExcelBorderWeight lineWeight,
	const sRGB& color
)
{	
#ifdef Q_OS_WIN
	QColor borderColor(color._red, color._green, color._blue);

	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::XlLineStyle xLlineStyle = static_cast<Excel::XlLineStyle>(lineStyle);
		Excel::XlBorderWeight xLlineWeight = static_cast<Excel::XlBorderWeight>(lineWeight);

		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != Q_NULLPTR)
		{
			range->BorderAround(QVariant(xLlineStyle), xLlineWeight, xlColorIndexNone, QVariant(borderColor));

			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
	Q_UNUSED(lineStyle)
	Q_UNUSED(lineWeight)
	Q_UNUSED(color)
#endif
}

void QTExcel::setCellBorderElement
(
	const QString& cellReference, 
	quint32 borderMask, 
	ExcelLineStyle lineStyle,
	ExcelBorderWeight lineWeight,
	const sRGB& color
)
{
#ifdef Q_OS_WIN
	int mask = 1;

	QColor borderColor(color._red, color._green, color._blue);

	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != NULL)
		{
			Excel::XlBordersIndex borderIndex;
			
			Excel::Borders* borders = range->Borders();
			if (borders != NULL)
			{
				while (mask <= eLastBorderMask)
				{  
					if (mask & borderMask)
					{
						switch (mask)
						{
							case eInsideHorizontal: borderIndex = xlInsideHorizontal; break;
							case eInsideVertical: borderIndex = xlInsideVertical;break;
							case eDiagonalDown: borderIndex = xlDiagonalDown; break;
							case eDiagonalUp: borderIndex = xlDiagonalUp; break;
							case eEdgeBottom: borderIndex = xlEdgeBottom; break;
							case eEdgeLeft: borderIndex = xlEdgeLeft; break;
							case eEdgeRight: borderIndex = xlEdgeRight; break;
							case eEdgeTop: borderIndex = xlEdgeTop; break;
							default: borderIndex = static_cast<Excel::XlBordersIndex>(0); break;
						}

						if (borderIndex != static_cast<Excel::XlBordersIndex>(0)) // we have a good border
						{
							Excel::Border* border;

							border = borders->Item(borderIndex);
							if (border != NULL)
							{
								border->SetLineStyle(lineStyle);
								border->SetWeight(lineWeight);
								border->SetColor(borderColor);

								delete border;
							}
						}
					}

					mask = mask << 1;
				}

				delete borders;
			}

			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
	Q_UNUSED(borderMask)
	Q_UNUSED(lineStyle)
	Q_UNUSED(lineWeight)
	Q_UNUSED(color)
#endif
}

void QTExcel::setCellHorizontalAlignment
(
	const QString& cellReference,
	ExcelHAlignment horizontalAlignment
)
{	
#ifdef Q_OS_WIN
	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != Q_NULLPTR)
		{
			Excel::XlHAlign xlHorizontalAlignment = static_cast<Excel::XlHAlign>(horizontalAlignment);
			range->SetHorizontalAlignment(xlHorizontalAlignment);

			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
	Q_UNUSED(horizontalAlignment)
#endif
}

void QTExcel::setCellVerticalAlignment
(
	const QString& cellReference, 
	ExcelVAlign verticalAlignment
)
{	
#ifdef Q_OS_WIN
	if (_excelContext->_workSheet != Q_NULLPTR)
	{
		Excel::Range* range = _excelContext->_workSheet->Range(QVariant(cellReference));
		if (range != NULL)
		{
			Excel::XlVAlign xlVerticalAlignment = static_cast<Excel::XlVAlign>(verticalAlignment);
			range->SetVerticalAlignment(xlVerticalAlignment);

			delete range;
		}
	}
#else
	Q_UNUSED(cellReference)
	Q_UNUSED(verticalAlignment)
#endif
}

QString QTExcel::produceCell
(
	const QString& column,	// A, B...
	quint32 row				// 1 ,2 ...
)
{
	QString result = QString("%1%2").arg(column).arg(row);

	return result;
}

// Produces a range like A1:B2
QString QTExcel::produceRange
(
	const QString& column, 		// A, B...
	quint32 row,				// 1 ,2 ...
	const QString& column2, 	// A, B...
	quint32 row2				// 1 ,2 ...
)
{
	QString result = QString("%1%2:%3%4").arg(column).arg(row).arg(column2).arg(row2);

	return result;
}

QString QTExcel::stringFromColumnIndex
(
	quint32 column
)
{
	quint32 numeric = (column - 1) % 26;
	char letter = static_cast<char>(65 + numeric);
	quint32 num2 = (column - 1) / 26;
	if (num2 > 0)
		return stringFromColumnIndex(num2) + QString(letter);
	else
		return QString(letter);
}
