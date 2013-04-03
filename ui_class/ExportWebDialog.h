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

#ifndef EXPORTWEBDIALOG_H_
#define EXPORTWEBDIALOG_H_

#include "ui_ExportWebDialog.h"

#include <QDialog>
#include "FileEntry.h"

typedef QVector<QSize>  PredefinedSizes;
class ExportToWeb;
class OpenglView;

class ExportWebDialog : public QDialog, private Ui::ExportWebDialog
{
	Q_OBJECT

public:
	ExportWebDialog(QWidget *);
	virtual ~ExportWebDialog();

//////////////////////////////////////////////////////////////////////
// Public Get function
//////////////////////////////////////////////////////////////////////
public:
	//! Initialise the Dialog
	void initDialog(const QString&, QList<FileEntry>, OpenglView*);

protected:
	//! Export album to web
	virtual void accept();

	//! Reject export
	virtual void reject();

//////////////////////////////////////////////////////////////////////
// Private Slot function
//////////////////////////////////////////////////////////////////////
private slots:
	//! The size of the thumbnail changed
	void thumbnailSizeChanged(int);

	//! The size of the image changed
	void imageSizeChanged(int);

	//! Update the thumbnail predefined size combo
	void updateThumbnailPredifinedSize();

	//! Update the image predefined size combo
	void updateImagePredifinedSize();

	//! Update image panel enable status
	void updateImagePanelEnabledState(bool);

	//! Update Export button state
	void updateExportButton();

	//! The number of row as been changed
	void numberOfRowChange(int);

	//! The number of line as been changed
	void numberOfLineChange(int);

	//! Browse for destination directory
	void browse();

	//! The web Title as changed
	void webTitleChanged();

	//! createSubDir State Changed
	void createSubDirStateChanged(int);

	//! Edit backround color
	void editBackgroundColor();



//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////
private:
	//! Create the predefined thumbnail and size
	void createPredefinedThumbnailSize();

	//! Create the predefined image size
	void createPredefinedImageSize();

	//! number of page
	int numberOfPages() const;

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The thumbnail max size
	QSize m_ThumbnailSize;

	//! The Image max size
	QSize m_ImageSize;

	//!The predefined thumbnail Size
	PredefinedSizes m_PredefinedThumbnailSizes;

	//!The predefined image Size
	PredefinedSizes m_PredefinedImageSizes;

	//! The export to web class
	ExportToWeb* m_pExportToWeb;

	//! The number of images
	int m_NbrOfImage;

	//! The target Path
	QString m_TargetPath;

	//! The current background color
	QColor m_CurrentBackgroundColor;

};

#endif /* EXPORTWEBDIALOG_H_ */
