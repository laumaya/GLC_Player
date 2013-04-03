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

#ifndef MODELSTRUCTURE_H_
#define MODELSTRUCTURE_H_

#include "ui_ModelStructure.h"
#include <QWidget>
#include <GLC_World>

class OpenglView;

class ModelStructure : public QWidget, private Ui::ModelStructure
{
	Q_OBJECT

public:


	struct ParentSelection
	{
		QList<QTreeWidgetItem*> treeWidgetList;
		int NumberOfChilds;
		int size;
	};

	ModelStructure(OpenglView*, QAction*, QAction*, QAction*, QAction*, QPair<GLC_uint, GLC_StructOccurence*>*, QWidget *parent= 0);
	virtual ~ModelStructure();

//////////////////////////////////////////////////////////////////////
// Public set function
//////////////////////////////////////////////////////////////////////
public:
	//! Set the file Entry (Model)
	void setWorld(const GLC_World&);

	//! Clear the model structure
	inline void clear()
	{
		m_World.clear();
		m_TreeWidgetHash.clear();
		structure->clear();
		m_ProductHash.clear();
	}
	//! connectSlots
	void connectSlots();

	//! Disconnect slots
	void disconnectSlots();

//////////////////////////////////////////////////////////////////////
// Public Slots functions
//////////////////////////////////////////////////////////////////////
public slots:

	//! update the tree selection
	void updateTreeSelection();

	//! Update Show / No show attributes
	void updateTreeShowNoShow();

	//! Update Show / No show attributes
	void updateSelectedTreeShowNoShow();

	//! Set Synchronisation mode
	void setSynchronisation(bool);

	//! Expand tree on the selected node
	void expandOnSelection();

	//! Collapse tree on the selected node
	void collapseOnSelection();


//////////////////////////////////////////////////////////////////////
// Private Slots functions
//////////////////////////////////////////////////////////////////////
private slots:
	//! Update the view selection
	void updateSelection();

	//! Unselect all tree item
	void unselectAll();

	//! Item double clicked
	void itemDoubleClicked();

	// Show selected instances
	void showSelectedInstances();

	// Hide selected instances
	void hideSelectedInstances();

	// Copy the selected occurence
	void copySelectedOccurence();

	// Paset the occurence from the clipboard to the selected occurence
	void pasteOnSelectedOccurence();

	// Edit the selected occurence position
	void editPosition();

//////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////
private:
	//! Create the structure
	void createStructure();

	//! Create QTreeWidgetItem by GLC_Product
	QTreeWidgetItem* createStructureItem(GLC_StructOccurence*);

	//! Add parent to the hash table
	void addParent(QSet<QTreeWidgetItem*>*, QTreeWidgetItem*);

	//! Update parent visibility
	void updateParentsVisibility(QSet<QTreeWidgetItem*>*);

	//! Update contextual menu
	void updateContextualMenu();

	//! Expand childs of the specified item
	void expandChilds(QTreeWidgetItem*, bool);

	//! Update parent selection
	void updateParentSelection(QHash<QTreeWidgetItem*, ParentSelection*> *, QTreeWidgetItem*);

	//! Filter the selection
	bool filterSelection(QHash<QTreeWidgetItem*, ParentSelection*> *, QTreeWidgetItem*);

	//! Update Copy paste
	void updateCopyPaste(const QList<QTreeWidgetItem*>* pSelectedTreeWidgetItem= NULL);

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The View
	OpenglView* m_pOpenglView;

	//! The world to view
	GLC_World m_World;

	//! Item hash table
	QHash<GLC_uint, QTreeWidgetItem*> m_TreeWidgetHash;

	//! Reframe on selection
	QAction* m_pActionReframeOnSelection;

	//! Show selected instances
	QAction* m_pActionShow;

	//! Hide selected instances
	QAction* m_pActionHide;

	//! Expand selected node
	QAction* m_pActionExpand;

	//! Collapse selected node
	QAction* m_pActionCollapse;

	//! The part Icon
	const QIcon m_PartIcon;

	//! The hidden part Icon
	const QIcon m_HiddenPartIcon;

	//! The product Icon
	const QIcon m_ProductIcon;

	//! The hidden product Icon
	const QIcon m_HiddenProductIcon;

	//! Show item foreground
	const QBrush m_ShowForeground;

	//! Hide item foreground
	const QBrush m_HideForeground;

	//! Product hash table
	QHash<GLC_uint, GLC_StructOccurence*> m_ProductHash;

	//! Indicate if the tree is synchronised
	bool m_IsSynchronise;

	//! Copy Action
	QAction* m_pCopyAction;

	//! Paste Action
	QAction* m_pPasteAction;

	//! Edit position action
	QAction* m_pEditPosition;

	//! The clipBoard
	QPair<GLC_uint, GLC_StructOccurence*>* m_pClipBoard;
};

#endif /* MODELSTRUCTURE_H_ */
