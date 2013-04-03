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

#include "ExportToWeb.h"
#include "opengl_view/OpenglView.h"
#include "AlbumFile.h"
#include <QXmlStreamWriter>
#include <QFile>

ExportToWeb::ExportToWeb(QList<FileEntry>& fileEntrySortedList, OpenglView* pOpenglView)
: m_FileEntrySortedList(fileEntrySortedList)
, m_pOpenglView(pOpenglView)
, m_AlbumName()
, m_NumberOfRow(0)
, m_NumberOfLine(0)
, m_NumberOfPages(0)
, m_ThumbnailSize(10,10)
, m_ImageSize(10,10)
, m_ExportAlbumAndModel(false)
, m_ViewThumbnailModelName(true)
, m_LinkThumbnailToModel(false)
, m_ViewImageModelInformations(true)
, m_LinkImageToModel(true)
, m_TargetPath()
, m_pHtmlWriter(NULL)
, m_QProgressDialog(tr("Exporting to Web, Please Wait...  "), tr("Cancel"), 0, 10)
, m_CurrentProgressDialog(0)
, m_ThumbnailPathName("Thumbnails")
, m_ImagePathName("Images")
, m_HtmlImagePagePathName("Images_Pages")
, m_3dModelPathName("3dModels")
, m_BackgroundColor()
{

}

ExportToWeb::~ExportToWeb()
{
	delete m_pHtmlWriter;
}

//////////////////////////////////////////////////////////////////////
// Public Set function
//////////////////////////////////////////////////////////////////////
// Set thumbnail size
void ExportToWeb::setThumbnailSize(const QSize& size)
{
	m_ThumbnailSize.setWidth(size.width());
	m_ThumbnailSize.setHeight(size.height());
}

// Set Image size
void ExportToWeb::setImageSize(const QSize& size)
{
	m_ImageSize.setWidth(size.width());
	m_ImageSize.setHeight(size.height());
}

// Export Album to web
bool ExportToWeb::exportToWeb(const QString& targetPath, const QString& albumName)
{
	// Update progress dailog parameters
	int maxProgress= m_FileEntrySortedList.size();
	if (m_ExportAlbumAndModel)
	{
		maxProgress+= m_FileEntrySortedList.size();
	}
	if ((m_ExportAlbumAndModel && !m_LinkThumbnailToModel) || !m_ExportAlbumAndModel)
	{
		maxProgress+= m_FileEntrySortedList.size() * 2; // Image + web pages
	}

	m_QProgressDialog.setMaximum(maxProgress);
	m_QProgressDialog.setModal(true);
	m_QProgressDialog.setMinimumDuration(500);

	m_AlbumName= albumName;
	m_TargetPath= targetPath;
	// Create Sub-directories structure
	if (!createSubDirectories())
	{
		QString message(tr("Failed to create sub-folders"));
		message+= QString("\n") + tr("Folders already exists");
		message+= QString("\n") + tr("Or you do not have the correct permissions for the involved folders.");
		QMessageBox::critical(NULL, tr("Export to Web"), message);
		return false;
	}
	if (!makeSnapShots())
	{
		QString message(tr("Failed to make snapshots"));
		QMessageBox::critical(NULL, tr("Export to Web"), message);
		return false;
	}

	if (!exportAlbumAndModels())
	{
		return false;
	}

	if (!exportHtml())
	{
		return false;
	}


	m_QProgressDialog.cancel();
	return true;
}

//////////////////////////////////////////////////////////////////////
// Public Slot function
//////////////////////////////////////////////////////////////////////

// Export album and model
void ExportToWeb::exportAlbumAndModel(const bool exportAlbumAndModel)
{
	m_ExportAlbumAndModel= exportAlbumAndModel;
}

// View Thumbnail model name
void ExportToWeb::viewThumbnailModelName(bool view)
{
	m_ViewThumbnailModelName= view;
}

// Link Thumbnail to model
void ExportToWeb::linkThumbnailToModel(bool link)
{
	m_LinkThumbnailToModel= link;
}

// View Image model location
void ExportToWeb::viewImageModelInformations(bool view)
{
	m_ViewImageModelInformations= view;
}

