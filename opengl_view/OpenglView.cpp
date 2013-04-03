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
#include "OpenglView.h"
#include <GLC_3DViewInstance>
#include <GLC_Interpolator>
#include <GLC_Exception>
#include <GLC_SelectionMaterial>
#include <GLC_State>
#include <GLC_Line3d>
#include <GLC_GeomTools>
#include <GLC_Octree>
#include <GLC_CuttingPlane>
#include <GLC_Axis>
#include <GLC_Global>
#include <GLC_FlyMover>
#include <GLC_RenderStatistics>
#include <GLC_UserInput>
#include <GLC_Context>

// For VSYNC problem under Mac OS X
#if defined(Q_OS_MAC)
#include <OpenGL.h>
#endif

// List of usable shader
ShaderList OpenglView::m_ShaderList;

OpenglView::OpenglView(QWidget *pParent)
: QGLWidget(new GLC_Context(QGLFormat(QGL::SampleBuffers)),pParent)
, m_GlView()
, m_MoverController()
, m_World()
, m_Light()
, m_Mode(GL_FILL)
, m_ViewState(V_NORMAL)
, m_ViewEnterState(VE_NORMAL)
, m_MotionStep(20)
, m_CurrentFps(0.0f)
, m_infoFont()
, m_SelectionMode(false)
, m_dislayInfoPanel(true)
, m_SnapShootMode(false)
, m_BlockSelection(false)
, m_GlobalShaderId(0)
, m_GlobalShaderName(tr("No Shader"))
, m_pQGLFramebufferObject(NULL)
, m_CaptureSize()
, m_SmoothCaptures(false)
, m_UiCollection()
, m_CurrentMoverType(GLC_MoverController::TrackBall)
, m_RenderFlag(glc::ShadingFlag)
, m_PlanePos()
, m_pClipPlane(NULL)
, m_3dWidgetCollection()
, m_3DWidgetManager(&m_GlView)
, m_LightAxisWidgetId(0)
, m_CuttingPLaneID(0)
, m_UserLights()
, m_CurrentLightIndex(-1)
{

	connect(&m_GlView, SIGNAL(updateOpenGL()), this, SLOT(updateGL()));
	//setMouseTracking(true);
	m_Light.setPosition(1.0, 1.0, 1.0);
	m_Light.setName(tr("Master Light"));

	// Set backroundImage
	m_GlView.loadBackGroundImage(":images/default_background.png");

	//Use the default mover controller
	QColor repColor;
	repColor.setRgbF(1.0, 0.11372, 0.11372, 0.8);
	m_MoverController= GLC_Factory::instance()->createDefaultMoverController(repColor, &m_GlView);

	connect(&m_MoverController, SIGNAL(repaintNeeded()), this, SLOT(updateGL()));
	// Create other UI element
	GLC_3DViewInstance line= GLC_Factory::instance()->createLine(GLC_Point3d(), glc::X_AXIS);
	line.geomAt(0)->setWireColor(Qt::red);
	m_UiCollection.add(line);
	line= GLC_Factory::instance()->createLine(GLC_Point3d(), glc::Y_AXIS);
	line.geomAt(0)->setWireColor(Qt::darkGreen);
	m_UiCollection.add(line);
	line= GLC_Factory::instance()->createLine(GLC_Point3d(), glc::Z_AXIS);
	line.geomAt(0)->setWireColor(Qt::blue);
	m_UiCollection.add(line);
}

OpenglView::~OpenglView()
{
	GLC_SelectionMaterial::deleteShader(context());
	if (!m_ShaderList.isEmpty())
	{
		const int size= m_ShaderList.size();
		for (int i= 0; i < size; ++i)
		{
			delete m_ShaderList[i];
		}
		m_ShaderList.clear();
	}

	const int lightCount= m_UserLights.size();
	for (int i= 0; i < lightCount; ++i)
	{
		delete m_UserLights.at(i);
	}
}

//////////////////////////////////////////////////////////////////////
// Public Interface
//////////////////////////////////////////////////////////////////////

// Initialyze OpenGL window
void OpenglView::initializeGL()
{
	m_GlView.cameraHandle()->setDefaultUpVector(glc::Z_AXIS);
	m_GlView.initGl();
	// For VSYNC problem under Mac OS X
	#if defined(Q_OS_MAC)
	const GLint swapInterval = 1;
	CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &swapInterval);
	#endif

	glEnable(GL_NORMALIZE);

	GLC_RenderStatistics::setActivationFlag(true);
	emit glInitialed();
}

// Reframe the Scene
void OpenglView::reframe(const GLC_BoundingBox& boundingBox, bool motion)
{
	const GLC_BoundingBox collectionBox= m_World.boundingBox();
	if (boundingBox.isEmpty())
	{
		if (!collectionBox.isEmpty())
		{
			GLC_Camera savCam(*(m_GlView.cameraHandle()));
			m_GlView.reframe(collectionBox);
			if (motion)
			{
				GLC_Camera newCam(*(m_GlView.cameraHandle()));
				m_GlView.cameraHandle()->setCam(savCam);
				motionReframe(newCam, m_MotionStep);
			}
			else
			{
				updateGL();
				emit viewChanged();
			}
		}
	}
	else
	{
		GLC_Camera savCam(*(m_GlView.cameraHandle()));
		m_GlView.reframe(boundingBox);
		if (savCam == *(m_GlView.cameraHandle()))
		{
			m_GlView.reframe(collectionBox);
		}
		if (motion)
		{
			GLC_Camera newCam(*(m_GlView.cameraHandle()));
			m_GlView.cameraHandle()->setCam(savCam);
			motionReframe(newCam, m_MotionStep);
		}
		else
		{
			updateGL();
		}
	}
}


