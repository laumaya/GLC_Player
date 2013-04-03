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

#ifndef SELECTIONPROPERTY_H_
#define SELECTIONPROPERTY_H_

#include "ui_SelectionProperty.h"
#include <GLC_Global>
#include <GLC_3DViewCollection>
#include <QWidget>

class GLC_Mesh2;

class SelectionProperty : public QWidget, private Ui::SelectionProperty
{
	Q_OBJECT

//////////////////////////////////////////////////////////////////////
/*!@name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	SelectionProperty(QAction*, QAction*, QAction*, QWidget *parent= 0);
	virtual ~SelectionProperty();
//@}

//////////////////////////////////////////////////////////////////////
/*!@name Public methods */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set the current selection
	void setSelection(PointerViewInstanceHash*);
	//! Unset the current selection
	void unsetSelection();
	//! Enable state of editProperty command
	inline void editPropertySetEnabled(const bool flag)
	{ editPropertyCmd->setEnabled(flag);}

//@}

private slots:
	//! Reverse selection normals
	void reverseNormals();

//////////////////////////////////////////////////////////////////////
// Signals
//////////////////////////////////////////////////////////////////////
signals:
	//! The view must be updated
	void updateView();
//////////////////////////////////////////////////////////////////////
/*!@name Private members */
//@{
//////////////////////////////////////////////////////////////////////
private:
	PointerViewInstanceHash* m_pSelections;
};

#endif /*SELECTIONPROPERTY_H_*/
