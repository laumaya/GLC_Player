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

#include "SendFilesDialog.h"
#include "AlbumFile.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QProgressDialog>

SendFilesDialog::SendFilesDialog(FileEntryHash* pFileEntryHash, QWidget *parent)
: QDialog(parent)
, m_pFileEntryHash(pFileEntryHash)
, m_SentFilesSize(0.0)
, m_TargetPath(QDir::homePath())
, m_createSubFolder(false)
, m_AlbumFileName()
, m_copyAlbumFile(true)
, m_NewAlbumFileName()
{
	 setupUi(this);
	 // Setup Origin file list
	 QStringList headerLabel;
	 headerLabel << tr("File Name") << tr("Location");
	 originFilesList->setHeaderLabels(headerLabel);
	 destinationFilesList->setHeaderLabels(headerLabel);

	 targetPath->setText(m_TargetPath);
	 // Signal and Slot connection
	 connect(copyAlbumFile, SIGNAL(stateChanged(int)), this, SLOT(copyAlbumFileStateChanged(int)));
	 connect(albumName, SIGNAL(textChanged(const QString)), this, SLOT(albumNameTextChanged()));

	 connect(browsCmd, SIGNAL(clicked()), this, SLOT(browse()));
	 connect(sendAllCmd, SIGNAL(clicked()), this, SLOT(sendAllFiles()));
	 connect(sendSelectedCmd, SIGNAL(clicked()), this, SLOT(sendSelectedFile()));
	 connect(notSendAllCmd, SIGNAL(clicked()), this, SLOT(notSendAllFiles()));
	 connect(notSendSelectedCmd, SIGNAL(clicked()), this, SLOT(notSendSelectedFile()));

	 connect(originFilesList, SIGNAL(itemSelectionChanged()), this, SLOT(originSelectionChanged()));
	 connect(destinationFilesList, SIGNAL(itemSelectionChanged()), this, SLOT(destinationSelectionChanged()));

	 connect(targetPath, SIGNAL(textChanged(const QString)), this, SLOT(updateTargetFilePath()));

		//! Create sub folder state change
		void createFolderStateChanged(int);

	 connect(createSubFolder, SIGNAL(stateChanged(int)), this, SLOT(createFolderStateChanged(int)));

}

SendFilesDialog::~SendFilesDialog()
{
}

// Set source Files
void SendFilesDialog::updateView(const QString& albumFileName)
{
	m_AlbumFileName= albumFileName;
	m_SentFilesSize= 0.0;
	originFilesList->clear();
	destinationFilesList->clear();

	// Update Ui
	if (m_AlbumFileName.isEmpty())
	{
		updateCurrentAlbum->setEnabled(false);
		updateCurrentAlbum->setCheckState(Qt::Unchecked);
		copyAlbumFile->setEnabled(true);
		copyAlbumFile->setCheckState(Qt::Checked);

		albumName->setText(tr("New Album"));
		albumName->setEnabled(true);
		m_copyAlbumFile= true;
	}
	else
	{
		updateCurrentAlbum->setEnabled(true);
		updateCurrentAlbum->setCheckState(Qt::Unchecked);
		copyAlbumFile->setEnabled(true);
		copyAlbumFile->setCheckState(Qt::Checked);
		albumName->setText(QFileInfo(m_AlbumFileName).baseName());
		albumName->setEnabled(true);
		m_copyAlbumFile= true;
	}

	notSendSelectedCmd->setEnabled(false);
	sendSelectedCmd->setEnabled(false);
	sendAllCmd->setEnabled(true);
	notSendAllCmd->setEnabled(false);

	// flags to know if create sub folder must be enabled
	bool createFolderIsEnabled= false;
	FileEntryHash::iterator i= m_pFileEntryHash->begin();
	while (i != m_pFileEntryHash->constEnd())
	{
		QFileInfo currentFileInfo(i.value().getFileName());
		QStringList currentStringList;
		currentStringList << currentFileInfo.fileName();
		currentStringList << currentFileInfo.absolutePath();

		QTreeWidgetItem* pCurTreeWidgetItem= new QTreeWidgetItem(currentStringList);
		pCurTreeWidgetItem->setData(0, Qt::UserRole, QVariant(i.key()));
		pCurTreeWidgetItem->setData(1, Qt::UserRole, QVariant(i.value().getFileName()));

		// Check if this file have attached file
		if (!i.value().attachedFileNames().isEmpty())
		{
			// Update create sub folder flag
			createFolderIsEnabled= true;
			// Add Attached files as child
			QStringList attachedFileNamesList= i.value().attachedFileNames();
			const int size= attachedFileNamesList.size();
			for (int curChild= 0; curChild < size; ++curChild)
			{
				QFileInfo childFileInfo(attachedFileNamesList[curChild]);
				QStringList childStringList;
				childStringList << childFileInfo.fileName();
				childStringList << childFileInfo.absolutePath();
				QTreeWidgetItem* pChildTreeWidgetItem= new QTreeWidgetItem(childStringList);
				// Child Item must be unselectable
				pChildTreeWidgetItem->setFlags(Qt::NoItemFlags);
				pChildTreeWidgetItem->setData(1, Qt::UserRole, QVariant(attachedFileNamesList[curChild]));
				pCurTreeWidgetItem->addChild(pChildTreeWidgetItem);
			}
		}
		originFilesList->addTopLevelItem(pCurTreeWidgetItem);
		++i;
	}

	// Update create subfolder check box enabled status
	if (createFolderIsEnabled)
	{
		createSubFolder->setEnabled(true);
		createSubFolder->setCheckState(Qt::Checked);
	}
	else
	{
		createSubFolder->setEnabled(false);
		createSubFolder->setCheckState(Qt::Unchecked);
	}

	// Update UI
	totalDestinationSize->setText(sizeFromDoubleToString(m_SentFilesSize));

	originFilesList->resizeColumnToContents(0);
	originFilesList->sortItems(0, Qt::AscendingOrder);
	destinationFilesList->sortItems(0, Qt::AscendingOrder);
	destinationFilesList->setColumnWidth(0, originFilesList->columnWidth(0));
	updateTargetFilePath();
}