// Change The enter state of the view
void OpenglView::changeEnterState(ViewEnterState_enum state)
{
	if (m_ViewState == V_NORMAL)
	{
		m_ViewEnterState= state;
	}
}

// Set the view Camera
void OpenglView::setCameraAndAngle(const GLC_Camera& cam, const double& angle)
{
	*(m_GlView.cameraHandle())= cam;
	m_GlView.setViewAngle(angle);
	emit viewChanged();
}

// Init Iso view
void OpenglView::initIsoView()
{
	m_GlView.cameraHandle()->setIsoView();
	updateGL();
	emit viewChanged();
}

// Change the default camera Up axis
void OpenglView::changeDefaultUp()
{
	GLC_Vector3d upVector(m_GlView.cameraHandle()->defaultUpVector());
	// circular permutation
	if (upVector == glc::X_AXIS)
		upVector= glc::Y_AXIS;
	else if (upVector == glc::Y_AXIS)
		upVector= glc::Z_AXIS;
	else if (upVector == glc::Z_AXIS)
		upVector= glc::X_AXIS;

	m_GlView.cameraHandle()->setDefaultUpVector(upVector);
	m_GlView.cameraHandle()->setUpCam(upVector);
	updateGL();
}

// Iso view
void OpenglView::isoView1(bool motion)
{
	// Update position of the camera
	changeView(m_GlView.cameraHandle()->isoView(), motion);
}

// Iso view
void OpenglView::isoView2()
{
	GLC_Camera newCam= m_GlView.cameraHandle()->isoView();
	newCam.rotateAroundTarget(newCam.defaultUpVector(), glc::PI / 2.0);
	// Update position of the camera
	changeView(newCam);
}
// Iso view
void OpenglView::isoView3()
{
	GLC_Camera newCam= m_GlView.cameraHandle()->isoView();
	newCam.rotateAroundTarget(newCam.defaultUpVector(), glc::PI);
	// Update position of the camera
	changeView(newCam);
}
// Iso view
void OpenglView::isoView4()
{
	GLC_Camera newCam= m_GlView.cameraHandle()->isoView();
	newCam.rotateAroundTarget(newCam.defaultUpVector(), 3.0 * glc::PI / 2.0);
	// Update position of the camera
	changeView(newCam);
}

// Front view
void OpenglView::frontView()
{
	// Update position of the camera
	changeView(m_GlView.cameraHandle()->frontView());
}

// Left view
void OpenglView::rightView()
{
	changeView(m_GlView.cameraHandle()->rightView());
}
// Top view
void OpenglView::topView()
{
	changeView(m_GlView.cameraHandle()->topView());
}
// Zoom in
void OpenglView::zoomIn()
{
	int max= static_cast<int>(static_cast<double>(m_MotionStep) / 4);
	if (max == 0) max= 1;
	const double targetFactor= 1.5;
	const double factor= exp(log(targetFactor)/ max);

	m_World.collection()->setLodUsage(true, &m_GlView);
	for (int i= 0; i < max - 1; ++i)
	{
		m_GlView.cameraHandle()->zoom(factor);
		updateGL();
	}
	m_World.collection()->setLodUsage(false, &m_GlView);
	m_GlView.cameraHandle()->zoom(factor);

	updateGL();
	emit viewChanged();

}
// Zoom out
void OpenglView::zoomOut()
{
	int max= static_cast<int>(static_cast<double>(m_MotionStep) / 4);
	if (max == 0) max= 1;
	const double targetFactor= 1.0 / 1.5;
	const double factor= exp(log(targetFactor)/ max);

	m_World.collection()->setLodUsage(true, &m_GlView);
	for (int i= 0; i < max - 1; ++i)
	{
		m_GlView.cameraHandle()->zoom(factor);
		updateGL();
	}
	m_World.collection()->setLodUsage(false, &m_GlView);
	m_GlView.cameraHandle()->zoom(factor);

	updateGL();
	emit viewChanged();
}

// Select All instances
void OpenglView::selectAll()
{
	m_World.selectAllWith3DViewInstanceInCurrentShowState();
	updateGL();
	emit updateSelection(m_World.collection()->selection());
}

// unselect Instance
void OpenglView::unselectAllSlot()
{
	// if a geometry is selected, unselect it
	m_World.unselectAll();
	updateGL();
	emit unselectAll();
}

// Reframe on selection
void OpenglView::reframeOnSelection()
{
	GLC_BoundingBox SelectionBox;
	PointerViewInstanceHash* pSelections= m_World.collection()->selection();
	PointerViewInstanceHash::iterator iEntry= pSelections->begin();
    while (iEntry != pSelections->constEnd())
    {
    	SelectionBox.combine(iEntry.value()->boundingBox());
    	iEntry++;
    }
	reframe(SelectionBox, true);
}

// Select specified ID
void OpenglView::selectInstance(const GLC_uint SelectionID)
{
	if (0 != SelectionID)
	{
		m_World.select(SelectionID);
	}
	updateGL();
	emit updateSelection(m_World.collection()->selection());
}

// Change Current mover type
void OpenglView::changeCurrentMoverToTrackBall()
{
	m_CurrentMoverType= GLC_MoverController::TrackBall;
}

// Change Current mover type
void OpenglView::changeCurrentMoverToTurnTable()
{
	m_CurrentMoverType= GLC_MoverController::TurnTable;
}

// Change Current mover type
void OpenglView::changeCurrentMoverToFly()
{
	m_CurrentMoverType= GLC_MoverController::Fly;
}

