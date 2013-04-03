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

#ifndef EXPORTPROGRESSDIALOG_H_
#define EXPORTPROGRESSDIALOG_H_
#include <QDialog>

#include "ui_ExportProgressDialog.h"
#include "../SaveFileThread.h"
#include <QString>
#include <GLC_World>

class ExportProgressDialog : public QDialog, private Ui::ExportProgressDialog
{
	Q_OBJECT
public:
	ExportProgressDialog(QWidget *pParent, const GLC_World& world, const QString& fileName);
	virtual ~ExportProgressDialog();

public:
	void startThread();
	virtual void reject();

private slots:
	void setValue(int);
private :
	SaveFileThread m_SaveFileThread;

	//! Mutex
	QReadWriteLock m_ReadWriteLock;

	//! Interupt flag
	bool m_Interupt;

};

#endif /* EXPORTPROGRESSDIALOG_H_ */