// Send the files
bool SendFilesDialog::sendFiles()
{
	// Check if the state permit to send files
	if (!buttonBox->button(QDialogButtonBox::Ok)->isEnabled()) return false;
	// The list of entry Id to update
	QList<GLC_uint> listOfEntryId;
	// The List of source files to send
	QList<QString> listOfFilesToSend;
	// The List of destination files
	QList<QString> listOfDestinationFile;

	// The Number of entry to update
	const int numberOfEntry= destinationFilesList->topLevelItemCount();
	// Fills list of entry and lists of files
	for (int i= 0; i < numberOfEntry; ++i)
	{
		QTreeWidgetItem* pTreeWidgetItem= destinationFilesList->topLevelItem(i);
		const GLC_uint currentId= pTreeWidgetItem->data(0, Qt::UserRole).toUInt();
		listOfEntryId << currentId;

		const QString sourceFileName= pTreeWidgetItem->data(1, Qt::UserRole).toString();
		listOfFilesToSend << sourceFileName;

		const QString targetFileName= pTreeWidgetItem->text(1) + QDir::separator() + pTreeWidgetItem->text(0);
		listOfDestinationFile << targetFileName;

		const int size= pTreeWidgetItem->childCount();
		// Fills the list with childs and create sub directory if needed
		if (size > 0)
		{
			// Create sub directory
			if (m_createSubFolder)
			{
				QDir entryDir(m_TargetPath);
				if (!entryDir.mkdir(QFileInfo(targetFileName).baseName()))
				{
					QString message(tr("Unable to create directory :") + QString("\n"));
					message+= m_TargetPath;
					QMessageBox::critical(this->parentWidget(), tr("Directory Creation"), message);
					return false;
				}
			}
			for (int j= 0; j < size; ++j)
			{
				QTreeWidgetItem* pChild= pTreeWidgetItem->child(j);

				const QString sourceChildFileName= pChild->data(1, Qt::UserRole).toString();
				listOfFilesToSend << sourceChildFileName;

				const QString targetChildFileName= pChild->text(1) + QDir::separator() + pChild->text(0);
				listOfDestinationFile << targetChildFileName;

				// Create attached file sub directory if necessary
				QDir entryBasePath(QFileInfo(targetFileName).absolutePath());
				QString AttachedFileRelativePath= entryBasePath.relativeFilePath(QFileInfo(targetChildFileName).absolutePath());
				if (!AttachedFileRelativePath.isEmpty())
				{
					entryBasePath.mkpath(AttachedFileRelativePath);
				}
			}
		}
	}
	const int numberOfFileToCopy= listOfFilesToSend.size();

	// Create progress dialog
	QProgressDialog progress(tr("Copying files..."), tr("Abort Copy"), 0, numberOfFileToCopy, this->parentWidget());
	progress.setModal(true);
	progress.setMinimumDuration(1000);

	Q_ASSERT(listOfFilesToSend.size() == listOfDestinationFile.size());
	// Check if source files exist
	for (int i= 0; i < numberOfFileToCopy; ++i)
	{
		if (!QFile::exists(listOfFilesToSend[i]))
		{
			progress.cancel();
			QString message(tr("Source file not found :") + QString("\n"));
			message+= listOfFilesToSend[i];
			QMessageBox::critical(this->parentWidget(), tr("Send Files"), message);
			return false;
		}
	}
	// All source files exists
	// Copying the Files
	for (int i= 0; i < numberOfFileToCopy; ++i)
	{
		// Update Progress dialog and chek fo cancellation
		progress.setValue(i);
		QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		if (progress.wasCanceled())
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
			progress.cancel();
			QString message(tr("Failed to copy file :") + QString("\n"));
			message+= listOfFilesToSend[i];
			message+= QString("\n") + QString("To") + QString("\n");
			message+= listOfDestinationFile[i];
			QMessageBox::critical(this->parentWidget(), tr("Send Files"), message);
			return false;
		}
	}
	// OK All files have been copied
	if (updateAlbum() || copyAlbum())
	{
		FileEntryHash* pNewFileEntryHash;
		if (updateAlbum())
		{
			pNewFileEntryHash= m_pFileEntryHash;
		}
		else
		{
			pNewFileEntryHash= new FileEntryHash(*m_pFileEntryHash);
		}
		// Update entries
		FileEntryHash::iterator iEntry= pNewFileEntryHash->begin();
		while (iEntry != pNewFileEntryHash->constEnd())
		{
			QString newFilePath;
			if (m_createSubFolder && (iEntry.value().numberOfAttachedFiles() > 0))
			{
				newFilePath= m_TargetPath + QDir::separator() + QFileInfo(iEntry.value().getFileName()).completeBaseName();
			}
			else
			{
				newFilePath= m_TargetPath;
			}
			QString newFileName= newFilePath + QDir::separator() + QFileInfo(iEntry.value().getFileName()).fileName();
			iEntry.value().updateFile(newFileName);
			++iEntry;
		}
		if (!updateAlbum())
		{
			QFile newAlbumFile(m_NewAlbumFileName);
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
				message+= m_NewAlbumFileName;
				QMessageBox::critical(this->parentWidget(), tr("Send Files"), message);
				return false;
			}

		}
	}

	return true;
}

