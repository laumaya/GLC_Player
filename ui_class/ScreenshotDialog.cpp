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

#include "ScreenshotDialog.h"
#include "../opengl_view/OpenglView.h"

ScreenshotDialog::ScreenshotDialog(OpenglView* pOpenglView, QWidget* pParent)
:QDialog(pParent)
, m_pOpenglView(pOpenglView)
, m_ScreenShot()
, m_PreviewScreenshot()
, m_CurrentBackgroundColor(Qt::white)
, m_CurrentBackgroundImageName()
, m_PreviewAspectRatio()
, m_ScreenshotAspectRatio()
, m_ImageAspectRatio()
, m_PreviewSize(400, 225)
, m_BackGroundMode(BackGroundDefault)
, m_PredefinedSizes()
, m_TargetImageSize()
, m_DefaultImageName()
, m_PreviousFilePath()
, m_MaxBufferSize(2000)
{
	setupUi(this);

	m_PreviewAspectRatio= static_cast<double>(m_PreviewSize.width()) / static_cast<double>(m_PreviewSize.height());

	// Check if frame buffer Object is supported
	if (GLC_State::frameBufferSupported())
	{
		imageInfos->clear();
		glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT, &m_MaxBufferSize);
		if (m_MaxBufferSize > 4000)
		{
			// Don't know why but it doesnt work if the size > 4000 on my computer ????
			m_MaxBufferSize= 4000;
		}
	}
	else
	{
		QString message(tr("Off screen capture not supported on this system"));
		message+= QString("\n") + tr("Image will be interpolated");
		imageInfos->setText(message);
	}

	spinBoxWidth->setMaximum(m_MaxBufferSize);
	spinBoxHeight->setMaximum(m_MaxBufferSize);

	createPredefinedSizeList();

	// Set default target size
	spinBoxWidth->setValue(m_TargetImageSize.width());
	spinBoxHeight->setValue(m_TargetImageSize.height());

	//Signal and slot connection
	connect(spinBoxWidth, SIGNAL(valueChanged(int)), this, SLOT(targetImageSizeChanged()));
	connect(spinBoxHeight, SIGNAL(valueChanged(int)), this, SLOT(targetImageSizeChanged()));

	// Background radio button
	connect(defaultBackgroundRadio, SIGNAL(clicked()), this, SLOT(activateDefaultBackground()));
	connect(transparentBackgroundRadio, SIGNAL(clicked()), this, SLOT(activateTransparentBackground()));
	connect(colorBackgroundRadio, SIGNAL(clicked()), this, SLOT(activateColorBackground()));
	connect(imageBackgroundRadio, SIGNAL(clicked()), this, SLOT(activateImageBackground()));

	// Edit backround color
	connect(editColorButton, SIGNAL(clicked()), this, SLOT(editBackgroundColor()));

	// Predefined Size Change
	connect(comboBoxPredifinedSize, SIGNAL(currentIndexChanged(int)), this, SLOT(predefinedSizeChanged(int)));

	// Browse Background Image
	connect(browseImageButton, SIGNAL(clicked()), this, SLOT(BrowseBackgroundImage()));

	// Image format
	connect(formatComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(imageFormatChanged(const QString&)));

}

ScreenshotDialog::~ScreenshotDialog()
{
	m_PredefinedSizes.clear();
}

//////////////////////////////////////////////////////////////////////
// Public Set function
//////////////////////////////////////////////////////////////////////

// Initialize the dialog
void ScreenshotDialog::initScreenshotDialog(const QString& fileName)
{
	m_DefaultImageName= fileName;
	// Compute image aspect ratio
	const int width= spinBoxWidth->value();
	const int height= spinBoxHeight->value();
	m_ImageAspectRatio= static_cast<double>(width) / static_cast<double>(height);

	m_ScreenshotAspectRatio= m_pOpenglView->viewportHandle()->aspectRatio();

	// Take the ScreenShot
	takeScreenshot();
	// Update the preview image
	updatePreviewImage();
}

//////////////////////////////////////////////////////////////////////
// Public Get function
//////////////////////////////////////////////////////////////////////
//! Screenshot have to be saved
void ScreenshotDialog::accept()
{
	QImage imageToSave;

	// Choose right background
	// Choose right background
	if (BackGroundTransparent == m_BackGroundMode)
	{
		QColor background(Qt::white);
		background.setAlpha(0);
		imageToSave= m_pOpenglView->takeScreenshot(true, m_TargetImageSize, QString(), background, m_ImageAspectRatio);
	}
	else if (BackGroundColor == m_BackGroundMode)
	{
		imageToSave= m_pOpenglView->takeScreenshot(true, m_TargetImageSize, QString(), m_CurrentBackgroundColor, m_ImageAspectRatio);
	}
	else if ((BackGroundImage == m_BackGroundMode) && !m_CurrentBackgroundImageName.isEmpty())
	{
		imageToSave= m_pOpenglView->takeScreenshot(true, m_TargetImageSize, m_CurrentBackgroundImageName, QColor(), m_ImageAspectRatio);
	}
	else
	{
		imageToSave= m_pOpenglView->takeScreenshot(true, m_TargetImageSize, QString(), QColor(), m_ImageAspectRatio);
	}

	const QString imageFormat= formatComboBox->currentText();
	QString extension('.');

	if (imageFormat == "JPEG")
	{
		extension.append("jpg");
	}
	else
	{
		extension.append(imageFormat.toLower());
	}

	// Save the image
	QString defaultImageName;
	if (m_PreviousFilePath.isEmpty())
	{
		defaultImageName= QDir::homePath() + QDir::separator() + m_DefaultImageName + extension;
	}
	else
	{
		defaultImageName= m_PreviousFilePath + QDir::separator() + m_DefaultImageName + extension;
	}
	qDebug() << defaultImageName;
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image As"), defaultImageName);
	if (!fileName.isEmpty())
	{
		m_PreviousFilePath= QFileInfo(fileName).absolutePath();
		fileName= m_PreviousFilePath + QDir::separator() + QFileInfo(fileName).completeBaseName() + extension;
		imageToSave.save(fileName, imageFormat.toLatin1().data(), 100);
		QDialog::accept();
	}
}


