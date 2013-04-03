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

#include "ModelStructure.h"
#include "../opengl_view/OpenglView.h"
#include "EditPositionDialog.h"

ModelStructure::ModelStructure(OpenglView* pOpenglView, QAction* pInstanceProperties, QAction* pHideUnselect
		, QAction* pCopy, QAction* pPaste, QPair<GLC_uint, GLC_StructOccurence*>*pClipBoard, QWidget *parent)
: QWidget(parent)
, m_pOpenglView(pOpenglView)
, m_World()
, m_TreeWidgetHash()
, m_pActionReframeOnSelection(new QAction(QIcon(":images/FitSelected.png"), tr("Fit on selection"), this))
, m_pActionShow(new QAction(QIcon(":images/Show.png"), tr("Show"), this))
, m_pActionHide(new QAction(QIcon(":images/NoShow.png"), tr("Hide"), this))
, m_pActionExpand(new QAction(tr("Expand"), this))
, m_pActionCollapse(new QAction(tr("Collapse"), this))
, m_PartIcon(":images/part.png")
, m_HiddenPartIcon(":images/partHidden.png")
, m_ProductIcon(":images/product.png")
, m_HiddenProductIcon(":images/productHidden.png")
, m_ShowForeground(Qt::black)
, m_HideForeground(Qt::gray)
, m_ProductHash()
, m_IsSynchronise(true)
, m_pCopyAction(pCopy)
, m_pPasteAction(pPaste)
, m_pEditPosition(new QAction(tr("Edit position"), this))
, m_pClipBoard(pClipBoard)
{
	setupUi(this);

	// Add contextual menu
	//Contextual menu Creation
	structure->addAction(m_pActionReframeOnSelection);
	connect(m_pActionReframeOnSelection, SIGNAL(triggered()), m_pOpenglView, SLOT(reframeOnSelection()));

	structure->addAction(pInstanceProperties);

	QAction* pSeparator= new QAction("separtor", this);
	pSeparator->setSeparator(true);
	structure->addAction(pSeparator);

	structure->addAction(m_pActionShow);
	connect(m_pActionShow, SIGNAL(triggered()), this, SLOT(showSelectedInstances()));
	m_pActionShow->setEnabled(false);

	structure->addAction(m_pActionHide);
	connect(m_pActionHide, SIGNAL(triggered()), this, SLOT(hideSelectedInstances()));
	m_pActionHide->setEnabled(false);

	structure->addAction(pHideUnselect);

	pSeparator= new QAction("separtor", this);
	pSeparator->setSeparator(true);
	structure->addAction(pSeparator);

	structure->addAction(m_pActionExpand);
	connect(m_pActionExpand, SIGNAL(triggered()), this, SLOT(expandOnSelection()));
	m_pActionExpand->setEnabled(false);

	structure->addAction(m_pActionCollapse);
	connect(m_pActionCollapse, SIGNAL(triggered()), this, SLOT(collapseOnSelection()));
	m_pActionCollapse->setEnabled(false);

	// Copy Paste
	pSeparator= new QAction("separtor", this);
	pSeparator->setSeparator(true);
	structure->addAction(pSeparator);
	structure->addAction(m_pCopyAction);
	structure->addAction(m_pPasteAction);

	structure->addAction(m_pEditPosition);
	connect(m_pEditPosition, SIGNAL(triggered()), this, SLOT(editPosition()));
	m_pEditPosition->setEnabled(false);

}

ModelStructure::~ModelStructure()
{

}

//////////////////////////////////////////////////////////////////////
// Public set function
//////////////////////////////////////////////////////////////////////

// Set the file Entry (Model)
void ModelStructure::setWorld(const GLC_World& world)
{
	clear();
	m_World= world;
	createStructure();
}

//! connectSlots
void ModelStructure::connectSlots()
{
	connect(structure, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelection()));
	connect(m_pOpenglView, SIGNAL(updateSelection(PointerViewInstanceHash *)), this, SLOT(updateTreeSelection()));
	connect(m_pOpenglView, SIGNAL(unselectAll()), this, SLOT(unselectAll()));
	connect(structure, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(itemDoubleClicked()));
	connect(m_pCopyAction, SIGNAL(triggered()), this, SLOT(copySelectedOccurence()));
	connect(m_pPasteAction, SIGNAL(triggered()), this, SLOT(pasteOnSelectedOccurence()));
}