// Browse for destination directory
void SendFilesDialog::browse()
{
	QString pathName= QFileDialog::getExistingDirectory(this, tr("Select Destination directory"), m_TargetPath);
	if (!pathName.isEmpty())
	{
		targetPath->setText(pathName);
	}
}

// Send All File
void SendFilesDialog::sendAllFiles()
{
	QTreeWidgetItem* pTreeWidgetItem;
	const int size= originFilesList->topLevelItemCount();
	double deltaSize= 0.0;
	for (int i= 0; i < size; ++i)
	{
		pTreeWidgetItem= originFilesList->takeTopLevelItem(0);
		GLC_uint modelId= pTreeWidgetItem->data(0, Qt::UserRole).toUInt();
		deltaSize+= static_cast<double>(QFileInfo(m_pFileEntryHash->value(modelId).getFileName()).size());
		deltaSize+= m_pFileEntryHash->value(modelId).attachedFilesSize();

		changeLocation(pTreeWidgetItem, m_TargetPath);
		destinationFilesList->addTopLevelItem(pTreeWidgetItem);
	}
	m_SentFilesSize+= deltaSize;

	// Update Ui
	totalDestinationSize->setText(sizeFromDoubleToString(m_SentFilesSize));

	notSendAllCmd->setEnabled(true);
	sendAllCmd->setEnabled(false);

	updateTargetFilePath();

}
// Not Send All Files
void SendFilesDialog::notSendAllFiles()
{
	QTreeWidgetItem* pTreeWidgetItem;
	const int size= destinationFilesList->topLevelItemCount();
	double deltaSize= 0.0;
	for (int i= 0; i < size; ++i)
	{
		pTreeWidgetItem= destinationFilesList->takeTopLevelItem(0);
		GLC_uint modelId= pTreeWidgetItem->data(0, Qt::UserRole).toUInt();
		deltaSize += static_cast<double>(QFileInfo(m_pFileEntryHash->value(modelId).getFileName()).size());
		deltaSize+= m_pFileEntryHash->value(modelId).attachedFilesSize();

		changeLocation(pTreeWidgetItem, QFileInfo(m_pFileEntryHash->value(modelId).getFileName()).absolutePath());
		originFilesList->addTopLevelItem(pTreeWidgetItem);
	}
	m_SentFilesSize+= -deltaSize;

	// Update Ui
	totalDestinationSize->setText(sizeFromDoubleToString(m_SentFilesSize));

	notSendAllCmd->setEnabled(false);
	sendAllCmd->setEnabled(true);

	updateTargetFilePath();
}

