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

#include "InstanceProperty.h"
#include <GLC_3DViewInstance>

InstanceProperty::InstanceProperty(OpenglView* pView, QAction* pAssignShader, QWidget *parent)
: QDockWidget(parent)
, m_pOpenglView(pView)
, m_pInstance(NULL)
, m_SubMaterialList()
, m_pListOfMaterial(NULL)
{
	setupUi(this);
	assignShaderCmd->setDefaultAction(pAssignShader);
	// Signal and Slot connection
	connect(doneCmd, SIGNAL(clicked()), this, SLOT(doneSlot()));
	connect(reverseNormalsCmd, SIGNAL(clicked()), this, SLOT(reverseNormals()));
	//connect(editMaterialsCmd, SIGNAL(toggled(bool)), this, SLOT(viewMaterialPropertySlot(bool)));
	//connect(subMaterialListCmd, SIGNAL(clicked()), this, SLOT(viewSubMaterialListSlot()));
}

InstanceProperty::~InstanceProperty()
{
	m_SubMaterialList.clear();
}

//////////////////////////////////////////////////////////////////////
// Public set methods
//////////////////////////////////////////////////////////////////////

// Set the instance to view
void InstanceProperty::setInstance(GLC_3DViewInstance* pInstance)
{
	m_pInstance= pInstance;

	// Update window fields
	update();
}

// Get List of material
ListOfMaterial* InstanceProperty::getListOfMaterials()
{
	m_SubMaterialList= m_pInstance->materialSet().toList();

	if (NULL == m_pListOfMaterial)
	{
		m_pListOfMaterial= new ListOfMaterial(m_pOpenglView, materialListGroup);
		QLayout *pLayout = materialListGroup->layout();
		if (NULL == pLayout)
		{
			pLayout= new QVBoxLayout;
			materialListGroup->setLayout(pLayout);
		}
		pLayout->addWidget(m_pListOfMaterial);
	}
	m_pListOfMaterial->show();

	return m_pListOfMaterial;
}


//////////////////////////////////////////////////////////////////////
// Public slots
//////////////////////////////////////////////////////////////////////

// Update windows fields
void InstanceProperty::update()
{
	Q_ASSERT(NULL != m_pInstance);

	// Instance fields
	const QString name(m_pInstance->name());
	meshName->setText(name);


	const int vertexNumber= m_pInstance->numberOfVertex();
	numberOfVertex->setText(QString::number(vertexNumber));
	const int faceNumber= m_pInstance->numberOfFaces();
	numberOfFace->setText(QString::number(faceNumber));
	bodyCount->setText(QString::number(m_pInstance->numberOfBody()));

	// Bounding box fields
	const GLC_Vector3d boxVector= m_pInstance->boundingBox().upperCorner() - m_pInstance->boundingBox().lowerCorner();
	const double bBoxX= fabs(boxVector.x());
	const double bBoxY= fabs(boxVector.y());
	const double bBoxZ= fabs(boxVector.z());
	bBoxXLenght->setText(QString::number(bBoxX));
	bBoxYLenght->setText(QString::number(bBoxY));
	bBoxZLenght->setText(QString::number(bBoxZ));

	emit viewSubMaterialList();
}

// Reverse selection normals
void InstanceProperty::reverseNormals()
{
	m_pInstance->reverseGeometriesNormals();
	emit updateView();
}

// Done instance property visualisation
void InstanceProperty::doneSlot()
{
	delete m_pListOfMaterial;
	m_pListOfMaterial= NULL;
	emit doneSignal();
}

