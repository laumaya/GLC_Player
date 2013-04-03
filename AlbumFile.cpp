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

#include "AlbumFile.h"
#include "./opengl_view/OpenglView.h"
#include <GLC_FileFormatException>
#include <GLC_Vector3d>
#include <GLC_Exception>
#include <GLC_Material>
#include <QGLContext>

const QString AlbumFile::m_Suffix("album");

AlbumFile::AlbumFile()
: m_pStreamWriter(NULL)
, m_pStreamReader(NULL)
, m_SessionPathDir()
, m_AlbumVersion("2.1")
, m_ReadedAlbumVersion()
, m_CurrentModelAbsoluteDir()
, m_CurrentModelRealAbsoluteDir()
{
}

AlbumFile::~AlbumFile()
{
	delete m_pStreamWriter;
	delete m_pStreamReader;
}

/////////////////////////////////////////////////////////////////////
// Public Interface
//////////////////////////////////////////////////////////////////////
// Load a album file
/* Return the file entry list as result
 */
QList<FileEntry> AlbumFile::loadAlbumFile(QFile* pSessionFile)
{
	Q_ASSERT(NULL == m_pStreamReader);

	// Open the session file in read only mode
	pSessionFile->open(QIODevice::ReadOnly);
	// The Session Path Dir
	m_SessionPathDir= QFileInfo(*pSessionFile).absoluteDir();

	m_pStreamReader= new QXmlStreamReader(pSessionFile);
	// Check the session file validity
	if (!checkAlbumFileValidity())
	{
		QString message("Album file not valid!");
		GLC_Exception e(message);
		delete m_pStreamReader;
		m_pStreamReader= NULL;
		throw e;
	}
	// The list of FileEntry to return
	QList<FileEntry> returnEntries;
	while (!m_pStreamReader->atEnd())
	{
		if ((QXmlStreamReader::StartElement == m_pStreamReader->readNext()) && (m_pStreamReader->name() == "Model"))
		{
			// Find Model element going to parse it
			returnEntries.append(currentModelToFileEnty());
		}
	}

	// Close the session file
	pSessionFile->close();

	delete m_pStreamReader;
	m_pStreamReader= NULL;

	return returnEntries;
}

// Save a session file
void AlbumFile::saveAlbumFile(const QList<FileEntry>& fileEntryList, QFile* pSessionFile)
{
	Q_ASSERT(NULL == m_pStreamWriter);
	// Open the Session file in write only mode
	pSessionFile->open(QIODevice::WriteOnly);
	// The Session Path Dir
	m_SessionPathDir= QFileInfo(*pSessionFile).absoluteDir();

	m_pStreamWriter= new QXmlStreamWriter(pSessionFile);
	m_pStreamWriter->setAutoFormatting(true);
	// Begin to write the xml document
	m_pStreamWriter->writeStartDocument();
	m_pStreamWriter->writeStartElement("Album");
	writeHeader();
	m_pStreamWriter->writeStartElement("Root");

	// File Entry list processing
	const int max= fileEntryList.size();
	for (int i = 0; i < max; ++i)
	{
		writeModel(fileEntryList[i]);
	}

	// Finish the writing of document
	m_pStreamWriter->writeEndElement(); // Root
	m_pStreamWriter->writeEndElement(); // Session
	m_pStreamWriter->writeEndDocument();

	// Close the session file
	pSessionFile->close();

	delete m_pStreamWriter;
	m_pStreamWriter= NULL;
}

// Return album suffix
QString AlbumFile::suffix()
{
	return m_Suffix;
}

//////////////////////////////////////////////////////////////////////
// private member
//////////////////////////////////////////////////////////////////////
// Write the header of xml document
void AlbumFile::writeHeader() const
{
	m_pStreamWriter->writeStartElement("Header");
	m_pStreamWriter->writeStartElement("Application");
	m_pStreamWriter->writeAttribute("Name", QCoreApplication::applicationName());
	m_pStreamWriter->writeAttribute("Version", m_AlbumVersion);
	m_pStreamWriter->writeEndElement(); // Application
	m_pStreamWriter->writeEndElement(); // Header
}