void OpenglView::toggleOctreeDisplay(bool display)
{
	if (display)
	{
		GLC_Material* pMat= new GLC_Material(Qt::red);
		pMat->setOpacity(0.1);
		GLC_SpacePartitioning* pSpacePart= m_World.collection()->spacePartitioningHandle();
		if (NULL != pSpacePart)
		{
			GLC_Octree* pOctree= dynamic_cast<GLC_Octree*>(pSpacePart);
			if (NULL != pOctree)
			{
				pOctree->createBox(pMat, &m_3dWidgetCollection);
			}
		}
	}
	else
	{
		m_3dWidgetCollection.clear();
	}
	updateGL();
}
void OpenglView::sectioning(bool isActive)
{
	if (isActive)
	{
		GLC_Point3d center= m_World.boundingBox().center();
		const GLC_Vector3d normal(-glc::Z_AXIS);
		const double l1= 1.1 * m_World.boundingBox().xLength();
		const double l2= 1.1 * m_World.boundingBox().yLength();
		GLC_CuttingPlane* pCuttingPlane= new GLC_CuttingPlane(center, normal, l1, l2);
		m_CuttingPLaneID= pCuttingPlane->id();

		connect(pCuttingPlane, SIGNAL(asChanged()), this, SLOT(sectionUpdated()));

		m_3DWidgetManager.add3DWidget(pCuttingPlane);
		// Test clipping plane
		m_pClipPlane= new GLC_Plane(normal, center);
		m_GlView.addClipPlane(GL_CLIP_PLANE0, m_pClipPlane);
	}
	else if (0 != m_CuttingPLaneID)
	{
		m_3DWidgetManager.remove3DWidget(m_CuttingPLaneID);
		m_GlView.removeClipPlane(GL_CLIP_PLANE0);
		m_CuttingPLaneID= 0;
	}

	updateGL();
}

void  OpenglView::showHideSectionPlane()
{
	QAction* action= qobject_cast<QAction*>(sender());
	if ((action != NULL) && action->isChecked())
	{
		m_3DWidgetManager.setWidgetVisible(m_CuttingPLaneID, true);
	}
	else
	{
		m_3DWidgetManager.setWidgetVisible(m_CuttingPLaneID, false);
	}
	updateGL();
}

void OpenglView::sectionUpdated()
{
	GLC_CuttingPlane* pCuttingPlane= dynamic_cast<GLC_CuttingPlane*>(sender());
	if (NULL != pCuttingPlane)
	{
		m_pClipPlane->setPlane(pCuttingPlane->normal(), pCuttingPlane->center());
	}
}

void OpenglView::toPerpective()
{
	m_GlView.setToOrtho(false);
	updateGL();
}

void OpenglView::toParallel()
{
	m_GlView.setToOrtho(true);
	updateGL();
}

void OpenglView::updateLightPosition()
{
	GLC_Axis* pAxis= dynamic_cast<GLC_Axis*>(sender());
	if ((NULL != pAxis) && (-1 != m_CurrentLightIndex))
	{
		m_UserLights[m_CurrentLightIndex]->setPosition(pAxis->center());
		updateGL();
	}
}

