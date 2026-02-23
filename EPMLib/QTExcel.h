#ifndef QTEXCEL_H
#define QTEXCEL_H

//The MIT License
//
//Copyright (c) 2008 Michael Simpson
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


// QT
#include <QtGlobal>
#include <QList>
#include <QString>
#include <QVariant>

#ifndef BUILD_STATIC
#	if defined(EXCEL_LIBRARY)
#		define EXCEL_EXPORT Q_DECL_EXPORT
#	else
#		define EXCEL_EXPORT Q_DECL_IMPORT
#	endif
#else
#	define EXCEL_EXPORT
#endif

struct ExcelContext;

struct sRGB
{
	sRGB() = default;
	sRGB(quint16 red, quint16 green, quint16 blue)
	{
		_red = red;
		_green = green;
		_blue = blue;
	}
	sRGB(const sRGB& copyMe) = default;
	sRGB& operator= (const sRGB& assignMe) = default;

	quint16 _red{0};
	quint16 _green{0};
	quint16 _blue{0};
};

class EXCEL_EXPORT QTExcel
{
public:

	QTExcel();
	~QTExcel();

	enum ExcelBorder
	{
		eInsideHorizontal = 1,
		eInsideVertical = 2,
		eDiagonalDown =  4,
		eDiagonalUp  = 8,
		eEdgeBottom = 16,
		eEdgeLeft = 32,
		eEdgeRight = 64,
		eEdgeTop = 128,
		eLastBorderMask = eEdgeTop
	};

	Q_DECLARE_FLAGS(ExcelBorders, ExcelBorder)

	enum ExcelHAlignment
	{
		eHAlignCenter          = -4108,
		eHAlignCenterAcrossSelection= 7,
		eHAlignDistributed     = -4117,
		eHAlignFill            = 5,
		eHAlignGeneral         = 1,
		eHAlignJustify         = -4130,
		eHAlignLeft            = -4131,
		eHAlignRight           = -4152
	};

	enum ExcelVAlign
	{
		eVAlignBottom          = -4107,
		eVAlignCenter          = -4108,
		eVAlignDistributed     = -4117,
		eVAlignJustify         = -4130,
		eVAlignTop             = -4160
	};

	enum ExcelLineStyle
	{
		eContinuous            = 1,
		eDash                  = -4115,
		eDashDot               = 4,
		eDashDotDot            = 5,
		eDot                   = -4118,
		eDouble                = -4119,
		eSlantDashDot          = 13,
		eLineStyleNone         = -4142
	};

	enum ExcelBorderWeight
	{
		eHairline              = 1,
		eMedium                = -4138,
		eThick                 = 4,
		eThin                  = 2
	};

	static bool excelAvailable();

	bool create(bool visible);
	bool open(const QString& excelXLSFile, bool visible = true);
	bool save();
	bool saveAs(const QString& xlsFile);
	void screenUpdating(bool updateScreen);
	void displayAlerts(bool displayAlerts);
	void setVisible(bool visible);
	bool version(quint16& major, quint16& minor);

	void quit();

	QStringList workBookNames();
	int workBookCount();
	bool selectWorkbook(int index);
	bool selectWorkbook(const QString& sheetName);
	bool activateWorkbook();

	QStringList workSheetNames();
	int workSheetCount();
	bool selectWorksheet(quint32 index);
	bool selectWorksheet(const QString& sheetName);
	bool activateWorksheet();
	bool renameSheet(const QString& newName);
	void setWorksheetGridLines(bool showGridlines);
	bool deleteWorksheet(quint32 index);

	void setColumnWidth(const QString& cellReferece, quint32 width);

	QVariant getCellValue(const QString& cellReference);
	quint32 getCellValues(const QString& cellReference, QList<QVariant>& results);

	void setCellValue(const QString& cellReference, const QString& cellValue);
	void setCellValue(const QString& cellReference, const QVariant& cellValue);

	void setCellValues(const QString& cellReference, const QStringList& cellValues);
	void setCellValues(const QString& cellReference, const QList<QVariant>& cellValues);

	sRGB getCellInteriorColor(const QString& cellReference);

	// Setting Fonts
	void setCellFont(const QString& cellReference, const QString& fontName);
	void setCellBold(const QString& cellReference, bool bold);
	void setCellFontSize(const QString& cellReference, int fontSize);
	void setCellFontColor(const QString& cellReference, const sRGB& color);
	void formatCharacterColor(const QString& cellReference, quint32 start, quint32 end, const sRGB& color);

	// Setting Formats
	void autoFitColumns(const QString& cellReference);
	void setColumnWidths(int firstColumn, int lastColumn, qreal columnWidth);
	void setCellInteriorColor(const QString& cellReference, const sRGB& color);

	void setCellHorizontalAlignment(const QString& cellReference, ExcelHAlignment horizontalAlignment);
	void setCellVerticalAlignment(const QString& cellReference, ExcelVAlign verticalAlignment);

	// Setting Borders
	void setCellBorderStyle(const QString& cellReference, ExcelLineStyle lineStyle,
		ExcelBorderWeight lineWeight, const sRGB& color);
	void setCellBorderElement(const QString& cellReference, quint32 borderMask, ExcelLineStyle lineStyle,
		ExcelBorderWeight lineWeight, const sRGB& color);

	QString produceCell(const QString& column, quint32 row);
	QString produceRange(const QString& column, quint32 row, const QString& column2, quint32 row2);

	QString stringFromColumnIndex(quint32 column);

private:
	bool openExcel(bool visible);

	ExcelContext*				_excelContext{Q_NULLPTR};
};

#endif
