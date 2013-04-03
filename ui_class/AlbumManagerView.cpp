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

#include "AlbumManagerView.h"
#include "ModelProperties.h"
#include "../opengl_view/OpenglView.h"

#include <QFileInfo>
#include <QtDebug>

AlbumManagerView::AlbumManagerView(OpenglView* pOpenglView, FileEntryHash* pAlbumModel, QWidget *parent)
: QWidget(parent)
, m_pOpenglView(pOpenglView)
, m_pAlbumModel(pAlbumModel)
, m_IconSize()
, m_DisplayThumbnails(true)
, m_NumberOfErrorModel(0)
, m_NumberOfUnloadedModel(0)
, m_ModelLoadingInProgress(false)
, m_StopLoading(false)
, m_pActionDeleteSelectedModel(new QAction(QIcon(":images/Remove.png"), tr("Remove Model"), this))
, m_pActionGetErrorInfo(new QAction(tr("Get Error Information"), this))
, m_pActionModelProperties(new QAction(QIcon(":images/ModelProperties.png"), tr("Model Properties"), this))
, m_pActionReloadCurrentModel(new QAction(QIcon(":images/Refresh.png"), tr("Reload"), this))
, m_pModelProperties(NULL)
{
	setupUi(this);
	// Signals relay
	// Refresh the models thumbnails
	connect(refreshThumbnailsButton, SIGNAL(clicked()), this , SLOT(refreshModelsIcons()));
	// Remove unload model
	connect(removeUnloadModelsButton, SIGNAL(clicked()), this , SLOT(removeUnloadModels()));
	// Remove models on error
	connect(removeOnErrorModelsButton, SIGNAL(clicked()), this , SLOT(removeModelsOnError()));
	// Start loading
	connect(startLoadingButton, SIGNAL(clicked()), this , SLOT(beforeStartLoading()));
	// Stop loading
	connect(stopLoadingButton, SIGNAL(clicked()), this , SLOT(beforeStopLoading()));
	// Current Model change
	connect(modelList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *))
				, this , SLOT(currentModelChangedSlot(QListWidgetItem *, QListWidgetItem *)));
	// model clicked
	connect(modelList, SIGNAL(itemClicked(QListWidgetItem *)), this , SLOT(modelItemClicked(QListWidgetItem *)));

	// Model list Icons
	modelList->setIconSize(m_IconSize);

	//Contextual menu Creation
	modelList->addAction(m_pActionDeleteSelectedModel);
	QList<QKeySequence> keys;
	keys.append(Qt::Key_Delete);
	keys.append(Qt::Key_Backspace);
	m_pActionDeleteSelectedModel->setShortcuts(keys);
	connect(m_pActionDeleteSelectedModel, SIGNAL(triggered()), this, SLOT(deleteCurrentModel()));
	//m_pActionDeleteSelectedModel->setEnabled(false);
	// Connect action get error information
	connect(m_pActionGetErrorInfo, SIGNAL(triggered()), this, SLOT(getErrorInformation()));

	// Model properties
	modelList->addAction(m_pActionModelProperties);
	connect(m_pActionModelProperties, SIGNAL(triggered()), this, SLOT(modelProperties()));

	// Reload current model
	modelList->addAction(m_pActionReloadCurrentModel);
	connect(m_pActionReloadCurrentModel, SIGNAL(triggered()), this, SLOT(reloadCurrentModel()));

	// Album name
	setAlbumName(tr("New Album"));

}

AlbumManagerView::~AlbumManagerView()
{
}

// Return the ID of the current model if there is no current model NULL is return
GLC_uint AlbumManagerView::currentModelId() const
{
	GLC_uint modelId= 0;
	if (NULL != modelList->currentItem())
	{
		modelId= modelList->currentItem()->data(Qt::UserRole).toUInt();
	}
	return modelId;
}
// Return the specified model id
GLC_uint AlbumManagerView::modelId(int index) const
{
	GLC_uint modelId= 0;
	if (NULL != modelList->item(index))
	{
		modelId= modelList->item(index)->data(Qt::UserRole).toUInt();
	}
	return modelId;

}

