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

#include "MaterialOpenglView.h"

#include <GLC_Factory>
#include <GLC_Exception>
#include <GLC_Context>

MaterialOpenglView::MaterialOpenglView(QWidget * pParent, const QGLWidget* pShareWidget, GLC_Material* pMaterial)
: QGLWidget(new QGLContext(QGLFormat()), pParent, pShareWidget)
, m_GlView()
, m_World()
, m_Light()
, m_pGeom(NULL)
, m_pMaterial(pMaterial)
{
	m_Light.setPosition(1.0, 1.0, 1.0);

}

MaterialOpenglView::~MaterialOpenglView()
{

}

//////////////////////////////////////////////////////////////////////
// Public Set Functions
//////////////////////////////////////////////////////////////////////

// update the view with specfied material
void MaterialOpenglView::update(GLC_Material* pMaterial)
{
	m_pMaterial= pMaterial;
	m_pGeom->replaceMasterMaterial(m_pMaterial);
	//m_pGeom->setTransparency(m_pMaterial->isTransparent());
	updateGL();
}

//////////////////////////////////////////////////////////////////////
// Portected services Functions
//////////////////////////////////////////////////////////////////////

// Initialize OpenGL window
void MaterialOpenglView::initializeGL()
{

	m_GlView.initGl();

	glEnable(GL_NORMALIZE);

	// Set background to dark gray
	m_GlView.setBackgroundColor(Qt::darkGray);
	m_GlView.deleteBackGroundImage();

	// Create the cylinder
	GLC_Factory* pFactory= GLC_Factory::instance();
	GLC_3DRep cylinder= pFactory->createCylinder(1.0, 3.0);
	m_pGeom= cylinder.geomAt(0);
	m_pGeom->replaceMasterMaterial(m_pMaterial);
	//m_pGeom->setTransparency(m_pMaterial->isTransparent());
    m_World.rootOccurrence()->addChild(new GLC_StructOccurrence(new GLC_3DRep(cylinder)));

	// Define position of the camera eye
	const GLC_Vector3d VectEye(0.0, -3.0, 1.5);
	// of the target
	const GLC_Vector3d VectTarget;
	// Up vector
	GLC_Vector3d VectUp(0.0, 1.0, 0.0);
	m_GlView.cameraHandle()->setCam(VectEye, VectTarget, VectUp);
	m_GlView.reframe(m_World.boundingBox());
	m_GlView.setDistMinAndMax(m_World.boundingBox());

}

// paint in the opengl widget
void MaterialOpenglView::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLC_Context::current()->glcLoadIdentity();

	// Enable and execute lighting
	try
	{
		m_Light.glExecute();
		m_GlView.glExecuteCam();

		// Display non transparent normal object
		m_World.render(0, glc::ShadingFlag);
		// Display transparent normal object
		m_World.render(0, glc::TransparentRenderFlag);

	}
	catch (GLC_Exception &e)
	{
		qDebug() << e.what();
	}

}

// The widget as been resized
void MaterialOpenglView::resizeGL(int width, int height)
{
	m_GlView.setWinGLSize(width, height);
}
