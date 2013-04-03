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

#include "SettingsDialog.h"
#include <QSettings>
#include <QtGui>
#include <GLC_State>
#include <QMessageBox>

SettingsDialog::SettingsDialog(const bool display, const QSize& size, const bool displayInfo
		, const bool quitConfirmation, const bool selectionShaderIsUsed
		, const bool vboIsUsed, const bool shaderIsUsed, const int defaultLod
		, const bool spacePartitionIsUsed, const bool useBoundingBox, const int partionDepth
		, const bool frustumCullingIsUsed, const bool pixelCullingIsUsed, int pixelCullingSize, QWidget *parent)
: QDialog(parent)
, m_InitLanguage()
, m_NewLanguage()
, m_ThumbnailSize(size)
, m_InitVboUsage(vboIsUsed)
, m_InitShaderUsage(shaderIsUsed)
{
	setupUi(this);

	// Init Display thumbnail check box
	if (display)
		displayThumbnail->setCheckState(Qt::Checked);
	else
		displayThumbnail->setCheckState(Qt::Unchecked);

	// Initialize Thumbnail size combo box
	QString sizeArray[6]= {"32x32", "40x40", "50x50", "60x60", "70x70", "80x80"};
	ThumbnailSize_Combo->addItem(sizeArray[0]);
	ThumbnailSize_Combo->addItem(sizeArray[1]);
	ThumbnailSize_Combo->addItem(sizeArray[2]);
	ThumbnailSize_Combo->addItem(sizeArray[3]);
	ThumbnailSize_Combo->addItem(sizeArray[4]);
	ThumbnailSize_Combo->addItem(sizeArray[5]);

	const QString stringSize(QString::number(m_ThumbnailSize.width()) + QString("x") + QString::number(m_ThumbnailSize.height()));
	int pos= 0;
	while (sizeArray[pos] != stringSize) pos++;
	ThumbnailSize_Combo->setCurrentIndex(pos);
	ThumbnailSize_Combo->setEnabled(display);

	#if defined(Q_OS_MAC)
	const QString settingsFileName(QDir::homePath() + "/Library/Application Support/" + QCoreApplication::applicationName() + QDir::separator() + "Settings.ini");
	QDir().mkpath(QFileInfo(settingsFileName).path());
	QSettings settings(settingsFileName, QSettings::IniFormat);
	#else
	QSettings settings;
	#endif

	m_InitLanguage= settings.value("currentLanguage").toString();
	languageComboBox->addItem(tr("English"));
	languageComboBox->addItem(tr("French"));

	if (m_InitLanguage == "fr")
	{
		languageComboBox->setCurrentIndex(1);
	}
	else
	{
		languageComboBox->setCurrentIndex(0);
	}

	// Init Display thumbnail check box
	if (displayInfo)
		displayViewInfoPanel->setCheckState(Qt::Checked);
	else
		displayViewInfoPanel->setCheckState(Qt::Unchecked);

	// Init Quit confirmation check box
	if (quitConfirmation)
		quitConfirmationActive->setCheckState(Qt::Checked);
	else
		quitConfirmationActive->setCheckState(Qt::Unchecked);

	// Init Opengl information
	if (shaderIsUsed)
	{
		useShader->setCheckState(Qt::Checked);
	}
	else
	{
		useShader->setCheckState(Qt::Unchecked);

	}

	if (selectionShaderIsUsed)
	{
		useSelectionShader->setCheckState(Qt::Checked);
	}
	else
	{
		useSelectionShader->setCheckState(Qt::Unchecked);
	}
	if (vboIsUsed)
	{
		qDebug() << "settings use VBO";
		useVbo->setCheckState(Qt::Checked);
	}
	else
	{
		useVbo->setCheckState(Qt::Unchecked);
	}
	setOpenGLInformation();


	// Init performance setting
	defaultLodLineEdit->setText(QString::number(defaultLod));
	levelOfDetailSlider->setValue(defaultLod);

	if (spacePartitionIsUsed)
	{
		useOctree->setCheckState(Qt::Checked);
	}
	else
	{
		useOctree->setCheckState(Qt::Unchecked);
	}

	// Bounding Box or bounding sphere radio
	bBoxRadio->setChecked(useBoundingBox);
	octreeDepthSpin->setValue(partionDepth);

	if (frustumCullingIsUsed)
	{
		useFrustumCulling->setCheckState(Qt::Checked);
	}
	else
	{
		useFrustumCulling->setCheckState(Qt::Unchecked);
	}

	if (pixelCullingIsUsed)
	{
		usePixelCulling->setCheckState(Qt::Checked);

	}
	else
	{
		usePixelCulling->setCheckState(Qt::Unchecked);
	}
	minimumPixelSize->setValue(pixelCullingSize);

	// Cache management
	if (GLC_State::cacheIsUsed())
	{
		workWithCacheCheckBox->setCheckState(Qt::Checked);
	}
	else
	{
		workWithCacheCheckBox->setCheckState(Qt::Unchecked);
	}
	targetPath->setText(GLC_State::currentCacheManager().absolutePath());

	if (GLC_State::currentCacheManager().compressionIsUsed())
	{
		usedCompressedCacheCheckBox->setCheckState(Qt::Checked);
	}
	else
	{
		usedCompressedCacheCheckBox->setCheckState(Qt::Unchecked);
	}
	int compressionLevel= GLC_State::currentCacheManager().compressionLevel();
	if (compressionLevel == -1)
	{
		usedDefaultCompressionCheckBox->setCheckState(Qt::Checked);
	}
	else
	{
		usedDefaultCompressionCheckBox->setCheckState(Qt::Unchecked);
	}
	CompressionLevelSpinBox->setValue(compressionLevel);


	// Signal and slot connection
	connect(languageComboBox, SIGNAL(currentIndexChanged(int)), this , SLOT(currentLanguageChange(int)));
	connect(displayThumbnail, SIGNAL(stateChanged(int)), this , SLOT(displayThumbnailStateChanged(int)));
	connect(ThumbnailSize_Combo, SIGNAL(currentIndexChanged(int)), this , SLOT(iconSizeChange(int)));
	connect(levelOfDetailSlider, SIGNAL(valueChanged(int)), this, SLOT(defaultLodChange(int)));
	// Cache management
	connect(browseCmd, SIGNAL(clicked()), this, SLOT(browse()));
	connect(updateCacheInfoCmd, SIGNAL(clicked()), this, SLOT(updateCacheInformations()));
	connect(cleanCacheCmd, SIGNAL(clicked()), this, SLOT(cleanCache()));

	// Performance
	connect(useOctree, SIGNAL(stateChanged(int)), this, SLOT(changeSpacePartitionning()));

	pageList->blockSignals(true);
	pageList->setCurrentRow(0);
	pageList->blockSignals(false);

}