// Write a model in the current xml document
void AlbumFile::writeModel(const FileEntry& fileEntry)
{
	m_pStreamWriter->writeStartElement("Model");
	m_pStreamWriter->writeAttribute("AFileName", fileEntry.getFileName());
	m_pStreamWriter->writeAttribute("RFileName", m_SessionPathDir.relativeFilePath(fileEntry.getFileName()));

	// Write camera element only if the file entry camera have been set
	if (fileEntry.cameraIsSet())
	{
		m_pStreamWriter->writeStartElement("Camera");
		double angle= fileEntry.getViewAngle();
		m_pStreamWriter->writeAttribute("Angle", QString::number(angle));

		m_pStreamWriter->writeStartElement("Eye");
		double x= fileEntry.getCamera().eye().x();
		m_pStreamWriter->writeAttribute("x", QString::number(x));
		double y= fileEntry.getCamera().eye().y();
		m_pStreamWriter->writeAttribute("y", QString::number(y));
		double z= fileEntry.getCamera().eye().z();
		m_pStreamWriter->writeAttribute("z", QString::number(z));
		m_pStreamWriter->writeEndElement(); // Eye

		m_pStreamWriter->writeStartElement("Target");
		x= fileEntry.getCamera().target().x();
		m_pStreamWriter->writeAttribute("x", QString::number(x));
		y= fileEntry.getCamera().target().y();
		m_pStreamWriter->writeAttribute("y", QString::number(y));
		z= fileEntry.getCamera().target().z();
		m_pStreamWriter->writeAttribute("z", QString::number(z));
		m_pStreamWriter->writeEndElement(); // Target

		m_pStreamWriter->writeStartElement("Up");
		x= fileEntry.getCamera().upVector().x();
		m_pStreamWriter->writeAttribute("x", QString::number(x));
		y= fileEntry.getCamera().upVector().y();
		m_pStreamWriter->writeAttribute("y", QString::number(y));
		z= fileEntry.getCamera().upVector().z();
		m_pStreamWriter->writeAttribute("z", QString::number(z));
		m_pStreamWriter->writeEndElement(); // Up

		m_pStreamWriter->writeStartElement("DefaultUp");
		x= fileEntry.getCamera().defaultUpVector().x();
		m_pStreamWriter->writeAttribute("x", QString::number(x));
		y= fileEntry.getCamera().defaultUpVector().y();
		m_pStreamWriter->writeAttribute("y", QString::number(y));
		z= fileEntry.getCamera().defaultUpVector().z();
		m_pStreamWriter->writeAttribute("z", QString::number(z));
		m_pStreamWriter->writeEndElement(); // Up

		m_pStreamWriter->writeEndElement(); // Camera
	}

	writeSetOfMaterials(fileEntry);

	writeTheListOfInvisibleInstance(fileEntry);

	writeTheListOfShadedInstance(fileEntry);

	m_pStreamWriter->writeEndElement(); // Model

}

