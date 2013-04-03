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

#include "ExportWebDialog.h"
#include "ExportToWeb.h"
#include "../opengl_view/OpenglView.h"


ExportWebDialog::ExportWebDialog(QWidget *pParent)
: QDialog(pParent)
, m_ThumbnailSize()
, m_ImageSize()
, m_PredefinedThumbnailSizes()
, m_pExportToWeb(NULL)
, m_NbrOfImage(0)
, m_TargetPath(QDir::homePath())
, m_CurrentBackgroundColor(Qt::white)
{
	setupUi(this);

	// Signal and slot connection
	// Predefined Size Change
	connect(thumbnailPredifinedSize, SIGNAL(currentIndexChanged(int)), this, SLOT(thumbnailSizeChanged(int)));
	connect(imagePredifinedSize, SIGNAL(currentIndexChanged(int)), this, SLOT(imageSizeChanged(int)));

	// Thumbails size changed
	connect(thumbnailWidth, SIGNAL(valueChanged(int)), this, SLOT(updateThumbnailPredifinedSize()));
	connect(thumbnailHeight, SIGNAL(valueChanged(int)), this, SLOT(updateThumbnailPredifinedSize()));

	// Image Size changed
	connect(imageWidth, SIGNAL(valueChanged(int)), this, SLOT(updateImagePredifinedSize()));
	connect(imageHeight, SIGNAL(valueChanged(int)), this, SLOT(updateImagePredifinedSize()));

	// Export Album check state changed
	connect(exportAlbumAndModels, SIGNAL(clicked(bool)), this, SLOT(updateImagePanelEnabledState(bool)));

	// Web title changed
	connect(webTitle, SIGNAL(textChanged(const QString&)), this, SLOT(webTitleChanged()));

	// Number of row change
	connect(numberOfRow, SIGNAL(valueChanged(int)), this, SLOT(numberOfRowChange(int)));
	connect(numberOfLine, SIGNAL(valueChanged(int)), this, SLOT(numberOfLineChange(int)));

	// Edit backround color
	connect(editColorButton, SIGNAL(clicked()), this, SLOT(editBackgroundColor()));
	QPixmap colorPixmap(16, 16);
	colorPixmap.fill(m_CurrentBackgroundColor);
	editColorButton->setIcon(QIcon(colorPixmap));


	// Target path
	 connect(browseCmd, SIGNAL(clicked()), this, SLOT(browse()));
	 connect(targetPath, SIGNAL(textChanged(const QString)), this, SLOT(updateExportButton()));
	 connect(createSubDir, SIGNAL(stateChanged(int)), this, SLOT(createSubDirStateChanged(int)));

	createPredefinedThumbnailSize();
	createPredefinedImageSize();

	// Check if subdirectory must be created
	if (createSubDir->checkState() == Qt::Checked)
	{
		targetPath->setText(QFileInfo(m_TargetPath + QDir::separator() + webTitle->text()).filePath());
	}
	else
	{
		targetPath->setText(m_TargetPath);
	}

}

ExportWebDialog::~ExportWebDialog()
{
	if (NULL != m_pExportToWeb)
	{
		delete m_pExportToWeb;
	}
}
//////////////////////////////////////////////////////////////////////
// Public Get function
//////////////////////////////////////////////////////////////////////

// Initialise the Dialog
void ExportWebDialog::initDialog(const QString& albumName, QList<FileEntry> fileEntrySortedList, OpenglView* pOpenglView)
{
	Q_ASSERT(NULL == m_pExportToWeb);

	m_pExportToWeb= new ExportToWeb(fileEntrySortedList, pOpenglView);
	m_pExportToWeb->exportAlbumAndModel(exportAlbumAndModels->checkState() == Qt::Checked);

	// Init m_pExportToWeb members

	m_pExportToWeb->exportAlbumAndModel(exportAlbumAndModels->checkState() == Qt::Checked);
	m_pExportToWeb->viewThumbnailModelName(thumbnailModelName->checkState() == Qt::Checked);
	m_pExportToWeb->linkThumbnailToModel(thumbnailModelLink->checkState() == Qt::Checked);
	m_pExportToWeb->viewImageModelInformations(imageModelInformation->checkState() == Qt::Checked);
	m_pExportToWeb->linkImageToModel(imageModelLink->checkState() == Qt::Checked);


	// Signals and slot connection
	// General
	connect(exportAlbumAndModels, SIGNAL(clicked(bool)), m_pExportToWeb, SLOT(exportAlbumAndModel(bool)));
	// Thumbnail
	connect(thumbnailModelName, SIGNAL(clicked(bool)), m_pExportToWeb, SLOT(viewThumbnailModelName(bool)));
	connect(thumbnailModelLink, SIGNAL(clicked(bool)), m_pExportToWeb, SLOT(linkThumbnailToModel(bool)));
	// Image
	connect(imageModelInformation, SIGNAL(clicked(bool)), m_pExportToWeb, SLOT(viewImageModelInformations(bool)));
	connect(imageModelLink, SIGNAL(clicked(bool)), m_pExportToWeb, SLOT(linkImageToModel(bool)));

	webTitle->setText(albumName);

	m_NbrOfImage= fileEntrySortedList.size();

	// Update the number of pages
	numberOfPage->setText(QString::number(numberOfPages()) + QString(" Pages"));
	m_pExportToWeb->updateNbrOfRow(numberOfRow->value());
	m_pExportToWeb->updateNbrOfLine(numberOfLine->value());
	m_pExportToWeb->updateNbrOfPage(numberOfPages());

}