// Link Image to model
void ExportToWeb::linkImageToModel(bool link)
{
	m_LinkImageToModel= link;
}

//////////////////////////////////////////////////////////////////////
// private services function
//////////////////////////////////////////////////////////////////////

// Create sub directories structure
bool ExportToWeb::createSubDirectories()
{
	bool result= true;

	Q_ASSERT(QFileInfo(m_TargetPath).isDir());
	QDir baseDir(m_TargetPath);
	// Create thumbnail subDir
	result= result && baseDir.mkdir(m_ThumbnailPathName);

	// Test if images must be exported
	if (!(m_ExportAlbumAndModel && m_LinkThumbnailToModel))
	{
		result= result && baseDir.mkdir(m_ImagePathName);
		result= result && baseDir.mkdir(m_HtmlImagePagePathName);
	}

	// Test if album and images must be exported
	if (m_ExportAlbumAndModel)
	{
		result= result && baseDir.mkdir(m_3dModelPathName);
	}

	return result;
}

// Make SnapShots
bool ExportToWeb::makeSnapShots()
{
	// Set thumbnail dir

	bool result= true;

	const QString thumbnailPathName(m_TargetPath + QDir::separator() + m_ThumbnailPathName + QDir::separator());
	const QString imagePathName(m_TargetPath + QDir::separator() + m_ImagePathName + QDir::separator());

	const bool saveImage= (m_ExportAlbumAndModel && !m_LinkThumbnailToModel) || !m_ExportAlbumAndModel;

	// Save Current world and view properties
	GLC_Camera savCam= m_pOpenglView->getCamera();
	double savAngle= m_pOpenglView->getViewAngle();
	GLC_World savWorld(m_pOpenglView->getWorld());

	// Go to snapshot mode
	m_pOpenglView->captureMode(m_ThumbnailSize, true, m_BackgroundColor);
	m_QProgressDialog.setLabelText(tr("Creating thumbnails, Please Wait...  "));
	// Save thumbnails
	QList<FileEntry>::iterator iEntry= m_FileEntrySortedList.begin();
	int currentEntryIndex= 0;
	while (iEntry != m_FileEntrySortedList.constEnd())
	{
		// Update Progress dialog and chek fo cancellation
		m_QProgressDialog.setValue(++m_CurrentProgressDialog);
		QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		if (m_QProgressDialog.wasCanceled())
		{
			return false;
		}

		// Load current entry world into the view
		m_pOpenglView->clear();
		GLC_World newWorld((*iEntry).getWorld());
		m_pOpenglView->add(newWorld);

		// Set view camera and view angle
		m_pOpenglView->setCameraAndAngle((*iEntry).getCamera(), (*iEntry).getViewAngle());

		// Take ScreenShot
		QImage currentImage(m_pOpenglView->takeScreenshot());

		// Built thumbnail file name
		QString currentImageFileName= thumbnailPathName + createImageName(currentEntryIndex);
		result= result && currentImage.save(currentImageFileName, "JPG", 100);

		++currentEntryIndex;
		++iEntry;
	}
	if (saveImage)
	{
		m_QProgressDialog.setLabelText(tr("Creating images, Please Wait...  "));
		m_pOpenglView->captureMode(m_ImageSize, false, m_BackgroundColor);
		iEntry= m_FileEntrySortedList.begin();
		currentEntryIndex= 0;

		while (iEntry != m_FileEntrySortedList.constEnd())
		{
			// Update Progress dialog and chek fo cancellation
			m_QProgressDialog.setValue(++m_CurrentProgressDialog);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
			if (m_QProgressDialog.wasCanceled())
			{
				return false;
			}

			// Load current entry world into the view
			m_pOpenglView->clear();
			GLC_World newWorld((*iEntry).getWorld());
			m_pOpenglView->add(newWorld);

			// Set view camera and view angle
			m_pOpenglView->setCameraAndAngle((*iEntry).getCamera(), (*iEntry).getViewAngle());

			// Take ScreenShot
			QImage currentImage(m_pOpenglView->takeScreenshot());

			// Built Image file name
			QString currentImageFileName= imagePathName + createImageName(currentEntryIndex);

			result= result && currentImage.save(currentImageFileName, "JPG", 100);

			++currentEntryIndex;
			++iEntry;
		}
	}

	// Retore view world
	m_pOpenglView->clear();
	m_pOpenglView->add(savWorld);
	m_pOpenglView->setCameraAndAngle(savCam, savAngle);

	// Return to normal mode
	m_pOpenglView->normalMode();

	return result;
}

