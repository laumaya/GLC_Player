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

#include "ModelProperties.h"
#include "ListOfMaterial.h"
#include "../opengl_view/OpenglView.h"
#include <QStringList>

ModelProperties::ModelProperties(OpenglView* pOpenglView, const FileEntry& fileEntry, QWidget* pParent)
: QDialog(pParent)
, m_pOpenglView(pOpenglView)
, m_FileEntry(fileEntry)
, m_pListOfMaterial(NULL)
{
	setupUi(this);
	setValues();

	// Signal and slot connection
	// If the framebuffer extension is present, display the list of material
	if (GLC_State::frameBufferSupported())
	{
		listOfMaterialsButton->setVisible(false);
		displayListOfMaterial(true);
	}
	else
	{
		connect(listOfMaterialsButton, SIGNAL(toggled(bool)), this, SLOT(displayListOfMaterial(bool)));
	}

	connect(this, SIGNAL(finished(int)), this, SLOT(done()));

	attachedFiles->setHeaderLabel("FileName");
}

ModelProperties::~ModelProperties()
{

}

//////////////////////////////////////////////////////////////////////
// Public functions
//////////////////////////////////////////////////////////////////////
// Set the file entry
void ModelProperties::setFileEntry(const FileEntry& fileEntry)
{
	m_FileEntry= fileEntry;
	setValues();
	// Display the list of material if frambuffer is supported
	if (GLC_State::frameBufferSupported())
	{
		listOfMaterialsButton->setVisible(false);
		displayListOfMaterial(true);
	}

}

//////////////////////////////////////////////////////////////////////
// Private Slots
//////////////////////////////////////////////////////////////////////

// Display the model's list of material
void ModelProperties::displayListOfMaterial(bool show)
{
	if (show)
	{
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
			m_pListOfMaterial->showOnlyItemList(true);
			QList<GLC_Material*> materialList= m_FileEntry.getWorld().listOfMaterials();
			m_pListOfMaterial->CreateOrUpdate(materialList);
		}
		m_pListOfMaterial->show();
	}
	else
	{

		m_pListOfMaterial->hide();
	}
}

// Done
void ModelProperties::done()
{
	if (NULL != m_pListOfMaterial)
	{
		materialListGroup->layout()->removeWidget(m_pListOfMaterial);
		delete m_pListOfMaterial;
		m_pListOfMaterial= NULL;
		listOfMaterialsButton->blockSignals(true);
		listOfMaterialsButton->setChecked(false);
		listOfMaterialsButton->blockSignals(false);
		// Free fileEntry
		m_FileEntry= FileEntry();
	}
}

// Set values
void ModelProperties::setValues()
{
	// The Model Name
	const QFileInfo fileInfo(m_FileEntry.getFileName());
	name->setText(fileInfo.fileName());
	// The Model Location
	location->setText(fileInfo.absolutePath());
	// The Model file size
	double fileSize= static_cast<double>(fileInfo.size());

	sizeText->setText(sizeFromDoubleToString(fileSize));

	// The attached Files
	attachedFiles->clear();
	attachedNumber->clear();
	QStringList attachedFilesList= m_FileEntry.attachedFileNames();
	fileSize= 0.0;
	if (!attachedFilesList.isEmpty())
	{
		QStringList::iterator i= attachedFilesList.begin();
		while (i != attachedFilesList.constEnd())
		{
			QFileInfo currentFileInfo(*i);
			QStringList currentStringList;
			currentStringList << currentFileInfo.fileName();
			QTreeWidgetItem* pCurTreeWidgetItem= new QTreeWidgetItem(currentStringList);
			attachedFiles->addTopLevelItem(pCurTreeWidgetItem);
			++i;
		}
		fileSize= m_FileEntry.attachedFilesSize();
		attachedFiles->resizeColumnToContents(0);
		attachedSize->setText(sizeFromDoubleToString(fileSize));
	}
	else
	{
		attachedSize->clear();
	}
	attachedNumber->setText(QString::number(attachedFilesList.size()));
	// The number of mesh
	numberOfMeshs->setText(QString::number(m_FileEntry.getNumberOfInstances()));
	// The number of number of vertex
	numberOfVertexs->setText(QString::number(m_FileEntry.getNumberOfVertexs()));
	// The number of faces
	numberOfFaces->setText(QString::number(m_FileEntry.getNumberOfFaces()));

	// The number of material
	numberOfMaterials->setText(QString::number(m_FileEntry.numberOfMaterials()));

}

// Convert size from double to String
QString ModelProperties::sizeFromDoubleToString(const double& size)
{
	QString stringSize;
	if (size > 1024 * 1024)
	{
		stringSize= QString::number(size / (1024 * 1024), 'f', 2) + tr(" Mo");
	}
	else if (size > (1024))
	{
		stringSize= QString::number(size / (1024), 'f', 2) + tr(" Ko");
	}
	else
	{
		stringSize= QString::number(size, 'f', 2) + tr(" Bytes");
	}
	return stringSize;
}