SettingsDialog::~SettingsDialog()
{
}

// Accept modifications
void SettingsDialog::accept()
{
	// Update the cache usage
	GLC_State::setCacheUsage(workWithCacheCheckBox->checkState() == Qt::Checked);
	GLC_State::currentCacheManager().setCachePath(targetPath->text());
	GLC_State::currentCacheManager().setCompressionUsage(usedCompressedCacheCheckBox->checkState() == Qt::Checked);
	if (usedDefaultCompressionCheckBox->checkState() == Qt::Checked)
	{
		GLC_State::currentCacheManager().setCompressionLevel(-1);
	}
	else
	{
		GLC_State::currentCacheManager().setCompressionLevel(CompressionLevelSpinBox->value());
	}

	bool restart= false;

	if (!m_NewLanguage.isEmpty() && (m_NewLanguage != m_InitLanguage))
	{
		#if defined(Q_OS_MAC)
		const QString settingsFileName(QDir::homePath() + "/Library/Application Support/" + QCoreApplication::applicationName() + QDir::separator() + "Settings.ini");
		QDir().mkpath(QFileInfo(settingsFileName).path());
		QSettings settings(settingsFileName, QSettings::IniFormat);
		#else
		QSettings settings;
		#endif
		settings.setValue("currentLanguage", m_NewLanguage);
		restart= true;
	}

	if (m_InitShaderUsage != shaderIsUsed())
	{
		restart= true;
	}

	if (restart)
	{
		QMessageBox::warning(this, tr("Warning"), tr("Restart Application to take setting into account."), QMessageBox::Ok);
	}

	QDialog::accept();
}
// Return true if thumbnail have to be shown
bool SettingsDialog::thumbnailHaveToBeDisplay() const
{
	return displayThumbnail->checkState() == Qt::Checked;
}

// get the thumbnail size
QSize SettingsDialog::getThumbnailSize() const
{
	return m_ThumbnailSize;
}

// return true if info have to be shown
bool SettingsDialog::infoHaveToBeShown() const
{
	return displayViewInfoPanel->checkState() == Qt::Checked;
}

