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

#ifndef MULTISCREENSHOTSDIALOG_H_
#define MULTISCREENSHOTSDIALOG_H_

#include <QDialog>
#include <QString>
#include <QColor>
#include <QImage>
#include <QVector>
#include <QSize>
#include "ScreenshotDialog.h"
#include <GLC_Vector3d>

#include "ui_MultiScreenshotsDialog.h"
#include "FileEntry.h"

class OpenglView;
class MultiShotsOpenglView;

class MultiScreenshotsDialog : public QDialog, private Ui::MultiScreenshotsDialog
{
	Q_OBJECT

public:
	MultiScreenshotsDialog(OpenglView*, QWidget*);
	virtual ~MultiScreenshotsDialog();

//////////////////////////////////////////////////////////////////////
// Public set function
//////////////////////////////////////////////////////////////////////
public:
	//! Initialize the dialog
	void initScreenshotDialog(FileEntry);

//////////////////////////////////////////////////////////////////////
// Public Get function
//////////////////////////////////////////////////////////////////////
public:
	//! Screenshot have to be saved
	void accept();

	//! Screenshot have not to be saved
	void reject();

//////////////////////////////////////////////////////////////////////
// Private Slot function
//////////////////////////////////////////////////////////////////////
private slots:
	//! The size off the target image changed
	void targetImageSizeChanged();

	//! Activate default background
	void activateDefaultBackground();

	//! Activate color background
	void activateColorBackground();

	//! Activate image background
	void activateImageBackground();

	//! Edit backround color
	void editBackgroundColor();

	//! Predefined size change
	void predefinedSizeChanged(int);

	//! Browse background Image
	void BrowseBackgroundImage();

	//! The number of shots have been changed
	void numberOfShotsChanged(int);

	//! The motion length changed
	void motionLengthChange(int);

	//! The gravity Axis Change
	void gravityAxisChange();

	//! Reverse rotation
	void reverseRotation();


//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////
private:
	//! Update the preview
	void updatePreview();

	//! Create predefined size list
	void createPredefinedSizeList();

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The Opengl View
	OpenglView* m_pOpenglView;

	//! The Screenshot
	QImage m_ScreenShot;

	//! The current Background color
	QColor m_CurrentBackgroundColor;

	//! The current Background image name
	QString m_CurrentBackgroundImageName;

	//! The screenshot Aspect ration
	double m_ScreenshotAspectRatio;

	//! The image aspect ration
	double m_ImageAspectRatio;

	//! The preview max size
	QSize m_PreviewSize;

	//!The current background mode
	BackGroundMode m_BackGroundMode;

	//!The predefined Size
	PredefinedSizes m_PredefinedSizes;

	//!The size of the target image
	QSize m_TargetImageSize;

	//! The default Image Name
	QString m_DefaultImageName;

	//! The previous filePath
	QString m_PreviousFilePath;

	//! the Maximum buffer Size
	int m_MaxBufferSize;

	//! The Opengl preview
	MultiShotsOpenglView* m_pMultiShotsOpenglView;

	//! The rotation Axis
	GLC_Vector3d m_RotationAxis;
};
#endif /* MULTISCREENSHOTSDIALOG_H_ */