// Check if the session file is valid
bool AlbumFile::checkAlbumFileValidity()
{
	// trying to find start Document of the xml file
	goToElement("Album");
	if (m_pStreamReader->atEnd()) return false;
	if (m_pStreamReader->hasError()) return false;

	// Trying to find the header of the album
	goToElement("Header");
	if (m_pStreamReader->atEnd()) return false;
	if (m_pStreamReader->hasError()) return false;

	// Cheking the validity of the header
	// Trying to find application element
	goToElement("Application");
	if (m_pStreamReader->atEnd()) return false;
	if (m_pStreamReader->hasError()) return false;
	// Get Album version and check it
	m_ReadedAlbumVersion= m_pStreamReader->attributes().value("Version").toString();
	if ((m_ReadedAlbumVersion != "1.5") && (m_ReadedAlbumVersion != "2.0") && m_ReadedAlbumVersion != m_AlbumVersion) return false;
	if (m_pStreamReader->hasError()) return false;

	//Trying to find root element
	goToElement("Root");
	if (m_pStreamReader->atEnd()) return false;
	if (m_pStreamReader->hasError()) return false;

	int NumberOfModels= 0;
	// Trying to find model elements
	while(!m_pStreamReader->atEnd())
	{
		if ((QXmlStreamReader::StartElement == m_pStreamReader->readNext()) && (m_pStreamReader->name() == "Model"))
		{
			++NumberOfModels;
			if (m_pStreamReader->attributes().value("AFileName").isEmpty()) return false;
			if (m_pStreamReader->attributes().value("RFileName").isEmpty()) return false;

			//Trying to find Camera element
			if (!checkCameraElementValidity())
			{
				qDebug() << "Camera invalid";
				return false;
			}

			if ((m_ReadedAlbumVersion == "2.0") || (m_ReadedAlbumVersion == m_AlbumVersion))
			{
				if (!checkMaterialsElementValidity())
				{
					qDebug() << "Materials invalid";
					return false;
				}

				if (!checkInvisibleInstanceElementValidity())
				{
					qDebug() << "InvisibleInstances invalid";
					return false;
				}

				if (!checkShadedInstanceElementValidity())
				{
					qDebug() << "ShadedInstances invalid";
					return false;
				}

			}
		}
		if (m_pStreamReader->hasError()) return false;
	}
	if (0 == NumberOfModels) return false;

	// Reset the xml stream reader to the begining
	QIODevice* pDevice= m_pStreamReader->device();
	pDevice->reset();
	m_pStreamReader->setDevice(pDevice);

	return true;

}

// Convert current model xml element to a FileEntry object
FileEntry AlbumFile::currentModelToFileEnty()
{
	// The model file name
	QString fileName(m_pStreamReader->attributes().value("AFileName").toString());

	// Store the obsolute fileName in order to find image fullFilePath
	m_CurrentModelAbsoluteDir= QFileInfo(fileName).absolutePath();

	// Check if the file exists
	if (!QFile::exists(fileName))
	{
		fileName= m_pStreamReader->attributes().value("RFileName").toString();
		fileName= m_SessionPathDir.absoluteFilePath(fileName);
	}

	m_CurrentModelRealAbsoluteDir= QFileInfo(fileName).absolutePath();

	// Go to camera element
	bool modelHaveCamera= true;
	if ((m_ReadedAlbumVersion == "2.0") || (m_ReadedAlbumVersion == m_AlbumVersion))
	{
		bool continu= true;
		while(continu && !m_pStreamReader->atEnd() && !((QXmlStreamReader::StartElement == m_pStreamReader->tokenType()) && (m_pStreamReader->name() == "Camera")))
		{
			if ((m_pStreamReader->name() == "Materials") && (QXmlStreamReader::StartElement == m_pStreamReader->tokenType()))
			{
				continu= false;
				modelHaveCamera= false;
			}
			else
			{
				m_pStreamReader->readNext();
			}
		}
	}
	else
	{
		goToElement("Camera");
	}

	GLC_Camera camera;
	double angle= 30.0;
	if (true == modelHaveCamera)
	{
		Q_ASSERT(!m_pStreamReader->atEnd());
		// Camera Angle of view
		angle= m_pStreamReader->attributes().value("Angle").toString().toDouble();

		double x, y, z; // Tempory vector coordinate
		// Go to camera Eye vector element
		goToElement("Eye");
		Q_ASSERT(!m_pStreamReader->atEnd());
		x= m_pStreamReader->attributes().value("x").toString().toDouble();
		y= m_pStreamReader->attributes().value("y").toString().toDouble();
		z= m_pStreamReader->attributes().value("z").toString().toDouble();
		GLC_Vector3d eye(x, y, z);

		// Go to camera Target vector element
		goToElement("Target");
		Q_ASSERT(!m_pStreamReader->atEnd());
		x= m_pStreamReader->attributes().value("x").toString().toDouble();
		y= m_pStreamReader->attributes().value("y").toString().toDouble();
		z= m_pStreamReader->attributes().value("z").toString().toDouble();
		GLC_Vector3d target(x, y, z);

		// Go to camera Up vector element
		goToElement("Up");
		Q_ASSERT(!m_pStreamReader->atEnd());
		x= m_pStreamReader->attributes().value("x").toString().toDouble();
		y= m_pStreamReader->attributes().value("y").toString().toDouble();
		z= m_pStreamReader->attributes().value("z").toString().toDouble();
		GLC_Vector3d up(x, y, z);

		if (m_AlbumVersion == m_ReadedAlbumVersion)
		{
			// Go to camera Default Up vector element
			goToElement("DefaultUp");
			Q_ASSERT(!m_pStreamReader->atEnd());
			x= m_pStreamReader->attributes().value("x").toString().toDouble();
			y= m_pStreamReader->attributes().value("y").toString().toDouble();
			z= m_pStreamReader->attributes().value("z").toString().toDouble();
			GLC_Vector3d defaultUp(x, y, z);
			camera.setDefaultUpVector(defaultUp);
		}
		else
		{
			camera.setDefaultUpVector(glc::Z_AXIS);
		}

		// The model camera
		camera.setCam(eye, target, up);
	}


	QSet<GLC_Material*> materialSet;
	QList<QString> invisibleInstanceName;
	QHash<QString, QList<QString> > listOfShadedInstance;
	if ((m_ReadedAlbumVersion == "2.0") || (m_ReadedAlbumVersion == m_AlbumVersion))
	{
		materialSet= readSetOfMaterials();
		invisibleInstanceName= readTheListOfInvisibleInstance();
		listOfShadedInstance= readTheListOfShadedInstance();
	}

	return FileEntry(fileName, camera, angle, materialSet, invisibleInstanceName, listOfShadedInstance);
}

