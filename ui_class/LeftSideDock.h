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

#ifndef LEFTSIDEDOCK_H_
#define LEFTSIDEDOCK_H_

#include "ui_LeftSideDock.h"
#include <QWidget>
#include "../FileEntry.h"
#include "ModelManagerView.h"

class AlbumManagerView;


class LeftSideDock : public QWidget, private Ui::LeftSideDock
{
	Q_OBJECT

public:

	LeftSideDock(AlbumManagerView*, ModelManagerView*, FileEntryHash*, QWidget *parent= 0);

	virtual ~LeftSideDock();

//////////////////////////////////////////////////////////////////////
// Public set functions
//////////////////////////////////////////////////////////////////////
public:
	//! The current modelChanged
	void currentModelChanged();

	//! Update tree show/no show
	inline void updateTreeShowNoShow()
	{
		if (stackedWidget->currentIndex() == 1)
			{m_pModelManagerView->updateTreeShowNoShow();}
	}

	//! Update tree show/no show
	inline void updateSelectedTreeShowNoShow()
	{
		if (stackedWidget->currentIndex() == 1)
			{m_pModelManagerView->updateSelectedTreeShowNoShow();}
	}

//////////////////////////////////////////////////////////////////////
// Private slots
//////////////////////////////////////////////////////////////////////
private slots:
	// Change the current manager (between Album and Model Manager)
	void changeCurrentManager(bool);

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The Album manager view
	AlbumManagerView* m_pAlbumManagerView;

	//! The Model manager view
	ModelManagerView* m_pModelManagerView;

	//! The Album Model
	FileEntryHash* m_pAlbumModel;

};

#endif /* LEFTSIDEDOCK_H_ */