//////////////////////////////////////////////////////////////////////
// Private Slot function
//////////////////////////////////////////////////////////////////////

// The size off the target image changed
void ScreenshotDialog::targetImageSizeChanged()
{
	// Compute image aspect ratio
	m_TargetImageSize.setWidth(spinBoxWidth->value());
	m_TargetImageSize.setHeight(spinBoxHeight->value());
	m_ImageAspectRatio= static_cast<double>(m_TargetImageSize.width()) / static_cast<double>(m_TargetImageSize.height());

	// Take the ScreenShot
	takeScreenshot();

	// Update the preview image
	updatePreviewImage();

	// Update predifined Combo box
	comboBoxPredifinedSize->blockSignals(true);
	const int currentIndex= m_PredefinedSizes.indexOf(m_TargetImageSize);
	if (currentIndex != -1)
	{
		comboBoxPredifinedSize->setCurrentIndex(currentIndex);
	}
	else
	{
		comboBoxPredifinedSize->setCurrentIndex(0);
	}
	comboBoxPredifinedSize->blockSignals(false);
}

// Activate default background
void ScreenshotDialog::activateDefaultBackground()
{
	if (m_BackGroundMode != BackGroundDefault)
	{
		editColorButton->setEnabled(false);
		browseImageButton->setEnabled(false);

		m_BackGroundMode= BackGroundDefault;
		// Take the ScreenShot
		takeScreenshot();

		// Update the preview image
		updatePreviewImage();
	}
}

void ScreenshotDialog::activateTransparentBackground()
{
	if (m_BackGroundMode != BackGroundTransparent)
	{
		editColorButton->setEnabled(false);
		browseImageButton->setEnabled(false);

		m_BackGroundMode= BackGroundTransparent;
		// Take the ScreenShot
		takeScreenshot();

		// Update the preview image
		updatePreviewImage();
	}

}

// Activate color background
void ScreenshotDialog::activateColorBackground()
{
	if (m_BackGroundMode != BackGroundColor)
	{
		editColorButton->setEnabled(true);
		browseImageButton->setEnabled(false);

		m_BackGroundMode= BackGroundColor;
		// Take the ScreenShot
		takeScreenshot();

		// Update the preview image
		updatePreviewImage();
	}

}

// Activate image background
void ScreenshotDialog::activateImageBackground()
{
	if (m_BackGroundMode != BackGroundImage)
	{
		editColorButton->setEnabled(false);
		browseImageButton->setEnabled(true);

		m_BackGroundMode= BackGroundImage;
		// Take the ScreenShot
		takeScreenshot();

		// Update the preview image
		updatePreviewImage();
	}

}

// Edit background color
void ScreenshotDialog::editBackgroundColor()
{
	QColor color = QColorDialog::getColor(m_CurrentBackgroundColor, this);
	if (color.isValid())
	{
		m_CurrentBackgroundColor= color;

		// Take the ScreenShot
		takeScreenshot();

		// Update the preview image
		updatePreviewImage();
	}
}

// Predefined size change
void ScreenshotDialog::predefinedSizeChanged(int index)
{
	if (index > 0)
	{
		// Find the predefined size
		m_TargetImageSize= m_PredefinedSizes[index];

		// Set the size
		spinBoxWidth->blockSignals(true);
		spinBoxHeight->blockSignals(true);

		spinBoxWidth->setValue(m_TargetImageSize.width());
		spinBoxHeight->setValue(m_TargetImageSize.height());

		targetImageSizeChanged();

		spinBoxWidth->blockSignals(false);
		spinBoxHeight->blockSignals(false);
	}
}

