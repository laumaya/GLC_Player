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

#ifndef MULTISHOTSOPENGLVIEW_H_
#define MULTISHOTSOPENGLVIEW_H_
#include <QGLWidget>
#include <QTimer>

#include <GLC_Viewport>
#include <GLC_World>
#include <GLC_Light>
#include <GLC_Vector3d>

#include <GLC_Camera>
#include "../FileEntry.h"
#include "OpenglView.h"

class MultiShotsOpenglView  : public QGLWidget
{
	Q_OBJECT
public:
	MultiShotsOpenglView(QWidget *, QGLWidget* pShareWidget);

	virtual ~MultiShotsOpenglView();

//////////////////////////////////////////////////////////////////////
// Public Get Functions
//////////////////////////////////////////////////////////////////////
public:
	//! Return the current Gravity Axis
	inline GLC_Vector3d currentGravityAxis() const {return m_RotationVector;}
//////////////////////////////////////////////////////////////////////
// Public Set Functions
//////////////////////////////////////////////////////////////////////
public:
	//! Init the view
	void initView();

	//! Return a pointer to the viewport
	inline GLC_Viewport* viewport() {return &m_GlView;}

	//! Update the motion Step and length
	void updateMotionStepAndLength(int, int);

	//! Update Gravity Axis
	void updateGravityAxis(const GLC_Vector3d&);

//////////////////////////////////////////////////////////////////////
// Protected services Functions
//////////////////////////////////////////////////////////////////////
private:
	//! Initialize OpenGL window
	void initializeGL();

	//! paint in the opengl widget
	void paintGL();

	//! The widget as been resized
	void resizeGL(int width, int height);
//////////////////////////////////////////////////////////////////////
// Private slots Functions
//////////////////////////////////////////////////////////////////////
private slots:
	//! Rotate the view
	void rotateView();
//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The viewport
	GLC_Viewport m_GlView;

	//! The world
	GLC_World m_World;

	//! The timer used for motion
	QTimer m_MotionTimer;

	//! The rotation vector
	GLC_Vector3d m_RotationVector;

	//! The View Camera
	GLC_Camera m_Camera;

	//! The View Angle
	double m_Angle;

	//! The main OpenGl view
	OpenglView* m_pOpenglView;

	//! The motion Step
	int m_MotionStep;

	//! The motion length
	int m_MotionLength;

};

#endif /* MULTISHOTSOPENGLVIEW_H_ */
