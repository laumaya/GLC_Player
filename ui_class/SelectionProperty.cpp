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

#include "SelectionProperty.h"
#include <GLC_3DViewInstance>

SelectionProperty::SelectionProperty(QAction* pShowHide, QAction* pAssignShader, QAction* pInstanceProperty, QWidget *parent)
: QWidget(parent)
, m_pSelections(NULL)
{
	setupUi(this);
	hideShowCmd->setDefaultAction(pShowHide);
	editPropertyCmd->setDefaultAction(pInstanceProperty);
	assignShaderCmd->setDefaultAction(pAssignShader);

	connect(reverseNormalsCmd, SIGNAL(clicked()), this, SLOT(reverseNormals()));

	numberOfObjectLabel->hide();
	numberOfObject->hide();
	editPropertyCmd->hide();
}

SelectionProperty::~SelectionProperty()
{
}

// Set the current selection
void SelectionProperty::setSelection(PointerViewInstanceHash* pSelections)
{
	m_pSelections= pSelections;
	PointerViewInstanceHash::iterator iEntry= m_pSelections->begin();

	int vertexNumber= 0;
	int faceNumber= 0;
	int subMaterialNumber= 0;
	QSet<GLC_Material*> materialSet;
    while (iEntry != m_pSelections->constEnd())
    {
		vertexNumber+= iEntry.value()->numberOfVertex();
		faceNumber+= iEntry.value()->numberOfFaces();
		materialSet.unite(iEntry.value()->materialSet());

		iEntry++;
    }
    subMaterialNumber+= materialSet.size();

	numberOfVertex->setText(QString::number(vertexNumber));
	numberOfFace->setText(QString::number(faceNumber));
	numberOfSubMaterial->setText(QString::number(subMaterialNumber));
	if (m_pSelections->size() > 1)
	{
		selectionName->clear();
		numberOfObject->setText(QString::number(m_pSelections->size()));
		editPropertyCmd->hide();
		numberOfObjectLabel->show();
		numberOfObject->show();
	}
	else
	{
		selectionName->setText(m_pSelections->begin().value()->name());
		numberOfObjectLabel->hide();
		numberOfObject->hide();
		editPropertyCmd->show();
	}
	reverseNormalsCmd->setEnabled(true);
	hideShowCmd->setEnabled(true);
}
// Unset the current selection
void SelectionProperty::unsetSelection()
{
	m_pSelections= NULL;
	selectionName->clear();
	numberOfVertex->clear();
	numberOfFace->clear();
	numberOfSubMaterial->clear();
	reverseNormalsCmd->setEnabled(false);
	hideShowCmd->setEnabled(false);

	numberOfObjectLabel->hide();
	numberOfObject->hide();
	editPropertyCmd->hide();
}
// Reverse selection normals
void SelectionProperty::reverseNormals()
{
	if (NULL != m_pSelections)
	{
		PointerViewInstanceHash::iterator iEntry= m_pSelections->begin();
	    while (iEntry != m_pSelections->constEnd())
	    {
	    	iEntry.value()->reverseGeometriesNormals();
			iEntry++;
	    }
		emit updateView();
	}
}