// Browse Image
void ScreenshotDialog::BrowseBackgroundImage()
{
	// Set Image filter
	QList<QByteArray> imageFormat(QImageReader::supportedImageFormats());
	const int numberOfFormat= imageFormat.size();
	QString filter(tr("All Images ("));
	for (int i= 0; i < numberOfFormat; ++i)
	{
		filter.append(QString(" *.") + QString(imageFormat[i]));
	}
	filter.append(")");

	QString imageFileName= QFileDialog::getOpenFileName(this, tr("Image File Name"), m_CurrentBackgroundImageName, filter);
	if (!imageFileName.isEmpty() && QFile(imageFileName).exists())
	{
		// Test if the Image is Loadable
		QImage testBackgroundLoading(imageFileName);
		if (!testBackgroundLoading.isNull())
		{
			m_CurrentBackgroundImageName= imageFileName;
			m_BackGroundMode= BackGroundImage;
			// Take the ScreenShot
			takeScreenshot();

			// Update the preview image
			updatePreviewImage();
		}
		else
		{
			QString message(tr("Unable to load image :") + QString("\n"));
			message+= imageFileName;
			QMessageBox::critical(this->parentWidget(), tr("Backround Image"), message);
		}
	}
}

void ScreenshotDialog::imageFormatChanged(const QString& format)
{
	if (format == "JPEG")
	{
		transparentBackgroundRadio->setEnabled(false);
		if (m_BackGroundMode == BackGroundTransparent)
		{
			colorBackgroundRadio->toggle();
			activateColorBackground();
		}
	}
	else
	{
		transparentBackgroundRadio->setEnabled(true);
	}
}
//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////

// Take the screenshot
void ScreenshotDialog::takeScreenshot()
{

	// Choose right background
	if (BackGroundTransparent == m_BackGroundMode)
	{
		m_ScreenShot= m_pOpenglView->takeScreenshot(false, m_pOpenglView->size(), QString(), QColor(Qt::white), m_ImageAspectRatio);
	}
	else if (BackGroundColor == m_BackGroundMode)
	{
		m_ScreenShot= m_pOpenglView->takeScreenshot(false, m_pOpenglView->size(), QString(), m_CurrentBackgroundColor, m_ImageAspectRatio);
	}
	else if ((BackGroundImage == m_BackGroundMode) && !m_CurrentBackgroundImageName.isEmpty())
	{
		m_ScreenShot= m_pOpenglView->takeScreenshot(false, m_pOpenglView->size(), m_CurrentBackgroundImageName, QColor(), m_ImageAspectRatio);
	}
	else
	{
		m_ScreenShot= m_pOpenglView->takeScreenshot(false, m_pOpenglView->size(), QString(), QColor(), m_ImageAspectRatio);
	}
}

// Update the preview image
void ScreenshotDialog::updatePreviewImage()
{

	QImage tempImage;

	// Scaled the screenshot to fit aspect ratio by the near smaller image
	tempImage= m_ScreenShot.scaled(m_ScreenShot.size().height() * m_ImageAspectRatio, m_ScreenShot.size().height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	m_PreviewScreenshot= tempImage.scaled(m_PreviewSize.width(), m_PreviewSize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	ImagePreview->setPixmap(QPixmap::fromImage(m_PreviewScreenshot));
}

// Create predefined size list
void ScreenshotDialog::createPredefinedSizeList()
{
	// Add items to the predifined size list and set predefined size vector
	QStringList listOfPredefinedSize;
	listOfPredefinedSize << tr("Custom"); //0
	m_PredefinedSizes.append(QSize(16, 16));

	listOfPredefinedSize << "320x240"; //1
	m_PredefinedSizes.append(QSize(320, 240));

	listOfPredefinedSize << "640x480"; //2
	m_PredefinedSizes.append(QSize(640, 480));

	listOfPredefinedSize << "720x480"; //3
	m_PredefinedSizes.append(QSize(720, 480));

	listOfPredefinedSize << "800x600"; //4
	m_PredefinedSizes.append(QSize(800,600));

	listOfPredefinedSize << "1024x768"; //5
	m_PredefinedSizes.append(QSize(1024, 768));

	listOfPredefinedSize << "1152x720"; //6
	m_PredefinedSizes.append(QSize(1152, 720));

	listOfPredefinedSize << "1280x800"; //7
	m_PredefinedSizes.append(QSize(1280, 800));

	listOfPredefinedSize << "1280x1024"; //8
	m_PredefinedSizes.append(QSize(1280, 1024));

	listOfPredefinedSize << "1440x900"; //9
	m_PredefinedSizes.append(QSize(1440, 900));

	listOfPredefinedSize << "1600x1200"; //10
	m_PredefinedSizes.append(QSize(1600, 1200));

	listOfPredefinedSize << "NTSC 646x485"; //11
	m_PredefinedSizes.append(QSize(646, 485));

	listOfPredefinedSize << "PAL 768x576"; //12
	m_PredefinedSizes.append(QSize(768, 576));

	listOfPredefinedSize << "PAL 780x576"; //13
	m_PredefinedSizes.append(QSize(780, 576));

	listOfPredefinedSize << "HD 1280x720"; //14
	m_PredefinedSizes.append(QSize(1280, 720));

	listOfPredefinedSize << "HD 1920x1080"; //15
	m_PredefinedSizes.append(QSize(1920, 1080));

	comboBoxPredifinedSize->addItems(listOfPredefinedSize);
	comboBoxPredifinedSize->setCurrentIndex(4);

	m_TargetImageSize= m_PredefinedSizes[4];
}