// Disconnect slots
void ModelStructure::disconnectSlots()
{
	disconnect(structure, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelection()));
	disconnect(m_pOpenglView, SIGNAL(updateSelection(PointerViewInstanceHash *)), this, SLOT(updateTreeSelection()));
	disconnect(m_pOpenglView, SIGNAL(unselectAll()), this, SLOT(unselectAll()));
	disconnect(structure, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(itemDoubleClicked()));
	disconnect(m_pCopyAction, SIGNAL(triggered()), this, SLOT(copySelectedOccurence()));
	disconnect(m_pPasteAction, SIGNAL(triggered()), this, SLOT(pasteOnSelectedOccurence()));

	updateCopyPaste();
}

//////////////////////////////////////////////////////////////////////
// Public Slots functions
//////////////////////////////////////////////////////////////////////

// update the tree selection
void ModelStructure::updateTreeSelection()
{
	if (structure->topLevelItemCount() > 0)
	{
		QHash<QTreeWidgetItem*, ParentSelection*> FatherHash;
		structure->blockSignals(true);
		structure->clearSelection();
		PointerViewInstanceHash* pSelections= m_World.collection()->selection();
		PointerViewInstanceHash::iterator iEntry= pSelections->begin();
		while (iEntry != pSelections->constEnd())
		{
			updateParentSelection(&FatherHash, m_TreeWidgetHash.value(iEntry.key()));
			//m_TreeWidgetHash.value(iEntry.key())->setSelected(true);
			++iEntry;
		}

		bool rootIsSelected= filterSelection(&FatherHash, structure->topLevelItem(0));
		structure->topLevelItem(0)->setSelected(rootIsSelected);

		QHash<QTreeWidgetItem*, ParentSelection*>::const_iterator iParent= FatherHash.constBegin();
		while(iParent != FatherHash.constEnd())
		{
			delete iParent.value();
			++iParent;
		}


		if ((pSelections->size() == 1) && m_IsSynchronise)
		{
			structure->scrollToItem(m_TreeWidgetHash.find(pSelections->begin().key()).value());
		}
		structure->blockSignals(false);

		QList<QTreeWidgetItem*> selectedTreeWidgetItem= structure->selectedItems();
		m_pEditPosition->setEnabled(1 == selectedTreeWidgetItem.size());
		updateCopyPaste(&selectedTreeWidgetItem);
		// Update contextualMenu
	    updateContextualMenu();
	}
}

// Update Show / No show attributes
void ModelStructure::updateTreeShowNoShow()
{
	QHash<GLC_uint, QTreeWidgetItem*>::iterator iEntry= m_TreeWidgetHash.begin();
	QSet<QTreeWidgetItem*> parentSet;

    while (iEntry != m_TreeWidgetHash.constEnd())
    {
    	const GLC_uint key= iEntry.key();

    	if (m_World.collection()->contains(key))
    	{
        	if (m_World.collection()->instanceHandle(key)->isVisible())
        	{
        		if (iEntry.value()->foreground(0) != m_ShowForeground)
        		{
            		iEntry.value()->setForeground(0, m_ShowForeground);
            		iEntry.value()->setIcon(0, m_PartIcon);
        		}
        	}
        	else
        	{
        		if (iEntry.value()->foreground(0) != m_HideForeground)
        		{
        	   		iEntry.value()->setForeground(0, m_HideForeground);
        	    	iEntry.value()->setIcon(0, m_HiddenPartIcon);
        		}
         	}
        	addParent(&parentSet, iEntry.value()->parent());
    	}
    	++iEntry;
    }
    updateParentsVisibility(&parentSet);
}
// Update Show / No show attributes
void ModelStructure::updateSelectedTreeShowNoShow()
{
	// Get the Hash of selected instance
	PointerViewInstanceHash *pSelections= m_World.collection()->selection();
	PointerViewInstanceHash::iterator iEntry= pSelections->begin();
	QSet<QTreeWidgetItem*> parentSet;
    while (iEntry != pSelections->constEnd())
    {
    	QTreeWidgetItem* pQTreeWidgetItem= m_TreeWidgetHash.find(iEntry.key()).value();
    	if (iEntry.value()->isVisible())
    	{
    		if (pQTreeWidgetItem->foreground(0) != m_ShowForeground)
    		{
        		pQTreeWidgetItem->setForeground(0, m_ShowForeground);
        		pQTreeWidgetItem->setIcon(0, m_PartIcon);
    		}
    	}
    	else
    	{
       		if (pQTreeWidgetItem->foreground(0) != m_HideForeground)
        	{
        		pQTreeWidgetItem->setForeground(0, m_HideForeground);
        		pQTreeWidgetItem->setIcon(0, m_HiddenPartIcon);
        	}
    	}
    	addParent(&parentSet, pQTreeWidgetItem->parent());
    	++iEntry;
    }

    updateParentsVisibility(&parentSet);
}

