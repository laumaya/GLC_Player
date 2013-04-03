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

#include "OpenAlbumOption.h"

OpenAlbumOption::OpenAlbumOption(QWidget *parent)
: QDialog(parent)
, m_SaveCurrentAlbum()
, m_ReplaceCurrentAlbum()
{
	setupUi(this);

	m_SaveCurrentAlbum= (saveCurrentAlbum->checkState() == Qt::Checked);
	m_ReplaceCurrentAlbum= replaceCurrentAlbum->isChecked();

	updateLabel();

	// Signals and slot connection
	connect(saveCurrentAlbum, SIGNAL(stateChanged(int)), this, SLOT(updateLabel()));
	connect(replaceCurrentAlbum, SIGNAL(clicked()), this, SLOT(updateLabel()));
	connect(addToCurrentAlbum, SIGNAL(clicked()), this, SLOT(updateLabel()));
}

OpenAlbumOption::~OpenAlbumOption()
{

}

// Accept modifications
void OpenAlbumOption::accept()
{
	m_SaveCurrentAlbum= (saveCurrentAlbum->checkState() == Qt::Checked);
	m_ReplaceCurrentAlbum= replaceCurrentAlbum->isChecked();

	QDialog::accept();
}

// Update label
void OpenAlbumOption::updateLabel()
{
	QString message;
	m_SaveCurrentAlbum= (saveCurrentAlbum->checkState() == Qt::Checked);
	m_ReplaceCurrentAlbum= replaceCurrentAlbum->isChecked();

	if (m_SaveCurrentAlbum)
	{
		message= tr("Save the current album and ");
	}
	if (m_ReplaceCurrentAlbum)
	{
		message+= tr("Replace the current Album");
	}
	else
	{
		message+= tr("Add to the current Album");
	}
	messageLabel->setText(message);
}
