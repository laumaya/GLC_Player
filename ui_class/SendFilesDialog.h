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

#ifndef SENDFILESDIALOG_H_
#define SENDFILESDIALOG_H_

#include <QDialog>
#include <QString>

#include "ui_SendFilesDialog.h"
#include "FileEntry.h"

class SendFilesDialog : public QDialog, private Ui::SendFilesDialog
{
	Q_OBJECT
public:
	SendFilesDialog(FileEntryHash*, QWidget *);
	~SendFilesDialog();

public:
	//! update dialog view
	void updateView(const QString&);

	//! Send the files
	bool sendFiles();

	//! Copy Album
	inline bool copyAlbum() const {return copyAlbumFile->checkState() == Qt::Checked;}

	//! Update Album
	inline bool updateAlbum() const {return updateCurrentAlbum->checkState() == Qt::Checked;}

	//! Return the new album file name
	inline QString newAlbumFileName() const {return m_NewAlbumFileName;}

private slots:
	//! Browse for destination directory
	void browse();

	//! Send All File
	void sendAllFiles();

	//! Send Selected File
	void sendSelectedFile();

	//! Not Send All Files
	void notSendAllFiles();

	//! Not Sent Selected File
	void notSendSelectedFile();

	//! The origin selection have been changed
	void originSelectionChanged();

	//! The destination selection have been changed
	void destinationSelectionChanged();

	//! Update Target File path
	void updateTargetFilePath();

	//! Create sub folder state change
	void createFolderStateChanged(int);

	//! Copy album file state changed
	void copyAlbumFileStateChanged(int);

	//! The album name as changed
	void albumNameTextChanged();

//////////////////////////////////////////////////////////////////////
// private services function
//////////////////////////////////////////////////////////////////////
private:
	//! Convert size from double to String
	QString sizeFromDoubleToString(const double&);

	//! Change the location of an QTreeWidgetItem
	void changeLocation(QTreeWidgetItem*, QString);

//////////////////////////////////////////////////////////////////////
// private member
//////////////////////////////////////////////////////////////////////
private:
	//! File Entry Hash table
	FileEntryHash* m_pFileEntryHash;

	//! The size of sent files
	double m_SentFilesSize;

	//! The target Path
	QString m_TargetPath;

	//! create Sub Folder
	bool m_createSubFolder;

	//! The Album File Name
	QString m_AlbumFileName;

	//! Copy Album file
	bool m_copyAlbumFile;

	//! The new Album File name
	QString m_NewAlbumFileName;

};

#endif /* SENDFILESDIALOG_H_ */