//! return true if quit confirmation is needed
bool SettingsDialog::quitConfirmation() const
{
	return quitConfirmationActive->checkState() == Qt::Checked;
}

// Return true is selection shader must be used
bool SettingsDialog::selectionShaderIsUsed() const
{
  return useSelectionShader->checkState() == Qt::Checked;
}

// Return true if vbo must be used
bool SettingsDialog::vboIsUsed() const
{
  return useVbo->checkState() == Qt::Checked;
}

// Return true if shader must be used
bool SettingsDialog::shaderIsUsed() const
{
	return useShader->checkState() == Qt::Checked;
}

// Return the default LOD value
int SettingsDialog::defaultLodValue() const
{
	return defaultLodLineEdit->text().toInt();
}

bool SettingsDialog::spacePartitionningIsUsed() const
{
	return useOctree->checkState() == Qt::Checked;
}

bool SettingsDialog::boundingBoxIsUsed() const
{
	return bBoxRadio->isChecked();
}

int SettingsDialog::octreeDepthValue() const
{
	return octreeDepthSpin->value();
}

bool SettingsDialog::frustumCullingIsUsed() const
{
	return useFrustumCulling->checkState() == Qt::Checked;
}
// Return true if pixel culling is used
bool SettingsDialog::pixelCullingIsUsed() const
{
	return usePixelCulling->checkState() == Qt::Checked;
}

int SettingsDialog::pixelCullingSize() const
{
	return minimumPixelSize->value();
}

// The user change the current language
void SettingsDialog::currentLanguageChange(int index)
{
	switch (index)
	{
		case 0:
			m_NewLanguage= "en";
			break;
		case 1:
			m_NewLanguage= "fr";
			break;
		default:
			break;
	}
}
// The user change the state of display thumbnail
void SettingsDialog::displayThumbnailStateChanged(int state)
{
	const bool unableState= (state == Qt::Checked);
	ThumbnailSize_Combo->setEnabled(unableState);
}
// The user Change the icon size
void SettingsDialog::iconSizeChange(int index)
{
	switch (index)
	{
		case 0:
			m_ThumbnailSize= QSize(32, 32);
			break;
		case 1:
			m_ThumbnailSize= QSize(40, 40);
			break;
		case 2:
			m_ThumbnailSize= QSize(50, 50);
			break;
		case 3:
			m_ThumbnailSize= QSize(60, 60);
			break;
		case 4:
			m_ThumbnailSize= QSize(70, 70);
			break;
		case 5:
			m_ThumbnailSize= QSize(80, 80);
			break;
		default:
			break;
	}
}

//! The user change default LOD
void SettingsDialog::defaultLodChange(int value)
{
	defaultLodLineEdit->setText(QString::number(value));
}

// Browse for destination directory
void SettingsDialog::browse()
{
	const QString targetPathString= targetPath->text();
	const QString pathName(QFileDialog::getExistingDirectory(this, tr("Select Destination directory"), targetPathString));
	if (!pathName.isEmpty())
	{
		if (GLC_State::currentCacheManager().setCachePath(pathName))
		{
			targetPath->setText(pathName);
			updateCacheInformations();
		}
		else
		{
			const QString cachePath= targetPath->text();
			QString message(tr("Cannot set the cache to path : ") + cachePath);
			QMessageBox::warning(this, QCoreApplication::applicationName(), message);
		}
	}
}

// Update the cache informations
void SettingsDialog::updateCacheInformations()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QDir cacheDir(targetPath->text());
	QPair<int, qint64> cacheInfos= cacheNumberAndSize(cacheDir, true);

	QString numberOfFiles= QString::number(cacheInfos.first);
	numberOfCacheFiles->setText(numberOfFiles);

	QString cacheSize= sizeFromQint64ToString(cacheInfos.second);
	sizeOfCache->setText(cacheSize);

	QApplication::restoreOverrideCursor();
}

