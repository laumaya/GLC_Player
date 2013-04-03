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

#include "MaterialProperty.h"
#include "../opengl_view/OpenglView.h"

#include <GLC_Material>
#include <GLC_Factory>
#include <QGLContext>
#include <GLC_Factory>

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
// Default constructor
MaterialProperty::MaterialProperty(OpenglView* pOpenglView, GLC_Material* pMat, QWidget * parent)
: QDockWidget(parent)
, m_pOpenglView(pOpenglView)
, m_pMaterial(pMat)
, m_pPreviewMaterial(NULL)
, m_AmbientColor()
, m_DiffuseColor()
, m_SpecularColor()
, m_EmissiveColor()
, m_pMaterialOpenglView(new MaterialOpenglView(this, pOpenglView, m_pMaterial))
, m_Alpha()
{
    setupUi(this);
    initValues();
    initWorld();

    // Signals and slot connection
    connect(editAmbient, SIGNAL(clicked()), this, SLOT(changeAmbientColor()));
    connect(editDiffuse, SIGNAL(clicked()), this, SLOT(changeDiffuseColor()));
    connect(editSpecular, SIGNAL(clicked()), this, SLOT(changeSpecularColor()));
    connect(editEmissive, SIGNAL(clicked()), this, SLOT(changeEmissiveColor()));
    connect(useTexture, SIGNAL(stateChanged(int)), this, SLOT(textureState(int)));
    connect(textureFileNameCombo, SIGNAL(editTextChanged(const QString&)), this, SLOT(textureFileNameChanged(const QString&)));
    connect(browseTexture, SIGNAL(clicked()), this, SLOT(setTextureFileName()));
    connect(applyCmd, SIGNAL(clicked()), this, SLOT(apply()));
    connect(cancelCmd, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(shiness, SIGNAL(valueChanged(double)), this, SLOT(enableCmd()));
    connect(opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(opacityChange(int)));

}

// Set material to edit
void MaterialProperty::setMaterial(GLC_Material* pMaterial)
{
	m_pMaterial= pMaterial;
	// Update Preview
	m_pPreviewMaterial= NULL;
	m_pMaterialOpenglView->update(m_pMaterial);
	initValues();
}


//////////////////////////////////////////////////////////////////////
// Private Slots Functions
//////////////////////////////////////////////////////////////////////
// Change Ambient color
void MaterialProperty::changeAmbientColor()
{
	QColor color = QColorDialog::getColor(m_AmbientColor, this);
	if (color.isValid())
	{
		colorLabelAmbient->setPalette(QPalette(color));
		m_AmbientColor= color;
		enableCmd();

	}

}
// Change Diffuse color
void MaterialProperty::changeDiffuseColor()
{
	QColor color = QColorDialog::getColor(m_DiffuseColor, this);
	if (color.isValid())
	{
		colorLabelDiffuse->setPalette(QPalette(color));
		m_DiffuseColor= color;
		enableCmd();
	}

}

// Change Specular color
void MaterialProperty::changeSpecularColor()
{
	QColor color = QColorDialog::getColor(m_SpecularColor, this);
	if (color.isValid())
	{
		colorLabelSpecular->setPalette(QPalette(color));
		m_SpecularColor= color;
		enableCmd();

	}

}

// Change Emissive color
void MaterialProperty::changeEmissiveColor()
{
	QColor color = QColorDialog::getColor(m_EmissiveColor, this);
	if (color.isValid())
	{
		colorLabelEmissive->setPalette(QPalette(color));
		m_EmissiveColor= color;

		enableCmd();

	}

}

// Set the current background image file name
void MaterialProperty::setTextureFileName()
{
	// Set Image filter
	QList<QByteArray> imageFormat(QImageReader::supportedImageFormats());
	const int numberOfFormat= imageFormat.size();
	QString filter(tr("All Images ("));
	for (int i= 0; i < numberOfFormat; ++i)
	{
		filter.append(QString(" *.") + QString(imageFormat[i]));
	}
	filter.append(")");

	QString textureFileName = QFileDialog::getOpenFileName(this, tr("Texture File Name")
												, textureFileNameCombo->currentText()
												, filter);
	if (!textureFileName.isEmpty())
	{
		textureFileNameCombo->setEditText(textureFileName);
		enableCmd();
	}

 }

// The texture filename as changed
void MaterialProperty::textureFileNameChanged(const QString& textureName)
{
	if (NULL == m_pPreviewMaterial)
	{
		m_pPreviewMaterial= new GLC_Material(*m_pMaterial);
	}

	try
	{
		GLC_Factory* pFactory= GLC_Factory::instance();
		GLC_Texture* pTexture= pFactory->createTexture(textureName);
		m_pPreviewMaterial->setTexture(pTexture);
		m_pMaterialOpenglView->update(m_pPreviewMaterial);
	}
	catch (std::exception &e)
	{
	   	QString message(tr("Failed to load image :") + textureName);
	   	QWidget *pParent= dynamic_cast<QWidget*>(this->parent());
	    QMessageBox::critical(pParent, QCoreApplication::applicationName(), message);
	}
}

// Active / inactive material texture
void MaterialProperty::textureState(int state)
{
	if (NULL == m_pPreviewMaterial)
	{
		m_pPreviewMaterial= new GLC_Material(*m_pMaterial);
	}

	if (state == Qt::Checked)
	{
		textureFileNameCombo->setEnabled(true);
		browseTexture->setEnabled(true);
		// Update preview if needed
		if (!textureFileNameCombo->currentText().isEmpty())
		{
			try
			{
				GLC_Factory* pFactory= GLC_Factory::instance();
				GLC_Texture* pTexture= pFactory->createTexture(textureFileNameCombo->currentText());
				m_pPreviewMaterial->setTexture(pTexture);
				m_pMaterialOpenglView->update(m_pPreviewMaterial);
			}
			catch (std::exception &e)
			{
			   	QString message(tr("Failed to load image :") + textureFileNameCombo->currentText());
			   	QWidget *pParent= dynamic_cast<QWidget*>(this->parent());
			    QMessageBox::critical(pParent, QCoreApplication::applicationName(), message);
			}
		}


	}
	else if (state == Qt::Unchecked)
	{
		textureFileNameCombo->setEnabled(false);
		browseTexture->setEnabled(false);
		// Update Preview if needed
		m_pPreviewMaterial->removeTexture();
		m_pMaterialOpenglView->update(m_pPreviewMaterial);
	}

	enableCmd();

}

void MaterialProperty::apply()
{
	m_pMaterial->setAmbientColor(m_AmbientColor);
	m_pMaterial->setDiffuseColor(m_DiffuseColor);
	m_pMaterial->setSpecularColor(m_SpecularColor);
	m_pMaterial->setEmissiveColor(m_EmissiveColor);
	m_pMaterial->setShininess(shiness->value());

	// texture assignement
	if (useTexture->checkState() == Qt::Checked)
	{
		try
		{
			GLC_Factory* pFactory= GLC_Factory::instance();
			GLC_Texture* pTexture= pFactory->createTexture(textureFileNameCombo->currentText());
			m_pMaterial->setTexture(pTexture);
		}
		catch (std::exception &e)
		{
		   	QString message(tr("Failed to load image :") + textureFileNameCombo->currentText());
		   	QWidget *pParent= dynamic_cast<QWidget*>(this->parent());
		    QMessageBox::critical(pParent, QCoreApplication::applicationName(), message);
		    return;
		}
	}
	else
	{
		m_pMaterial->removeTexture();
	}
	// Set transparency after material to overwrite material alpha.
	m_pMaterial->setOpacity(m_Alpha);
	// Disable command button
	cancelCmd->setEnabled(false);
	applyCmd->setEnabled(false);

	m_pOpenglView->updateGL();
	emit materialUpdated(m_pMaterial);
}

// Cancel modification
void MaterialProperty::cancel()
{
	initValues();
	m_pPreviewMaterial= new GLC_Material(*m_pMaterial);
	m_pMaterialOpenglView->update(m_pPreviewMaterial);
}

//!Enable command button
void MaterialProperty::enableCmd()
{
	// Enable command button
	cancelCmd->setEnabled(true);
	applyCmd->setEnabled(true);

	if (NULL == m_pPreviewMaterial)
	{
		m_pPreviewMaterial= new GLC_Material(*m_pMaterial);
	}
	m_pPreviewMaterial->setAmbientColor(m_AmbientColor);
	m_pPreviewMaterial->setDiffuseColor(m_DiffuseColor);
	m_pPreviewMaterial->setSpecularColor(m_SpecularColor);
	m_pPreviewMaterial->setEmissiveColor(m_EmissiveColor);
	m_pPreviewMaterial->setShininess(shiness->value());
	m_pPreviewMaterial->setOpacity(m_Alpha);

	m_pMaterialOpenglView->update(m_pPreviewMaterial);
}

// Opacity Change
void MaterialProperty::opacityChange(int value)
{
	m_Alpha= static_cast<double>(value) / 100.0;
	enableCmd();
}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////
// init background dialog values
void MaterialProperty::initValues()
{
	textureFileNameCombo->blockSignals(true);
	materialName->blockSignals(true);
	shiness->blockSignals(true);
	colorLabelAmbient->blockSignals(true);
	colorLabelDiffuse->blockSignals(true);
	colorLabelSpecular->blockSignals(true);
	colorLabelEmissive->blockSignals(true);
	useTexture->blockSignals(true);

	materialName->setText(m_pMaterial->name());
	m_AmbientColor= m_pMaterial->ambientColor();
	m_DiffuseColor= m_pMaterial->diffuseColor();
	m_SpecularColor= m_pMaterial->specularColor();
	m_EmissiveColor= m_pMaterial->emissiveColor();
	shiness->setValue(m_pMaterial->shininess());

	opacitySlider->blockSignals(true);
	m_Alpha= m_pMaterial->opacity();
	opacitySlider->setValue(static_cast<int>(m_Alpha * 100.0));
	opacitySlider->blockSignals(false);

	colorLabelAmbient->setPalette(QPalette(m_AmbientColor));
	colorLabelDiffuse->setPalette(QPalette(m_DiffuseColor));
	colorLabelSpecular->setPalette(QPalette(m_SpecularColor));
	colorLabelEmissive->setPalette(QPalette(m_EmissiveColor));

	if (m_pMaterial->hasTexture())
	{
		useTexture->setChecked(true);
		textureFileNameCombo->setEnabled(true);
		browseTexture->setEnabled(true);
		textureFileNameCombo->setEditText(m_pMaterial->textureFileName());
	}
	else
	{
		useTexture->setChecked(false);
		textureFileNameCombo->clearEditText();
		textureFileNameCombo->setEnabled(false);
		browseTexture->setEnabled(false);

	}

	// Disable command button
	cancelCmd->setEnabled(false);
	applyCmd->setEnabled(false);

	textureFileNameCombo->blockSignals(false);
	materialName->blockSignals(false);
	shiness->blockSignals(false);
	colorLabelAmbient->blockSignals(false);
	colorLabelDiffuse->blockSignals(false);
	colorLabelSpecular->blockSignals(false);
	colorLabelEmissive->blockSignals(false);
	useTexture->blockSignals(false);
}

// init material representatio world
void MaterialProperty::initWorld()
{
	QLayout *pLayout = previewGroup->layout();
	if (NULL == pLayout)
	{
		pLayout= new QVBoxLayout;
		previewGroup->setLayout(pLayout);
	}
	pLayout->addWidget(m_pMaterialOpenglView);
}


