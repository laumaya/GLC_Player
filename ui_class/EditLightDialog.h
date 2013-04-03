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

#ifndef EDITLIGHTDIALOG_H_
#define EDITLIGHTDIALOG_H_

#include "ui_EditLightDialog.h"

#include <QDialog>
#include <QList>
#include <QColor>
#include <QPicture>
#include <QListWidgetItem>

class GLC_Light;
class QGLContext;

//////////////////////////////////////////////////////////////////////
//! \class EditLightDialog
/*! \brief EditLightDialog : a dialog box for background setting*/
//////////////////////////////////////////////////////////////////////
class EditLightDialog : public QDialog, private Ui::EditLightDialog
{
	Q_OBJECT
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////	
public:
//! Default constructor
	EditLightDialog(GLC_Light* pLight, QList<GLC_Light*>* pLightList, QWidget *parent= 0);
//@}


signals:
	void lightUpdated();
	void currentLightChanged(int);
//////////////////////////////////////////////////////////////////////
/*! \name Private slots Functions*/
//@{
//////////////////////////////////////////////////////////////////////		    
private slots:
	//! Change Ambient color
	void changeAmbientColor();
	//! Change Diffuse color
	void changeDiffuseColor();
	//! Change Specular color
	void changeSpecularColor();
	//! Add light to the light list
	void addLight();
	//! Remove current light of the lighgt list
	void removeLight();
	//! The current light as changed
	void changeCurrentLight(int index);
	//! Accept modifications
	void accept();
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Private services Functions*/
//@{
//////////////////////////////////////////////////////////////////////		    
private:
	//! init background dialog values
	void initValues();
	//! update light values
	void updateValues();
	//! update colors
	void updateColors();
//@}
	
//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////		
private:
	GLC_Light* m_pMasterLight;
	
	QList<GLC_Light*>* m_pLightList;

	QColor m_AmbientColor;
	QColor m_DiffuseColor;
	QColor m_SpecularColor;
	
	GLC_Light* m_pCurrentLight;
	
};

#endif /*EDITLIGHTDIALOG_H_*/