void OpenglView::editLight(int index)
{
	if (0 != m_LightAxisWidgetId)
	{
		m_3DWidgetManager.remove3DWidget(m_LightAxisWidgetId);
		m_LightAxisWidgetId= 0;
	}
	if (index > 0)
	{
		m_CurrentLightIndex= index -1;
		GLC_Axis* pAxis= new GLC_Axis(m_UserLights.at(m_CurrentLightIndex)->position());
		m_LightAxisWidgetId= pAxis->id();
		m_3DWidgetManager.add3DWidget(pAxis);
		connect(pAxis, SIGNAL(asChanged()), this, SLOT(updateLightPosition()));
	}
	updateGL();
}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////
void OpenglView::paintGL()
{
	GLC_RenderStatistics::reset();
	QTime time;
	time.start();
	setDistMinAndMax();

	m_World.collection()->updateInstanceViewableState();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLC_Context::current()->glcLoadIdentity();
	if (!m_SnapShootMode) glEnable(GL_MULTISAMPLE);
	try
	{
		// Enable and execute lighting
		m_Light.glExecute();
		m_GlView.glExecuteCam();
		if (!m_UserLights.isEmpty())
		{
			const int size= m_UserLights.size();
			for (int i= 0; i < size; ++i)
			{
				m_UserLights[i]->glExecute();
			}
		}

		m_GlView.useClipPlane(true);

		// Test if there is a global shader
		if ((0 != m_GlobalShaderId) && !GLC_State::isInSelectionMode())
			GLC_Shader::use(m_GlobalShaderId);

		// Display non transparent normal object
		m_World.render(0, m_RenderFlag);

		// Display non transparent instance of the shaders group
		if (GLC_State::glslUsed())
		{
			m_World.renderShaderGroup(m_RenderFlag);
		}

		// Display transparent normal object
		if (!GLC_State::isInSelectionMode())
		{
			m_World.render(0, glc::TransparentRenderFlag);
			// Display transparent instance of the shaders group
			if (GLC_State::glslUsed())
			{
				m_World.renderShaderGroup(glc::TransparentRenderFlag);
			}
		}

		// Display Selected Objects
		const int numberOfSelectedNode= m_World.collection()->selectionSize();
		if ((numberOfSelectedNode > 0) && GLC_State::selectionShaderUsed() && !GLC_State::isInSelectionMode())
		{
			if (numberOfSelectedNode != m_World.collection()->drawableObjectsSize())
			{
				//Draw the selection with Zbuffer
				m_World.render(1, m_RenderFlag);
			}
			glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
			// Draw the selection transparent
	        glEnable(GL_CULL_FACE);
	        glEnable(GL_BLEND);
	        glDepthFunc(GL_ALWAYS);
	        glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	        m_World.render(1, m_RenderFlag);

    	    // Restore attributtes
	        glPopAttrib();
		}
		else if (numberOfSelectedNode > 0)
		{
			m_World.render(1, m_RenderFlag);
		}

		// Test if there is a global shader
		if (0 != m_GlobalShaderId) GLC_Shader::unuse();

		m_GlView.useClipPlane(false);

		if (!m_SnapShootMode && !m_SelectionMode) // Don't display orbit circle in snapshootmode
		{
		    glDisable(GL_BLEND);
		    glDepthMask(GL_TRUE);
		    glEnable(GL_DEPTH_TEST);

			m_MoverController.drawActiveMoverRep();
			m_3dWidgetCollection.render(0, glc::WireRenderFlag);
			m_3dWidgetCollection.render(0, glc::TransparentRenderFlag);

		}
		// 3D widget manager
		//glPushAttrib(GL_ENABLE_BIT);
		//glDisable(GL_DEPTH_TEST);
		m_3DWidgetManager.render();
		//glPopAttrib();
	}
	catch (GLC_Exception &e)
	{
		qDebug() << e.what();
	}


	updateFps(time.elapsed());
	if (!m_SelectionMode && m_dislayInfoPanel && !m_SnapShootMode)
	{
		// Display info area
		GLC_Context::current()->glcMatrixMode(GL_PROJECTION);
		GLC_Context::current()->glcPushMatrix();
		GLC_Context::current()->glcLoadIdentity();
		GLC_Context::current()->glcOrtho(-1,1,-1,1,-1,1);
		GLC_Context::current()->glcMatrixMode(GL_MODELVIEW);
		GLC_Context::current()->glcPushMatrix();
		GLC_Context::current()->glcLoadIdentity();
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);

		displayInfo();

		// Restore 3DState
		glPopAttrib();
		GLC_Context::current()->glcPopMatrix(); // restore modelview

		GLC_Context::current()->glcMatrixMode(GL_PROJECTION);
		GLC_Context::current()->glcPopMatrix();
		GLC_Context::current()->glcMatrixMode(GL_MODELVIEW);
	}
	if (!m_SnapShootMode) glEnable(GL_MULTISAMPLE);
}


void OpenglView::resizeGL(int width, int height)
{
	m_GlView.setWinGLSize(width, height);
}

void OpenglView::mousePressEvent(QMouseEvent * e)
{
	GLC_UserInput userInput(e->x(), e->y());

	if ((m_ViewState == V_NORMAL) && ((m_ViewEnterState == VE_NORMAL)))
	{
		switch (e->button())
		{
			// Left Button Pressed
			case Qt::LeftButton:
				if (!m_BlockSelection)
				{
					bool multiSelection= ((e->modifiers() == Qt::ControlModifier) || (e->modifiers() == Qt::ShiftModifier));
					select(e->x(),e->y(), multiSelection, e);
				}
			break;

			// Right Button Pressed
			case Qt::RightButton:
			if (e->modifiers() == Qt::NoModifier)
			{
				m_MoverController.setActiveMover(m_CurrentMoverType, userInput);
				setCursor(Qt::ClosedHandCursor);
				m_ViewState= V_ORBITING;
				m_World.collection()->setLodUsage(true, &m_GlView);
			}
			else if (e->modifiers() == Qt::ShiftModifier)
			{
				m_MoverController.setActiveMover(GLC_MoverController::Zoom, userInput);
				setCursor(Qt::SizeVerCursor);
				m_ViewState= V_ZOOMING;
				m_World.collection()->setLodUsage(true, &m_GlView);
			}
			break;

			// Mid Button Pressed
			case Qt::MidButton:
			if (e->modifiers() == Qt::NoModifier)
			{
				m_MoverController.setActiveMover(GLC_MoverController::Pan, userInput);
				setCursor(Qt::SizeAllCursor);
				m_ViewState= V_PANNING;
				m_World.collection()->setLodUsage(true, &m_GlView);
			}
			else if (e->modifiers() == Qt::ShiftModifier)
			{
				m_MoverController.setActiveMover(GLC_MoverController::Target, userInput);
				updateGL();
				emit viewChanged();
			}

			break;
			// Default case
			default:
			break;
		}
	}
	else if ((m_ViewState == V_NORMAL) && (e->button() == Qt::LeftButton))
	{
		switch (m_ViewEnterState)
		{
			case VE_PANNING:
			m_MoverController.setActiveMover(GLC_MoverController::Pan, userInput);
			setCursor(Qt::SizeAllCursor);
			m_ViewState= V_PANNING;
			m_World.collection()->setLodUsage(true, &m_GlView);
			break;

			case VE_ORBITING:
			m_MoverController.setActiveMover(m_CurrentMoverType, userInput);
			setCursor(Qt::ClosedHandCursor);
			m_ViewState= V_ORBITING;
			m_World.collection()->setLodUsage(true, &m_GlView);
			break;

			case VE_POINTING:
			m_MoverController.setActiveMover(GLC_MoverController::Target, userInput);
			m_MoverController.setNoMover();
			updateGL();
			break;

			case VE_ZOOMING:
			m_MoverController.setActiveMover(GLC_MoverController::Zoom, userInput);
			setCursor(Qt::SizeVerCursor);
			m_ViewState= V_ZOOMING;
			m_World.collection()->setLodUsage(true, &m_GlView);
			break;
		}
	}

	//QGLWidget::mousePressEvent(e);
}