// Export Album and model
bool ExportToWeb::exportAlbumAndModels()
{
	bool result= true;
	if (m_ExportAlbumAndModel)
	{
		const QString targetPath(m_TargetPath + QDir::separator() + m_3dModelPathName);
		// The List of source files to send
		QList<QString> listOfFilesToSend;
		// The List of destination files
		QList<QString> listOfDestinationFile;

		// Fills list of entry and lists of files
		QList<FileEntry>::iterator iEntry= m_FileEntrySortedList.begin();
		while (iEntry != m_FileEntrySortedList.constEnd())
		{
			const QString entryFileName((*iEntry).getFileName());
			listOfFilesToSend << entryFileName;
			// Get entry attached file list
			QStringList attachedFilesList= (*iEntry).attachedFileNames();

			// Change Attached file location
			if (attachedFilesList.isEmpty())
			{
				// Add the 3D Model file to the list of destination file
				listOfDestinationFile << QString(targetPath + QDir::separator() + QFileInfo(entryFileName).fileName());
			}
			else
			{
				// Get Entry Base Dir
				const QDir baseDir(targetPath);

				// Create new Base Path
				const QString newBasePath(targetPath + QDir::separator() + QFileInfo(entryFileName).baseName());
				if (!baseDir.mkdir(QFileInfo(entryFileName).baseName()))
				{
					QString message(tr("Unable to create directory :") + QString("\n"));
					message+= newBasePath;
					QMessageBox::critical(NULL, tr("Directory Creation"), message);
					return false;
				}

				// Add the 3D Model file to the list of destination file
				listOfDestinationFile << QString(newBasePath + QDir::separator() + QFileInfo(entryFileName).fileName());

				// New Base Dir for attached files
				const QDir newBaseDir(newBasePath);

				const QDir originBaseDir(QFileInfo(entryFileName).absolutePath());

				// Get the entry attached files list
				QStringList attachedFilesList= (*iEntry).attachedFileNames();
				listOfFilesToSend << attachedFilesList;

				// Get number of childs
				const int size= attachedFilesList.size();
				for (int i= 0; i < size; ++i)
				{
					// Child absolute file name
					const QString childAbsoluteFileName= attachedFilesList[i];
					//Child relative file name
					const QString relativeChildPath= originBaseDir.relativeFilePath(childAbsoluteFileName);

					// Check if attached file path is different to entry file path
					QString targetFileName;
					if (!relativeChildPath.isEmpty())
					{
						QString newPath= QFileInfo(newBasePath + QDir::separator() + relativeChildPath).absolutePath();
						newBaseDir.mkpath(newPath);
						targetFileName= newPath + QDir::separator() + QFileInfo(attachedFilesList[i]).fileName();
					}
					else
					{
						targetFileName= newBasePath + QDir::separator() + QFileInfo(attachedFilesList[i]).fileName();
					}

					// Add the attached file to list of destination file
					listOfDestinationFile << targetFileName;
				}
			}

			++iEntry;
		}
		const int numberOfFileToCopy= listOfFilesToSend.size();

		Q_ASSERT(listOfFilesToSend.size() == listOfDestinationFile.size());
		// Check if source files exist
		for (int i= 0; i < numberOfFileToCopy; ++i)
		{
			if (!QFile::exists(listOfFilesToSend[i]))
			{
				m_QProgressDialog.cancel();
				QString message(tr("Source file not found :") + QString("\n"));
				message+= listOfFilesToSend[i];
				QMessageBox::critical(NULL, tr("Send Files"), message);
				return false;
			}
		}
		// All source files exists
		// Copying the Files
		m_QProgressDialog.setLabelText(tr("Copying Files, Please Wait...  "));
		for (int i= 0; i < numberOfFileToCopy; ++i)
		{
			// Update Progress dialog and chek fo cancellation
			m_QProgressDialog.setValue(++m_CurrentProgressDialog);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
			if (m_QProgressDialog.wasCanceled())
			{
				return false;
			}

			const QString targetFile= listOfDestinationFile[i];
			// Test if the destination file already exist
			if (QFile::exists(listOfDestinationFile[i]))
			{
				// The destination file exists, delete it
				QFile::remove(listOfDestinationFile[i]);
			}
			if (!QFile::copy(listOfFilesToSend[i], listOfDestinationFile[i]))
			{
				m_QProgressDialog.cancel();
				QString message(tr("Failed to copy file :") + QString("\n"));
				message+= listOfFilesToSend[i];
				message+= QString("\n") + QString("To") + QString("\n");
				message+= listOfDestinationFile[i];
				QMessageBox::critical(NULL, tr("Send Files"), message);
				return false;
			}
		}

		// Create New FileEntry Hash table
		FileEntryHash* pNewFileEntryHash= new FileEntryHash();

		iEntry= m_FileEntrySortedList.begin();
		while (iEntry != m_FileEntrySortedList.constEnd())
		{
			pNewFileEntryHash->insert((*iEntry).id(), *iEntry);
			iEntry++;
		}


		// Update entries
		FileEntryHash::iterator iEntryHash= pNewFileEntryHash->begin();
		while (iEntryHash != pNewFileEntryHash->constEnd())
		{
			QString newFilePath;
			if (iEntryHash.value().numberOfAttachedFiles() > 0)
			{
				newFilePath= targetPath + QDir::separator() + QFileInfo(iEntryHash.value().getFileName()).completeBaseName();
			}
			else
			{
				newFilePath= targetPath;
			}
			QString newFileName= newFilePath + QDir::separator() + QFileInfo(iEntryHash.value().getFileName()).fileName();
			iEntryHash.value().updateFile(newFileName);
			++iEntryHash;
		}

		// Save new album
		QFile newAlbumFile(targetPath + QDir::separator() + m_AlbumName + QString(".album"));
		qDebug() << newAlbumFile.fileName();
		if (newAlbumFile.open(QIODevice::WriteOnly))
		{
			newAlbumFile.close();
			AlbumFile newAlbum;
			newAlbum.saveAlbumFile(pNewFileEntryHash->values(), &newAlbumFile);
			pNewFileEntryHash->clear();
			delete pNewFileEntryHash;
		}
		else
		{
			delete pNewFileEntryHash;
			QString message(tr("Failed to create Album :") + QString("\n"));
			message+= newAlbumFile.fileName();
			QMessageBox::critical(NULL, tr("Send Files"), message);
			return false;
		}

	}
	return result;
}

