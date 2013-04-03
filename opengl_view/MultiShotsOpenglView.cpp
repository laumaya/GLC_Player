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

#include "MultiShotsOpenglView.h"
#include <GLC_Exception>
#include <GLC_State>
#include <GLC_Context>

MultiShotsOpenglView::MultiShotsOpenglView(QWidget * pParent, QGLWidget* pShareWidget)
: QGLWidget(new GLC_Context(QGLFormat(QGL::SampleBuffers)), pParent, pShareWidget)
, m_GlView()
, m_World()
, m_MotionTimer()
, m_RotationVector()
, m_Camera()
, m_Angle()
, m_pOpenglView(dynamic_cast<OpenglView*>(pShareWidget))
, m_MotionStep(1)
, m_MotionLength(1000)
{

	// Set backroundImage
	m_GlView.loadBackGroundImage(":images/default_background.png");

	// Signal and slot connection
	connect(&m_MotionTimer, SIGNAL(timeout()), this, SLOT(rotateView()));

}

MultiShotsOpenglView::~MultiShotsOpenglView()
{

}

//////////////////////////////////////////////////////////////////////
// Public Set Functions
//////////////////////////////////////////////////////////////////////
// Init the view
void MultiShotsOpenglView::initView()
{

	m_World= m_pOpenglView->getWorld();
	m_Camera= m_pOpenglView->getCamera();
	double xUp= fabs(m_Camera.upVector().x());
	double yUp= fabs(m_Camera.upVector().y());
	double zUp= fabs(m_Camera.upVector().z());
	if ((xUp > yUp) && (xUp > zUp))
	{
		m_RotationVector= glc::X_AXIS;
	}
	else if ((yUp > xUp) && (yUp > zUp))
	{
		m_RotationVector= glc::Y_AXIS;
	}
	else
	{
		m_RotationVector= glc::Z_AXIS;
	}
	m_Angle= m_pOpenglView->getViewAngle();

}

// Update the motion Step
void MultiShotsOpenglView::updateMotionStepAndLength(int step, int motionLength)
{
	m_MotionStep= step;
	// Second -> millisecond
	m_MotionLength= motionLength * 1000;

	m_MotionTimer.start(m_MotionLength / step);
}

// Update Gravity Axis
void MultiShotsOpenglView::updateGravityAxis(const GLC_Vector3d& newAxis)
{
	if (newAxis == -m_RotationVector)
	{
		m_RotationVector= newAxis;
	}
	else if (newAxis != m_RotationVector)
	{
		m_RotationVector= newAxis;
		(*m_GlView.cameraHandle())= m_Camera;
	}
}

//////////////////////////////////////////////////////////////////////
// Portected services Functions
//////////////////////////////////////////////////////////////////////

// Initialize OpenGL window
void MultiShotsOpenglView::initializeGL()
{
	m_GlView.initGl();

	glEnable(GL_NORMALIZE);

	(*m_GlView.cameraHandle())= m_Camera;
	m_GlView.setViewAngle(m_Angle);

}

// paint in the opengl widget
void MultiShotsOpenglView::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLC_Context::current()->glcLoadIdentity();
	m_GlView.setDistMinAndMax(m_World.collection()->boundingBox());

	try
	{
		m_pOpenglView->getLight()->glExecute();
		m_GlView.glExecuteCam();

		if (!m_pOpenglView->getLights()->isEmpty())
		{
			const int size= m_pOpenglView->getLights()->size();
			for (int i= 0; i < size; ++i)
			{
				m_pOpenglView->getLights()->operator[](i)->glExecute();
			}
		}


		// Test if there is a global shader
		const GLuint globalShaderId= m_pOpenglView->globalShaderId();
		if (0 != globalShaderId) GLC_Shader::use(globalShaderId);


		// Display non transparent normal object
		m_World.render(0, glc::ShadingFlag);
		// Display non transparent instance of the shaders group
		if (GLC_State::glslUsed())
		{
			m_World.renderShaderGroup(glc::ShadingFlag);
		}

		// Display transparent normal object
		m_World.render(0, glc::TransparentRenderFlag);
		// Display transparent instance of the shaders group
		if (GLC_State::glslUsed())
		{
			m_World.renderShaderGroup(glc::TransparentRenderFlag);
		}
		// Display Selected Objects
		const int numberOfSelectedNode= m_World.collection()->selectionSize();
		if ((numberOfSelectedNode > 0) && GLC_State::selectionShaderUsed())
		{

			if (numberOfSelectedNode != m_World.collection()->drawableObjectsSize())
			{
				//Draw the selection with Zbuffer
				m_World.render(1);
			}
			glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
			// Draw the selection transparent
	        glEnable(GL_CULL_FACE);
	        glEnable(GL_BLEND);
	        glDepthFunc(GL_ALWAYS);
	        glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	        m_World.render(1, glc::ShadingFlag);

    	    // Restore attributtes
	        glPopAttrib();
		}
		else if (numberOfSelectedNode > 0)
		{
			m_World.render(1, glc::ShadingFlag);
		}
		// Test if there is a global shader
		if (0 != globalShaderId) GLC_Shader::unuse();

	}
	catch (GLC_Exception &e)
	{
		qDebug() << e.what();
	}

}

// The widget as been resized
void MultiShotsOpenglView::resizeGL(int width, int height)
{
	m_GlView.setWinGLSize(width, height);
}

//////////////////////////////////////////////////////////////////////
// Private slots Functions
//////////////////////////////////////////////////////////////////////
// Rotate the view
void MultiShotsOpenglView::rotateView()
{
	m_GlView.cameraHandle()->rotateAroundTarget(m_RotationVector, 2.0 * glc::PI / static_cast<double>(m_MotionStep));
	updateGL();
}
