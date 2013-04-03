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

#include "LeftSideDock.h"
#include "AlbumManagerView.h"

LeftSideDock::LeftSideDock(AlbumManagerView* pAlbumManagerView, ModelManagerView* pModelManagerView, FileEntryHash* pAlbumModel, QWidget *parent)
: QWidget(parent)
, m_pAlbumManagerView(pAlbumManagerView)
, m_pModelManagerView(pModelManagerView)
, m_pAlbumModel(pAlbumModel)
{
	setupUi(this);
	// Remove unused widget of the stackedwidget
	QWidget* pCurrentWidget= stackedWidget->currentWidget();
	while (pCurrentWidget != NULL)
	{
		stackedWidget->removeWidget(pCurrentWidget);
		delete pCurrentWidget;
		pCurrentWidget= stackedWidget->currentWidget();
	}
	// Put The album manager view into the stackedWidget
	stackedWidget->addWidget(m_pAlbumManagerView);
	// Put The Model manager view into the stackedWidget
	stackedWidget->addWidget(m_pModelManagerView);
	// Album manager as the current widget
	stackedWidget->setCurrentIndex(0);

	// Signals and slot connection
	connect(albumCmd, SIGNAL(toggled(bool)), this, SLOT(changeCurrentManager(bool)));

}

LeftSideDock::~LeftSideDock()
{

}

//////////////////////////////////////////////////////////////////////
// Public set functions
//////////////////////////////////////////////////////////////////////
// The current modelChanged
void LeftSideDock::currentModelChanged()
{
	if (stackedWidget->currentIndex() == 1)
	{
		m_pModelManagerView->setModel(m_pAlbumModel->value(m_pAlbumManagerView->currentModelId()));
		m_pModelManagerView->updateView();
	}
}

//////////////////////////////////////////////////////////////////////
// Private slots
//////////////////////////////////////////////////////////////////////
// Change the current manager (between Album and Model Manager)
void LeftSideDock::changeCurrentManager(bool isAlbum)
{
	if (isAlbum)
	{
		m_pModelManagerView->disconnectSlots();
		stackedWidget->setCurrentIndex(0);
	}
	else
	{
		stackedWidget->setCurrentIndex(1);
		m_pModelManagerView->setModel(m_pAlbumModel->value(m_pAlbumManagerView->currentModelId()));
		m_pModelManagerView->updateView();
		m_pModelManagerView->connectSlots();
	}
}
