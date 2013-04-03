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

#include "OpenFileThread.h"
#include <GLC_FileFormatException>
#include <GLC_Octree>

OpenFileThread::OpenFileThread()
: m_ModelId(0)
, m_pLoadingFile(NULL)
, m_World()
, m_ErrorMsg()
, m_AttachedFileName()
{
	connect(GLC_Factory::instance(), SIGNAL(currentQuantum(int)), this, SIGNAL(currentQuantum(int)));
}

OpenFileThread::~OpenFileThread()
{
	m_AttachedFileName.clear();
	delete m_pLoadingFile;
}

GLC_World OpenFileThread::getWorld()
{
	GLC_World resultWorld(m_World);
	m_World.clear();
	return resultWorld;
}

GLC_uint OpenFileThread::getModelId() const
{
	return m_ModelId;
}

void OpenFileThread::setOpenFile(const GLC_uint id, QFile *openFile)
{
	m_ModelId= id;
	delete m_pLoadingFile;
	m_pLoadingFile= new QFile(openFile->fileName());
	m_AttachedFileName.clear();
	m_World.clear();

}
void OpenFileThread::run()
{
	try
	{
		m_World= GLC_Factory::instance()->createWorldFromFile(*m_pLoadingFile, &m_AttachedFileName);

		if (GLC_State::isSpacePartitionningActivated())
		{
			GLC_Octree* pOctree= new GLC_Octree(m_World.collection());
			pOctree->updateSpacePartitioning();
			m_World.collection()->bindSpacePartitioning(pOctree);
			m_World.collection()->setSpacePartitionningUsage(true);
		}
		m_ErrorMsg= "";
	}
	catch (GLC_FileFormatException &e)
	{
		switch (e.exceptionType())
		{
		case GLC_FileFormatException::FileNotFound :
			m_ErrorMsg= tr("File \"") + QFileInfo(m_pLoadingFile->fileName()).fileName() + tr("\" not found");
			break;
		case GLC_FileFormatException::FileNotSupported :
			m_ErrorMsg= tr("File not supported");
			break;
		case GLC_FileFormatException::WrongFileFormat :
			m_ErrorMsg= tr("File corrupted");
			break;
		case GLC_FileFormatException::NoMeshFound :
			m_ErrorMsg= tr("No mesh found");
			break;
		default :
			break;
		}

		m_pLoadingFile->close();
		//m_ErrorMsg= e.what();
		emit loadError();
	}
	catch (std::exception &e)
	{
		m_ErrorMsg= e.what();
		emit loadError();
	}

}