// Return icon name with a size
QString AlbumManagerView::getIconName(bool WithError) const
{
	// Built the icon name
	QString iconName;
	if (WithError)
	{
		iconName= ":images/LogoError";
	}
	else
	{
		iconName= ":images/Logo";
	}

	if (m_IconSize.width() == 32)
	{
		iconName.append(".png");
	}
	else if (m_IconSize.width() == 40)
	{
		iconName.append("40x40.png");
	}
	else if (m_IconSize.width() == 50)
	{
		iconName.append("50x50.png");
	}
	else if (m_IconSize.width() == 60)
	{
		iconName.append("60x60.png");
	}
	else if (m_IconSize.width() == 70)
	{
		iconName.append("70x70.png");
	}
	else if (m_IconSize.width() == 80)
	{
		iconName.append("80x80.png");
	}
	return iconName;
}

// return the first unload model name
GLC_uint AlbumManagerView::firstUnloadModelId() const
{
	GLC_uint modelId= 0;
	// The number of item in the list
	const int max= modelList->count();

	// Get the icon name
	int i= 0;
	while ((i < max) && (0 == modelId))
	{
		if (modelList->item(i)->foreground() == QBrush(Qt::gray))
		{
			modelId= modelList->item(i)->data(Qt::UserRole).toUInt();
		}
		else ++i;
	}
	return modelId;
}

// Return the sorted list of fileEntry
QList<FileEntry> AlbumManagerView::sortedFileEntryList() const
{
	// The number of item in the list
	const int max= modelList->count();

	QList<FileEntry> sortedList;
	for (int i= 0; i < max; ++i)
	{
		sortedList << m_pAlbumModel->value(modelId(i));
	}

	return sortedList;
}


// Add a model to the list
void AlbumManagerView::addModel(const GLC_uint modelId)
{
	const QFileInfo model(m_pAlbumModel->value(modelId).getFileName());
	++m_NumberOfUnloadedModel;
	QListWidgetItem* pItem= new QListWidgetItem(model.fileName());
	pItem->setData(Qt::UserRole, QVariant(modelId));
	pItem->setForeground(QBrush(Qt::gray));
	if (m_DisplayThumbnails)
	{
		pItem->setIcon(QPixmap(getIconName(false)));
	}
	modelList->addItem(pItem);

	// Update UI buttons
	if (((modelList->count() == 1) || (m_StopLoading == false)) && !stopLoadingButton->isEnabled())
	{
		startLoadingButton->setEnabled(true);
		startLoadingButton->setChecked(true);
		stopLoadingButton->setEnabled(true);
	}


	// Update UI Info
	numberOfModels->setText(QString::number(modelList->count()));
}

// Set the model list icon size
void AlbumManagerView::setIconSize(const QSize& size)
{
	m_IconSize= size;
	modelList->setIconSize(m_IconSize);

	refreshModelsIcons();
}

// Clear the album
void AlbumManagerView::clear()
{
	modelList->clear();
	m_NumberOfErrorModel= 0;
	m_NumberOfUnloadedModel= 0;
	// Update UI Button
	resetButton();
	// Update UI Info
	setAlbumName(tr("New Album"));
	numberOfModels->setText(QString::number(0));
	numberOfLoadedModels->setText(QString::number(0));
	// Clear current model informations panel
	clearCurrentModelInfo();
}

// Reset the album
void AlbumManagerView::resetButton()
{
	m_StopLoading= false;
	// Update UI Button
	refreshThumbnailsButton->setEnabled(false);
	startLoadingButton->setEnabled(false);
	startLoadingButton->setChecked(false);
	stopLoadingButton->setEnabled(false);
	stopLoadingButton->setChecked(false);
	removeUnloadModelsButton->setEnabled(false);
	removeOnErrorModelsButton->setEnabled(false);
}

// Set the thumbnails display
void AlbumManagerView::setThumbnailsDisplay(const bool display)
{
	m_DisplayThumbnails= display;
	refreshThumbnailsButton->setEnabled(m_DisplayThumbnails && (modelList->count() > 0));
}

