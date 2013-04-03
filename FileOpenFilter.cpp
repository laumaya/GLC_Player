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

#include "FileOpenFilter.h"
#include "glc_player.h"
#include <QStringList>

FileOpenFilter::FileOpenFilter(glc_player * pPlayer)
: m_pPlayer(pPlayer)
{
}

bool FileOpenFilter::eventFilter(QObject *pObject, QEvent *pEvent)
{
	if (pEvent->type() == QEvent::FileOpen)
	{
		// Proceed filtered event
		QFileOpenEvent *pFileOpenEvent = static_cast<QFileOpenEvent*>(pEvent);
		QStringList localFiles;
		m_pPlayer->openOnEvent(pFileOpenEvent->file());
		return true;
	}
	else return QObject::eventFilter(pObject, pEvent);
 };
