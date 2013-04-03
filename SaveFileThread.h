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

#ifndef SAVEFILETHREAD_H_
#define SAVEFILETHREAD_H_

#include <QThread>
#include <QString>
#include <GLC_WorldTo3dxml>

class SaveFileThread : public QThread
{
	Q_OBJECT
public:
	//! Default constructor
	SaveFileThread(const GLC_World& world);

	//! Destructor
	virtual ~SaveFileThread();
public:
	//! Set the fileName to
	void setFileToSave(const QString& fileName, GLC_WorldTo3dxml::ExportType exportType);

	//! set interrupt flag adress
	inline void setInterupt(QReadWriteLock* pReadWriteLock, bool* pInterupt)
	{m_WorldTo3dxml.setInterupt(pReadWriteLock, pInterupt);}

	//! Run the thread
	void run();
signals:
	//! Progress
	void currentQuantum(int);

	//! Error occur while trying to save
	void saveError();

private :
	//! The exporter
	GLC_WorldTo3dxml m_WorldTo3dxml;
	//! Export type
	GLC_WorldTo3dxml::ExportType m_ExportType;
	//! The name of file to save
	QString m_FileName;
};

#endif /* SAVEFILETHREAD_H_ */
