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

#include "EditLightDialog.h"

#include <GLC_Light>

#include <QtGui>

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
// Default constructor
EditLightDialog::EditLightDialog(GLC_Light* pLight, QList<GLC_Light*>* pLightList, QWidget * parent)
: QDialog(parent)
, m_pMasterLight(pLight)
, m_pLightList(pLightList)
, m_AmbientColor()
, m_DiffuseColor()
, m_SpecularColor()
, m_pCurrentLight(m_pMasterLight)
{
    setupUi(this);
    initValues();
  	        
    // Signals and slot connection
    connect(editAmbient, SIGNAL(clicked()), this, SLOT(changeAmbientColor()));
    connect(editDiffuse, SIGNAL(clicked()), this, SLOT(changeDiffuseColor()));
    connect(editSpecular, SIGNAL(clicked()), this, SLOT(changeSpecularColor()));

    connect(AddLightButton, SIGNAL(clicked()), this, SLOT(addLight()));
    connect(deleteLightButton, SIGNAL(clicked()), this, SLOT(removeLight()));

    connect(listOfLightWidget, SIGNAL(currentRowChanged(int)), this, SLOT(changeCurrentLight(int)));


    deleteLightButton->setEnabled(false);
    listOfLightWidget->setCurrentRow(0);

}

//////////////////////////////////////////////////////////////////////
// Private Slots Functions
//////////////////////////////////////////////////////////////////////
// Change Ambient color
void EditLightDialog::changeAmbientColor()
{
	QColor color = QColorDialog::getColor(m_AmbientColor, this);
	if (color.isValid())
	{
		colorLabelAmbient->setPalette(QPalette(color));
		m_AmbientColor= color;
		updateValues();
		emit lightUpdated();
	}
 	
}
// Change Diffuse color
void EditLightDialog::changeDiffuseColor()
{
	QColor color = QColorDialog::getColor(m_DiffuseColor, this);
	if (color.isValid())
	{
		colorLabelDiffuse->setPalette(QPalette(color));
		m_DiffuseColor= color;
		updateValues();
		emit lightUpdated();
	}
}

// Change Specular color
void EditLightDialog::changeSpecularColor()
{
	QColor color = QColorDialog::getColor(m_SpecularColor, this);
	if (color.isValid())
	{
		colorLabelSpecular->setPalette(QPalette(color));
		m_SpecularColor= color;
		updateValues();
		emit lightUpdated();
	}
 	
}

void EditLightDialog::addLight()
{
	QGLContext* pCurrentContext= const_cast<QGLContext*>(QGLContext::currentContext());
	if ((GLC_Light::builtAbleLightCount(pCurrentContext) - 4) > 0)
	{
		int lightIndex= GLC_Light::maxLightCount() - GLC_Light::builtAbleLightCount(pCurrentContext);
		QString lightName= tr("Light0") + QString::number(lightIndex);
		GLC_Light* pLight= new GLC_Light();
		pLight->setName(lightName);
		pLight->setTwoSided(m_pMasterLight->isTwoSided());
		m_pLightList->append(pLight);

		QListWidgetItem* pItem= new QListWidgetItem(QIcon(":images/LightBulb.png"), lightName);
		listOfLightWidget->addItem(pItem);
		listOfLightWidget->setCurrentItem(pItem);

		m_pCurrentLight= pLight;

		updateColors();
		deleteLightButton->setEnabled(true);
		AddLightButton->setEnabled((GLC_Light::builtAbleLightCount(pCurrentContext) - 4) != 0);

	}
}

void EditLightDialog::removeLight()
{
	int currentRow= listOfLightWidget->currentRow();
	if (currentRow > 0)
	{
		delete listOfLightWidget->takeItem(currentRow);
		delete m_pLightList->takeAt(currentRow - 1);
		changeCurrentLight(currentRow - 1);
	}
	AddLightButton->setEnabled(true);
	deleteLightButton->setEnabled(!m_pLightList->isEmpty());
}

void EditLightDialog::changeCurrentLight(int index)
{
	if (index == 0)
	{
		m_pCurrentLight= m_pMasterLight;
		deleteLightButton->setEnabled(false);
	}
	else
	{
		m_pCurrentLight= m_pLightList->at(index - 1);
		deleteLightButton->setEnabled(true);
	}

	updateColors();
	emit currentLightChanged(index);
}

void EditLightDialog::accept()
{
	listOfLightWidget->setCurrentRow(0);
	QDialog::accept();
}


//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////
// init background dialog values
void EditLightDialog::initValues()
{
	
	m_AmbientColor= m_pCurrentLight->ambientColor();
	m_DiffuseColor= m_pCurrentLight->diffuseColor();
	m_SpecularColor= m_pCurrentLight->specularColor();
	
	colorLabelAmbient->setPalette(QPalette(m_AmbientColor));
	colorLabelDiffuse->setPalette(QPalette(m_DiffuseColor));
	colorLabelSpecular->setPalette(QPalette(m_SpecularColor));

	QListWidgetItem* pItem= new QListWidgetItem(QIcon(":images/LightBulb.png"), m_pCurrentLight->name());
	listOfLightWidget->addItem(pItem);
	const int size= m_pLightList->size();
	for (int i= 0; i < size; ++i)
	{
		pItem= new QListWidgetItem(QIcon(":images/LightBulb.png"), m_pLightList->at(i)->name());
		listOfLightWidget->addItem(pItem);
	}
}

// update light values
void EditLightDialog::updateValues()
{
	m_pCurrentLight->setAmbientColor(m_AmbientColor);
	m_pCurrentLight->setDiffuseColor(m_DiffuseColor);
	m_pCurrentLight->setSpecularColor(m_SpecularColor);
}

void EditLightDialog::updateColors()
{
	m_AmbientColor= m_pCurrentLight->ambientColor();
	m_DiffuseColor= m_pCurrentLight->diffuseColor();
	m_SpecularColor= m_pCurrentLight->specularColor();

	colorLabelAmbient->setPalette(QPalette(m_AmbientColor));
	colorLabelDiffuse->setPalette(QPalette(m_DiffuseColor));
	colorLabelSpecular->setPalette(QPalette(m_SpecularColor));
}

