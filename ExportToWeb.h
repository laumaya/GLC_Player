/****************************************************************************

 This file is part of GLC-Player.
 Copyright (C) 2007-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 2.2.0, packaged on July 2010.

 http://www.glc-player.net

 GLC-Player is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GLC-Player is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GLC-Player; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

#ifndef EXPORTTOWEB_H_
#define EXPORTTOWEB_H_

#include "FileEntry.h"
#include <QObject>
#include <QString>
#include <QSize>
#include <QProgressDialog>

class OpenglView;
class QXmlStreamWriter;

class ExportToWeb : public QObject
{
	Q_OBJECT

public:
	ExportToWeb(QList<FileEntry>&, OpenglView*);
	virtual ~ExportToWeb();

//////////////////////////////////////////////////////////////////////
// Public Get function
//////////////////////////////////////////////////////////////////////
public:
	//! Get the number of models to export
	inline int numberOfModel() const {return m_FileEntrySortedList.size();}

	//! Return the Index Path Name
	inline QString indexPathName() const {return m_TargetPath + QDir::separator() + QString("index.html");}

//////////////////////////////////////////////////////////////////////
// Public Set function
//////////////////////////////////////////////////////////////////////
public:
	//! Set the Album Name
	inline void setAlbumName(const QString& name) {m_AlbumName= name;}

	//! Set Thumbnail size
	void setThumbnailSize(const QSize&);

	//! Set Image size
	void setImageSize(const QSize&);

	//! Export Album to web
	bool exportToWeb(const QString&, const QString&);

	//! Update the number of row
	inline void updateNbrOfRow(int value) {m_NumberOfRow= value;}

	//! Update the number of lines
	inline void updateNbrOfLine(int value) {m_NumberOfLine= value;}

	//! Update the number of pages
	inline void updateNbrOfPage(int value) {m_NumberOfPages= value;}

	//! Set backgroundColor
	inline void setBackgroundColor(const QColor& color) {m_BackgroundColor= color;}

//////////////////////////////////////////////////////////////////////
// Public Slot function
//////////////////////////////////////////////////////////////////////
public slots:

	//! Export album and model
	void exportAlbumAndModel(bool);

	//! View Thumbnail model name
	void viewThumbnailModelName(bool);

	//! Link Thumbnail to model
	void linkThumbnailToModel(bool);

	//! View Image model informations
	void viewImageModelInformations(bool);

	//! Link Image to model
	void linkImageToModel(bool);

//////////////////////////////////////////////////////////////////////
// private services function
//////////////////////////////////////////////////////////////////////
private:

	//! Create sub directories structure
	bool createSubDirectories();

	//! Make SnapShots
	bool makeSnapShots();

	//! Export Album and model
	bool exportAlbumAndModels();

	//! Export Html
	bool exportHtml();

	//! Write html header
	void writeHtmlHeader(const QString&);

	//! Write html title
	void writeHtmlTitle(const QString&);

	//! Write html page link
	void writeHtmlPageLink(int);

	//! Return fileName
	QString createFileName(int);

	//! Write the table
	void writeHtmlTable(int);

	//! Write a cell
	void writeHtmlCell(int);

	//! Create the image name
	QString createImageName(int);

	//! Create model name
	QString createModelName(int);

	//! Write html images pages
	bool writeHtmlImagesPages();

	//! Create image page file name
	QString createImagePageFileName(int);

	//! Write html infomration cell
	void writeHtmlInformationCell(const QString&, const QString&);


//////////////////////////////////////////////////////////////////////
// private member
//////////////////////////////////////////////////////////////////////
private:
	//! File Entry sorted list
	QList<FileEntry> m_FileEntrySortedList;

	//! The Opengl View
	OpenglView* m_pOpenglView;

	//! The Album Name
	QString m_AlbumName;

	//! The Number of row
	int m_NumberOfRow;

	//! The Number of line
	int m_NumberOfLine;

	//! The Number of page
	int m_NumberOfPages;

	//! The Thumbnail size
	QSize m_ThumbnailSize;

	//! The Image Size
	QSize m_ImageSize;

	//! Export Album and Model
	bool m_ExportAlbumAndModel;

	//! View the thumbnail model name
	bool m_ViewThumbnailModelName;

	//! Link thumbnail to model
	bool m_LinkThumbnailToModel;

	//! View the image model informations
	bool m_ViewImageModelInformations;

	//! Link image to model
	bool m_LinkImageToModel;

	//! The target Path
	QString m_TargetPath;

	//! xml/html Writer
	QXmlStreamWriter* m_pHtmlWriter;

	//! Export to web progress dialog
	QProgressDialog m_QProgressDialog;

	//! Current progress
	int m_CurrentProgressDialog;

	//! Thumbnail path name
	const QString m_ThumbnailPathName;

	//! Image path name
	const QString m_ImagePathName;

	//! Html Image Page Path name
	const QString m_HtmlImagePagePathName;

	//! 3D models path name
	const QString m_3dModelPathName;

	//! Background Color
	QColor m_BackgroundColor;


};

#endif /* EXPORTTOWEB_H_ */
