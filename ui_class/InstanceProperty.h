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

#ifndef INSTANCEPROPERTY_H_
#define INSTANCEPROPERTY_H_

#include "ui_InstanceProperty.h"
#include <GLC_Global>
#include <GLC_3DViewCollection>
#include <QDockWidget>
#include "ListOfMaterial.h"

class GLC_3DViewInstance;
class OpenglView;

class InstanceProperty : public QDockWidget, private Ui::InstanceProperty
{
	Q_OBJECT

//////////////////////////////////////////////////////////////////////
/*!@name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	InstanceProperty(OpenglView*, QAction*, QWidget *parent= 0);

	virtual ~InstanceProperty();
//@}

//////////////////////////////////////////////////////////////////////
/*!@name Get methods */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return The instance
	inline GLC_3DViewInstance* getInstance()
	{return m_pInstance;}

//@}

//////////////////////////////////////////////////////////////////////
/*!@name Set methods */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set the instance to view
	void setInstance(GLC_3DViewInstance*);

	//! Get List of material
	ListOfMaterial* getListOfMaterials();

	//! Create or update list of material
	inline void createOrUpdateListOfMaterial()
	{ m_pListOfMaterial->CreateOrUpdate(m_SubMaterialList);}

//@}

//////////////////////////////////////////////////////////////////////
// Signals
//////////////////////////////////////////////////////////////////////
signals:
	//! Done Instance property viualisation
	void doneSignal();

	//! The view must be updated
	void updateView();

	//! View sub material list
	void viewSubMaterialList();


//////////////////////////////////////////////////////////////////////
/*!@name Slots */
//@{
//////////////////////////////////////////////////////////////////////
public slots:
	//! Update windows fields
	void update();

private slots:
	//! Reverse selection normals
	void reverseNormals();

	//! Done instance property visualisation
	void doneSlot();

//@}


//////////////////////////////////////////////////////////////////////
/*!@name Private members */
//@{
//////////////////////////////////////////////////////////////////////
private:
	//! The View
	OpenglView* m_pOpenglView;

	//! Current Instance
	GLC_3DViewInstance* m_pInstance;

	//! Current Mesh material list
	QList<GLC_Material*> m_SubMaterialList;

	//! The model's list of material
	ListOfMaterial* m_pListOfMaterial;

//@}
};

#endif /*INSTANCEPROPERTY_H_*/
