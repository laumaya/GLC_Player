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

#include "ExportProgressDialog.h"

ExportProgressDialog::ExportProgressDialog(QWidget *pParent, const GLC_World& world, const QString& fileName)
: QDialog(pParent)
, m_SaveFileThread(world)
, m_ReadWriteLock()
, m_Interupt(false)
{
	m_SaveFileThread.setFileToSave(fileName, GLC_WorldTo3dxml::Compressed3dxml);
	m_SaveFileThread.setInterupt(&m_ReadWriteLock,&m_Interupt);
	setupUi(this);
	progressBar->setValue(0);
	connect(&m_SaveFileThread, SIGNAL(currentQuantum(int)), this, SLOT(setValue(int)));
	connect(&m_SaveFileThread, SIGNAL(finished()), this, SLOT(accept()));
	connect(&m_SaveFileThread, SIGNAL(saveError()), this, SLOT(reject()));

}

ExportProgressDialog::~ExportProgressDialog()
{

}
void ExportProgressDialog::startThread()
{
	m_SaveFileThread.start(QThread::LowPriority);
	QDialog::exec();
}
void ExportProgressDialog::reject()
{
	if (m_SaveFileThread.isRunning())
	{
		m_ReadWriteLock.lockForWrite();
		m_Interupt= true;
		m_ReadWriteLock.unlock();
		m_SaveFileThread.wait();
	}
	QDialog::reject();
}

void ExportProgressDialog::setValue(int value)
{
	{
		progressBar->setValue(value);
	}
}
