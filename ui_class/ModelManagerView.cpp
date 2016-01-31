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

#include "ModelManagerView.h"
#include "ListOfMaterial.h"
#include "../opengl_view/OpenglView.h"

ModelManagerView::ModelManagerView(OpenglView* pOpenglView, QAction* pInstanceProperties, QAction* pHideUnselect
		, QAction* pCopy, QAction* pPaste, QPair<GLC_uint, GLC_StructOccurrence*>* pClipBoard, QWidget *parent)
: QWidget(parent)
, m_pOpenglView(pOpenglView)
, m_FileEntry()
, m_ViewIsUpToDate(false)
, m_pModelStructure(NULL)
{
	setupUi(this);
	// Add the model structure widget
	QLayout *pLayout = modelCmdGroup->layout();
	if (NULL == pLayout)
	{
		pLayout= new QVBoxLayout;
		modelCmdGroup->setLayout(pLayout);
	}
	m_pModelStructure= new ModelStructure(m_pOpenglView, pInstanceProperties, pHideUnselect, pCopy, pPaste, pClipBoard, this);
	pLayout->addWidget(m_pModelStructure);

	connect(modelPropertiesCmd, SIGNAL(clicked()), this , SIGNAL(currentModelProperties()));

	connect(synchroCmd, SIGNAL(toggled(bool)), m_pModelStructure , SLOT(setSynchronisation(bool)));
	connect(expandCmd, SIGNAL(clicked()), m_pModelStructure , SLOT(expandOnSelection()));
	connect(collapseCmd, SIGNAL(clicked()), m_pModelStructure , SLOT(collapseOnSelection()));


}

ModelManagerView::~ModelManagerView()
{
	delete m_pModelStructure;
}

//////////////////////////////////////////////////////////////////////
// Public set functions
//////////////////////////////////////////////////////////////////////

// Set the file Entry (Model)
void ModelManagerView::setModel(const FileEntry& fileEntry)
{
	qDebug() << "ModelManagerView::setModel";
	if (!(m_FileEntry == fileEntry) && fileEntry.isLoaded())
	{
		m_FileEntry= fileEntry;
		m_ViewIsUpToDate= false;
		m_pModelStructure->clear();
	}
	else if (!fileEntry.isLoaded())
	{
		m_FileEntry= FileEntry();
		m_ViewIsUpToDate= false;
		m_pModelStructure->clear();
	}
	else
	{
		qDebug() << "Bizarre!!!!";
	}
}

// Update the view
void ModelManagerView::updateView()
{
	qDebug() << "ModelManagerView::updateView";
	if (!m_ViewIsUpToDate)
	{
		if (!(FileEntry() == m_FileEntry) && !m_FileEntry.name().isEmpty() && m_FileEntry.isLoaded())
		{
			// Update the model Name
			modelCmdGroup->setTitle(tr("Model : ") + m_FileEntry.name().left(30));

			// Update the model structure
			displayModelStructure();
			m_pModelStructure->updateTreeSelection();

			// Update Ui
			modelPropertiesCmd->setEnabled(true);

		}
		else
		{
			// Update the model Name
			modelCmdGroup->setTitle(tr("No Model"));

			// Update Ui
			modelPropertiesCmd->setEnabled(false);
		}
		m_ViewIsUpToDate= true;
	}
	else
	{
		m_pModelStructure->updateTreeShowNoShow();
		m_pModelStructure->updateTreeSelection();
	}

}

//! Clear the model manager view
void ModelManagerView::clear()
{
	m_FileEntry= FileEntry();
	m_ViewIsUpToDate= false;
	m_pModelStructure->clear();
	modelCmdGroup->setTitle(tr("No Model"));
	// Update Ui
	modelPropertiesCmd->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////
// Private Slots
//////////////////////////////////////////////////////////////////////

// Display the model structure
void ModelManagerView::displayModelStructure()
{
	if (!m_ViewIsUpToDate)
	{
		m_pModelStructure->setWorld(m_FileEntry.getWorld());
	}
}