// Export Html
bool ExportToWeb::exportHtml()
{
	if (!m_ExportAlbumAndModel || !m_LinkThumbnailToModel)
	{
		// write the images html pages
		writeHtmlImagesPages();
	}

	QString fileName;
	QString pageName;
	for (int i= 0; i < m_NumberOfPages; ++i)
	{

		// Current page File Name
		fileName= createFileName(i);
		QString fullFileName(m_TargetPath + QDir::separator() + fileName + ".html");

		// Current html file
		QFile htmlFile(fullFileName);
		htmlFile.open(QIODevice::WriteOnly);

		m_pHtmlWriter= new QXmlStreamWriter(&htmlFile);
		//m_pHtmlWriter->setCodec("ISO 8859-1");
		m_pHtmlWriter->setAutoFormatting(true);
		// Begin to write the html document
		m_pHtmlWriter->writeStartElement("html");

		if (i == 0) pageName= m_AlbumName;
		else pageName= fileName;
		// Write the header
		writeHtmlHeader(pageName);
		// Write the Body
		m_pHtmlWriter->writeStartElement("body");

		// Write the title
		writeHtmlTitle(m_AlbumName);

		if (m_NumberOfPages > 1)
		{
			// Write pages link
			writeHtmlPageLink(i);
		}

		// Write the table
		writeHtmlTable(i);

		m_pHtmlWriter->writeEndElement(); // body
		m_pHtmlWriter->writeEndElement(); // html

		m_pHtmlWriter->writeEndDocument();
		htmlFile.close();
	}

	delete m_pHtmlWriter;
	m_pHtmlWriter= NULL;
	return true;
}

