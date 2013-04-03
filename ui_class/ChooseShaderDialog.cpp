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

#include "ChooseShaderDialog.h"

ChooseShaderDialog::ChooseShaderDialog(OpenglView* pOpenglView, QWidget *parent)
: QDialog(parent)
, m_pOpenglView(pOpenglView)
, m_pShaderList(pOpenglView->getShaderListHandle())
, m_IconSize(200, 200)
{
	setupUi(this);

}

ChooseShaderDialog::~ChooseShaderDialog()
{

}

// get the shader Id
GLuint ChooseShaderDialog::shaderId() const
{
	const int currentRow= shaderListWidget->currentRow();
	if (currentRow == 0) return 0;
	else
	{
		return (*m_pShaderList)[currentRow - 1]->id();
	}
}

// Get the shader Name
QString ChooseShaderDialog::shaderName() const
{
	const int currentRow= shaderListWidget->currentRow();
	if (currentRow == 0) return tr("No Shader");
	else
	{
		return (*m_pShaderList)[currentRow - 1]->name();
	}
}

// Update list thumbnails
void ChooseShaderDialog::UpdateThumbnailsList()
{
	const GLuint previous= m_pOpenglView->globalShaderId();
	int currentRow= 0;
	shaderListWidget->clear();
	// Initialize the list of shader
	shaderListWidget->setIconSize(m_IconSize);
	// Add the default shader to the view
	QListWidgetItem* pItem= new QListWidgetItem(tr("No Shader (Phong)"));
	m_pOpenglView->setGlobalShaderId(0, QString());
	pItem->setIcon(QPixmap::fromImage(takeSnapShoot()).scaled(m_IconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	shaderListWidget->addItem(pItem);
	// Update the list of shader
	const int size= m_pShaderList->size();
	for (int i= 0; i < size; ++i)
	{
		GLC_Shader* pShader= (*m_pShaderList)[i];
		if (previous == pShader->id())
		{
			currentRow= i + 1;
		}
		pItem= new QListWidgetItem(pShader->name());
		m_pOpenglView->setGlobalShaderId(pShader->id(), QString());
		pItem->setIcon(QPixmap::fromImage(takeSnapShoot()).scaled(m_IconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
		shaderListWidget->addItem(pItem);
	}
	shaderListWidget->setCurrentRow(currentRow);
	m_pOpenglView->setGlobalShaderId(previous, QString());
}

// Take a snapshoot
QImage ChooseShaderDialog::takeSnapShoot()
{
	m_pOpenglView->setSnapShootMode(true);
	m_pOpenglView->setAutoBufferSwap(false);
	m_pOpenglView->viewportHandle()->forceAspectRatio(1.0);
	m_pOpenglView->updateGL();
	m_pOpenglView->viewportHandle()->updateAspectRatio();
	m_pOpenglView->viewportHandle()->updateProjectionMat();
	QImage snapShoot= m_pOpenglView->grabFrameBuffer();
	m_pOpenglView->setSnapShootMode(false);
	m_pOpenglView->updateGL();
	m_pOpenglView->setAutoBufferSwap(true);
	return snapShoot;
}

