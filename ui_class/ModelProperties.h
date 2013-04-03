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

#ifndef MODELPROPERTIES_H_
#define MODELPROPERTIES_H_

#include <QDialog>
#include "ui_ModelProperties.h"
#include "FileEntry.h"

class ListOfMaterial;
class OpenglView;


class ModelProperties : public QDialog, private Ui::ModelProperties
{
	Q_OBJECT

public:
	ModelProperties(OpenglView*, const FileEntry&, QWidget*);
	virtual ~ModelProperties();

public:
	//! Set the file entry
	void setFileEntry(const FileEntry&);


private slots:
	//! Display the model's list of material
	void displayListOfMaterial(bool);
	//! Done
	void done();


//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////
private:
	//! Set values
	void setValues();
	//! Convert size from double to String
	QString sizeFromDoubleToString(const double&);

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The View
	OpenglView* m_pOpenglView;

	//! The File Entry
	FileEntry m_FileEntry;

	//! The model's list of material
	ListOfMaterial* m_pListOfMaterial;
};

#endif /* MODELPROPERTIES_H_ */