// Write html header
void ExportToWeb::writeHtmlHeader(const QString& pageName)
{
	m_pHtmlWriter->writeStartElement("head");
	m_pHtmlWriter->writeStartElement("meta");
	m_pHtmlWriter->writeAttribute("http-equiv", "Content-Type");
	m_pHtmlWriter->writeAttribute("content", "text/html; charset=UTF-8");
	m_pHtmlWriter->writeEndElement(); // meta
	m_pHtmlWriter->writeStartElement("title");
	m_pHtmlWriter->writeCharacters(pageName);
	m_pHtmlWriter->writeEndElement(); // title
	m_pHtmlWriter->writeEndElement(); // head
}

// Write html title
void ExportToWeb::writeHtmlTitle(const QString& title)
{
	m_pHtmlWriter->writeStartElement("h2");
	m_pHtmlWriter->writeAttribute("align", "center");
	m_pHtmlWriter->writeCharacters(title);
	m_pHtmlWriter->writeEndElement(); // h2
}

// Write html page link
void ExportToWeb::writeHtmlPageLink(int currentPage)
{
	for (int i= 0; i < m_NumberOfPages; ++i)
	{
		const QString currentFileName(QString("Page ") + QString::number(i + 1));

		if (i == currentPage)
		{
			// Write the current page name
			m_pHtmlWriter->writeCharacters(currentFileName);
		}
		else
		{
			// Write a link to the specified file
			m_pHtmlWriter->writeStartElement("a");
			m_pHtmlWriter->writeAttribute("href", createFileName(i) + QString(".html"));
			m_pHtmlWriter->writeCharacters(currentFileName);
			m_pHtmlWriter->writeEndElement(); // a
		}

		if (i < (m_NumberOfPages - 1))
		{
			// Print separtor
			m_pHtmlWriter->writeCharacters(" | ");
		}

	}
}

// Return fileName
QString ExportToWeb::createFileName(int index)
{
	QString fileName;
	const QString fileNamePrefix(QString("000000000000").left(QString::number(m_FileEntrySortedList.size()).length()));
	if (index == 0)
	{
		fileName= "index";
	}
	else
	{
		fileName= m_AlbumName + QString(fileNamePrefix + QString::number(index)).right(fileNamePrefix.length());
	}
	return fileName;
}

// Write the table
void ExportToWeb::writeHtmlTable(int page)
{

	m_pHtmlWriter->writeStartElement("p");
	m_pHtmlWriter->writeStartElement("table");
	m_pHtmlWriter->writeAttribute("cellspacing", "2");
	m_pHtmlWriter->writeAttribute("cellpadding", "2");
	m_pHtmlWriter->writeAttribute("width", "100%");
	// compute the number of line to write
	const int totalNbrOfCell= m_FileEntrySortedList.size();
	const int maxNbrOfCellPearPage= m_NumberOfRow * m_NumberOfLine;
	int cellIndex= 0;
	if (page > 0)
	{
		cellIndex= maxNbrOfCellPearPage * page;
	}
	int nbrOfLineToWrite;
	if (page == (m_NumberOfPages - 1))
	{
		const int nbrOfCellToWrite= totalNbrOfCell - cellIndex;
		nbrOfLineToWrite= static_cast<int>(ceil(static_cast<double>(nbrOfCellToWrite) / static_cast<double>(m_NumberOfRow)));
	}
	else
	{
		nbrOfLineToWrite= m_NumberOfLine;
	}

	for (int currentLine= 0; currentLine < nbrOfLineToWrite; ++currentLine)
	{
		m_pHtmlWriter->writeStartElement("tr");
		int currentRowIndex= 0;
		while ((currentRowIndex < m_NumberOfRow))// and (cellIndex < totalNbrOfCell))
		{
			writeHtmlCell(cellIndex);
			++currentRowIndex;
			++cellIndex;
		}
		m_pHtmlWriter->writeEndElement(); // tr
	}

	m_pHtmlWriter->writeEndElement(); // table
	m_pHtmlWriter->writeEndElement(); // p
}