void OpenglView::mouseReleaseEvent(QMouseEvent * e)
{
	m_World.collection()->setLodUsage(false, &m_GlView);

	if (m_ViewEnterState == VE_NORMAL)
	{
		if ((Qt::LeftButton == e->button()) && (m_ViewState == V_NORMAL))
		{
			glc::WidgetEventFlag eventFlag= m_3DWidgetManager.mouseReleaseEvent(e);
			if (eventFlag == glc::BlockedEvent) updateGL();

		}
		else
		{
			m_MoverController.setNoMover();
			unsetCursor();
			m_ViewState= V_NORMAL;
			updateGL();

		}
	}
	else if((e->button() == Qt::LeftButton)
			&& ((m_ViewEnterState == VE_PANNING) || (m_ViewEnterState == VE_ORBITING) || (m_ViewEnterState == VE_ZOOMING)))
	{
		m_MoverController.setNoMover();
		unsetCursor();
		m_ViewState= V_NORMAL;
		updateGL();
	}
}
void OpenglView::mouseMoveEvent(QMouseEvent * e)
{
	if ((m_ViewState == V_ORBITING) || (m_ViewState == V_ZOOMING) || (m_ViewState == V_PANNING))
	{
		const bool needUpdate= m_MoverController.move(GLC_UserInput(e->x(), e->y()));
		if (needUpdate)
		{
			updateGL();
			emit viewChanged();
		}
	}
	else
	{
		m_World.collection()->setLodUsage(true, &m_GlView);
		bool needUpdate= (m_3DWidgetManager.mouseMoveEvent(e) != glc::IgnoreEvent);
		if (needUpdate)
		{
			updateGL();
		}
		m_World.collection()->setLodUsage(false, &m_GlView);

	}
}

void OpenglView::wheelEvent(QWheelEvent *e)
{
	if (V_NORMAL == m_ViewState)
	{
		m_World.collection()->setLodUsage(true, &m_GlView);
		if (e->delta() > 0)
		{
			zoomIn();
		}
		else
		{
			zoomOut();
		}
	}
	else if (V_ORBITING == m_ViewState)
	{
		// Get the current mover
		GLC_Mover* pCurrentMover= m_MoverController.activeMover();
		GLC_FlyMover* pFlyMover= dynamic_cast<GLC_FlyMover*>(pCurrentMover);
		if (NULL != pFlyMover)
		{
			double factor= 1.3;
			if (e->delta() < 0)
			{
				factor= 1.0 / factor;
			}
			pFlyMover->increaseVelocity(factor);
		}
	}
}
void OpenglView::mouseDoubleClickEvent(QMouseEvent *e)
{
	if ((m_World.selectionSize() > 0) && (e->button() == Qt::LeftButton) && (e->modifiers() == Qt::NoModifier))
	{
		reframeOnSelection();
	}
}

// Rotate the camera softely with specified step
void OpenglView::rotateCamera(GLC_Camera newCam, const int step)
{
	// Save vectors of view's camera
	GLC_Point3d curEye(m_GlView.cameraHandle()->eye());
	GLC_Vector3d vectCurUp(m_GlView.cameraHandle()->upVector());
	GLC_Point3d curTarget(m_GlView.cameraHandle()->target());

	// Declare Rotations matrixs and rotaion interpolator
	GLC_Matrix4x4 matRot;
	GLC_Matrix4x4 matRotUp;
	GLC_Interpolator angleInterpolator;

	if ((m_GlView.cameraHandle()->forward() == newCam.forward()) &&
		(m_GlView.cameraHandle()->upVector() == newCam.upVector()))
	{
		// View Camera is equal to destination camera, inverse destination view direction
		GLC_Vector3d newEye=(- (newCam.eye() - newCam.target()) + newCam.target());
		newCam.setCam(newEye, newCam.target(), newCam.upVector());
		const double angle= glc::PI / static_cast<double>(step);
		matRot.setMatRot(newCam.upVector(), angle);
	}
	else if ((m_GlView.cameraHandle()->forward() == (-newCam.forward())) &&
		(m_GlView.cameraHandle()->upVector() == newCam.upVector()))
	{
		// View Camera is the inverse off destination camera
		const double angle= - glc::PI / static_cast<double>(step);
		matRot.setMatRot(newCam.upVector(), angle);
	}
	else
	{

		angleInterpolator.SetInterpolMat(step, (curEye - curTarget) , (newCam.eye() - newCam.target()), INTERPOL_ANGULAIRE);
		matRot= angleInterpolator.GetInterpolMat();
		// VecteurUp
		angleInterpolator.SetVecteurs(vectCurUp, newCam.upVector());
		matRotUp= angleInterpolator.GetInterpolMat();

	}

	for (int i= 0; i < step - 1; ++i)
	{
		curEye= (matRot * (curEye - curTarget)) + curTarget;
		vectCurUp= matRotUp * vectCurUp;

		m_GlView.cameraHandle()->setCam(curEye, curTarget, vectCurUp);
		updateGL();
	}

	m_GlView.cameraHandle()->setCam(newCam.eye(), newCam.target(), newCam.upVector());

	updateGL();
	emit viewChanged();

}
// reframe softely with specified step
void OpenglView::motionReframe(const GLC_Camera newCam, const int step)
{
	if ((m_GlView.cameraHandle()->eye() != newCam.eye()) ||
		(m_GlView.cameraHandle()->target() != newCam.target()))
		{
			GLC_Point3d curEye(m_GlView.cameraHandle()->eye());
			GLC_Point3d curTarget(m_GlView.cameraHandle()->target());
			const GLC_Vector3d vectCurUp(m_GlView.cameraHandle()->upVector());

			GLC_Interpolator transInterpolator;
			transInterpolator.SetInterpolMat(step, curEye, newCam.eye(), INTERPOL_LINEAIRE);
			GLC_Matrix4x4 matEye(transInterpolator.GetInterpolMat());
			transInterpolator.SetVecteurs(curTarget, newCam.target());
			GLC_Matrix4x4 matTarget(transInterpolator.GetInterpolMat());
			m_World.collection()->setLodUsage(true, &m_GlView);
			for (int i= 0; i < step - 1; ++i)
			{
				curEye= matEye * curEye;
				curTarget = matTarget * curTarget;

				m_GlView.cameraHandle()->setCam(curEye, curTarget, vectCurUp);
				updateGL();
			}
			m_World.collection()->setLodUsage(false, &m_GlView);
			m_GlView.cameraHandle()->setCam(newCam.eye(), newCam.target(), newCam.upVector());

			updateGL();
			emit viewChanged();

		}
}
// Update the current fps
void OpenglView::updateFps(int elapsed)
{
	static const int max= 20;
	static int fpsVector[max];
	static int j=0;
	float averageFps= 0.0f;


  	if (elapsed > 0)
  	{
    	fpsVector[j]= elapsed;
	  	j= (j + 1) % max;
  	}
	for (int i= 0;i < max;++i)
	{
		averageFps+= fpsVector[i];
	}
	averageFps= averageFps / static_cast<float>(max);
	m_CurrentFps= 1000.0f / averageFps;
	float relativeFps;

	// The maximum relative fps must be under 60
	// Cause VSSYNC
	if (m_CurrentFps > 60.0f) relativeFps= 60.0f;
	else relativeFps= m_CurrentFps;

	m_MotionStep= static_cast<int>(relativeFps / 3.0f);
	if (0 == m_MotionStep) m_MotionStep= 1;
}

