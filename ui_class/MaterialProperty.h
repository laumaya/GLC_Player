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

#ifndef MATERIALPROPERTY_H_
#define MATERIALPROPERTY_H_

#include "ui_MaterialProperty.h"
#include "../opengl_view/MaterialOpenglView.h"

#include <QDockWidget>
#include <GLC_World>
#include <GLC_Camera>


class GLC_Material;
class QGLContext;
class OpenglView;
class GLC_Geometry;

class MaterialProperty : public QDockWidget, private Ui::MaterialProperty
{
	Q_OBJECT
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
//! Default constructor
	MaterialProperty(OpenglView*, GLC_Material*, QWidget *parent= 0);

//@}

public:
	//! Set material to edit
	void setMaterial(GLC_Material*);

signals:
	//! The material as been updated
	void materialUpdated(GLC_Material*);

//////////////////////////////////////////////////////////////////////
/*! \name Private slots Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private slots:
	//! Change Ambient color
	void changeAmbientColor();
	//! Change Diffuse color
	void changeDiffuseColor();
	//! Change Specular color
	void changeSpecularColor();
	//! Change Emissive color
	void changeEmissiveColor();
	//! Set the texture file name
	void setTextureFileName();
	//! The texture filename as changed
	void textureFileNameChanged(const QString&);
	//! Activate desativate texture
	void textureState(int);
	//! Apply modifications
	void apply();
	//! Cancel modification
	void cancel();
	//!Enable command button
	void enableCmd();
	//! Opacity Change
	void opacityChange(int);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Private services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:
	//! init background dialog values
	void initValues();
	//! init material representatio world
	void initWorld();

//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	OpenglView* m_pOpenglView;

	GLC_Material* m_pMaterial;
	GLC_Material* m_pPreviewMaterial;

	QColor m_AmbientColor;
	QColor m_DiffuseColor;
	QColor m_SpecularColor;
	QColor m_EmissiveColor;

	//! The material openglView
	MaterialOpenglView* m_pMaterialOpenglView;

	//! The material Alpha
	double m_Alpha;


};

#endif /*MATERIALPROPERTY_H_*/
