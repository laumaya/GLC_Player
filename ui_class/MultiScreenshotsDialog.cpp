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

#include "MultiScreenshotsDialog.h"
#include "../opengl_view/MultiShotsOpenglView.h"
#include "../opengl_view/OpenglView.h"
#include <QGLFramebufferObject>

MultiScreenshotsDialog::MultiScreenshotsDialog(OpenglView* pOpenglView, QWidget* pParent)
:QDialog(pParent)
, m_pOpenglView(pOpenglView)
, m_ScreenShot()
, m_CurrentBackgroundColor(Qt::white)
, m_CurrentBackgroundImageName()
, m_ScreenshotAspectRatio()
, m_ImageAspectRatio()
, m_PreviewSize(400, 225)
, m_BackGroundMode(BackGroundDefault)
, m_PredefinedSizes()
, m_TargetImageSize()
, m_DefaultImageName()
, m_PreviousFilePath()
, m_MaxBufferSize(2000)
, m_pMultiShotsOpenglView(NULL)
, m_RotationAxis()
{
	setupUi(this);

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
	connect(colorBackgroundRadio, SIGNAL(clicked()), this, SLOT(activateColorBackground()));
	connect(imageBackgroundRadio, SIGNAL(clicked()), this, SLOT(activateImageBackground()));

	// Edit backround color
	connect(editColorButton, SIGNAL(clicked()), this, SLOT(editBackgroundColor()));

	// Predefined Size Change
	connect(comboBoxPredifinedSize, SIGNAL(currentIndexChanged(int)), this, SLOT(predefinedSizeChanged(int)));

	// Browse Background Image
	connect(browseImageButton, SIGNAL(clicked()), this, SLOT(BrowseBackgroundImage()));

	// The number of shots have been changed
	connect(nbrOfShots, SIGNAL(valueChanged(int)), this, SLOT(numberOfShotsChanged(int)));

	// The motion length have changed
	connect(motionLength, SIGNAL(valueChanged(int)), this, SLOT(motionLengthChange(int)));

	// Gravity Axis change
	connect(reverseCmd, SIGNAL(clicked()), this, SLOT(reverseRotation()));
	connect(gravityX, SIGNAL(clicked()), this, SLOT(gravityAxisChange()));
	connect(gravityY, SIGNAL(clicked()), this, SLOT(gravityAxisChange()));
	connect(gravityZ, SIGNAL(clicked()), this, SLOT(gravityAxisChange()));
	connect(gravityCam, SIGNAL(clicked()), this, SLOT(gravityAxisChange()));
}

MultiScreenshotsDialog::~MultiScreenshotsDialog()
{
	m_PredefinedSizes.clear();
}

//////////////////////////////////////////////////////////////////////
// Public Set function
//////////////////////////////////////////////////////////////////////

// Initialize the dialog
void MultiScreenshotsDialog::initScreenshotDialog(FileEntry fileEntry)
{
    // Init Preview OpenGl view
	m_pMultiShotsOpenglView= new MultiShotsOpenglView(this, m_pOpenglView);

    QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(m_pMultiShotsOpenglView->sizePolicy().hasHeightForWidth());
    m_pMultiShotsOpenglView->setSizePolicy(sizePolicy1);

    QLayout *pLayout = previewOpenglGroup->layout();
	if (NULL == pLayout)
	{
		pLayout= new QHBoxLayout;
		previewOpenglGroup->setLayout(pLayout);
	}
	pLayout->addWidget(m_pMultiShotsOpenglView);

	m_pMultiShotsOpenglView->initView();
	// Update the motion step
	m_pMultiShotsOpenglView->updateMotionStepAndLength(nbrOfShots->value(), motionLength->value());

	// Get The gravity axis
	m_RotationAxis= m_pMultiShotsOpenglView->currentGravityAxis();
	if (m_RotationAxis == glc::X_AXIS)
	{
		gravityX->setChecked(true);
	}
	else if (m_RotationAxis == glc::Y_AXIS)
	{
		gravityY->setChecked(true);
	}
	else if (m_RotationAxis == glc::Z_AXIS)
	{
		gravityZ->setChecked(true);
	}
	else
	{
		gravityCam->setChecked(true);
		m_RotationAxis= m_pOpenglView->viewportHandle()->cameraHandle()->upVector();
	}

	m_DefaultImageName= QFileInfo(fileEntry.getFileName()).baseName();
	baseShotName->setText(m_DefaultImageName);

	// Compute image aspect ratio
	const int width= spinBoxWidth->value();
	const int height= spinBoxHeight->value();
	m_ImageAspectRatio= static_cast<double>(width) / static_cast<double>(height);

	m_ScreenshotAspectRatio= m_pOpenglView->viewportHandle()->aspectRatio();

	// Update the preview
	// Choose right background
	if (BackGroundColor == m_BackGroundMode)
	{
		m_pMultiShotsOpenglView->viewport()->setBackgroundColor(m_CurrentBackgroundColor);
		m_pMultiShotsOpenglView->viewport()->deleteBackGroundImage();
	}
	else if ((BackGroundImage == m_BackGroundMode) && !m_CurrentBackgroundImageName.isEmpty())
	{
		m_pMultiShotsOpenglView->viewport()->loadBackGroundImage(m_CurrentBackgroundImageName);

	}
	else
	{
		m_pMultiShotsOpenglView->viewport()->loadBackGroundImage(":images/default_background.png");
	}

	updatePreview();
}