//! Set Synchronisation mode
void ModelStructure::setSynchronisation(bool flag)
{
	if (m_IsSynchronise != flag)
	{
		if (false == m_IsSynchronise)
		{
			m_IsSynchronise= flag;
			updateTreeSelection();
		}
		else
		{
			m_IsSynchronise= flag;
		}
	}
}

//! Expand tree on the selected node
void ModelStructure::expandOnSelection()
{
	QList<QTreeWidgetItem*> selectedTreeWidgetItem= structure->selectedItems();
	if (!selectedTreeWidgetItem.isEmpty())
	{
		const int size= selectedTreeWidgetItem.size();
		for (int i= 0; i < size; ++i)
		{
			expandChilds(selectedTreeWidgetItem[i], true);
		}
	}
}

//! Expand tree on the selected node
void ModelStructure::collapseOnSelection()
{
	QList<QTreeWidgetItem*> selectedTreeWidgetItem= structure->selectedItems();
	if (!selectedTreeWidgetItem.isEmpty())
	{
		const int size= selectedTreeWidgetItem.size();
		for (int i= 0; i < size; ++i)
		{
			expandChilds(selectedTreeWidgetItem[i], false);
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Private Slots functions
//////////////////////////////////////////////////////////////////////
// Update the view selection
void ModelStructure::updateSelection()
{
	disconnect(m_pOpenglView, SIGNAL(updateSelection(PointerViewInstanceHash *)), this, SLOT(updateTreeSelection()));
	disconnect(m_pOpenglView, SIGNAL(unselectAll()), this, SLOT(unselectAll()));
	m_World.unselectAll();

	QList<QTreeWidgetItem*> selectedTreeWidgetItem= structure->selectedItems();
	QSet<GLC_StructOccurence*> selectedOccurence;
	if (!selectedTreeWidgetItem.isEmpty())
	{
		const int size= selectedTreeWidgetItem.size();
		for (int i= 0; i < size; ++i)
		{
			GLC_uint occurenceId= selectedTreeWidgetItem.at(i)->data(0, Qt::UserRole).toUInt();
			m_World.select(occurenceId);
		}

		m_pOpenglView->selectInstance(0); // To have signal feedback
	}
	updateCopyPaste(&selectedTreeWidgetItem);
	m_pEditPosition->setEnabled(1 == selectedTreeWidgetItem.size());

	connect(m_pOpenglView, SIGNAL(updateSelection(PointerViewInstanceHash *)), this, SLOT(updateTreeSelection()));
	connect(m_pOpenglView, SIGNAL(unselectAll()), this, SLOT(unselectAll()));

	// Update Instance property Action
	updateContextualMenu();
}

// Unselect all tree item
void ModelStructure::unselectAll()
{
	structure->blockSignals(true);
	structure->clearSelection();
	structure->blockSignals(false);

	updateCopyPaste();

	// Update Instance property Action
	updateContextualMenu();
}

// Item double clicked
void ModelStructure::itemDoubleClicked()
{
	// Get the clicked item
	if (m_pActionReframeOnSelection->isEnabled())
	{
		m_pOpenglView->reframeOnSelection();
	}
}

// Show selected instances
void ModelStructure::showSelectedInstances()
{
	m_World.showSelected3DViewInstance();
	m_pOpenglView->updateGL();
	updateSelectedTreeShowNoShow();
}

// Hide selected instances
void ModelStructure::hideSelectedInstances()
{
	m_World.hideSelected3DViewInstance();
	m_pOpenglView->updateGL();
	updateSelectedTreeShowNoShow();
}

void ModelStructure::copySelectedOccurence()
{
	QList<QTreeWidgetItem*> selectedTreeWidgetItem= structure->selectedItems();
	Q_ASSERT(1 == selectedTreeWidgetItem.size());
	const GLC_uint occurenceId= selectedTreeWidgetItem.first()->data(0, Qt::UserRole).toUInt();
	Q_ASSERT(m_World.containsOccurence(occurenceId));
	m_pClipBoard->second = m_World.occurence(occurenceId);
}

void ModelStructure::pasteOnSelectedOccurence()
{
	QList<QTreeWidgetItem*> selectedTreeWidgetItem= structure->selectedItems();
	Q_ASSERT(1 == selectedTreeWidgetItem.size());
	const GLC_uint occurenceId= selectedTreeWidgetItem.first()->data(0, Qt::UserRole).toUInt();
	Q_ASSERT(m_World.containsOccurence(occurenceId));
	Q_ASSERT(NULL != m_pClipBoard->second);
	Q_ASSERT(occurenceId != m_pClipBoard->second->id());

	// Copy occurence from the clipBoard
	GLC_StructOccurence* pOccurence= new GLC_StructOccurence(m_World.worldHandle(), *(m_pClipBoard->second), false);
	// Get the parent occurence
	GLC_StructOccurence* pParentOccurence= m_World.occurence(occurenceId);
	pParentOccurence->addChild(pOccurence);

	// Update tree widget
	QTreeWidgetItem* pNewItem= createStructureItem(pOccurence);
	selectedTreeWidgetItem.first()->addChild(pNewItem);
	//updateTreeShowNoShow();
	m_pOpenglView->updateGL();

}

void ModelStructure::editPosition()
{
	qDebug() << "ModelStructure::editPosition()";
	QList<QTreeWidgetItem*> selectedTreeWidgetItem= structure->selectedItems();
	Q_ASSERT(1 == selectedTreeWidgetItem.size());
	const GLC_uint occurenceId= selectedTreeWidgetItem.first()->data(0, Qt::UserRole).toUInt();
	Q_ASSERT(m_World.containsOccurence(occurenceId));

	EditPositionDialog editPosition(m_World.occurence(occurenceId), this);
	connect(&editPosition, SIGNAL(positionUpdated()), m_pOpenglView, SLOT(updateGL()));
	editPosition.exec();
	m_pOpenglView->updateGL();
}
//////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////

// Create the structure
void ModelStructure::createStructure()
{
	GLC_StructOccurence* pRoot= m_World.rootOccurence();
	QTreeWidgetItem* pRootItem= createStructureItem(pRoot);
	structure->addTopLevelItem(pRootItem);
	updateTreeShowNoShow();
	structure->expandItem(pRootItem);
}

// Create QTreeWidgetItem by GLC_Product
QTreeWidgetItem* ModelStructure::createStructureItem(GLC_StructOccurence* pOccurence)
{

	QString itemName;
	if (!pOccurence->name().isEmpty())
	{
		itemName= pOccurence->name();
	}
	else
	{
		itemName= QString("Product");
	}

	QTreeWidgetItem* pStructureItem= new QTreeWidgetItem();
	if (pOccurence->childCount() > 0)
	{
		pStructureItem->setIcon(0, m_ProductIcon);
		pStructureItem->setText(0, itemName);
		pStructureItem->setData(0, Qt::UserRole, QVariant(pOccurence->id()));
		{
			QList<GLC_StructOccurence*> childProducts= pOccurence->children();
			const int size= childProducts.size();
			for (int i= 0; i < size; ++i)
			{
				pStructureItem->addChild(createStructureItem(childProducts[i]));
			}
		}

		m_ProductHash.insert(pOccurence->id(), pOccurence);
	}
	else
	{
		pStructureItem->setText(0, pOccurence->name());
		pStructureItem->setData(0, Qt::UserRole, QVariant(pOccurence->id()));
		if (pOccurence->isVisible())
		{
			pStructureItem->setForeground(0, m_ShowForeground);
			pStructureItem->setIcon(0, m_PartIcon);
		}
		else
		{
			pStructureItem->setForeground(0, m_HideForeground);
			pStructureItem->setIcon(0, m_HiddenPartIcon);
		}
		m_TreeWidgetHash.insert(pOccurence->id(), pStructureItem);

	}

	return pStructureItem;
}

// Add parents to the hash table
void ModelStructure::addParent(QSet<QTreeWidgetItem*>* pParentSet, QTreeWidgetItem* pParent)
{
	if (NULL != pParent)
	{
		pParentSet->insert(pParent);
		addParent(pParentSet, pParent->parent());
	}
}
// Update parent visibility
void ModelStructure::updateParentsVisibility(QSet<QTreeWidgetItem*>* pParentSet)
{
    // Update Parents
    QSet<QTreeWidgetItem*>::iterator iParentIterator= pParentSet->begin();
    while (iParentIterator != pParentSet->constEnd())
    {
		// Item is a child, select It
		GLC_uint instanceId= (*iParentIterator)->data(0, Qt::UserRole).toUInt();
		GLC_StructOccurence* pCurrentProduct= m_ProductHash.value(instanceId);
    	if (pCurrentProduct->isVisible())
    	{
    		if ((*iParentIterator)->foreground(0) != m_ShowForeground)
    		{
        		(*iParentIterator)->setForeground(0, m_ShowForeground);
        		(*iParentIterator)->setIcon(0, m_ProductIcon);
    		}
    	}
    	else
    	{
    		if ((*iParentIterator)->foreground(0) != m_HideForeground)
    		{
        		(*iParentIterator)->setForeground(0, m_HideForeground);
        		(*iParentIterator)->setIcon(0, m_HiddenProductIcon);
    		}
    	}

    	++iParentIterator;
    }

}

//! Update contextual menu
void  ModelStructure::updateContextualMenu()
{
	// Update Reframe on selection Action
	if (m_World.collection()->selectionSize() > 0)
	{
		m_pActionReframeOnSelection->setEnabled(true);
		m_pActionShow->setEnabled(true);
		m_pActionHide->setEnabled(true);
		m_pActionExpand->setEnabled(true);
		m_pActionCollapse->setEnabled(true);
	}
	else
	{
		m_pActionReframeOnSelection->setEnabled(false);
		m_pActionShow->setEnabled(false);
		m_pActionHide->setEnabled(false);
		m_pActionExpand->setEnabled(false);
		m_pActionCollapse->setEnabled(false);
	}
}

// Expand childs of the specified item
void ModelStructure::expandChilds(QTreeWidgetItem* pItem, bool expand)
{
	// Expand the item
	pItem->setExpanded(expand);

	// Get item childs
	const int size= pItem->childCount();
	for (int i= 0; i < size; ++i)
	{
		expandChilds(pItem->child(i), expand);
	}
}

// Update parent selection
void ModelStructure::updateParentSelection(QHash<QTreeWidgetItem*, ParentSelection*>* pParentHash, QTreeWidgetItem* pChild)
{
	QTreeWidgetItem* pParentItem= pChild->parent();
	if (NULL != pParentItem)
	{
		if (!pParentHash->contains(pParentItem))
		{
			ParentSelection* pCurrentParent= new ParentSelection();
			pCurrentParent->treeWidgetList.append(pChild);
			pCurrentParent->NumberOfChilds= 1;
			pCurrentParent->size= pParentItem->childCount();
			pParentHash->insert(pParentItem, pCurrentParent);

			updateParentSelection(pParentHash, pParentItem);
		}
		else
		{
			ParentSelection* pCurrentParent= pParentHash->value(pParentItem);
			pCurrentParent->treeWidgetList.append(pChild);
			++(pCurrentParent->NumberOfChilds);
		}
	}
}

// Filter the selection
bool ModelStructure::filterSelection(QHash<QTreeWidgetItem*, ParentSelection*> *pParentHash, QTreeWidgetItem* pParent)
{
	bool isSelected= true;
	if ( !pParentHash->isEmpty())
	{
		// Retrieve parent selection structure
		ParentSelection* pParentSelection= pParentHash->value(pParent);
		if (NULL != pParentSelection)
		{
			// The Item have childs
			if (pParentSelection->NumberOfChilds == pParentSelection->size)
			{
				// All node childs have been pre-selected
				QList<QTreeWidgetItem*> selectedChild;
				for (int i= 0; i < pParentSelection->NumberOfChilds; ++i)
				{
					const bool childSelected= filterSelection(pParentHash, pParentSelection->treeWidgetList[i]);
					if (childSelected)
					{
						selectedChild.append(pParentSelection->treeWidgetList[i]);
					}
					isSelected= isSelected && childSelected;
				}
				if (!isSelected)
				{
					for (int i= 0; i < selectedChild.size(); ++i)
					{
						selectedChild[i]->setSelected(true);
					}
				}
			}
			else
			{
				// there are child that are not been pre-selected
				for (int i= 0; i < pParentSelection->NumberOfChilds; ++i)
				{
					if (filterSelection(pParentHash, pParentSelection->treeWidgetList[i]))
					{
						pParentSelection->treeWidgetList[i]->setSelected(true);
					}
				}
				isSelected= false;
			}
		}
		else
		{
			isSelected= true;
		}
	}
	else
	{
		isSelected= false;
	}

	return isSelected;
}

void ModelStructure::updateCopyPaste(const QList<QTreeWidgetItem*>* pSelectedTreeWidgetItem)
{
	if ((NULL != pSelectedTreeWidgetItem) && (1 == pSelectedTreeWidgetItem->size()))
	{
		m_pCopyAction->setEnabled(true);
		if (NULL != m_pClipBoard->second)
		{
			const GLC_uint occurenceId= pSelectedTreeWidgetItem->first()->data(0, Qt::UserRole).toUInt();
			if (occurenceId != m_pClipBoard->second->id())
			{
				m_pPasteAction->setEnabled(true);
			}
			else
			{
				m_pPasteAction->setEnabled(false);
			}
		}
	}
	else
	{
		m_pCopyAction->setEnabled(false);
		m_pPasteAction->setEnabled(false);
	}

}