// Write a cell
void ExportToWeb::writeHtmlCell(int cellIndex)
{
	m_pHtmlWriter->writeStartElement("td");
	m_pHtmlWriter->writeAttribute("align", "center");
	const int cellWidth= static_cast<int>(ceil(100.0/ static_cast<double>(m_NumberOfRow)));
	m_pHtmlWriter->writeAttribute("width", QString(QString::number(cellWidth) + QString("%")));
	if (cellIndex < m_FileEntrySortedList.size())
	{
		const QString imageName(createImageName(cellIndex));
		QString thumbnailLink;
		if (m_ExportAlbumAndModel && m_LinkThumbnailToModel)
		{
			thumbnailLink= createModelName(cellIndex);
		}
		else
		{
			thumbnailLink= m_HtmlImagePagePathName + QDir::separator() + createImagePageFileName(cellIndex);
		}
		m_pHtmlWriter->writeStartElement("a");
		m_pHtmlWriter->writeAttribute("href", thumbnailLink);
		m_pHtmlWriter->writeStartElement("img");
		m_pHtmlWriter->writeAttribute("border", "0");
		const QString thumbnailUrl(m_ThumbnailPathName + QDir::separator() + imageName);
		m_pHtmlWriter->writeAttribute("alt", thumbnailUrl);
		m_pHtmlWriter->writeAttribute("src", thumbnailUrl);
		m_pHtmlWriter->writeEndElement(); // img
		m_pHtmlWriter->writeEndElement(); // a
		if (m_ViewThumbnailModelName)
		{
			m_pHtmlWriter->writeStartElement("br");
			m_pHtmlWriter->writeCharacters(QFileInfo(m_FileEntrySortedList[cellIndex].getFileName()).fileName());
			m_pHtmlWriter->writeEndElement(); // br
		}
	}
	m_pHtmlWriter->writeEndElement(); // td
}

// Create the image name
QString ExportToWeb::createImageName(int index)
{
	const QString imageExt(".jpg");
	const QString baseNumber(QString("000000000000").left(QString::number(m_FileEntrySortedList.size()).length()));
	const int baseShift(baseNumber.length());

	return QString(baseNumber + QString::number(index)).right(baseShift) + imageExt;
}

// Create model name
QString ExportToWeb::createModelName(int index)
{
	FileEntry currentEntry= m_FileEntrySortedList[index];
	QString modelName= QFileInfo(currentEntry.getFileName()).fileName();
	if (currentEntry.numberOfAttachedFiles() > 0)
	{
		modelName= m_3dModelPathName + QDir::separator() + QFileInfo(modelName).completeBaseName() + QDir::separator() + modelName;
	}
	else
	{
		modelName= m_3dModelPathName + QDir::separator() + modelName;
	}
	return QFileInfo(modelName).filePath();
}

// Create image page file name
QString ExportToWeb::createImagePageFileName(int index)
{
	const QString pageExt(".html");
	const QString baseNumber(QString("000000000000").left(QString::number(m_FileEntrySortedList.size()).length()));
	const int baseShift(baseNumber.length());

	return QString(baseNumber + QString::number(index)).right(baseShift) + pageExt;
}

