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

#ifndef EDITCAMERA_H_
#define EDITCAMERA_H_

#include "ui_EditCamera.h"
#include <QWidget>

class GLC_Viewport;

class EditCamera : public QWidget, private Ui::EditCamera
{
	Q_OBJECT
//////////////////////////////////////////////////////////////////////
/*!@name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////	
public:
	//! Default constructor
	EditCamera(GLC_Viewport*, QWidget *parent= 0);
	virtual ~EditCamera();
//@}
signals:
	void valueChanged();
//////////////////////////////////////////////////////////////////////
/*!\name Public slots Functions*/
//@{
//////////////////////////////////////////////////////////////////////		    
public slots:

	//! Update dialog values
	void updateValues();
//@}
	
//////////////////////////////////////////////////////////////////////
/*! \name Private slots Functions*/
//@{
//////////////////////////////////////////////////////////////////////		    
private slots:
	void updateFov();
	void updateEye();
	void updateTarget();
	void updateDistance();
//@}
	
//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////		
private:
	//! The View
	GLC_Viewport* m_pView;


};

#endif /*EDITCAMERA_H_*/
