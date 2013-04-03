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

#include "FileEntry.h"
#include "./opengl_view/OpenglView.h"
#include <GLC_Octree>

// Default constructor
FileEntry::FileEntry(const QString& fileName)
: m_Id(glc::GLC_GenUserID())
, m_FileName(fileName)
, m_IsLoading(false)
, m_pIsLoaded(new bool(false))
, m_World()
, m_Error()
, m_Camera()
, m_CameraIsSet(false)
, m_PolyMode(GL_FILL)
, m_NumberOfFaces()
, m_NumberOfVertex()
, m_NumberOfMaterials()
, m_AngleOfView(35.0) // default angle is 35 degre
, m_AttachedFileNames()
, m_ModifiedMaterials()
, m_pNumberOfEntry(new int(1))
, m_pInvisibleListOfInstanceName(NULL)
, m_pShadedInstanceList(NULL)
{
	m_Camera.setDefaultUpVector(glc::Z_AXIS);
}

// Construct from fileName and camera
FileEntry::FileEntry(const QString& fileName, const GLC_Camera& cam, const double& angle
		, QSet<GLC_Material*> materialsSet, const QList<QString>& invisibleInstance
		, const QHash<QString, QList<QString> >& shadedInstance)
: m_Id(glc::GLC_GenUserID())
, m_FileName(fileName)
, m_IsLoading(false)
, m_pIsLoaded(new bool(false))
, m_World()
, m_Error()
, m_Camera(cam)
, m_CameraIsSet(!(cam == GLC_Camera()))
, m_PolyMode(GL_FILL)
, m_NumberOfFaces()
, m_NumberOfVertex()
, m_NumberOfMaterials()
, m_AngleOfView(angle)
, m_AttachedFileNames()
, m_ModifiedMaterials(materialsSet)
, m_pNumberOfEntry(new int(1))
, m_pInvisibleListOfInstanceName(new QList<QString>(invisibleInstance))
, m_pShadedInstanceList(new QHash<QString, QList<QString> >(shadedInstance))
{

}

// Copy constructor
FileEntry::FileEntry(const FileEntry& entry)
: m_Id(entry.m_Id)
, m_FileName(entry.m_FileName)
, m_IsLoading(entry.m_IsLoading)
, m_pIsLoaded(entry.m_pIsLoaded)
, m_World(entry.m_World)
, m_Error(entry.m_Error)
, m_Camera(entry.m_Camera)
, m_CameraIsSet(entry.m_CameraIsSet)
, m_NumberOfFaces(entry.m_NumberOfFaces)
, m_NumberOfVertex(entry.m_NumberOfVertex)
, m_NumberOfMaterials(entry.m_NumberOfMaterials)
, m_AngleOfView(entry.m_AngleOfView)
, m_AttachedFileNames(entry.m_AttachedFileNames)
, m_ModifiedMaterials(entry.m_ModifiedMaterials)
, m_pNumberOfEntry(entry.m_pNumberOfEntry)
, m_pInvisibleListOfInstanceName(entry.m_pInvisibleListOfInstanceName)
, m_pShadedInstanceList(entry.m_pShadedInstanceList)
{
	// Increment the number of entry
	++(*m_pNumberOfEntry);
}
// the destructor
FileEntry::~FileEntry()
{
	m_AttachedFileNames.clear();
	if ((--(*m_pNumberOfEntry)) == 0)
	{
		if (!isLoaded())
		{
			QList<GLC_Material*> modifiedMaterialList(m_ModifiedMaterials.toList());
			const int size= modifiedMaterialList.size();
			for (int i= 0; i < size; ++i)
			{
				if (modifiedMaterialList[i]->isUnused())
				{
					delete modifiedMaterialList[i];
				}
				++i;
			}
		}
		delete m_pNumberOfEntry;
		delete m_pInvisibleListOfInstanceName;
		delete m_pShadedInstanceList;
		delete m_pIsLoaded;
	}

	m_ModifiedMaterials.clear();
}

//////////////////////////////////////////////////////////////////////
// Public Interface
//////////////////////////////////////////////////////////////////////

