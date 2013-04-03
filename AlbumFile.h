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

#ifndef SESSIONFILE_H_
#define SESSIONFILE_H_

#include "FileEntry.h"
#include <QFile>
#include <QList>
#include <QString>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

//////////////////////////////////////////////////////////////////////
//! \class Sessionfile
/*! \brief SessionFile : The session file reader and loader*/
//////////////////////////////////////////////////////////////////////
class AlbumFile
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////

public:
	//! Default constructor
	AlbumFile();

	//! Default destructor
	virtual ~AlbumFile();
//@}
/////////////////////////////////////////////////////////////////////
// Public Interface
//////////////////////////////////////////////////////////////////////
public:
	//! Load a session file
	/* Return the file entry list as result
	 */
	QList<FileEntry> loadAlbumFile(QFile*);

	//! Save a session file
	/*
	 * The file must exist and be in write mode
	 */
	void saveAlbumFile(const QList<FileEntry>&, QFile*);

	//! Return album suffix
	static QString suffix();

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////
private:
	//! Write the header of xml document
	void writeHeader() const;

	//! Write a model in the current xml document
	void writeModel(const FileEntry&);

	//! Check if the Album file is valid
	bool checkAlbumFileValidity();

	//! Convert current model xml element to a FileEntry object
	FileEntry currentModelToFileEnty();

	//! Return a Set of GLC_Material
	QSet<GLC_Material*> readSetOfMaterials();

	//! Read the list of invisible instance
	QList<QString> readTheListOfInvisibleInstance();

	//! Read the list of shaded instance
	QHash<QString, QList<QString> > readTheListOfShadedInstance();

	//! Write the set of material
	void writeSetOfMaterials(const FileEntry&);

	//! Write the list of invisible instance
	void writeTheListOfInvisibleInstance(const FileEntry&);

	//! Write the list of shaded instance
	void writeTheListOfShadedInstance(const FileEntry&);

	//! Check validity of camera element
	bool checkCameraElementValidity();

	//! Check validity of materials element
	bool checkMaterialsElementValidity();

	//! Check validity of Invisible Instance element
	bool checkInvisibleInstanceElementValidity();

	//! Check validity of Shaded Instance element
	bool checkShadedInstanceElementValidity();

	//! Go to Element
	void goToElement(const QString&);

	//! Return true if the end of specified element is not reached
	inline bool endElementNotReached(const QString& element)
	{return !m_pStreamReader->atEnd() && !(m_pStreamReader->isEndElement() && (m_pStreamReader->name() == element));}


//////////////////////////////////////////////////////////////////////
// private member
//////////////////////////////////////////////////////////////////////
private:
	//! Album suffix
	static const QString m_Suffix;

	//! Xml Reader
	QXmlStreamWriter* m_pStreamWriter;

	//! Xml Writer
	QXmlStreamReader* m_pStreamReader;

	//! The session Path Dir
	QDir m_SessionPathDir;

	//! The current Album version
	const QString m_AlbumVersion;

	//! The Readed Album Version
	QString m_ReadedAlbumVersion;

	//! The current model absolute Dir
	QDir m_CurrentModelAbsoluteDir;

	//! The current model real absolute Dir
	QDir m_CurrentModelRealAbsoluteDir;

};

#endif /*SESSIONFILE_H_*/
