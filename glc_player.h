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

#ifndef GLC_PLAYER_H
#define GLC_PLAYER_H

#include "ui_glc_player.h"
#include "opengl_view/OpenglView.h"
#include "OpenFileThread.h"
#include "FileEntry.h"

#include <GLC_Global>

#include <QMainWindow>

class SelectionProperty;
class EditLightDialog;
class AlbumManagerView;
class ModelManagerView;
class InstanceProperty;
class MaterialProperty;
class ChooseShaderDialog;
class ListOfMaterial;
class OpenAlbumOption;
class SendFilesDialog;
class ScreenshotDialog;
class MultiScreenshotsDialog;
class ExportWebDialog;
class LeftSideDock;

class glc_player : public QMainWindow, private Ui::glc_playerClass
{
    Q_OBJECT

public:
    glc_player(QWidget *parent = 0);
    ~glc_player();
//////////////////////////////////////////////////////////////////////
// Virtual protected function
//////////////////////////////////////////////////////////////////////
protected:
	//! Close event handler
	void closeEvent(QCloseEvent*);

public:
//////////////////////////////////////////////////////////////////////
// public Methods
//////////////////////////////////////////////////////////////////////
	void openOnEvent(QString);

//////////////////////////////////////////////////////////////////////
// public Slots
//////////////////////////////////////////////////////////////////////
public slots:
	//! The selection has changed
	void updateSelection(PointerViewInstanceHash*);
	//! Info panel not supported
	void hideInfoPanel();
//////////////////////////////////////////////////////////////////////
// private Slots
//////////////////////////////////////////////////////////////////////
private slots:
	//! Unselect All instances
	void unselectAll();
	//! Set Instance visibility
	void showOrHide();
	//! Hide unseleted instance
	void hideUnselected();
	//! Show All instance
	void showAll();
	//! Swap visible space
	void swapVisibleSpace();
	//! update the value of the progress bar
	void updateProgressBar(int);
	void updateProgressBarForExport(int);
	//! Create a empty new model
	void newModel();
	//! Remove all file item from the current album
	bool newAlbum(bool confirmation= true);
	//! Menu File
	//! Open An existing file
	void open();
	//! Open An existing album
	void openAlbum();
	//! Open Models from the given path
	void openModelsFromPath();
	//! Save the current album
	void saveAlbum();
	//! Save As a new album
	void saveAlbumAs();
	//! Open a Recent File
	void openRecentFile();
	//! Open a Recent Album
	void openRecentAlbum();
	//! Send File to folder
	void sendToFolder();
	//! Export album to web
	void exportToWeb();
	//! Export current model
	void exportCurrentModel();
	//! View and edit instance property
	void instanceProperty();
	//! View and edit material property
	void viewMaterialProperty(GLC_Material*);
	//! View the list of material
	void viewListOfMaterial();
	//! Assign shader to the current selection
	void assignShader();
	//! Reframe the view
	void reframe();
	//! Select mode
	void selectMode();
	//! define view center mode
	void viewCenterMode();
	//! Pan the view mode
	void panMode();
	//! Rotate the view mode
	void rotateMode();
	//! Zoom the view mode
	void zoomMode();
	//! Menu Tools
	//! Show application settings dialog
	void showSettings();
	//! A file as been Opened
	void fileOpened();
	//! load of File failed
	void loadFileFailed();
	//! Current file Item Changed
	void currentFileItemChanged(QListWidgetItem *, QListWidgetItem *);
	//! Display a message in status bar
	void displayMessageInStatusBar(QString);
	//! Remove unload item
	void removeUnloadFileItem();
	//! Remove item on error
	void removeItemOnError();
	//! Start Loading
	void startLoadingButton();
	//! Stop Loading
	void stopLoadingButton();
	//! Take a snapshot
	void takeSnapShot();
	//! Take multi snapshot
	void takeMultiShots();
	//! Delete the specified item
	void deleteItem(const GLC_uint);
	//! Album Management visibility toggle
	void albumManagementVisibilityToggle();
	//! Instance Property visibility toggle
	void instancePropertyVisibilityToggle();
	//! The album management visibility change
	void albumManagementVisibilityChanged(bool);
	//! The Instance property visibility change
	void instancePropertyVisibilityChanged(bool);
	//! Camera property toggle
	void cameraPropertyVisibilityToggle();
	//! The camera property visibility change
	void cameraPropertyVisibilityChanged(bool);
	//! Selection property toggle
	void selectionPropertyVisibilityToggle();
	//! The Selection property visibility change
	void selectionPropertyVisibilityChanged(bool);
	//! Go to Point Rendering mode
	void pointsRenderingMode();
	//! Go to wireframe Rendering mode
	void wireframeRenderingMode();
	//! Go to shading Rendering Mode
	void shadingRenderingMode();
	//! Go to shading and wire render mode
	void shadingAndWireRenderingMode();
	//! Display about GLC_Player dialog box
	void aboutPlayer();
	//! Go to fullScreen Mode
	void fullScreen();
	//! Display GLC_Player help page
	void help();
	//! Update the view
	void updateView();
	//! Get information about failed loading
	void getErrorInformation();
	//! Display edit light dialog box
	void editLightDialog();
	//! Two sided lightning
	void twoSidedLightning();
	//! Compute the icon of a newly loaded file in back buffer
	void computeIconInBackBuffer(int, bool forceCurrent= false);
	//! Return to normal mode
	void returnToNormalMode();
	//! Choose shader
	void chooseShader();
	//! reload model
	void reloadModel(GLC_uint);
	//! Update current file Entry Material
	void updateCurrentEntryMaterial(GLC_Material*);
	//! The Opengl as been initialised
	void glInitialed();
	//! Change Current mover to track ball mover
	void changeCurrentMoverToTrackBall();
	//! Change Current mover to turn table mover
	void changeCurrentMoverToTurnTable();
	// Change Current mover to fly mover
	void changeCurrentMoverToFly();
	// Change the current projection mode
	void changeCurrentProjectionMode();
	// Showw error log
	void showErrorLog();