// Overload "=" operator
FileEntry& FileEntry::operator=(const FileEntry& fileEntry)
{
	if (!(this == &fileEntry))
	{
		m_Id= fileEntry.m_Id;
		m_FileName= fileEntry.m_FileName;
		m_IsLoading= fileEntry.m_IsLoading;
		m_pIsLoaded= fileEntry.m_pIsLoaded;
		m_World= fileEntry.m_World;
		m_Error= fileEntry.m_Error;
		m_Camera= fileEntry.m_Camera;
		m_CameraIsSet= fileEntry.m_CameraIsSet;
		m_NumberOfFaces= fileEntry.m_NumberOfFaces;
		m_NumberOfVertex= fileEntry.m_NumberOfVertex;
		m_NumberOfMaterials= fileEntry.m_NumberOfMaterials;
		m_AngleOfView= fileEntry.m_AngleOfView;
		m_AttachedFileNames.clear();
		m_AttachedFileNames= fileEntry.m_AttachedFileNames;
		m_ModifiedMaterials= fileEntry.m_ModifiedMaterials;
		m_pNumberOfEntry= fileEntry.m_pNumberOfEntry;
		m_pInvisibleListOfInstanceName= fileEntry.m_pInvisibleListOfInstanceName;
		m_pShadedInstanceList= fileEntry.m_pShadedInstanceList;
		// Increment the number of entry
		++(*m_pNumberOfEntry);
	}
	return *this;
}

// OverLoad "==" operator
bool FileEntry::operator==(const FileEntry& fileEntry)
{
	bool result= true;

	if (this != &fileEntry)
	{
		result= result && (m_Id == fileEntry.m_Id);
	}

	return result;
}

// Set the GLC_World of this file entry
void FileEntry::setWorld(GLC_World& world)
{
	m_World= world;
	m_NumberOfFaces= m_World.numberOfFaces();
	m_NumberOfVertex= m_World.numberOfVertex();
	m_NumberOfMaterials= m_World.numberOfMaterials();
	m_AttachedFileNames.clear();

	// Set camera default up vector
	if (!m_CameraIsSet)
	{
		m_Camera.setDefaultUpVector(m_World.upVector());
	}
	QList<GLC_Material*> worldListOfMaterial(m_World.listOfMaterials());
	//Update Modified material
	QList<GLC_Material*> listOfModifiedMaterial(m_ModifiedMaterials.toList());
	int size= listOfModifiedMaterial.size();
	int i= 0;
	while (i < size)
	{
		// Get Material Name
		const QString currentMaterialName= listOfModifiedMaterial[i]->name();
		// Find the material
		QList<GLC_Material*>::iterator iMaterial= worldListOfMaterial.begin();
		while (iMaterial != worldListOfMaterial.constEnd())
		{
			if ((*iMaterial)->name() == currentMaterialName)
			{
				// Modified World material
				(*iMaterial)->setMaterial(listOfModifiedMaterial[i]);
				// Delete unused modified material
				delete (listOfModifiedMaterial[i]);

				// Set modified material pointer to world material
				listOfModifiedMaterial[i]= (*iMaterial);
				iMaterial= worldListOfMaterial.end();
			}
			else
			{
				++iMaterial;
			}
		}
		++i;
	}
	m_ModifiedMaterials.clear();
	m_ModifiedMaterials= QSet<GLC_Material*>::fromList(listOfModifiedMaterial);

	// Get the list of instance
	QList<GLC_3DViewInstance*> listOfInstance(m_World.collection()->instancesHandle());
	QHash<QString, GLC_3DViewInstance*> instanceHash;
	size= listOfInstance.size();
	for (int i= 0; i < size; ++i)
	{
		instanceHash.insert(listOfInstance[i]->name(), listOfInstance[i]);
	}

	// Search invisible instance
	if (NULL != m_pInvisibleListOfInstanceName)
	{
		size= m_pInvisibleListOfInstanceName->size();
		for (int i= 0; i < size; ++i)
		{
			const QString instanceName= (*m_pInvisibleListOfInstanceName)[i];
			if (instanceHash.contains(instanceName))
			{
				instanceHash.value(instanceName)->setVisibility(false);
			}
		}
		delete m_pInvisibleListOfInstanceName;
		m_pInvisibleListOfInstanceName= NULL;
	}


	// Search shaded instance
	if (NULL != m_pShadedInstanceList)
	{
		// get the shader list
		ShaderList listOfShader= OpenglView::shaderList();
		for (ShaderList::const_iterator iShader= listOfShader.constBegin(); iShader != listOfShader.constEnd(); ++iShader)
		{
			if (m_pShadedInstanceList->contains((*iShader)->name()))
			{
				// Bind the shader
				const GLuint currentShaderId= (*iShader)->id();
				m_World.collection()->bindShader(currentShaderId);

				QList<QString> instanceNameList= m_pShadedInstanceList->value((*iShader)->name());

				for (QList<QString>::const_iterator iInstanceName= instanceNameList.constBegin(); iInstanceName != instanceNameList.constEnd(); ++iInstanceName)
				{
					if (instanceHash.contains(*iInstanceName))
					{
						m_World.collection()->changeShadingGroup(instanceHash.value(*iInstanceName)->id(), currentShaderId);
					}
				}
			}
		}
		delete m_pShadedInstanceList;
		m_pShadedInstanceList= NULL;
	}

	*m_pIsLoaded= true;
}