// Send Selected File
void SendFilesDialog::sendSelectedFile()
{
	QList<QTreeWidgetItem*> selectedTreeWidgetItem= originFilesList->selectedItems();
	if (!selectedTreeWidgetItem.isEmpty())
	{
		const int size= selectedTreeWidgetItem.size();
		double deltaSize= 0.0;
		for (int i= 0; i < size; ++i)
		{
			GLC_uint modelId= selectedTreeWidgetItem[i]->data(0, Qt::UserRole).toUInt();
			deltaSize+= static_cast<double>(QFileInfo(m_pFileEntryHash->value(modelId).getFileName()).size());
			deltaSize+= m_pFileEntryHash->value(modelId).attachedFilesSize();

			const int indexOfItemToTake= originFilesList->indexOfTopLevelItem(selectedTreeWidgetItem[i]);
			QTreeWidgetItem* pTreeWidgetItem= originFilesList->takeTopLevelItem(indexOfItemToTake);
			changeLocation(pTreeWidgetItem, m_TargetPath);
			destinationFilesList->addTopLevelItem(pTreeWidgetItem);
		}
		m_SentFilesSize+= deltaSize;

		// Update Ui
		totalDestinationSize->setText(sizeFromDoubleToString(m_SentFilesSize));

		if (0 == originFilesList->topLevelItemCount())
		{
			sendAllCmd->setEnabled(false);
		}
		if (!notSendAllCmd->isEnabled())
		{
			notSendAllCmd->setEnabled(true);
		}
		updateTargetFilePath();
	}

}
// Not Sent Selected File
void SendFilesDialog::notSendSelectedFile()
{
	QList<QTreeWidgetItem*> selectedTreeWidgetItem= destinationFilesList->selectedItems();
	if (!selectedTreeWidgetItem.isEmpty())
	{
		const int size= selectedTreeWidgetItem.size();
		double deltaSize= 0.0;
		for (int i= 0; i < size; ++i)
		{
			GLC_uint modelId= selectedTreeWidgetItem[i]->data(0, Qt::UserRole).toUInt();
			deltaSize+= static_cast<double>(QFileInfo(m_pFileEntryHash->value(modelId).getFileName()).size());
			deltaSize+= m_pFileEntryHash->value(modelId).attachedFilesSize();

			const int indexOfItemToTake= destinationFilesList->indexOfTopLevelItem(selectedTreeWidgetItem[i]);
			QTreeWidgetItem* pTreeWidgetItem= destinationFilesList->takeTopLevelItem(indexOfItemToTake);
			changeLocation(pTreeWidgetItem, QFileInfo(m_pFileEntryHash->value(modelId).getFileName()).absolutePath());
			originFilesList->addTopLevelItem(pTreeWidgetItem);
		}
		m_SentFilesSize+= -deltaSize;

		// Update Ui
		totalDestinationSize->setText(sizeFromDoubleToString(m_SentFilesSize));

		if (0 == destinationFilesList->topLevelItemCount())
		{
			notSendAllCmd->setEnabled(false);
		}
		if (!sendAllCmd->isEnabled())
		{
			sendAllCmd->setEnabled(true);
		}
		updateTargetFilePath();
	}

}

// The origin selection have been changed
void SendFilesDialog::originSelectionChanged()
{
	if (originFilesList->selectedItems().isEmpty())
	{
		sendSelectedCmd->setEnabled(false);
	}
	else
	{
		sendSelectedCmd->setEnabled(true);
	}
}

