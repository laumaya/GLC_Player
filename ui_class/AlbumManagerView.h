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

#ifndef ALBUMMANAGERVIEW_H_
#define ALBUMMANAGERVIEW_H_

#include "ui_AlbumManagerView.h"
#include <QWidget>
#include <QHash>
#include "FileEntry.h"

class ModelProperties;
class OpenglView;

class AlbumManagerView : public QWidget, private Ui::AlbumManagerView
{
	Q_OBJECT
//////////////////////////////////////////////////////////////////////
/*!@name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	AlbumManagerView(OpenglView*, FileEntryHash*, QWidget *parent= 0);

	virtual ~AlbumManagerView();
//@}

//////////////////////////////////////////////////////////////////////
/*!\name Public Get methods*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Get the model list icon size
	inline QSize iconSize() const {return m_IconSize;}

	//! Return true if there is a current model
	inline bool haveCurrentModel() const {return (NULL != modelList->currentItem());}

	//! Return true if the index is current
	inline bool isCurrent(int index) const {return modelList->item(index) != modelList->currentItem();}

	//! Return the ID of the current model if there is no current model NULL is return
	GLC_uint currentModelId() const;

	//! Return the specified model id
	GLC_uint modelId(int) const;

	//! Return the current item
	inline QListWidgetItem * currentItem() {return modelList->currentItem();}

	//! Return specidied item
	inline QListWidgetItem * item(int index) {return modelList->item(index);}

	//! return true if thumbnails are displayed
	inline bool thumbnailsAreDisplay() const {return m_DisplayThumbnails;}

	//! Return icon name with a size
	QString getIconName(bool) const;

	//! return the row of specified QListWidgetItem
	inline int row(QListWidgetItem* pItem) const {return modelList->row(pItem);}

	//! return the number of unload model in the view
	inline int numberOfUnloadedModels() const {return m_NumberOfUnloadedModel;}

	//! return the number of model on error
	inline int numberOfErrorModels() const {return m_NumberOfErrorModel;}

	//! return the first unload model name
	GLC_uint firstUnloadModelId() const;

	//! return the current row
	inline int currentRow() const {return modelList->currentRow();}

	//! Return the sorted list of fileEntry
	QList<FileEntry> sortedFileEntryList() const;

//@}

//////////////////////////////////////////////////////////////////////
/*!\name Public Set methods*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Add a model to the list
	void addModel(const GLC_uint);

	//! Set the model list icon size
	void setIconSize(const QSize&);

	//! Clear the album
	void clear();

	//! Reset the album
	void resetButton();

	//! Set the thumbnails display
	void setThumbnailsDisplay(const bool);

	//! Change widget color and return his index
	int modelLoaded(const GLC_uint);

	//! Change widget color and return his index
	int modelLoadFailed(const GLC_uint);

	//! Set the current model
	inline void setCurrent(int index)
	{modelList->setCurrentItem(modelList->item(index));}

	//! Update Current model info
	void updateCurrentModelInfo(int, int);

	//! clear Current model info
	void clearCurrentModelInfo();

	//! Set a snapshoot
	void setSnapShoot(int, const QImage &);

	//! Set the album name
	inline void setAlbumName(const QString& name)
	{album_groupBox->setTitle(name);}

	//! Set enabled status of QAction
	void setEnabledStatus(bool);

	//! Go to the next item
	void nextItem();

	//! Go to the previous item
	void previousItem();

//@}

signals:
	//! Remove unload model
	void removeUnloadFileItem();

	//! Remove on error model
	void removeOnErrorModels();

	//! Start loading models
	void startLoading();

	//! Stop loading
	void stopLoading();

	//! The current model changed
	void currentModelChanged(QListWidgetItem *, QListWidgetItem *);

	//! Display Message
	void displayMessage(QString);

	//! the model specified by name as been deleted
	void deleteModel(GLC_uint);

	//! Update progress Bar
	void updateProgressBar(int);

	//! compute the specified model thumbnails in back buffer
	void computeIconInBackBuffer(int);

	//! New Album
	void newAlbum(bool);

	//! Reload current model
	void reloadCurrentModelSignal(GLC_uint);

//////////////////////////////////////////////////////////////////////
/*!\name Public slots Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public slots:
	//! Refresh the models icon
	void refreshModelsIcons();

	//! Details model properties
	void modelProperties();

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Private slots Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private slots:
	//! Model Item as been clicked
	void modelItemClicked(QListWidgetItem *);

	//! Remove unload models
	void removeUnloadModels();

	//! Remove models on error
	void removeModelsOnError();

	//! Update UI before sending signal startLoading
	void beforeStartLoading();

	//! Update UI before sending signal stopLoading
	void beforeStopLoading();

	//! Delete the current model
	void deleteCurrentModel();

	//! Reload current model
	void reloadCurrentModel();

	//! The current model changed
	void currentModelChangedSlot(QListWidgetItem *, QListWidgetItem *);

	//! Get error information
	void getErrorInformation();


//@}

//////////////////////////////////////////////////////////////////////
/*! \name Private services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:

//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The View
	OpenglView* m_pOpenglView;

	//! The Album Model
	FileEntryHash* m_pAlbumModel;

	//! Size of the icon in model list
	QSize m_IconSize;

	//! Thumbnail visibility
	bool m_DisplayThumbnails;

	//! Number of model with error
	int m_NumberOfErrorModel;

	//! Number of model not loaded
	int m_NumberOfUnloadedModel;

	//! State of model loading in progress
	bool m_ModelLoadingInProgress;

	//! Loading of models have stoped
	bool m_StopLoading;

	//! Delete selected item
	QAction* m_pActionDeleteSelectedModel;

	//! Get error information
	QAction* m_pActionGetErrorInfo;

	//! Get model properties
	QAction* m_pActionModelProperties;

	//! Reload current model
	QAction* m_pActionReloadCurrentModel;

	//! Model Properties dialog
	ModelProperties* m_pModelProperties;


};

#endif /*ALBUMMANAGERVIEW_H_*/
