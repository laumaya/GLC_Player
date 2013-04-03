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

#ifndef FILEENTRY_H_
#define FILEENTRY_H_

#include <GLC_World>
#include <GLC_Camera>

#include <QString>
#include <QHash>
#include <QSet>

//////////////////////////////////////////////////////////////////////
//! \class FileEntry
/*! \brief FileEntry : The entry of File list*/
//////////////////////////////////////////////////////////////////////
class FileEntry
{

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	FileEntry(const QString& fileName= QString());

	//! Construct from fileName and camera
	FileEntry(const QString&, const GLC_Camera&, const double&, QSet<GLC_Material*>, const QList<QString>&, const QHash<QString, QList<QString> >&);

	//! Copy constructor
	FileEntry(const FileEntry&);

	//! the destructor
	virtual ~FileEntry();
//@}
//////////////////////////////////////////////////////////////////////
// Public Interface
//////////////////////////////////////////////////////////////////////
public:
	//! Get FileEntry Id
	inline GLC_uint id() const
	{return m_Id;}

	//! Set Filentry Id
	inline void setId(const GLC_uint id)
	{m_Id= id;}

	//! Overload "=" operator
	FileEntry& operator =(const FileEntry&);

	//! OverLoad "==" operator
	bool operator ==(const FileEntry&);

	//! Get the File Name of this entry
	inline QString getFileName() const
	{return m_FileName;}

	//! Get the name of this entry
	inline QString name() const
	{return QFileInfo(m_FileName).fileName();}

	//! Set the GLC_3DViewInstance of this file entry
	void setWorld(GLC_World&);

	//! Get the GLC_World of this file entry
	inline GLC_World getWorld() const
	{return m_World;}

	//! return true if this file entry is loaded
	inline bool isLoaded() const
	{return *m_pIsLoaded;}

	//! return true if this file entry is loading
	inline bool isLoading() const
	{return m_IsLoading;}

	//! Return true if the model is on Error
	inline bool isOnError() const
	{return !m_Error.isEmpty();}

	//! return true if this entry is ready to load
	bool isReadyToLoad() const;

	//! return true if an error occur while trying to load the file
	inline bool errorOccurWhileLoading() const
	{return !m_Error.isEmpty();}

	//! set the loading error
	inline void setError(const QString& error)
	{m_Error= error;}

	//! return the loading error
	inline QString getError() const
	{return m_Error;}

	//! Set the loading status of this entry
	inline void setLoadingStatus(bool status)
	{m_IsLoading= status;}

	//! return true if the camera has been set
	inline bool cameraIsSet() const
	{return m_CameraIsSet;}

	//! Get the entry camera
	inline GLC_Camera getCamera() const
	{return m_Camera;}

	//! Get the Angle of view
	inline double getViewAngle() const
	{return m_AngleOfView;}

	//! Set the entry Camera
	inline void setCameraAndAngle(const GLC_Camera& cam, const double& angle)
	{
		m_CameraIsSet= true;
		m_Camera= cam;
		m_AngleOfView= angle;
	}
	//! Get the number of instance of this entry
	inline int getNumberOfInstances()
	{return m_World.collection()->size();}

	//! Get the number of Meshes of this entry
	inline int getNumberOfMeshes()
	{return m_World.numberOfBody();}

	//! Get the number of Faces of this entry
	inline int getNumberOfFaces() const
	{return m_NumberOfFaces;}

	//! Get the number of Vertex of this entry
	inline int getNumberOfVertexs() const
	{return m_NumberOfVertex;}

	//! Get the number of Materials of this entry
	inline int numberOfMaterials() const
	{return m_NumberOfMaterials;}

	//! get the entry polygon mode
	inline GLenum getPolygonMode() const
	{return m_PolyMode;}

	//! Set the entry Polygon mode
	inline void setPolygonMode(GLenum mode)
	{
		m_World.collection()->setPolygonModeForAll(GL_FRONT_AND_BACK, mode);
		m_PolyMode= mode;
	}
	inline bool operator==(const FileEntry& other) const
	{return m_FileName == other.m_FileName;}

	//! Reload this file model
	void reload();

	//! Set the attached file name list
	inline void setAttachedFileNames(QStringList list)
	{m_AttachedFileNames= list;}

	//! Return the list of attached file names
	QStringList attachedFileNames() const {return m_AttachedFileNames;}

	//! Return the number of attached file
	inline int numberOfAttachedFiles() const {return m_AttachedFileNames.size();}

	//! Return Attached file size
	double attachedFilesSize() const;

	//! Update Entry file location without reloading it
	void updateFile(const QString&);

	//! Add material to modified material Set
	void addModifiedMaterial(GLC_Material*);

	//! Get the modified material Set
	inline QSet<GLC_Material*> modifiedMaterialSet() const {return m_ModifiedMaterials;}

	//! Get the list of invisible instance name
	QList<QString> listOfInvisibleInstanceName() const;

	//! Return instances handle from the specified shading group
	inline QList<QString> instanceNamesFromShadingGroup(GLuint id) const
	{return m_World.instanceNamesFromShadingGroup(id);}

	//! Return the number of used shading group
	inline int numberOfUsedShadingGroup() const
	{return m_World.numberOfUsedShadingGroup();}

	//! Return the default Up vector
	inline GLC_Vector3d defaultUpVector() const
	{return m_Camera.defaultUpVector();}

	//! Set the default Up vector
	inline void setDefaultUpVector(const GLC_Vector3d& vect)
	{m_Camera.setDefaultUpVector(vect);}

	//! Set the default LOD value
	void setDefaultLodValue(int);

	//! Set the space partitionning usage
	void setSpacePartionningUsage(bool usage);

	//! set VBO usage
	void setVboUsage(bool usage);
//////////////////////////////////////////////////////////////////////
// private member
//////////////////////////////////////////////////////////////////////
private:
	//! The id associate to this entry
	GLC_uint m_Id;

	//! The filename associate to this entry
	QString m_FileName;

	//! this entry is in loading process
	bool m_IsLoading;

	//! The instance as been set
	bool* m_pIsLoaded;

	//! The GLC_World of this file entry
	GLC_World m_World;

	//! Error description if error occur while loading
	QString m_Error;

	//! The camera off entry
	GLC_Camera m_Camera;

	//! true if the camer has been set
	bool m_CameraIsSet;

	//! The polygon mode
	GLenum m_PolyMode;

	//! Number of Faces
	int m_NumberOfFaces;

	//! Number of Vertex
	int m_NumberOfVertex;

	//! Number of materials
	unsigned int m_NumberOfMaterials;

	//! the angle of view
	double m_AngleOfView;

	//! The List off Attached file
	QStringList m_AttachedFileNames;

	//! The Set off modified material
	QSet<GLC_Material*> m_ModifiedMaterials;

	//! Number of this Entry
	int* m_pNumberOfEntry;

	//! The loading invisibility list of instance name
	QList<QString>* m_pInvisibleListOfInstanceName;

	//! The Qhash containing shaded instance list
	QHash<QString, QList<QString> >* m_pShadedInstanceList;

};

typedef QHash<GLC_uint, FileEntry> FileEntryHash;

#endif /*FILEENTRY_H_*/
