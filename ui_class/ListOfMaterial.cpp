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

#include "ListOfMaterial.h"
#include "../opengl_view/OpenglView.h"
#include <QGLFramebufferObject>


ListOfMaterial::ListOfMaterial(OpenglView* pOpenglView, QWidget *parent)
: QWidget(parent)
, m_MaterialList()
, m_pOpenglView(pOpenglView)
, m_World()
, m_Camera()
, m_pGeom(NULL)
, m_IconSize(40, 40)
, m_pFrameBuffer(NULL)
, m_FrameBufferSize(128, 128)
{
	setupUi(this);

	// Model list Icons
	materialList->setIconSize(m_IconSize);

	// Create the world used for the preview
	initWorld();

	// Signal and Slot connection
	// Current Model change
	connect(materialList, SIGNAL(currentRowChanged (int)), this , SLOT(updateMaterialProperty(int)));


}

ListOfMaterial::~ListOfMaterial()
{
	delete m_pFrameBuffer;
}

//////////////////////////////////////////////////////////////////////
// Public set function
//////////////////////////////////////////////////////////////////////
// Create or Update the list
void ListOfMaterial::CreateOrUpdate(QList<GLC_Material*>& materialsList)
{
	// Create frame buffer if supported
	if ((NULL == m_pFrameBuffer) && GLC_State::frameBufferSupported())
	{
		qDebug() << "ListOfMaterial::ListOfMaterial Use Frame buffer";
		m_pFrameBuffer= new QGLFramebufferObject(m_FrameBufferSize, QGLFramebufferObject::Depth);
	}

	m_MaterialList.clear();
	m_MaterialList= materialsList;

	// Save the view
	const double masterAngle= m_pOpenglView->getViewAngle();;
	GLC_World masterWorld(m_pOpenglView->getWorld());
	GLC_Camera masterCam(m_pOpenglView->getCamera());

	// Prepare the view to make list snapShoot
	m_pOpenglView->setAutoBufferSwap(false);
	// Setup the view for preview
	m_pOpenglView->add(m_World);
	if (m_Camera == GLC_Camera())
	{
		m_Camera.setEyeCam(GLC_Vector3d(0.0, -3.0, 1.5));
		m_pOpenglView->setCameraAndAngle(m_Camera, 35.0);
		m_pOpenglView->reframe(GLC_BoundingBox(), false);
		m_Camera= m_pOpenglView->getCamera();
	}
	else
	{
		m_pOpenglView->setCameraAndAngle(m_Camera, 35.0);
	}

	// Set background to dark gray
	m_pOpenglView->viewportHandle()->setBackgroundColor(Qt::white);
	m_pOpenglView->viewportHandle()->deleteBackGroundImage();

	// Desactivate two sided lighting
	const bool isTwosided= m_pOpenglView->getLight()->isTwoSided();
	if (isTwosided)
	{
		m_pOpenglView->getLight()->setTwoSided(false);
	}
	m_pOpenglView->setSnapShootMode(true);
	const int width= m_IconSize.width();
	const int height= m_IconSize.height();

	const double aspectRatio= static_cast<double>(width) / static_cast<double>(height);
	m_pOpenglView->viewportHandle()->forceAspectRatio(aspectRatio);

	// Clear the QListWidget
	materialList->clear();
	// Add materials in in QListWidget
	const int size= m_MaterialList.size();

	bool frameBufferIsUsed= false;
	if (NULL != m_pFrameBuffer)
	{
		frameBufferIsUsed= m_pFrameBuffer->bind();
		if (frameBufferIsUsed)
		{
			glViewport(0, 0, m_FrameBufferSize.width(), m_FrameBufferSize.height());
		}
	}

	// Create progress dialog
	QProgressDialog progress(tr("Creating Material previews... Please wait."), tr("Cancel"), 0, size, this);
	progress.setModal(true);
	progress.setMinimumDuration(1000);
	int i= 0;
	while ((i < size) && !progress.wasCanceled())
	{
		// Update Progress dialog
		progress.setValue(i);
		QCoreApplication::processEvents();

		GLC_Material* pCurrentMaterial= m_MaterialList[i];
		// Make material Opaque
		const float matAlpha= pCurrentMaterial->diffuseColor().alphaF();
		pCurrentMaterial->setOpacity(1.0);
		m_pGeom->replaceMasterMaterial(pCurrentMaterial);


		QListWidgetItem* pItem= new QListWidgetItem(pCurrentMaterial->name());


		// Choose between Color buffer or frame buffer
		if (frameBufferIsUsed)
		{
			m_pFrameBuffer->bind();
			m_pOpenglView->updateGL();
			Q_ASSERT(m_pFrameBuffer->isValid());
			// Make the screenShoot
			QImage snapShoot= m_pFrameBuffer->toImage();
			pItem->setIcon(QPixmap::fromImage(snapShoot).scaled(m_IconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
			m_pFrameBuffer->release();
		}
		else
		{
			m_pOpenglView->updateGL();
			// Make the screenShoot
			QImage snapShoot= m_pOpenglView->grabFrameBuffer();
			pItem->setIcon(QPixmap::fromImage(snapShoot).scaled(m_IconSize, Qt::IgnoreAspectRatio)); //, Qt::SmoothTransformation
		}

		// Add item to the list
		materialList->addItem(pItem);

		// Restore material Alpha
		pCurrentMaterial->setOpacity(matAlpha);

		++i;
	}
	// Restore normal windowing buffer if needed
	if (frameBufferIsUsed)
	{
		m_pFrameBuffer->release();
		glViewport(0, 0, m_pOpenglView->size().width(), m_pOpenglView->size().height());
	}
	materialList->setEnabled(true);
	m_pOpenglView->viewportHandle()->updateAspectRatio();
	m_pOpenglView->viewportHandle()->updateProjectionMat();
	// Restore the view
	m_pOpenglView->clear();
	m_pOpenglView->add(masterWorld);
	m_pOpenglView->setToVisibleState();
	m_pOpenglView->setCameraAndAngle(masterCam, masterAngle);
	m_pOpenglView->setAutoBufferSwap(true);
	m_pOpenglView->setSnapShootMode(false);

	if (isTwosided)
	{
		m_pOpenglView->getLight()->setTwoSided(true);
	}

	m_pOpenglView->updateGL();
	materialList->setCurrentRow(0);
	numberOfSubMaterial->setText(QString::number(size));
}

// Show only item list
void ListOfMaterial::showOnlyItemList(bool onlyItemList)
{
	numberOfSubMaterial->setVisible(!onlyItemList);
	labelNbrSubMaterial->setVisible(!onlyItemList);
}

//////////////////////////////////////////////////////////////////////
// Public Slot function
//////////////////////////////////////////////////////////////////////

// Update the current row
void ListOfMaterial::updateRow()
{
	const int row= materialList->currentRow();
	// Save the view
	const double masterAngle= m_pOpenglView->getViewAngle();;
	GLC_World masterWorld(m_pOpenglView->getWorld());
	GLC_Camera masterCam(m_pOpenglView->getCamera());
	// Prepare the view to make list snapShoot
	m_pOpenglView->setAutoBufferSwap(false);
	// Setup the view for preview
	m_pOpenglView->add(m_World);

	m_pOpenglView->setCameraAndAngle(m_Camera, 35.0);

	// Set background to dark gray
	m_pOpenglView->viewportHandle()->setBackgroundColor(Qt::white);
	m_pOpenglView->viewportHandle()->deleteBackGroundImage();

	// Desactivate two sided lighting
	const bool isTwosided= m_pOpenglView->getLight()->isTwoSided();
	if (isTwosided)
	{
		m_pOpenglView->getLight()->setTwoSided(false);
	}
	m_pOpenglView->setSnapShootMode(true);
	const int width= m_IconSize.width();
	const int height= m_IconSize.height();

	const double aspectRatio= static_cast<double>(width) / static_cast<double>(height);
	m_pOpenglView->viewportHandle()->forceAspectRatio(aspectRatio);

	// Refresh icon of an item of QListWidget
	GLC_Material* pCurrentMaterial= m_MaterialList[row];
	m_pGeom->replaceMasterMaterial(pCurrentMaterial);

	m_pOpenglView->updateGL();
	// Make the screenShoot
	QImage snapShoot= m_pOpenglView->grabFrameBuffer();

	materialList->item(row)->setIcon(QPixmap::fromImage(snapShoot).scaled(m_IconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

	// Restore the view
	m_pOpenglView->clear();
	m_pOpenglView->add(masterWorld);
	m_pOpenglView->setToVisibleState();
	m_pOpenglView->setCameraAndAngle(masterCam, masterAngle);
	m_pOpenglView->setAutoBufferSwap(true);
	m_pOpenglView->setSnapShootMode(false);
	m_pOpenglView->viewportHandle()->updateAspectRatio();
	m_pOpenglView->viewportHandle()->updateProjectionMat();

	if (isTwosided)
	{
		m_pOpenglView->getLight()->setTwoSided(true);
	}

	m_pOpenglView->updateGL();
}

//////////////////////////////////////////////////////////////////////
// Private Slot function
//////////////////////////////////////////////////////////////////////

// The current item as been changed
void ListOfMaterial::updateMaterialProperty(int index)
{
	if (-1 != index)
	{
		emit updateMaterialSignal(m_MaterialList[index]);
	}
}

//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////

// init material representation world
void ListOfMaterial::initWorld()
{
	GLC_Factory* pFactory= GLC_Factory::instance();
	GLC_3DRep cylinder= pFactory->createCylinder(1.0, 3.0);
	m_pGeom= cylinder.geomAt(0);
	m_World.rootOccurence()->addChild(new GLC_StructOccurence(new GLC_3DRep(cylinder)));
}