// Write html images pages
bool ExportToWeb::writeHtmlImagesPages()
{
	const int max= m_FileEntrySortedList.size();
	m_QProgressDialog.setLabelText(tr("Creating html images pages, Please Wait...  "));
	for (int i= 0; i < max; ++i)
	{
		// Update Progress dialog and chek fo cancellation
		m_QProgressDialog.setValue(++m_CurrentProgressDialog);
		QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		if (m_QProgressDialog.wasCanceled())
		{
			return false;
		}

		// Current page File Name
		const QString fileName= createImagePageFileName(i);
		QString fullFileName(QFileInfo(m_TargetPath + QDir::separator() + m_HtmlImagePagePathName + QDir::separator() + fileName).filePath());

		// Current html file
		QFile htmlFile(fullFileName);
		htmlFile.open(QIODevice::WriteOnly);

		m_pHtmlWriter= new QXmlStreamWriter(&htmlFile);
		//m_pHtmlWriter->setCodec("ISO 8859-1");
		m_pHtmlWriter->setAutoFormatting(true);
		// Begin to write the html document
		m_pHtmlWriter->writeStartElement("html");

		// Write the header
		writeHtmlHeader(fileName);
		// Write the Body
		m_pHtmlWriter->writeStartElement("body");

		// Write the title
		const QString imageName(createImageName(i));

		writeHtmlTitle(QString("Image ") + imageName);

		// Test if there is a link to model
		if (m_ExportAlbumAndModel && m_LinkImageToModel)
		{
			m_pHtmlWriter->writeStartElement("a");
			m_pHtmlWriter->writeAttribute("href", QFileInfo(QString("..") + QDir::separator() + createModelName(i)).filePath());
		}
		// Add the image
		m_pHtmlWriter->writeStartElement("img");
		m_pHtmlWriter->writeAttribute("border", "0");
		const QString imageUrl(QFileInfo(QString("..") + QDir::separator() + m_ImagePathName + QDir::separator() + imageName).filePath());
		m_pHtmlWriter->writeAttribute("alt", imageUrl);
		m_pHtmlWriter->writeAttribute("src", imageUrl);
		m_pHtmlWriter->writeEndElement(); // img

		// Test if there is a link to model
		if (m_ExportAlbumAndModel && m_LinkImageToModel)
		{
			m_pHtmlWriter->writeEndElement(); // a
		}

		if (m_ViewImageModelInformations)
		{
			// Create table
			m_pHtmlWriter->writeStartElement("table");
			m_pHtmlWriter->writeAttribute("cellspacing", "2");
			m_pHtmlWriter->writeAttribute("cellpadding", "2");

			m_pHtmlWriter->writeAttribute("width", "700");

			// Write image model name
			const QString modelName(QFileInfo(m_FileEntrySortedList[i].getFileName()).fileName());
			writeHtmlInformationCell(tr("Name"), modelName);

			// Write Model Size
			FileEntry currentEntry= m_FileEntrySortedList[i];
			double fileSize= static_cast<double>(QFileInfo(currentEntry.getFileName()).size());
			QString stringSize;
			if (fileSize > 1024 * 1024)
			{
				stringSize= QString::number(fileSize / (1024 * 1024), 'f', 2) + tr(" Mo");
			}
			else if (fileSize > (1024))
			{
				stringSize= QString::number(fileSize / (1024), 'f', 2) + tr(" Ko");
			}
			else
			{
				stringSize= QString::number(fileSize, 'f', 2) + tr(" Bytes");
			}
			writeHtmlInformationCell("Size", stringSize);

			// Write Number of Instances
			writeHtmlInformationCell(tr("Object(s)"), QString::number(currentEntry.getNumberOfInstances()));

			// Write Number of vertexs
			writeHtmlInformationCell(tr("Vertex(s)"), QString::number(currentEntry.getNumberOfVertexs()));

			// Write Number of Faces
			writeHtmlInformationCell(tr("Face(s)"), QString::number(currentEntry.getNumberOfFaces()));

			// Write Number of Materials
			writeHtmlInformationCell(tr("Materials(s)"), QString::number(currentEntry.numberOfMaterials()));

			m_pHtmlWriter->writeEndElement(); // table
		}

		m_pHtmlWriter->writeEndElement(); // body
		m_pHtmlWriter->writeEndElement(); // html

		m_pHtmlWriter->writeEndDocument();
		htmlFile.close();
	}

	delete m_pHtmlWriter;
	m_pHtmlWriter= NULL;
	return true;
}
// Write html infomration cell
void ExportToWeb::writeHtmlInformationCell(const QString& title, const QString& text)
{
	m_pHtmlWriter->writeStartElement("tr");

	m_pHtmlWriter->writeStartElement("td");
	m_pHtmlWriter->writeAttribute("width", "100");
	m_pHtmlWriter->writeCharacters(title);
	m_pHtmlWriter->writeEndElement(); // td

	m_pHtmlWriter->writeStartElement("td");
	m_pHtmlWriter->writeCharacters(text);
	m_pHtmlWriter->writeEndElement(); // td

	m_pHtmlWriter->writeEndElement(); // tr
}