// Change widget color and return his index
int AlbumManagerView::modelLoaded(const GLC_uint modelId)
{
	int curItem= 0;
	while (modelList->item(curItem)->data(Qt::UserRole).toUInt() != modelId)
	{
		++curItem;
	}
	modelList->item(curItem)->setForeground(QBrush(Qt::black));
	--m_NumberOfUnloadedModel;

	// Update UI buttons
	if (m_NumberOfUnloadedModel == 0)
	{
		refreshThumbnailsButton->setEnabled(true);

		startLoadingButton->setEnabled(false);
		startLoadingButton->setChecked(false);

		stopLoadingButton->setEnabled(false);
		stopLoadingButton->setChecked(false);

		if (m_NumberOfErrorModel > 0)
		{
			removeOnErrorModelsButton->setEnabled(true);
		}

	}
	else if (m_StopLoading)
	{
		refreshThumbnailsButton->setEnabled(true);
		startLoadingButton->setEnabled(true);
		removeUnloadModelsButton->setEnabled(true);
		if (m_NumberOfErrorModel > 0)
		{
			removeOnErrorModelsButton->setEnabled(true);
		}
	}

	// Update UI info
	numberOfLoadedModels->setText(QString::number(modelList->count() - m_NumberOfUnloadedModel));

	return curItem;
}

// Change widget color and return his index
int AlbumManagerView::modelLoadFailed(const GLC_uint modelId)
{
	int curItem= 0;
	while (modelList->item(curItem)->data(Qt::UserRole).toUInt() != modelId)
	{
		++curItem;
	}
	modelList->item(curItem)->setForeground(QBrush(Qt::red));
	--m_NumberOfUnloadedModel;

	if (m_DisplayThumbnails)
	{
		modelList->item(curItem)->setIcon(QPixmap(getIconName(true)).scaled(m_IconSize, Qt::IgnoreAspectRatio));
	}
	++m_NumberOfErrorModel;

	//Update UI buttons
	if (m_NumberOfErrorModel == 1)
		removeOnErrorModelsButton->setEnabled(true);

	if (m_NumberOfUnloadedModel == 0)
	{
		refreshThumbnailsButton->setEnabled(true);

		startLoadingButton->setEnabled(false);
		startLoadingButton->setChecked(false);

		stopLoadingButton->setEnabled(false);
		stopLoadingButton->setChecked(false);

		removeOnErrorModelsButton->setEnabled(true);
	}
	else if (m_StopLoading)
	{
		refreshThumbnailsButton->setEnabled(true);
		startLoadingButton->setEnabled(true);
		removeUnloadModelsButton->setEnabled(true);
		removeOnErrorModelsButton->setEnabled(true);
	}

	//Update UI info
	numberOfLoadedModels->setText(QString::number(modelList->count() - m_NumberOfUnloadedModel));

	return curItem;
}

// Update Current model info
void AlbumManagerView::updateCurrentModelInfo(int instances, int faces)
{
	numberOfMesh->setText(QString::number(instances));
	numberOfFaces->setText(QString::number(faces));
}

// clear Current model info
void AlbumManagerView::clearCurrentModelInfo()
{
	numberOfMesh->clear();
	numberOfFaces->clear();
}

