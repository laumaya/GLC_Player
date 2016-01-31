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

#ifndef EDITPOSITIONDIALOG_H_
#define EDITPOSITIONDIALOG_H_

#include <QDialog>
#include "ui_EditPosition.h"
#include <GLC_Matrix4x4>

class GLC_StructOccurrence;

class EditPositionDialog : public QDialog, private Ui::EditPosition
{
	Q_OBJECT
	
public:
	EditPositionDialog(GLC_StructOccurrence* pSTructOccurence, QWidget *pParent = 0);
	~EditPositionDialog();
		
public slots:
	//! Reject modifications
	virtual void reject();

private slots:

	//! Reject modifications
	void updateMatrix();

signals:
	//! Position updated
	void positionUpdated();

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////		
private:
	GLC_StructOccurrence* m_pOccurence;
	GLC_Matrix4x4 m_Matrix;
	
};

#endif /*EDITPOSITIONDIALOG_H_*/