// Export Album to web
void ExportWebDialog::accept()
{

	QString pathName(m_TargetPath);
	// Check if subdirectory must be created
	if (createSubDir->checkState() == Qt::Checked)
	{
		const QString subDir(webTitle->text());
		pathName+= QFileInfo(QDir::separator() + subDir).filePath();
		QDir basePath(m_TargetPath);
		if (QFileInfo(m_TargetPath + QDir::separator() + subDir).exists())
		{
			QString message(tr("Album folder already exists, use it ?"));
			QMessageBox::StandardButton returnButton;
			returnButton= QMessageBox::question(this, tr("Export To Web"), message, QMessageBox::No | QMessageBox::Yes);
			if (QMessageBox::No == returnButton)
			{
				return;
			}
		}
		else if (!basePath.mkdir(subDir))
		{
			QString message(tr("Failed to create Album folder : ") + m_TargetPath + QDir::separator() + subDir);
			QMessageBox::critical(NULL, tr("Export to Web"), message);
			QDialog::accept();
			return;
		}
	}

	hide();
	m_pExportToWeb->setThumbnailSize(m_ThumbnailSize);
	m_pExportToWeb->setImageSize(m_ImageSize);

	if (editColorButton->isChecked())
	{
		m_pExportToWeb->setBackgroundColor(m_CurrentBackgroundColor);
	}

	if (m_pExportToWeb->exportToWeb(pathName, webTitle->text()))
	{
		QMessageBox::StandardButton returnButton;
		returnButton= QMessageBox::question(this, tr("Export To Web"), tr("Export succesfull. Open Result?"), QMessageBox::No | QMessageBox::Yes);
		if (QMessageBox::Yes == returnButton)
		{
			QDesktopServices::openUrl(QUrl::fromLocalFile(m_pExportToWeb->indexPathName()));
		}
	}

	delete m_pExportToWeb;
	m_pExportToWeb=NULL;

	QDialog::accept();
}

void ExportWebDialog::reject()
{
	delete m_pExportToWeb;
	m_pExportToWeb=NULL;

	QDialog::reject();
}
//////////////////////////////////////////////////////////////////////
// Private Slot function
//////////////////////////////////////////////////////////////////////

// The size off the thumbnail changed
void ExportWebDialog::thumbnailSizeChanged(int index)
{
	if (index > 0)
	{
		// Find the predefined size
		m_ThumbnailSize= m_PredefinedThumbnailSizes[index];

		// Set the size
		thumbnailWidth->blockSignals(true);
		thumbnailHeight->blockSignals(true);

		thumbnailWidth->setValue(m_ThumbnailSize.width());
		thumbnailHeight->setValue(m_ThumbnailSize.height());

		thumbnailWidth->blockSignals(false);
		thumbnailHeight->blockSignals(false);
	}
}

// The size of the image changed
void ExportWebDialog::imageSizeChanged(int index)
{
	if (index > 0)
	{
		// Find the predefined size
		m_ImageSize= m_PredefinedImageSizes[index];

		// Set the size
		imageWidth->blockSignals(true);
		imageHeight->blockSignals(true);

		imageWidth->setValue(m_ImageSize.width());
		imageHeight->setValue(m_ImageSize.height());

		imageWidth->blockSignals(false);
		imageHeight->blockSignals(false);
	}
}


// Update the thumbnail predefined size combo
void ExportWebDialog::updateThumbnailPredifinedSize()
{
	m_ThumbnailSize.setWidth(thumbnailWidth->value());
	m_ThumbnailSize.setHeight(thumbnailHeight->value());


	thumbnailPredifinedSize->blockSignals(true);
	const int currentIndex= m_PredefinedThumbnailSizes.indexOf(m_ThumbnailSize);
	if (currentIndex != -1)
	{
		thumbnailPredifinedSize->setCurrentIndex(currentIndex);
	}
	else
	{
		thumbnailPredifinedSize->setCurrentIndex(0);
	}
	thumbnailPredifinedSize->blockSignals(false);
}