// The destination selection have been changed
void SendFilesDialog::destinationSelectionChanged()
{
	if (destinationFilesList->selectedItems().isEmpty())
	{
		notSendSelectedCmd->setEnabled(false);
	}
	else
	{
		notSendSelectedCmd->setEnabled(true);
	}
}

// Update Ok button State
void SendFilesDialog::updateTargetFilePath()
{
	const int destinationFileSize= destinationFilesList->topLevelItemCount();
	const bool fileToSendNotEmpty=  (0 != destinationFileSize);
	const bool targetDirectorySet= !targetPath->text().isEmpty();

	if (fileToSendNotEmpty && targetDirectorySet && QFileInfo(targetPath->text()).isDir() && (!m_copyAlbumFile || (m_copyAlbumFile && !albumName->text().isEmpty())))
	{
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
		m_TargetPath= QDir::cleanPath(targetPath->text());
		m_NewAlbumFileName= m_TargetPath + QDir::separator() + QFileInfo(albumName->text()).completeBaseName() + QChar('.') + AlbumFile::suffix();
		// Update Destination file target directory
		for (int i= 0; i < destinationFileSize; ++i)
		{
			QTreeWidgetItem* pTreeWidgetItem= destinationFilesList->topLevelItem(i);
			changeLocation(pTreeWidgetItem, m_TargetPath);
		}

	}
	else if (targetDirectorySet && QFileInfo(targetPath->text()).isDir() )
	{
		m_TargetPath= QDir::cleanPath(targetPath->text());
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	}
	else
	{
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	}
}

// Create sub folder state change
void SendFilesDialog::createFolderStateChanged(int state)
{
	if (state == Qt::Checked)
	{
		m_createSubFolder= true;
		updateTargetFilePath();
	}
	else
	{
		m_createSubFolder= false;
		updateTargetFilePath();
	}
}

// Copy album file state changed
void SendFilesDialog::copyAlbumFileStateChanged(int state)
{
	if (state == Qt::Checked)
	{
		m_copyAlbumFile= true;
		albumName->setEnabled(true);
	}
	else
	{
		m_copyAlbumFile= false;
		albumName->setEnabled(false);
	}
	updateTargetFilePath();

}

// The album name as changed
void SendFilesDialog::albumNameTextChanged()
{
	updateTargetFilePath();
}

//////////////////////////////////////////////////////////////////////
// private services function
//////////////////////////////////////////////////////////////////////
// Convert size from double to String
QString SendFilesDialog::sizeFromDoubleToString(const double& size)
{
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

// Change the location of an QTreeWidgetItem
void SendFilesDialog::changeLocation(QTreeWidgetItem* pWidget, QString newPath)
{
	const GLC_uint entryId= pWidget->data(0, Qt::UserRole).toUInt();
	FileEntryHash::iterator iEntry= m_pFileEntryHash->find(entryId);

	// Get entry attached file list
	QStringList attachedFilesList= iEntry.value().attachedFileNames();

	// Check if a sub folder must be created
	if (m_createSubFolder && (newPath != QFileInfo(iEntry.value().getFileName()).absolutePath()) && !attachedFilesList.isEmpty())
	{
		QString subFolderPath;
		subFolderPath= QFileInfo(iEntry.value().getFileName()).baseName();
		newPath= QDir::cleanPath(newPath + QDir::separator() + subFolderPath);
	}

	// Change Attached file location
	if (!attachedFilesList.isEmpty())
	{
		// Get Entry Base Dir
		const QDir baseDir(pWidget->text(1));
		// Get number of entry child
		const int size= pWidget->childCount();
		for (int i= 0; i < size; ++i)
		{
			QTreeWidgetItem* pChild= pWidget->child(i);
			// Get Child base Path
			const QString childAbsoluteFileName= pChild->text(1) + QDir::separator() + pChild->text(0);
			// Relative child Path
			const QString relativeChildPath= baseDir.relativeFilePath(childAbsoluteFileName);

			// Check if attached file path is different to entry file path
			if (relativeChildPath.isEmpty())
			{
				pChild->setText(1, newPath);
			}
			else
			{
				const QString newChildPath= QFileInfo(newPath + QDir::separator() + relativeChildPath).absolutePath();
				pChild->setText(1, newChildPath);
			}
		}
	}
	pWidget->setText(1, newPath);
}
