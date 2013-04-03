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

#ifndef MODELMANAGERVIEW_H_
#define MODELMANAGERVIEW_H_

#include "ui_ModelManagerView.h"
#include <QWidget>
#include "../FileEntry.h"
#include "ModelStructure.h"

class ListOfMaterial;
class OpenglView;

class ModelManagerView : public QWidget, private Ui::ModelManagerView
{
	Q_OBJECT

public:
	ModelManagerView(OpenglView*, QAction*, QAction*, QAction*, QAction*, QPair<GLC_uint, GLC_StructOccurence*>*, QWidget *parent= 0);
	virtual ~ModelManagerView();

//////////////////////////////////////////////////////////////////////
// Public get function
//////////////////////////////////////////////////////////////////////
public:

	//! Return true if the FileEntry is the current One
	bool isCurrentFileEntry(const FileEntry& entry) const
	{
		return entry == m_FileEntry;
	}
//////////////////////////////////////////////////////////////////////
// Public set function
//////////////////////////////////////////////////////////////////////
public:
	//! Set the file Entry (Model)
	void setModel(const FileEntry&);

	//! Update the view
	void updateView();

	//! Disconnect Slots
	inline void disconnectSlots()
	{
		m_pModelStructure->disconnectSlots();
	}

	//! Disconnect Slots
	inline void connectSlots()
	{
		m_pModelStructure->connectSlots();
	}

	//! Clear the model manager view
	void clear();

	//! Update tree show/no show
	inline void updateTreeShowNoShow()
	{
		m_pModelStructure->updateTreeShowNoShow();
	}

	//! Update tree show/no show
	inline void updateSelectedTreeShowNoShow()
	{
		m_pModelStructure->updateSelectedTreeShowNoShow();
	}

//////////////////////////////////////////////////////////////////////
// Signals
//////////////////////////////////////////////////////////////////////
signals:
	//! Current model properties
	void currentModelProperties();

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private slots:

	//! Display the model structure
	void displayModelStructure();


//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The View
	OpenglView* m_pOpenglView;

	//! The entry to view
	FileEntry m_FileEntry;

	//! The view as been already updated
	bool m_ViewIsUpToDate;

	//! The model's Structure
	ModelStructure* m_pModelStructure;

};

#endif /* MODELMANAGERVIEW_H_ */