// Update the image predefined size combo
void ExportWebDialog::updateImagePredifinedSize()
{
	m_ImageSize.setWidth(imageWidth->value());
	m_ImageSize.setHeight(imageHeight->value());


	imagePredifinedSize->blockSignals(true);
	const int currentIndex= m_PredefinedImageSizes.indexOf(m_ImageSize);
	if (currentIndex != -1)
	{
		imagePredifinedSize->setCurrentIndex(currentIndex);
	}
	else
	{
		imagePredifinedSize->setCurrentIndex(0);
	}
	imagePredifinedSize->blockSignals(false);
}

// update image panel enable status
void ExportWebDialog::updateImagePanelEnabledState(bool state)
{
	if (state)
	{
		if (thumbnailModelLink->checkState() == Qt::Unchecked)
		{
			imagesGroup->setEnabled(true);
		}
		else
		{
			imagesGroup->setEnabled(false);
		}
	}
	else
	{
		imagesGroup->setEnabled(true);
	}
}

// Update Export button state
void ExportWebDialog::updateExportButton()
{
	bool exportStatus= false;
	if (webTitle->text().isEmpty())
	{
		exportStatus= false;
	}
	else
	{
		// Check if target path is set
		QString targetPathName(targetPath->text());
		if (!targetPathName.isEmpty())
		{
			// Check if subdirectory must be created
			if ((createSubDir->checkState() == Qt::Checked) && !webTitle->text().isEmpty())
			{
				const int targetPathLength= targetPath->text().length() - (webTitle->text().length() + 1);
				QString targetPathName(targetPath->text().left(targetPathLength));
				if (QFileInfo(targetPathName).isDir())
				{
					m_TargetPath= targetPathName;
					exportStatus= true;
				}
				else exportStatus= false;
			}
			else
			{
				if (QFileInfo(targetPath->text()).isDir())
				{
					m_TargetPath= targetPath->text();
					exportStatus= true;
				}
				else exportStatus= false;
			}
		}
		else exportStatus= false;
	}
	exportButton->setEnabled(exportStatus);
}

// The number of row as been changed
void ExportWebDialog::numberOfRowChange(int value)
{
	// Update the number of pages
	numberOfPage->setText(QString::number(numberOfPages()) + QString(" Pages"));

	m_pExportToWeb->updateNbrOfRow(value);
	m_pExportToWeb->updateNbrOfPage(numberOfPages());
}

// The number of line as been changed
void ExportWebDialog::numberOfLineChange(int value)
{
	// Update the number of pages
	numberOfPage->setText(QString::number(numberOfPages()) + QString(" Pages"));

	m_pExportToWeb->updateNbrOfLine(value);
	m_pExportToWeb->updateNbrOfPage(numberOfPages());
}

// Browse for destination directory
void ExportWebDialog::browse()
{
	QString pathName(QFileDialog::getExistingDirectory(this, tr("Select Destination directory"), m_TargetPath));
	if (!pathName.isEmpty())
	{
		// Check if subdirectory must be created
		if (createSubDir->checkState() == Qt::Checked)
		{
			targetPath->setText(pathName + QDir::separator() + webTitle->text());
		}
		else targetPath->setText(pathName);
	}
}

// The web Title as changed
void ExportWebDialog::webTitleChanged()
{
	// Check if subdirectory must be created
	if (createSubDir->checkState() == Qt::Checked)
	{
		targetPath->setText(m_TargetPath + QDir::separator() + webTitle->text());
	}

	updateExportButton();
}

// createSubDir State Changed
void ExportWebDialog::createSubDirStateChanged(int state)
{
	// Check if subdirectory must be created
	if (state == Qt::Checked)
	{
		targetPath->setText(m_TargetPath + QDir::separator() + webTitle->text());
	}
	else
	{
		targetPath->setText(m_TargetPath);
	}
}

// Edit background color
void ExportWebDialog::editBackgroundColor()
{
	QColor color = QColorDialog::getColor(m_CurrentBackgroundColor, this);
	if (color.isValid())
	{
		m_CurrentBackgroundColor= color;
		QPixmap colorPixmap(16, 16);
		colorPixmap.fill(m_CurrentBackgroundColor);
		editColorButton->setIcon(QIcon(colorPixmap));
	}
}


//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////