// Return a Set of GLC_Material
QSet<GLC_Material*> AlbumFile::readSetOfMaterials()
{
	QSet<GLC_Material*> materialSet;

	// Go to camera Materials element
	goToElement("Materials");
	Q_ASSERT(!m_pStreamReader->atEnd());
	int materialsSize= m_pStreamReader->attributes().value("size").toString().toInt();
	for (int i= 0; i < materialsSize; ++i)
	{
		goToElement("Material");
		Q_ASSERT(!m_pStreamReader->atEnd());
		// Create the material
		GLC_Material* pMaterial= new GLC_Material();

		QString materialName(m_pStreamReader->attributes().value("name").toString());
		pMaterial->setName(materialName);

		// Go Material Color Element
		QVector<QColor> materialColors;
		QVector<QString> materialColorsName;
		materialColorsName << "Ambiant" << "Diffuse" << "Specular" << "LightEmission";
		for (int j= 0; j < 4; ++j)
		{
			goToElement(materialColorsName[j]);
			Q_ASSERT(!m_pStreamReader->atEnd());
			QColor currentColor;
			currentColor.setRed(m_pStreamReader->attributes().value("r").toString().toInt());
			currentColor.setGreen(m_pStreamReader->attributes().value("g").toString().toInt());
			currentColor.setBlue(m_pStreamReader->attributes().value("b").toString().toInt());
			currentColor.setAlpha(m_pStreamReader->attributes().value("a").toString().toInt());
			materialColors << currentColor;
		}

		// Go to Material shininess element
		goToElement("Shininess");
		Q_ASSERT(!m_pStreamReader->atEnd());
		float materialShininess= m_pStreamReader->attributes().value("value").toString().toFloat();

		// Go to Material Texture element
		goToElement("Texture");
		Q_ASSERT(!m_pStreamReader->atEnd());
		QString textureFileName(m_pStreamReader->attributes().value("fileName").toString());

		// Set the material color attributes
		pMaterial->setAmbientColor(materialColors[0]);
		pMaterial->setDiffuseColor(materialColors[1]);
		pMaterial->setSpecularColor(materialColors[2]);
		pMaterial->setEmissiveColor(materialColors[3]);
		pMaterial->setShininess(materialShininess);
		pMaterial->setOpacity(materialColors[1].alphaF());

		if (!textureFileName.isEmpty())
		{
			qDebug() << "Texture File Name Before : " << textureFileName;
			qDebug() << "Current model path : " << m_CurrentModelAbsoluteDir.absolutePath();
			//if (not QFileInfo(textureFileName).exists())
			{
				textureFileName= m_CurrentModelAbsoluteDir.relativeFilePath(textureFileName);
				qDebug() << "Relative texture file path : " << textureFileName;
				textureFileName= m_CurrentModelRealAbsoluteDir.absoluteFilePath(textureFileName);
				qDebug() << "Texture File Name " << textureFileName;

			}
			if (QFileInfo(textureFileName).exists())
			{
				pMaterial->setTexture(new GLC_Texture(textureFileName));
			}
		}

		// Add the material to the list
		materialSet.insert(pMaterial);
	}

	return materialSet;
}

