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

#ifndef OPENFILETHREAD_H_
#define OPENFILETHREAD_H_

#include <GLC_Factory>

#include <QtGui>
#include <QStringList>

class GLC_Geometry;

class OpenFileThread : public QThread
{
	Q_OBJECT
public:
	OpenFileThread();
	virtual ~OpenFileThread();

	GLC_World getWorld();
	GLC_uint getModelId() const;
	//
	void setOpenFile(const GLC_uint, QFile *);

	//! Method which run when the thread is started
	void run();

	//! Return the error Msg
	inline QString getErrorMsg() const
	{return m_ErrorMsg;}

	//! Return the list of Attached files
	inline QStringList attachedFiles() const
	{return m_AttachedFileName;}

signals:
	void currentQuantum(int);
	void loadError();

private:
	//! The model Id
	GLC_uint m_ModelId;

	//! The File to load
	QFile *m_pLoadingFile;

	//! The World to get
	GLC_World m_World;

	//! Error Message if error occur
	QString m_ErrorMsg;

	//! The list of attached file
	QStringList m_AttachedFileName;

};

#endif /*OPENFILETHREAD_H_*/
