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

#ifndef SETTINGSDIALOG_H_
#define SETTINGSDIALOG_H_

#include "ui_SettingsDialog.h"

#include <QDialog>
#include <QSize>
#include <QPair>
#include <QDir>

class SettingsDialog : public QDialog, private Ui::SettingsDialog
{
	Q_OBJECT
public:
	//! Default constructor
	SettingsDialog(const bool display, const QSize& size, const bool displayInfo
			, const bool quitConfirmation, const bool selectionShaderIsUsed
			, const bool vboIsUsed, const bool shaderIsUsed, const int defaultLod
			, const bool spacePartitionIsUsed, const bool useBoundingBox, const int partionDepth
			, const bool frustumCullingIsUsed, const bool pixelCullingIsUsed, int pixelCullingSize, QWidget *parent);

	virtual ~SettingsDialog();

public:
	//! Accept modifications
	void accept();

	//! Return true if thumbnail have to be shown
	bool thumbnailHaveToBeDisplay() const;

	//! get the thumbnail size
	QSize getThumbnailSize() const;

	//! return true if info have to be shown
	bool infoHaveToBeShown() const;

	//! return true if quit confirmation is needed
	bool quitConfirmation() const;

	//! Return true is selection shader must be used
	bool selectionShaderIsUsed() const;

	//! Return true if vbo must be used
	bool vboIsUsed() const;

	//! Return true if shader must be used
	bool shaderIsUsed() const;

	//! Return the default LOD value
	int defaultLodValue() const;

	//! Return true if space partitionning is used
	bool spacePartitionningIsUsed() const;

	//! Return true if the bounding box are used for octree
	bool boundingBoxIsUsed() const;

	//! Return the octree depth value
	int octreeDepthValue() const;

	//! Return true if frustum culling is uses
	bool frustumCullingIsUsed() const;

	//! Return true if pixel culling is used
	bool pixelCullingIsUsed() const;

	//! Return the pixel culling size
	int pixelCullingSize() const;


private slots:
	//! The user change the current language
	void currentLanguageChange(int);

	//! The user change the state of display thumbnail
	void displayThumbnailStateChanged(int);

	//! The user Change the icon size
	void iconSizeChange(int);

	//! The user change default LOD
	void defaultLodChange(int);

	//! Browse for destination directory
	void browse();

	//! Update the cache informations
	void updateCacheInformations();

	//! Clean The cache directory
	void cleanCache();

	//! Space partitionning usage as change
	void changeSpacePartitionning();


private:
	//! Set OpenGl information labels
	void setOpenGLInformation();

	//! Return a pair containing the number of files and the size of the cache
	QPair<int, qint64> cacheNumberAndSize(const QDir&, bool) const;

	//! Convert size from qint64 to String
	QString sizeFromQint64ToString(const qint64&);

	//! delete all entries of the specified dir
	void deleteEntriesOfDir(const QDir&);


private:
	//! Initial language
	QString m_InitLanguage;

	//! New Language
	QString m_NewLanguage;

	//! selected thumbnail Size
	QSize m_ThumbnailSize;

	//! Initial value of vbo usage
	const bool m_InitVboUsage;

	//! Initial value of shader usage
	const bool m_InitShaderUsage;
};

#endif /*SETTINGSDIALOG_H_*/