// Create the predefined thumbnail and size
void ExportWebDialog::createPredefinedThumbnailSize()
{
	// Add items to the predifined size list and set predefined size vector
	QStringList listOfPredefinedSize;
	listOfPredefinedSize << tr("Custom"); //0
	m_PredefinedThumbnailSizes.append(QSize(16, 16));

	listOfPredefinedSize << "32x32"; //1
	m_PredefinedThumbnailSizes.append(QSize(32, 32));

	listOfPredefinedSize << "40x40"; //2
	m_PredefinedThumbnailSizes.append(QSize(40, 40));

	listOfPredefinedSize << "50x50"; //3
	m_PredefinedThumbnailSizes.append(QSize(50, 50));

	listOfPredefinedSize << "60x60"; //4
	m_PredefinedThumbnailSizes.append(QSize(60,60));

	listOfPredefinedSize << "80x80"; //5
	m_PredefinedThumbnailSizes.append(QSize(80, 80));

	listOfPredefinedSize << "100x100"; //6
	m_PredefinedThumbnailSizes.append(QSize(100, 100));

	listOfPredefinedSize << "150x100"; //7
	m_PredefinedThumbnailSizes.append(QSize(150, 100));

	listOfPredefinedSize << "150x150"; //8
	m_PredefinedThumbnailSizes.append(QSize(150, 150));

	listOfPredefinedSize << "200x200"; //9
	m_PredefinedThumbnailSizes.append(QSize(200, 200));

	listOfPredefinedSize << "250x200"; //10
	m_PredefinedThumbnailSizes.append(QSize(250, 200));

	listOfPredefinedSize << "250x250"; //11
	m_PredefinedThumbnailSizes.append(QSize(250, 250));

	listOfPredefinedSize << "300x200"; //12
	m_PredefinedThumbnailSizes.append(QSize(300, 200));

	listOfPredefinedSize << "300x300"; //13
	m_PredefinedThumbnailSizes.append(QSize(300, 300));

	thumbnailPredifinedSize->addItems(listOfPredefinedSize);
	thumbnailPredifinedSize->setCurrentIndex(7);

	m_ThumbnailSize= m_PredefinedThumbnailSizes[7];

}

// Create the predefined image size
void ExportWebDialog::createPredefinedImageSize()
{
	QStringList listOfPredefinedSize;
	listOfPredefinedSize << tr("Custom"); //0
	m_PredefinedImageSizes.append(QSize(320, 240));

	listOfPredefinedSize << "320x240"; //1
	m_PredefinedImageSizes.append(QSize(320, 240));

	listOfPredefinedSize << "640x480"; //2
	m_PredefinedImageSizes.append(QSize(640, 480));

	listOfPredefinedSize << "720x480"; //3
	m_PredefinedImageSizes.append(QSize(720, 480));

	listOfPredefinedSize << "800x600"; //4
	m_PredefinedImageSizes.append(QSize(800,600));

	listOfPredefinedSize << "1024x768"; //5
	m_PredefinedImageSizes.append(QSize(1024, 768));

	listOfPredefinedSize << "1152x720"; //6
	m_PredefinedImageSizes.append(QSize(1152, 720));

	listOfPredefinedSize << "1280x800"; //7
	m_PredefinedImageSizes.append(QSize(1280, 800));

	listOfPredefinedSize << "1280x1024"; //8
	m_PredefinedImageSizes.append(QSize(1280, 1024));

	listOfPredefinedSize << "1440x900"; //9
	m_PredefinedImageSizes.append(QSize(1440, 900));

	listOfPredefinedSize << "1600x1200"; //10
	m_PredefinedImageSizes.append(QSize(1600, 1200));

	listOfPredefinedSize << "NTSC 646x485"; //11
	m_PredefinedImageSizes.append(QSize(646, 485));

	listOfPredefinedSize << "PAL 768x576"; //12
	m_PredefinedImageSizes.append(QSize(768, 576));

	listOfPredefinedSize << "PAL 780x576"; //13
	m_PredefinedImageSizes.append(QSize(780, 576));

	listOfPredefinedSize << "HD 1280x720"; //14
	m_PredefinedImageSizes.append(QSize(1280, 720));

	listOfPredefinedSize << "HD 1920x1080"; //15
	m_PredefinedImageSizes.append(QSize(1920, 1080));

	imagePredifinedSize->addItems(listOfPredefinedSize);
	imagePredifinedSize->setCurrentIndex(3);

	m_ImageSize= m_PredefinedImageSizes[3];
}

// number of page
int ExportWebDialog::numberOfPages() const
{
	const int nbrOfCase= numberOfLine->value() * numberOfRow->value();
	int nbrOfPage= static_cast<int>(ceil(static_cast<double>(m_NbrOfImage) / static_cast<double>(nbrOfCase)));

	return nbrOfPage;

}

