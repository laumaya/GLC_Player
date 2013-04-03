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

#include "SaveFileThread.h"
#include <GLC_Exception>

SaveFileThread::SaveFileThread(const GLC_World& world)
: m_WorldTo3dxml(world)
, m_ExportType()
, m_FileName()
{
	connect(&m_WorldTo3dxml, SIGNAL(currentQuantum(int)), this, SIGNAL(currentQuantum(int)));
	setTerminationEnabled(true);
}

SaveFileThread::~SaveFileThread()
{

}

void SaveFileThread::setFileToSave(const QString& fileName, GLC_WorldTo3dxml::ExportType exportType)
{
	m_FileName= fileName;
	m_ExportType= exportType;
}

void SaveFileThread::run()
{
	Q_ASSERT(!m_FileName.isEmpty());
	try
	{
		m_WorldTo3dxml.exportTo3dxml(m_FileName, m_ExportType);
	}
	catch (GLC_Exception& e)
	{
		emit saveError();
	}
}