// Read the list of invisible instance
QList<QString> AlbumFile::readTheListOfInvisibleInstance()
{

	QList<QString> listOfInvisibleInstanceName;

	// Go to InvisibleInstances element
	goToElement("InvisibleInstances");
	Q_ASSERT(!m_pStreamReader->atEnd());
	int size= m_pStreamReader->attributes().value("size").toString().toInt();
	for (int i= 0; i < size; ++i)
	{
		goToElement("Instance");
		Q_ASSERT(!m_pStreamReader->atEnd());
		QString instanceName(m_pStreamReader->attributes().value("name").toString());
		listOfInvisibleInstanceName << instanceName;
		m_pStreamReader->readNext();
	}
	return listOfInvisibleInstanceName;
}
// Read the list of shaded instance
QHash<QString, QList<QString> > AlbumFile::readTheListOfShadedInstance()
{
	QHash<QString, QList<QString> > result;
	// Go to shader element
	goToElement("Shaders");
	Q_ASSERT(!m_pStreamReader->atEnd());
	const int size= m_pStreamReader->attributes().value("size").toString().toInt();
	for (int i= 0; i < size; ++i)
	{
		goToElement("Shader");
		Q_ASSERT(!m_pStreamReader->atEnd());
		QString shaderName(m_pStreamReader->attributes().value("name").toString());
		const int shaderSize= m_pStreamReader->attributes().value("size").toString().toInt();
		QList<QString> instanceList;
		for (int j= 0; j < shaderSize; ++j)
		{
			goToElement("Instance");
			Q_ASSERT(!m_pStreamReader->atEnd());
			QString instanceName(m_pStreamReader->attributes().value("name").toString());
			instanceList << instanceName;
			m_pStreamReader->readNext();
		}
		result.insert(shaderName, instanceList);
		m_pStreamReader->readNext();
	}

	return result;
}

// Write the set of material
void AlbumFile::writeSetOfMaterials(const FileEntry& fileEntry)
{
	QSet<GLC_Material*> materialsSet(fileEntry.modifiedMaterialSet());

	const int size= materialsSet.size();
	m_pStreamWriter->writeStartElement("Materials");
	m_pStreamWriter->writeAttribute("size", QString::number(size));
	if (!materialsSet.isEmpty())
	{
        QSet<GLC_Material *>::const_iterator i = materialsSet.constBegin();
        while (i != materialsSet.constEnd())
        {
			m_pStreamWriter->writeStartElement("Material");
			m_pStreamWriter->writeAttribute("name", (*i)->name());

			QList<QColor> colorList;
			colorList.append((*i)->ambientColor());
			colorList.append((*i)->diffuseColor());
			colorList.append((*i)->specularColor());
			colorList.append((*i)->emissiveColor());
			QList<QString> colorNameList;
			colorNameList << "Ambiant" << "Diffuse" << "Specular" << "LightEmission";
			for (int j= 0; j < 4; ++j)
			{
				QColor currentColor(colorList[j]);
				m_pStreamWriter->writeStartElement(colorNameList[j]);
				m_pStreamWriter->writeAttribute("r", QString::number(currentColor.red()));
				m_pStreamWriter->writeAttribute("g", QString::number(currentColor.green()));
				m_pStreamWriter->writeAttribute("b", QString::number(currentColor.blue()));
				m_pStreamWriter->writeAttribute("a", QString::number(currentColor.alpha()));
				m_pStreamWriter->writeEndElement();
			}
			// Write material Shininess
			m_pStreamWriter->writeStartElement("Shininess");
			m_pStreamWriter->writeAttribute("value", QString::number((*i)->shininess()));
			m_pStreamWriter->writeEndElement(); // Shininess

			// Write material texture
			m_pStreamWriter->writeStartElement("Texture");
			m_pStreamWriter->writeAttribute("fileName", (*i)->textureFileName());
			m_pStreamWriter->writeEndElement(); // Texture

			m_pStreamWriter->writeEndElement(); // Material
            ++i;
        }
	}
	m_pStreamWriter->writeEndElement(); // Materials
}

