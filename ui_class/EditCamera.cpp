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

#include "EditCamera.h"
#include <GLC_Viewport>

EditCamera::EditCamera(GLC_Viewport* pView, QWidget *parent)
: QWidget(parent)
, m_pView(pView)
{
	setupUi(this);
    updateValues();


    // Signals and slot connection
    connect(angle, SIGNAL(valueChanged(double)), this, SLOT(updateFov()));

    connect(xEye, SIGNAL(valueChanged(double)), this, SLOT(updateEye()));
    connect(yEye, SIGNAL(valueChanged(double)), this, SLOT(updateEye()));
    connect(zEye, SIGNAL(valueChanged(double)), this, SLOT(updateEye()));

    connect(xTarget, SIGNAL(valueChanged(double)), this, SLOT(updateTarget()));
    connect(yTarget, SIGNAL(valueChanged(double)), this, SLOT(updateTarget()));
    connect(zTarget, SIGNAL(valueChanged(double)), this, SLOT(updateTarget()));

    connect(distance, SIGNAL(valueChanged(double)), this, SLOT(updateDistance()));

}

EditCamera::~EditCamera()
{
}

// Update dialog values
void EditCamera::updateValues()
{
	// Main view
	angle->blockSignals(true);
	angle->setValue(m_pView->viewAngle());
	angle->blockSignals(false);

	// Camera
	xEye->blockSignals(true);
	yEye->blockSignals(true);
	zEye->blockSignals(true);
	xEye->setValue(m_pView->cameraHandle()->eye().x());
	yEye->setValue(m_pView->cameraHandle()->eye().y());
	zEye->setValue(m_pView->cameraHandle()->eye().z());
	xEye->blockSignals(false);
	yEye->blockSignals(false);
	zEye->blockSignals(false);

	xTarget->blockSignals(true);
	yTarget->blockSignals(true);
	zTarget->blockSignals(true);
	xTarget->setValue(m_pView->cameraHandle()->target().x());
	yTarget->setValue(m_pView->cameraHandle()->target().y());
	zTarget->setValue(m_pView->cameraHandle()->target().z());
	xTarget->blockSignals(false);
	yTarget->blockSignals(false);
	zTarget->blockSignals(false);

	distance->blockSignals(true);
	distance->setValue(m_pView->cameraHandle()->distEyeTarget());
	distance->blockSignals(false);
}

//////////////////////////////////////////////////////////////////////
// Private Slots Functions
//////////////////////////////////////////////////////////////////////

void EditCamera::updateFov()
{
	m_pView->setViewAngle(angle->value());
	emit valueChanged();
}

void EditCamera::updateEye()
{
	const GLC_Vector3d vectEye(xEye->value(), yEye->value(), zEye->value());
	m_pView->cameraHandle()->setEyeCam(vectEye);
	distance->blockSignals(true);
	distance->setValue(m_pView->cameraHandle()->distEyeTarget());
	distance->blockSignals(false);
	emit valueChanged();
}

void EditCamera::updateTarget()
{
	const GLC_Vector3d vectTarget(xTarget->value(), yTarget->value(), zTarget->value());
	m_pView->cameraHandle()->setTargetCam(vectTarget);
	distance->blockSignals(true);
	distance->setValue(m_pView->cameraHandle()->distEyeTarget());
	distance->blockSignals(false);
	emit valueChanged();
}

void EditCamera::updateDistance()
{
	if (!qFuzzyCompare(distance->value(), 0.0))
	{
		m_pView->cameraHandle()->setDistEyeTarget(distance->value());
		// Camera
		xEye->blockSignals(true);
		yEye->blockSignals(true);
		zEye->blockSignals(true);
		xEye->setValue(m_pView->cameraHandle()->eye().x());
		yEye->setValue(m_pView->cameraHandle()->eye().y());
		zEye->setValue(m_pView->cameraHandle()->eye().z());
		xEye->blockSignals(false);
		yEye->blockSignals(false);
		zEye->blockSignals(false);
		emit valueChanged();
	}
}