// Display info panel
void OpenglView::displayInfo()
{
	QSize textPosition(76,5);
	QSize screenSize(size());
	int screenHeight= screenSize.height();
	float panelRatio= static_cast<float>(screenHeight - 42) / screenHeight;
	double displayRatio= static_cast<double>(screenSize.height()) / static_cast<double>(screenSize.width());
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBlendFunc(GL_ONE,GL_SRC_ALPHA);
	QColor areaColor(5, 5, 50, 128);
	qglColor(areaColor);

	glBegin(GL_QUADS);
		glVertex2f(-1.f,-panelRatio); glVertex2f( 1.f,-panelRatio);
		glVertex2f( 1.f,-1.f); glVertex2f(-1.f,-1.f);
	glEnd();

	// Display the frame rate
	qglColor(Qt::white);
	m_infoFont.setPixelSize(12);
	screenSize-= textPosition;
	QString fps(QString::number(m_CurrentFps, 'f', 2) + QString(" Fps"));
	renderText(screenSize.width(), screenSize.height(), fps, m_infoFont);
	// If there is one selected object, display its name
	if(m_World.selectionSize() == 1)
	{
		QString selectionName(m_World.selectedOccurenceList().first()->name());
		// truncate the string to 50 characters
		selectionName= selectionName.left(50);
		renderText(10, screenSize.height(), selectionName, m_infoFont);
	}
	else if (GLC_State::glslUsed())
	{
		QString currentShaderName(tr("Global Shader : ") + m_GlobalShaderName);
		currentShaderName= currentShaderName.left(50);
		renderText(10, screenSize.height(), currentShaderName, m_infoFont);
	}

	// Display render statistics
	QString bodyCount= tr("Body Count : ") + QString::number(GLC_RenderStatistics::bodyCount());
	renderText(screenSize.width() / 2 - 100, screenSize.height(), bodyCount, m_infoFont);
	QString triangleCount = tr("Triangle Count : ") + QString::number(GLC_RenderStatistics::triangleCount());
	renderText(screenSize.width() / 2 + 100, screenSize.height(), triangleCount, m_infoFont);


	GLC_Matrix4x4 uiMatrix(m_GlView.cameraHandle()->viewMatrix());
	// Change matrix to follow camera orientation
	const double scaleFactor= 0.08;
	GLC_Context::current()->glcTranslated(1.0 - (scaleFactor * displayRatio * 1.4), - panelRatio + (scaleFactor * 1.3), 0.0);
	GLC_Context::current()->glcScaled(scaleFactor * displayRatio, scaleFactor, scaleFactor);
	GLC_Context::current()->glcMultMatrix(uiMatrix);

	qglColor(Qt::red);
	renderText(1.0, 0.0, 0.0, "X");
	qglColor(Qt::darkGreen);
	renderText(0.0, 1.0, 0.0, "Y");
	qglColor(Qt::blue);
	renderText(0.0, 0.0, 1.0, "Z");
	m_UiCollection.render(0, glc::ShadingFlag);


	// OpenGL error handler
	GLenum error= glGetError();
	if (error != GL_NO_ERROR)
	{// QT render text cause an openGL error
		QString msg(tr("Sorry, Display info panel not supported on your computer, restarting GLC_Player"));
		QMessageBox::warning(this, tr("Warning"), msg, QMessageBox::Ok);
		m_dislayInfoPanel= false;
		emit hideInfoPanel();
	}
}