	void sectioning();


protected:
	//! Key press event
	void keyPressEvent (QKeyEvent *);

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////
private:
	//! Create RecentFileActionArray
	void createRecentFileActionsArray();
	//! Create RecentFileActionArray
	void createRecentAlbumActionsArray();
	//! Update Recent Files
	void updateRecentsFiles();
	//! Update Recent Albums
	void updateRecentsAlbums();
	//! Open the file
	void openModel(const QString&, const GLC_uint);
	//! Add current file to recent file
	void addToRecentFiles(const QString&);
	//! Add current Album to recent Albums
	void addToRecentAlbums(const QString&);
	//! drag enter event
	void dragEnterEvent(QDragEnterEvent*);
	//! drop event
	void dropEvent(QDropEvent*);
	//! Load successful
	void loadSuccessful(QString);
	//! Add items
	/*! By default new item are gray*/
	void addItems(QStringList);
	//! Start Loading FileItem
	void startLoading();
	//! Set the current File Item
	void setCurrentFileItem(const GLC_uint);
	//! Take a snapshot of the specifies item with the specifie ratio
	//! Return Snapshot as QImage
	QImage takeSnapShoot(int, double, bool forceCurrent= false);
	//! Read Application Settings
	void readSettings();
	//! Write Application Settings
	void writeSettings();
	//! Ask for saving the current Session
	bool getOpenAlbumOptionDlg();
	//! Apply album saving
	void applySavingAlbum(const QString&);
	//! Open specified album
	void openAlbum(const QString&);
	//! Return the list of file name within the given path
	QStringList fileNameFromGivenPath(const QString path, const QStringList& filter);

//////////////////////////////////////////////////////////////////////
// private member
//////////////////////////////////////////////////////////////////////
private:
    OpenglView m_OpenglView;
    //! current file path
    QString m_CurrentPath;
    //! Current Album Path
    QString m_CurrentAlbumPath;
    //! Loading progressBar
    QProgressBar *m_pProgressBar;
    //! Error Message dialog
	QErrorMessage* m_pQErrorMessage;
	//! Recent File list
	QStringList m_RecentFilesList;
	//! Recent Album list
	QStringList m_RecentAlbumsList;
	//! Maximum number of recent file
	enum {MaxRecentFiles = 10};
	QAction* m_pRecentFileActionsArray[MaxRecentFiles];
	QAction* m_pRecentAlbumActionsArray[MaxRecentFiles];
	//! Current File name
	QString m_CurrentFileName;
	//! Current album name
	QString m_CurrentAlbumName;
	//! The thread wich open file
	OpenFileThread m_OpenFileThread;
	//! File Entry Hash table
	FileEntryHash m_FileEntryHash;
	//! Model name Set
	QSet<QString> m_modelName;
	//! A File Loading is in progress
	bool m_FileLoadingInProgress;
	//! A list Loading is in progress
	bool m_ListLoadingInProgress;
	//! continue List Loading
	bool m_ContinuListLoading;
	//! Make the first loading file current
	bool m_MakeFirstFileCurrent;
	//! display off the info panel
	bool m_dislayInfoPanel;
	//! The selection property widget`
	SelectionProperty* m_pSelectionProperty;
	//! Edit Light dialog box
	EditLightDialog* m_pEditLightDialog;
	//! Activate quit confirmation
	bool m_QuitConfirmation;
	//! The left side dock
	LeftSideDock* m_pLeftSideDock;
	//! The Album management widget
	AlbumManagerView* m_pAlbumManagerView;
	//! The model management widget
	ModelManagerView* m_pModelManagerView;
	//! The Instance property dock widget
	InstanceProperty* m_pInstanceProperty;
	//! Instance property visibility action
	QAction* m_pInstancePropertyVis;
	//! The material property dock widget
	MaterialProperty* m_pMaterialProperty;
	//! The chose shader dialog
	ChooseShaderDialog* m_pChooseShaderDialog;
	//! The material list
	ListOfMaterial* m_pListOfMaterial;
	//! Open Album Option dialog
	OpenAlbumOption* m_pOpenAlbumOption;
	//! Send File dialog
	SendFilesDialog* m_pSendFilesDialog;
	//! Screenshot Dialog
	ScreenshotDialog* m_pScreenshotDialog;
	//! Multi screenshot Dialog
	MultiScreenshotsDialog* m_pMultiScreenshotsDialog;
	//! Export web dialog
	ExportWebDialog* m_pExportWebDialog;
	//! Use selection Shader
	bool m_UseSelectionShader;
	//! Vbo Usage (-1 : not set, 0 : don't use, 1 Use)
	int m_UseVbo;
	//! Shader Usage (-1 : not set, 0 : don't use, 1 Use)
	int m_UseShader;
	//! Default LOD value
	int m_DefaultLodValue;
	//! Pixel culling usage
	bool m_UsePixelCulling;
	//! Pixel Culling size
	int m_PixelCullingSize;
	//! Frustum culling usage
	bool m_UseFrustumCulling;
	//! Space partition usage
	bool m_UseSpacePartion;
	//! Bounding box for octree usage
	bool m_UseOctreeBoundingBox;
	//! Octree depth
	int m_OctreeDepth;
	//! Clipboard
	QPair<GLC_uint, GLC_StructOccurence* > m_ClipBoard;

};

#endif // GLC_PLAYER_H