// Clean The cache directory
void SettingsDialog::cleanCache()
{
	const QString cachePath= targetPath->text();
	QString message(tr("Remove all entries from the path :") + cachePath);
	if (QMessageBox::warning(this, QCoreApplication::applicationName(), message, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

		deleteEntriesOfDir(QDir(targetPath->text()));

		QApplication::restoreOverrideCursor();

		updateCacheInformations();
	}
}

void SettingsDialog::changeSpacePartitionning()
{
	if (useOctree->checkState() == Qt::Checked)
	{
		useFrustumCulling->setCheckState(Qt::Checked);
	}
}

//! Set OpenGl information labels
void SettingsDialog::setOpenGLInformation()
{
	// Set Opengl information labels
	openglVersion->setText(GLC_State::version());
	openglHardware->setText(GLC_State::renderer());
	// VBO Support
	if (m_InitVboUsage)
	{
		vboUsage->setText(tr("Used"));
	}
	else
	{
		vboUsage->setText(tr("Not Used"));
		QPalette myPalette;
		myPalette.setColor(QPalette::Normal, QPalette::WindowText, Qt::red);
		vboUsage->setPalette(myPalette);

		if (!GLC_State::vboSupported())
		{
			useVbo->setCheckState(Qt::Unchecked);
			useVbo->setEnabled(false);
		}
	}
	// GLSL SUpport
	if (GLC_State::glslUsed())
	{
		glslUsage->setText(tr("Used"));
	}
	else
	{
		glslUsage->setText(tr("Not Used"));
		QPalette myPalette;
		myPalette.setColor(QPalette::Normal, QPalette::WindowText, Qt::red);
		glslUsage->setPalette(myPalette);
		// Disable use selection shader check box
		useSelectionShader->setEnabled(false);
		useSelectionShader->setCheckState(Qt::Unchecked);
		if (!GLC_State::glslSupported())
		{
			useShader->setEnabled(false);
		}
	}

	// Frame buffer support
	if (GLC_State::frameBufferSupported())
	{
		framebufferUsage->setText(tr("Used"));
	}
	else
	{
		framebufferUsage->setText(tr("Not Used"));
		QPalette myPalette;
		myPalette.setColor(QPalette::Normal, QPalette::WindowText, Qt::red);
		framebufferUsage->setPalette(myPalette);
	}

}

// Return a pair containing the number of files and the size of the cache
QPair<int, qint64> SettingsDialog::cacheNumberAndSize(const QDir& dir, bool subdir) const
{
	QPair<int, qint64> numberAndSize(0, 0);
	// Get cache files from the dir
	{
		QStringList filter;
		filter << QString("*." + GLC_BSRep::suffix());

		QFileInfoList bRepInfoList= dir.entryInfoList(filter, QDir::Files);
		const int size= bRepInfoList.size();
		numberAndSize.first+= size;
		for (int i= 0; i < size; ++i)
		{
			numberAndSize.second+= bRepInfoList.at(i).size();
		}
	}

	// Get cache sub directories
	if (subdir)
	{
		QFileInfoList subDirInfoList= dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot );
		const int size= subDirInfoList.size();
		for (int i= 0; i < size; ++i)
		{
			QPair<int, qint64> subDirNumberAndSize(0, 0);
			QDir subDir(subDirInfoList.at(i).filePath());
			subDirNumberAndSize= cacheNumberAndSize(subDir, false);
			numberAndSize.first+= subDirNumberAndSize.first;
			numberAndSize.second+= subDirNumberAndSize.second;
		}
	}

	return numberAndSize;
}

// Convert size from qint64 to String
QString SettingsDialog::sizeFromQint64ToString(const qint64& sizeInt)
{
	double size= static_cast<double>(sizeInt);

	QString stringSize;
	if (size > 1024 * 1024)
	{
		stringSize= QString::number(size / (1024 * 1024), 'f', 2) + tr(" Mo");
	}
	else if (size > (1024))
	{
		stringSize= QString::number(size / (1024), 'f', 2) + tr(" Ko");
	}
	else
	{
		stringSize= QString::number(size, 'f', 2) + tr(" Bytes");
	}
	return stringSize;

}

// delete all entries of the specified dir
void SettingsDialog::deleteEntriesOfDir(const QDir& dir)
{
	// Delete files of the dir
	{

		QFileInfoList filesInfoList= dir.entryInfoList(QDir::Files);
		const int size= filesInfoList.size();
		for (int i= 0; i < size; ++i)
		{
			QFile currentFile(filesInfoList.at(i).filePath());
			currentFile.remove();
		}
	}

	// Delete sub dir of the dir
	{
		QFileInfoList subDirInfoList= dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot );
		const int size= subDirInfoList.size();
		for (int i= 0; i < size; ++i)
		{

			QDir subDir(subDirInfoList.at(i).filePath());
			deleteEntriesOfDir(subDir);
			dir.rmdir(subDir.dirName());
		}
	}

}