// Select
void OpenglView::select(int x, int y, bool multiSelection, QMouseEvent* pMouseEvent)
{

	const bool spacePartitioningIsUsed= m_World.collection()->spacePartitioningIsUsed();
	// Selection frustum
	if (spacePartitioningIsUsed)
	{
		GLC_Frustum selectionFrustum(m_GlView.selectionFrustum(x, y));
		m_World.collection()->updateInstanceViewableState(selectionFrustum);
		m_World.collection()->setSpacePartitionningUsage(false);
	}

	m_SelectionMode= true;
	setAutoBufferSwap(false);

	m_World.collection()->setLodUsage(true, &m_GlView);
	GLC_uint SelectionID= m_GlView.renderAndSelect(x, y);
	m_World.collection()->setLodUsage(false, &m_GlView);

	if (spacePartitioningIsUsed)
	{
		m_World.collection()->updateInstanceViewableState(m_GlView.frustum());
		m_World.collection()->setSpacePartitionningUsage(true);
	}

	// 3DWidget manager test
	glc::WidgetEventFlag eventFlag= m_3DWidgetManager.mousePressEvent(pMouseEvent);

	m_SelectionMode= false;
	setAutoBufferSwap(true);

	if (eventFlag == glc::BlockedEvent)
	{
		updateGL();
		return;
	}
	else if (m_World.containsOccurence(SelectionID))
	{

		if ((!m_World.isSelected(SelectionID)) && (m_World.selectionSize() > 0) && (!multiSelection))
		{
			m_World.unselectAll();
			emit unselectAll();
		}
		if (!m_World.isSelected(SelectionID))
		{
			m_World.select(SelectionID);
			updateGL();
			emit updateSelection(m_World.collection()->selection());
		}
		else if (m_World.isSelected(SelectionID) && multiSelection)
		{
			m_World.unselect(SelectionID);
			updateGL();
			emit updateSelection(m_World.collection()->selection());
		}
		else
		{
			m_World.unselectAll();
			m_World.select(SelectionID);
			updateGL();
			emit updateSelection(m_World.collection()->selection());
		}
	}
	else if ((m_World.selectionSize() > 0) && (!multiSelection))
	{
		// if a geometry is selected, unselect it
		m_World.unselectAll();
		updateGL();
		emit unselectAll();
	}
	else if (eventFlag == glc::AcceptEvent)
	{
		updateGL();
	}
}

// Change the current view
void OpenglView::changeView(GLC_Camera newCam, bool motion)
{
	newCam.setDistEyeTarget(m_GlView.cameraHandle()->distEyeTarget());
	if (motion)
	{
		rotateCamera(newCam, m_MotionStep);
	}
	else
	{
		rotateCamera(newCam, 0);
	}
}

GLC_BoundingBox OpenglView::globalBoundingBox()
{
	GLC_BoundingBox bBox(m_World.boundingBox());
	bBox.combine(m_3DWidgetManager.boundingBox());

	return bBox;
}
void OpenglView::setDistMinAndMax()
{
	m_GlView.setDistMinAndMax(globalBoundingBox());
}
void OpenglView::setTwoSidedLightning(bool flag)
{
	m_Light.setTwoSided(flag);
	const int size= m_UserLights.size();
	for (int i= 0; i < size; ++i)
	{
		m_UserLights[i]->setTwoSided(flag);
	}
}
// Swap visible space
void OpenglView::swapVisibleSpace()
{
	m_World.collection()->swapShowState();
	if (m_World.collection()->showState())
	{
		m_GlView.loadBackGroundImage(":images/default_background.png");
	}
	else
	{
		m_GlView.loadBackGroundImage(":images/NoShow_background.png");
	}
}
// set the view to visible state
void OpenglView::setToVisibleState()
{
	m_GlView.loadBackGroundImage(":images/default_background.png");
}
// set the view to visible state
void OpenglView::setToInVisibleState()
{
	m_GlView.loadBackGroundImage(":images/NoShow_background.png");
}

