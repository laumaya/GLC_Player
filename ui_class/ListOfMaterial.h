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

#ifndef LISTOFMATERIAL_H_
#define LISTOFMATERIAL_H_

#include "ui_ListOfMaterial.h"
#include <QWidget>
#include <QList>
#include <QSize>
#include <GLC_World>
#include <GLC_Camera>

class GLC_Material;
class OpenglView;
class GLC_Geometry;
class QGLFramebufferObject;

class ListOfMaterial : public QWidget, private Ui::ListOfMaterial
{
	Q_OBJECT
public:
	ListOfMaterial(OpenglView*, QWidget *parent= 0);
	virtual ~ListOfMaterial();
//////////////////////////////////////////////////////////////////////
// Public set function
//////////////////////////////////////////////////////////////////////
public:
	//! Create or Update the list
	void CreateOrUpdate(QList<GLC_Material*>&);

	//! Clear the list of material
	inline void clear()
	{
		m_MaterialList.clear();
		materialList->clear();
		numberOfSubMaterial->clear();
	}

	//! Show only item list
	void showOnlyItemList(bool);

signals:
	//! The current Item as been changed
	void updateMaterialSignal(GLC_Material*);

//////////////////////////////////////////////////////////////////////
// Public Slot function
//////////////////////////////////////////////////////////////////////
public slots:
	//! Update the current row
	void updateRow();

//////////////////////////////////////////////////////////////////////
// Private Slot function
//////////////////////////////////////////////////////////////////////
private slots:
	//! The current item as been changed
	void updateMaterialProperty(int);

//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////
private:
	//! init material representatio world
	void initWorld();

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The list of material to display
	QList<GLC_Material*> m_MaterialList;

	//! The View
	OpenglView* m_pOpenglView;

	//! Preview Model
	GLC_World m_World;
	GLC_Camera m_Camera;
	GLC_Geometry* m_pGeom;

	//! Item size
	QSize m_IconSize;

	//! The Frame buffer
	QGLFramebufferObject* m_pFrameBuffer;

	//! The frame buffer Size
	QSize m_FrameBufferSize;

};

#endif /* LISTOFMATERIAL_H_ */