// return true if this entry is ready to load
bool FileEntry::isReadyToLoad() const
{
	return (!m_IsLoading) && (!isLoaded()) && (!errorOccurWhileLoading());
}

// Reload this file model
void FileEntry::reload()
{
	if (*m_pIsLoaded == true)
	{
		m_IsLoading= false;
		*m_pIsLoaded= false;
		m_World.clear();
		m_Error.clear();
		m_Camera= GLC_Camera();
		m_CameraIsSet= false;
		m_PolyMode= GL_FILL;
		m_NumberOfFaces= 0;
		m_NumberOfVertex= 0;
		m_NumberOfMaterials= 0;
		m_AngleOfView= 35.0;// default angle is 35 degre

		m_AttachedFileNames.clear();

		m_ModifiedMaterials.clear();
	}
}

// Return Attached file size
double FileEntry::attachedFilesSize() const
{
	double filesSize= 0.0;
	if (!m_AttachedFileNames.isEmpty())
	{
		const int size= m_AttachedFileNames.size();
		for (int i= 0; i < size; ++i)
		{
			filesSize+= static_cast<double>(QFileInfo(m_AttachedFileNames[i]).size());
		}
	}
	return filesSize;
}

// Update Entry file location without reloading it
void FileEntry::updateFile(const QString& newFileName)
{
	const QString oldPath= QFileInfo(m_FileName).absolutePath();
	const QString newPath= QFileInfo(newFileName).absolutePath();
	m_FileName= newFileName;
	QStringList::iterator iAttachedFile= m_AttachedFileNames.begin();
	while (iAttachedFile != m_AttachedFileNames.constEnd())
	{
		const QString oldAttachedFilePath(QFileInfo(*iAttachedFile).absolutePath());
		const QString attachedFileName(QFileInfo(*iAttachedFile).fileName());

		// Test if the attached file path is not entry file path
		QDir baseDir(oldPath);
		const QString relativeFilePath= baseDir.relativeFilePath(*iAttachedFile);

		const QString newName= newPath + QDir::separator() + relativeFilePath;

		// Update attached file name
		*iAttachedFile= newName;
		++iAttachedFile;
	}
}

// Add material to modified material list
void FileEntry::addModifiedMaterial(GLC_Material* pMaterial)
{
	m_ModifiedMaterials.insert(pMaterial);
}

// Get the list of invisible instance name
QList<QString> FileEntry::listOfInvisibleInstanceName() const
{
	QList<GLC_3DViewInstance*> instancesHandle= m_World.instancesHandle();
	QList<QString> namesList;
	const int size= instancesHandle.size();
	for (int i= 0; i < size; ++i)
	{
		if (!instancesHandle.at(i)->isVisible())
		{
			namesList.append(instancesHandle.at(i)->name());
		}
	}
	return namesList;
}

// Set the default LOD value
void FileEntry::setDefaultLodValue(int value)
{
	QList< GLC_3DViewInstance * > 	instancesHandle= m_World.collection()->instancesHandle();

	const int size= instancesHandle.size();
	for (int i= 0; i < size; ++i)
	{
		instancesHandle[i]->setDefaultLodValue(value);
	}
}

void FileEntry::setSpacePartionningUsage(bool usage)
{
	if (!m_World.isEmpty())
	{
		m_World.collection()->setSpacePartitionningUsage(usage);
		if (usage)
		{
			GLC_Octree* pOctree= new GLC_Octree(m_World.collection());
			pOctree->updateSpacePartitioning();
			m_World.collection()->bindSpacePartitioning(pOctree);
		}
		else
		{
			m_World.collection()->unbindSpacePartitioning();
		}
	}
}

void FileEntry::setVboUsage(bool usage)
{
	m_World.collection()->setVboUsage(usage);
}