// Take a Screenshot of the current view
QImage OpenglView::takeScreenshot(const bool tryToUseFrameBuffer, const QSize& targetSize, const QString& backImageName, const QColor& backColor, double aspectRatio)
{
	// Test if framebuffer must be used
	bool useFrameBuffer;
	if ((tryToUseFrameBuffer) && GLC_State::frameBufferSupported())
	{
		// Use frame buffer
		useFrameBuffer= true;
	}
	else
	{
		useFrameBuffer= false;
	}
	QImage imageToSave;

	setAutoBufferSwap(false);
	setSnapShootMode(true);

	// Test if the background must be changed
	if (!backImageName.isEmpty())
	{
		// if m_CurrentBackgroundImageName is not empty, the file exist and is readable
		m_GlView.loadBackGroundImage(backImageName);

	}
	else if (backColor.isValid())
	{
		m_GlView.setBackgroundColor(backColor);
		m_GlView.deleteBackGroundImage();

	}

	if (useFrameBuffer)
	{
		QGLFramebufferObjectFormat frameBufferFormat;
		frameBufferFormat.setSamples(this->format().samples());
		frameBufferFormat.setAttachment(QGLFramebufferObject::Depth);
		// Create the framebuffer
		QGLFramebufferObject framebufferObject(targetSize, frameBufferFormat);
		framebufferObject.bind();
		m_GlView.setWinGLSize(framebufferObject.width(), framebufferObject.height());
		updateGL();
		imageToSave= framebufferObject.toImage();
		framebufferObject.release();
		m_GlView.setWinGLSize(size().width(), size().height());
	}
	if (imageToSave.isNull())
	{
		// Change view aspect ratio and
		m_GlView.forceAspectRatio(aspectRatio);
		updateGL();
		m_GlView.updateAspectRatio();
		imageToSave=  grabFrameBuffer();
		QSize shotSize= imageToSave.size();
		imageToSave= imageToSave.scaled(static_cast<int>(shotSize.height() * aspectRatio), shotSize.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

		imageToSave= imageToSave.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		// To match the exact size of image
		imageToSave= imageToSave.scaled(targetSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	}

	// Retore the view
	if (isInShowState())
	{
		setToVisibleState();
	}
	else
	{
		setToInVisibleState();
	}

	setAutoBufferSwap(true);
	setSnapShootMode(false);
	m_GlView.updateProjectionMat();
	updateGL();

	return imageToSave;
}

// Change the view state to capture mode
void OpenglView::captureMode(const QSize& captureSize, bool smooth, const QColor& color)
{
	m_SmoothCaptures= smooth;
	m_CaptureSize.setWidth(captureSize.width());
	m_CaptureSize.setHeight(captureSize.height());
	if (color.isValid())
	{
		m_GlView.setBackgroundColor(color);
		m_GlView.deleteBackGroundImage();
	}
	else
	{
		setToVisibleState();
	}
	setAutoBufferSwap(false);
	setSnapShootMode(true);

	if (GLC_State::frameBufferSupported())
	{
		QSize realSize(m_CaptureSize);
		if (m_SmoothCaptures)
		{
			realSize.setWidth(m_CaptureSize.width() * 4);
			realSize.setHeight(m_CaptureSize.height() * 4);
		}
		delete m_pQGLFramebufferObject;

		QGLFramebufferObjectFormat frameBufferFormat;
		frameBufferFormat.setSamples(this->format().samples());
		frameBufferFormat.setAttachment(QGLFramebufferObject::Depth);

		// Create the framebuffer
		m_pQGLFramebufferObject= new QGLFramebufferObject(realSize, frameBufferFormat);
	}
}

// Return a screenshot of the current view in capture mode
QImage OpenglView::takeScreenshot()
{
	QImage imageToSave;

	if (GLC_State::frameBufferSupported())
	{
		m_pQGLFramebufferObject->bind();
		m_GlView.setWinGLSize(m_pQGLFramebufferObject->size().width(), m_pQGLFramebufferObject->size().height());
		updateGL();
		imageToSave= m_pQGLFramebufferObject->toImage();
		if (m_SmoothCaptures)
		{
			imageToSave= imageToSave.scaled(m_CaptureSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		}
		m_pQGLFramebufferObject->release();
	}
	else
	{
		const double aspectRatio= static_cast<double>(m_CaptureSize.width()) / static_cast<double>(m_CaptureSize.height());
		m_GlView.forceAspectRatio(aspectRatio);
		updateGL();
		m_GlView.updateAspectRatio();
		imageToSave=  grabFrameBuffer();
		QSize shotSize= imageToSave.size();
		imageToSave= imageToSave.scaled(static_cast<int>(shotSize.height() * aspectRatio), shotSize.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

		imageToSave= imageToSave.scaled(m_CaptureSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		// To match the exact size of image
		imageToSave= imageToSave.scaled(m_CaptureSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	}

	return imageToSave;
}

// Change the state to normal mode
void OpenglView::normalMode()
{
	if (GLC_State::frameBufferSupported())
	{
		m_GlView.setWinGLSize(size().width(), size().height());
		delete m_pQGLFramebufferObject;
		m_pQGLFramebufferObject= NULL;
	}

	if (isInShowState())
	{
		setToVisibleState();
	}
	else
	{
		setToInVisibleState();
	}

	setAutoBufferSwap(true);
	setSnapShootMode(false);
	m_GlView.updateProjectionMat();
	updateGL();
}

// Initialize shader list
void OpenglView::initShaderList()
{
	if (GLC_State::glslUsed() && !GLC_State::selectionShaderUsed())
	{
		GLC_State::setSelectionShaderUsage(true);
		// Set selection Shader
		QFile vertexShaderFile(":shaders/select.vert");
		QFile fragmentShaderFile(":shaders/select.frag");
		GLC_SelectionMaterial::setShaders(vertexShaderFile, fragmentShaderFile, context());
		// Initialize selection shader
		GLC_SelectionMaterial::initShader(context());

		if (m_ShaderList.isEmpty())
		{
			GLC_Shader* pShader= new GLC_Shader();
			// Toon Shader
			QFile vertexToonShaderFile(":shaders/toon.vert");
			QFile fragmentToonShaderFile(":shaders/toon.frag");
			pShader->setVertexAndFragmentShader(vertexToonShaderFile, fragmentToonShaderFile);
			pShader->createAndCompileProgrammShader();
			m_ShaderList.append(pShader);

			// Minnaert Shader
			pShader= new GLC_Shader();
			QFile vertex2ShaderFile(":shaders/minnaert.vert");
			QFile fragment2ShaderFile(":shaders/minnaert.frag");
			pShader->setVertexAndFragmentShader(vertex2ShaderFile, fragment2ShaderFile);
			pShader->createAndCompileProgrammShader();
			m_ShaderList.append(pShader);

			// Gooch Shader
			pShader= new GLC_Shader();
			QFile vertex3ShaderFile(":shaders/goochShading.vert");
			QFile fragment3ShaderFile(":shaders/goochShading.frag");
			pShader->setVertexAndFragmentShader(vertex3ShaderFile, fragment3ShaderFile);
			pShader->createAndCompileProgrammShader();
			m_ShaderList.append(pShader);
		}
	}

}