//////////////////////////////////////////////////////////////////////
// Public Get function
//////////////////////////////////////////////////////////////////////
//! Screenshot have to be saved
void MultiScreenshotsDialog::accept()
{
	// Get the folder in which image file must be saved
	QString pathName= QFileDialog::getExistingDirectory(this, tr("Select Destination directory"));
	if (pathName.isEmpty())
	{
		return;
	}
	else if(!QDir(pathName).exists())
	{
		return;
	}
	hide();

	// Delete the preview view
	delete m_pMultiShotsOpenglView;
	m_pMultiShotsOpenglView= NULL;

	m_pOpenglView->makeCurrent();
	// Test if framebuffer must be used
	bool useFrameBuffer= GLC_State::frameBufferSupported();

	QImage imageToSave;

	m_pOpenglView->setAutoBufferSwap(false);
	m_pOpenglView->setSnapShootMode(true);

	// Test if the background must be changed
	if ((m_BackGroundMode == BackGroundImage) && !m_CurrentBackgroundImageName.isEmpty())
	{
		// if m_CurrentBackgroundImageName is not empty, the file exist and is readable
		m_pOpenglView->viewportHandle()->loadBackGroundImage(m_CurrentBackgroundImageName);

	}
	else if (m_BackGroundMode == BackGroundColor)
	{
		m_pOpenglView->viewportHandle()->setBackgroundColor(m_CurrentBackgroundColor);
		m_pOpenglView->viewportHandle()->deleteBackGroundImage();
	}

	// Save the current camera
	GLC_Camera savCamera(*(m_pOpenglView->viewportHandle()->cameraHandle()));

	// Create Rotation Matrix
	const int numberOfShots= nbrOfShots->value();
	GLC_Matrix4x4 RotationMatrix(m_RotationAxis, 2.0 * glc::PI / static_cast<double>(numberOfShots));

	QString baseImageName(pathName + QDir::separator() + baseShotName->text());

	// Create progress dialog
	QProgressDialog progress(tr("Creating shots..."), tr("Cancel"), 0, numberOfShots, this);
	progress.setModal(true);
	progress.setMinimumDuration(1000);

	bool saveSucces= true;
	bool continu= true;
	if (useFrameBuffer)
	{
		// Create the framebuffer
		QGLFramebufferObjectFormat frameBufferFormat;
		frameBufferFormat.setSamples(m_pOpenglView->format().samples());
		frameBufferFormat.setAttachment(QGLFramebufferObject::Depth);
		// Create the framebuffer
		QGLFramebufferObject framebufferObject(m_TargetImageSize, frameBufferFormat);

		m_pOpenglView->viewportHandle()->setWinGLSize(framebufferObject.width(), framebufferObject.height());
		int i= 0;
		while( (i < numberOfShots) && saveSucces && continu)
		{

			framebufferObject.bind();
			m_pOpenglView->updateGL();
			//imageToSave= framebufferObject.toImage();
			if (imageToSave.isNull())
			{
				framebufferObject.release();
				break;
			}
			// Save the image
			const QString nameSuffix((QString("0000") + QString::number(i)).right(4));
			const QString nameOfImageToSave(baseImageName + nameSuffix + QString(".jpg"));
			saveSucces= imageToSave.save(nameOfImageToSave, "JPG", 100);
			// Move view camera
			m_pOpenglView->viewportHandle()->cameraHandle()->translate(- savCamera.target());
			m_pOpenglView->viewportHandle()->cameraHandle()->move(RotationMatrix);
			m_pOpenglView->viewportHandle()->cameraHandle()->translate(savCamera.target());

			// Release Frame buffer
			framebufferObject.release();

			// Update Progress dialog and chek fo cancellation
			progress.setValue(i);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
			if (progress.wasCanceled())
			{
				continu= false;
			}

			++i;
		}
		m_pOpenglView->viewportHandle()->setWinGLSize(m_pOpenglView->size().width(), m_pOpenglView->size().height());
	}
	if (imageToSave.isNull())
	{
		// Change view aspect ratio and
		m_pOpenglView->viewportHandle()->forceAspectRatio(m_ImageAspectRatio);

		int i= 0;
		while( (i < numberOfShots) && saveSucces && continu)
		{
			m_pOpenglView->updateGL();
			imageToSave=  m_pOpenglView->grabFrameBuffer();
			QSize shotSize= imageToSave.size();
			imageToSave= imageToSave.scaled(shotSize.height() * m_ImageAspectRatio, shotSize.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

			imageToSave= imageToSave.scaled(m_TargetImageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			// To match the exact size of image
			imageToSave= imageToSave.scaled(m_TargetImageSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

			// Save The Image
			const QString nameSuffix((QString("0000") + QString::number(i)).right(4));
			const QString nameOfImageToSave(baseImageName + nameSuffix + QString(".jpg"));
			saveSucces= imageToSave.save(nameOfImageToSave, "JPG", 100);

			//Move View Camera
			m_pOpenglView->viewportHandle()->cameraHandle()->translate(- savCamera.target());
			m_pOpenglView->viewportHandle()->cameraHandle()->move(RotationMatrix);
			m_pOpenglView->viewportHandle()->cameraHandle()->translate(savCamera.target());

			// Update Progress dialog and chek fo cancellation
			progress.setValue(i);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
			if (progress.wasCanceled())
			{
				continu= false;
			}

			++i;
		}
	}

	// Retore the view
	*(m_pOpenglView->viewportHandle()->cameraHandle())= savCamera;
	if (m_pOpenglView->isInShowState())
	{
		m_pOpenglView->setToVisibleState();
	}
	else
	{
		m_pOpenglView->setToInVisibleState();
	}

	m_pOpenglView->setAutoBufferSwap(true);
	m_pOpenglView->setSnapShootMode(false);
	m_pOpenglView->viewportHandle()->updateProjectionMat();
	m_pOpenglView->updateGL();

	if (!saveSucces)
	{
		QMessageBox::critical(this, QCoreApplication::applicationName(), tr("An error occur while trying to save image file"));
	}
	else if (!continu)
	{
		QMessageBox::warning(this, QCoreApplication::applicationName(), tr("Screenshots process canceled"));
	}
	QDialog::accept();
}

// Screenshot have not to be saved
void MultiScreenshotsDialog::reject()
{
	delete m_pMultiShotsOpenglView;
	m_pMultiShotsOpenglView= NULL;
	QDialog::reject();
}

//////////////////////////////////////////////////////////////////////
// Private Slot function
//////////////////////////////////////////////////////////////////////

// The size off the target image changed
void MultiScreenshotsDialog::targetImageSizeChanged()
{
	// Compute image aspect ratio
	m_TargetImageSize.setWidth(spinBoxWidth->value());
	m_TargetImageSize.setHeight(spinBoxHeight->value());
	m_ImageAspectRatio= static_cast<double>(m_TargetImageSize.width()) / static_cast<double>(m_TargetImageSize.height());

	// Update the preview image
	updatePreview();

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
void MultiScreenshotsDialog::activateDefaultBackground()
{
	if (m_BackGroundMode != BackGroundDefault)
	{
		editColorButton->setEnabled(false);
		browseImageButton->setEnabled(false);

		m_BackGroundMode= BackGroundDefault;

		// Update the preview
		m_pMultiShotsOpenglView->viewport()->loadBackGroundImage(":images/default_background.png");
		updatePreview();
	}
}

// Activate color background
void MultiScreenshotsDialog::activateColorBackground()
{
	if (m_BackGroundMode != BackGroundColor)
	{
		editColorButton->setEnabled(true);
		browseImageButton->setEnabled(false);

		m_BackGroundMode= BackGroundColor;
		// Update the preview image
		m_pMultiShotsOpenglView->viewport()->setBackgroundColor(m_CurrentBackgroundColor);
		m_pMultiShotsOpenglView->viewport()->deleteBackGroundImage();
		updatePreview();
	}

}

// Activate image background
void MultiScreenshotsDialog::activateImageBackground()
{
	if (m_BackGroundMode != BackGroundImage)
	{
		editColorButton->setEnabled(false);
		browseImageButton->setEnabled(true);

		m_BackGroundMode= BackGroundImage;
		if (!m_CurrentBackgroundImageName.isEmpty())
		{
			m_pMultiShotsOpenglView->viewport()->loadBackGroundImage(m_CurrentBackgroundImageName);
			// Update the preview image
			updatePreview();
		}

	}

}

// Edit background color
void MultiScreenshotsDialog::editBackgroundColor()
{
	QColor color = QColorDialog::getColor(m_CurrentBackgroundColor, this);
	if (color.isValid())
	{
		m_CurrentBackgroundColor= color;

		// Update the preview
		m_pMultiShotsOpenglView->viewport()->setBackgroundColor(m_CurrentBackgroundColor);
		m_pMultiShotsOpenglView->viewport()->deleteBackGroundImage();
		updatePreview();
	}
}

// Predefined size change
void MultiScreenshotsDialog::predefinedSizeChanged(int index)
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
void MultiScreenshotsDialog::BrowseBackgroundImage()
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
			// Update the preview
			m_pMultiShotsOpenglView->viewport()->loadBackGroundImage(m_CurrentBackgroundImageName);
			updatePreview();
		}
		else
		{
			QString message(tr("Unable to load image :") + QString("\n"));
			message+= imageFileName;
			QMessageBox::critical(this->parentWidget(), tr("Backround Image"), message);
		}
	}
}


// The number of shots have been changed
void MultiScreenshotsDialog::numberOfShotsChanged(int value)
{
	m_pMultiShotsOpenglView->updateMotionStepAndLength(value, motionLength->value());
}

// The motion length changed
void MultiScreenshotsDialog::motionLengthChange(int value)
{
	m_pMultiShotsOpenglView->updateMotionStepAndLength(nbrOfShots->value(), value);
}

// The gravity Axis Change
void MultiScreenshotsDialog::gravityAxisChange()
{
	if (gravityX->isChecked())
	{
		m_RotationAxis= glc::X_AXIS;
	}
	else if (gravityY->isChecked())
	{
		m_RotationAxis= glc::Y_AXIS;
	}
	else if (gravityZ->isChecked())
	{
		m_RotationAxis= glc::Z_AXIS;
	}
	else
	{
		m_RotationAxis= m_pOpenglView->viewportHandle()->cameraHandle()->upVector();
	}
	m_pMultiShotsOpenglView->updateGravityAxis(m_RotationAxis);
}

// Reverse rotation
void MultiScreenshotsDialog::reverseRotation()
{
	m_RotationAxis= -m_RotationAxis;
	m_pMultiShotsOpenglView->updateGravityAxis(m_RotationAxis);
}

//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////


// Update the preview image
void MultiScreenshotsDialog::updatePreview()
{
	QSize previewSize;
	if (m_ImageAspectRatio > (500.0 / 300.0))
	{
		previewSize.setWidth(500);
		previewSize.setHeight(static_cast<int>(static_cast<double>(500) / m_ImageAspectRatio));
	}
	else
	{
		previewSize.setWidth(m_ImageAspectRatio * 300);
		previewSize.setHeight(300);
	}
    m_pMultiShotsOpenglView->setMinimumSize(previewSize);
    m_pMultiShotsOpenglView->setMaximumSize(previewSize);

}

// Create predefined size list
void MultiScreenshotsDialog::createPredefinedSizeList()
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