// Write the list of invisible instance
void AlbumFile::writeTheListOfInvisibleInstance(const FileEntry& fileEntry)
{
	QList<QString> listOfInvisibleInstance(fileEntry.listOfInvisibleInstanceName());
	const int size= listOfInvisibleInstance.size();
	m_pStreamWriter->writeStartElement("InvisibleInstances");
	m_pStreamWriter->writeAttribute("size", QString::number(size));
	for (int i= 0; i < size; ++i)
	{
		m_pStreamWriter->writeStartElement("Instance");
		m_pStreamWriter->writeAttribute("name", listOfInvisibleInstance[i]);
		m_pStreamWriter->writeEndElement(); // Instance
	}
	m_pStreamWriter->writeEndElement(); // InvisibleInstances

}

// Write the list of shaded instance
void AlbumFile::writeTheListOfShadedInstance(const FileEntry& fileEntry)
{
	QList<GLC_Shader*> shaderList= OpenglView::shaderList();
	const int size= shaderList.size();
	m_pStreamWriter->writeStartElement("Shaders");
	m_pStreamWriter->writeAttribute("size", QString::number(fileEntry.numberOfUsedShadingGroup()));
	for (int i= 0; i < size; ++i)
	{
		QList<QString> listOfShadedInstanceName(fileEntry.instanceNamesFromShadingGroup(shaderList[i]->id()));
		if (!listOfShadedInstanceName.isEmpty())
		{
			const int shaderSize= listOfShadedInstanceName.size();
			m_pStreamWriter->writeStartElement("Shader");
			m_pStreamWriter->writeAttribute("name", shaderList[i]->name());
			m_pStreamWriter->writeAttribute("size", QString::number(shaderSize));
			for (int j= 0; j < shaderSize; ++j)
			{
				m_pStreamWriter->writeStartElement("Instance");
				m_pStreamWriter->writeAttribute("name", listOfShadedInstanceName[j]);
				m_pStreamWriter->writeEndElement(); // Instance
			}
			m_pStreamWriter->writeEndElement(); // Shader
		}
	}
	m_pStreamWriter->writeEndElement(); // ShadedInstances
}

// Check validity of camera element
bool AlbumFile::checkCameraElementValidity()
{
	//Trying to find Camera element
	if ((m_ReadedAlbumVersion == "2.0") || (m_ReadedAlbumVersion == m_AlbumVersion))
	{
		while(!m_pStreamReader->atEnd() && !((QXmlStreamReader::StartElement == m_pStreamReader->tokenType()) && (m_pStreamReader->name() == "Camera")))
		{
			if ((m_pStreamReader->name() == "Materials") && (QXmlStreamReader::StartElement == m_pStreamReader->tokenType()))
			{
				qDebug() << m_pStreamReader->name().toString();
				// the camera of model has not been set
				return true;
			}
			m_pStreamReader->readNext();
		}
	}
	else
	{
		goToElement("Camera");
	}

	if (m_pStreamReader->atEnd()) return false;
	if (m_pStreamReader->hasError()) return false;
	if (m_pStreamReader->attributes().value("Angle").isEmpty()) return false;

	//Trying to find Eye element
	goToElement("Eye");
	if (m_pStreamReader->atEnd()) return false;
	if (m_pStreamReader->hasError()) return false;
	if (m_pStreamReader->attributes().value("x").isEmpty()) return false;
	if (m_pStreamReader->attributes().value("y").isEmpty()) return false;
	if (m_pStreamReader->attributes().value("z").isEmpty()) return false;

	//Trying to find Target element
	goToElement("Target");
	if (m_pStreamReader->atEnd()) return false;
	if (m_pStreamReader->hasError()) return false;
	if (m_pStreamReader->attributes().value("x").isEmpty()) return false;
	if (m_pStreamReader->attributes().value("y").isEmpty()) return false;
	if (m_pStreamReader->attributes().value("z").isEmpty()) return false;

	//Trying to find Up element
	goToElement("Up");
	if (m_pStreamReader->atEnd()) return false;
	if (m_pStreamReader->hasError()) return false;
	if (m_pStreamReader->attributes().value("x").isEmpty()) return false;
	if (m_pStreamReader->attributes().value("y").isEmpty()) return false;
	if (m_pStreamReader->attributes().value("z").isEmpty()) return false;

	return true;

}
// Check validity of materials element
bool AlbumFile::checkMaterialsElementValidity()
{
	goToElement("Materials");
	if (m_pStreamReader->atEnd()) return false;
	if (m_pStreamReader->hasError()) return false;
	if (m_pStreamReader->attributes().value("size").isEmpty()) return false;
	const int size= m_pStreamReader->attributes().value("size").toString().toInt();
	int i= 0;
	for (i= 0; i < size; ++i)
	{
		goToElement("Material");
		if (m_pStreamReader->atEnd()) return false;
		if (m_pStreamReader->hasError()) return false;
		if (m_pStreamReader->attributes().value("name").isEmpty()) return false;
		QVector<QString> materialColorsName;
		materialColorsName << "Ambiant" << "Diffuse" << "Specular" << "LightEmission";
		for (int j= 0; j < 4; ++j)
		{
			goToElement(materialColorsName[j]);
			if (m_pStreamReader->atEnd()) return false;
			if (m_pStreamReader->hasError()) return false;
			QColor currentColor;
			if (m_pStreamReader->attributes().value("r").isEmpty()) return false;
			if (m_pStreamReader->attributes().value("g").isEmpty()) return false;
			if (m_pStreamReader->attributes().value("b").isEmpty()) return false;
			if (m_pStreamReader->attributes().value("a").isEmpty()) return false;
		}
	}
	return true;
}