// Set a snapshoot
void AlbumManagerView::setSnapShoot(int i, const QImage &image)
{
	modelList->item(i)->setIcon(QPixmap::fromImage(image).scaled(m_IconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

// Set enabled status of QAction
void AlbumManagerView::setEnabledStatus(bool status)
{
	m_pActionDeleteSelectedModel->setEnabled(status);
	m_pActionReloadCurrentModel->setEnabled(status);
	m_pActionModelProperties->setEnabled(status);
}

// Go to the next item
void AlbumManagerView::nextItem()
{
	int row= modelList->currentRow() + 1;
	if (row < modelList->count())
		modelList->setCurrentRow(row);
}

// Go to the previous item
void AlbumManagerView::previousItem()
{
	int row= modelList->currentRow() - 1;
	if ((row < modelList->count()) && row >= 0)
		modelList->setCurrentRow(row);
}

// Refresh the models icon
void AlbumManagerView::refreshModelsIcons()
{
	// The number of item in the list
	const int max= modelList->count();


	if (m_DisplayThumbnails)
	{
		// Create progress dialog
		QProgressDialog progress(tr("Updating thumbnails... Please wait."), tr("Cancel"), 0, max, this);
		progress.setModal(true);
		progress.setMinimumDuration(500);

		// refresh model list icon size
		modelList->setIconSize(m_IconSize);

		// Get the icon name
		for (int i= 0; i < max; ++i)
		{
			if (modelList->item(i)->foreground() == QBrush(Qt::black))
			{
				emit computeIconInBackBuffer(i);
			}
			else if (modelList->item(i)->foreground() == QBrush(Qt::gray))
			{
				modelList->item(i)->setIcon(QPixmap(getIconName(false)));
			}
			else
			{
				modelList->item(i)->setIcon(QPixmap(getIconName(true)));
			}

			// Update Progress dialog and chek fo cancellation
			progress.setValue(i);
			QCoreApplication::processEvents(); //QEventLoop::ExcludeUserInputEvents

			if (progress.wasCanceled())
			{
				modelList->setSpacing(1);
				return;
			}

		}

	}
	else
	{
		// Thumbnail haven't to be displayed use the default one
		modelList->setIconSize(QSize());
		for (int i= 0; i < max; ++i)
		{
			modelList->item(i)->setIcon(QIcon());
		}
	}
	// Force items to be laid out again
	modelList->setSpacing(1);
}

// Details model properties
void AlbumManagerView::modelProperties()
{
	if (NULL != modelList->currentItem())
	{
		GLC_uint idOfModelToDelete= currentModelId();
		FileEntryHash::iterator iEntry= m_pAlbumModel->find(idOfModelToDelete);
		if (!iEntry.value().isLoading())
		{
			if (NULL == m_pModelProperties)
			{
				m_pModelProperties= new ModelProperties(m_pOpenglView, m_pAlbumModel->value(idOfModelToDelete), nativeParentWidget());
			}
			else
			{
				m_pModelProperties->setFileEntry(m_pAlbumModel->value(idOfModelToDelete));
			}
			m_pModelProperties->exec();
		}
	}

}

// Model Item as been clicked
void AlbumManagerView::modelItemClicked(QListWidgetItem *pClickedItem)
{
	if (pClickedItem == modelList->currentItem())
	{
		QString modelName(m_pAlbumModel->value(pClickedItem->data(Qt::UserRole).toUInt()).getFileName());
		emit displayMessage(modelName);
	}
}

// Remove unload models
void AlbumManagerView::removeUnloadModels()
{
	if (modelList->count() > m_NumberOfUnloadedModel )
	{
		int i= 0;
		while ((i < modelList->count()) && (m_NumberOfUnloadedModel > 0))
		{
			// Test if the model is unload
			if (modelList->item(i)->foreground() == QBrush(Qt::gray))
			{
				delete modelList->takeItem(i);
				--m_NumberOfUnloadedModel;
			}
			else
			{
				++i;
			}
		}
		// Check if the number of unload model is equal to 0
		Q_ASSERT(0 == m_NumberOfUnloadedModel);

		//Update UI buttons
		startLoadingButton->setEnabled(false);
		startLoadingButton->setChecked(false);

		stopLoadingButton->setEnabled(false);
		stopLoadingButton->setChecked(false);

		removeUnloadModelsButton->setEnabled(false);
		//Update UI info
		numberOfModels->setText(QString::number(modelList->count()));

		m_StopLoading= false;
		emit removeUnloadFileItem();
	}
	else
	{
		emit newAlbum(false);
	}

}

// Remove models on error
void AlbumManagerView::removeModelsOnError()
{
	if (modelList->count() > m_NumberOfErrorModel )
	{
		int i= 0;
		while ((i < modelList->count()) && (m_NumberOfErrorModel > 0))
		{
			// Test if the model is on error
			if (modelList->item(i)->foreground() == QBrush(Qt::red))
			{
				delete modelList->takeItem(i);
				--m_NumberOfErrorModel;
			}
			else
			{
				++i;
			}
		}
		// Check if the number of error model is equal to 0
		Q_ASSERT(0 == m_NumberOfErrorModel);
		//modelList->removeAction(m_pActionGetErrorInfo);

		//Update UI buttons
		removeOnErrorModelsButton->setEnabled(false);
		//Update UI info
		numberOfModels->setText(QString::number(modelList->count()));
		numberOfLoadedModels->setText(QString::number(modelList->count() - m_NumberOfUnloadedModel));
		emit removeOnErrorModels();
	}
	else
	{
		emit newAlbum(false);
	}
}

// update ui before sending signal startLoading
void AlbumManagerView::beforeStartLoading()
{
	if (startLoadingButton->isChecked())
	{
		stopLoadingButton->setChecked(false);
		refreshThumbnailsButton->setEnabled(false);
		removeUnloadModelsButton->setEnabled(false);
		m_StopLoading= false;
		emit startLoading();
	}
	else
	{
		startLoadingButton->setChecked(true);
	}
}

// Update UI before sending signal stopLoading
void AlbumManagerView::beforeStopLoading()
{
	if (stopLoadingButton->isChecked())
	{
		startLoadingButton->setChecked(false);
		refreshThumbnailsButton->setEnabled(true);
		m_StopLoading= true;
		emit stopLoading();
	}
	else
	{
		stopLoadingButton->setChecked(true);
	}
}

// Delete the current model
void AlbumManagerView::deleteCurrentModel()
{
	if (NULL != modelList->currentItem())
	{
		GLC_uint idOfModelToDelete= currentModelId();
		FileEntryHash::iterator iEntry= m_pAlbumModel->find(idOfModelToDelete);
		if (!iEntry.value().isLoading())
		{
			if (modelList->count() > 1)
			{
				int currentRow= modelList->currentRow();
				// Get the model loading status and name before removing
				delete modelList->takeItem(currentRow);
				// Test entry status
				if (iEntry.value().isOnError())
				{
					--m_NumberOfErrorModel;
					if (m_NumberOfErrorModel == 0) removeOnErrorModelsButton->setEnabled(false);

				}
				else if (!iEntry.value().isLoaded())
				{
					--m_NumberOfUnloadedModel;
				}
				// Update UI Buttons
				numberOfModels->setText(QString::number(modelList->count()));
				numberOfLoadedModels->setText(QString::number(modelList->count() - m_NumberOfUnloadedModel));

				if (m_NumberOfUnloadedModel == 0)
				{
					m_StopLoading= false;
					startLoadingButton->setEnabled(false);
					startLoadingButton->setChecked(false);

					stopLoadingButton->setEnabled(false);
					stopLoadingButton->setChecked(false);
					removeUnloadModelsButton->setEnabled(false);
				}

				emit deleteModel(idOfModelToDelete);
			}
			// This is the last model -> New Album
			else emit newAlbum(false);
		}

	}

}

// Reload current model
void AlbumManagerView::reloadCurrentModel()
{
	if (NULL != modelList->currentItem())
	{
		// Get the current model id
		GLC_uint modelId= currentModelId();

		if (m_pAlbumModel->value(modelId).isLoaded())
		{
			modelList->currentItem()->setForeground(QBrush(Qt::gray));
			if (m_DisplayThumbnails)
			{
				modelList->currentItem()->setIcon(QPixmap(getIconName(false)));
			}
			++m_NumberOfUnloadedModel;
			// Update UI info
			numberOfLoadedModels->setText(QString::number(modelList->count() - m_NumberOfUnloadedModel));

			emit reloadCurrentModelSignal(modelId);
		}
	}
}

// The current model changed
void AlbumManagerView::currentModelChangedSlot(QListWidgetItem *pCurrent, QListWidgetItem *pPrevious)
{

	if (NULL != pCurrent)
	{
		// Get the current model id
		GLC_uint modelId= currentModelId();
		QList<QAction*> actionList(modelList->actions());
		const bool containsErrorAction= actionList.contains(m_pActionGetErrorInfo);
		FileEntryHash::const_iterator iEntry= m_pAlbumModel->find(modelId);
		if (iEntry.value().isOnError())
		{
			if (!containsErrorAction)
			{
				modelList->removeAction(m_pActionModelProperties);
				modelList->insertAction(m_pActionReloadCurrentModel, m_pActionGetErrorInfo);
			}
		}
		else
		{
			if (containsErrorAction)
			{
				modelList->removeAction(m_pActionGetErrorInfo);
				modelList->insertAction(m_pActionReloadCurrentModel, m_pActionModelProperties);
			}
		}
	}

	emit currentModelChanged(pCurrent, pPrevious);
}

// Get error information
void AlbumManagerView::getErrorInformation()
{
	QString message= m_pAlbumModel->value(currentModelId()).getError();
	QMessageBox::information(this->parentWidget(), tr("Error Information"), message);
}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////

