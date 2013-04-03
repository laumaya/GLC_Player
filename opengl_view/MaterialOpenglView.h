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

#ifndef MATERIALOPENGLVIEW_H_
#define MATERIALOPENGLVIEW_H_

#include <QGLWidget>
#include <GLC_Viewport>
#include <GLC_World>
#include <GLC_Light>
#include <GLC_Material>

class MaterialOpenglView  : public QGLWidget
{
	Q_OBJECT
public:
	MaterialOpenglView(QWidget *, const QGLWidget*, GLC_Material*);

	virtual ~MaterialOpenglView();

//////////////////////////////////////////////////////////////////////
// Public Set Functions
//////////////////////////////////////////////////////////////////////
public:
	//! update the view with specfied material
	void update(GLC_Material*);

//////////////////////////////////////////////////////////////////////
// Protected services Functions
//////////////////////////////////////////////////////////////////////
protected:
	//! Initialize OpenGL window
	void initializeGL();

	//! paint in the opengl widget
	void paintGL();

	//! The widget as been resized
	void resizeGL(int width, int height);

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The viewport
	GLC_Viewport m_GlView;

	//! The world
	GLC_World m_World;

	//! The Light
	GLC_Light m_Light;

	//! The geometry to display
	GLC_Geometry* m_pGeom;

	//! The current material
	GLC_Material* m_pMaterial;

};

#endif /* MATERIALOPENGLVIEW_H_ */