// Check validity of Invisible Instance element
bool AlbumFile::checkInvisibleInstanceElementValidity()
{
	goToElement("InvisibleInstances");
	if (m_pStreamReader->atEnd()) return false;
	if (m_pStreamReader->hasError()) return false;
	if (m_pStreamReader->attributes().value("size").isEmpty()) return false;
	const int size= m_pStreamReader->attributes().value("size").toString().toInt();
	int i= 0;
	for (i= 0; i < size; ++i)
	{
		goToElement("Instance");
		if (m_pStreamReader->atEnd()) return false;
		if (m_pStreamReader->hasError()) return false;
		if (m_pStreamReader->attributes().value("name").isEmpty()) return false;
		m_pStreamReader->readNext();
	}
	return true;
}

// Check validity of Shaded Instance element
bool AlbumFile::checkShadedInstanceElementValidity()
{
	// Go to shader element
	goToElement("Shaders");
	if (m_pStreamReader->atEnd()) return false;
	if (m_pStreamReader->hasError()) return false;
	if (m_pStreamReader->attributes().value("size").isEmpty()) return false;
	const int size= m_pStreamReader->attributes().value("size").toString().toInt();
	for (int i= 0; i < size; ++i)
	{
		goToElement("Shader");
		if (m_pStreamReader->atEnd()) return false;
		if (m_pStreamReader->hasError()) return false;
		if (m_pStreamReader->attributes().value("name").isEmpty()) return false;
		if (m_pStreamReader->attributes().value("size").isEmpty()) return false;
		QString shaderName(m_pStreamReader->attributes().value("name").toString());
		const int shaderSize= m_pStreamReader->attributes().value("size").toString().toInt();
		for (int j= 0; j < shaderSize; ++j)
		{
			goToElement("Instance");
			if (m_pStreamReader->atEnd()) return false;
			if (m_pStreamReader->hasError()) return false;
			if (m_pStreamReader->attributes().value("name").isEmpty()) return false;
			m_pStreamReader->readNext();
		}
		m_pStreamReader->readNext();
	}
	return true;
}

// Go to Element
void AlbumFile::goToElement(const QString& elementName)
{
	while(!m_pStreamReader->atEnd() && !((QXmlStreamReader::StartElement == m_pStreamReader->tokenType()) && (m_pStreamReader->name() == elementName)))
	{
		m_pStreamReader->readNext();
	}
}

