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

#ifndef CHOOSESHADERDIALOG_H_
#define CHOOSESHADERDIALOG_H_

#include "ui_ChooseShaderDialog.h"
#include "../opengl_view/OpenglView.h"

#include <QDialog>

class ChooseShaderDialog : public QDialog, private Ui::ChooseShaderDialog
{
	Q_OBJECT
public:
	ChooseShaderDialog(OpenglView*, QWidget *parent= NULL);
	virtual ~ChooseShaderDialog();
//////////////////////////////////////////////////////////////////////
// Public Get Functions
//////////////////////////////////////////////////////////////////////
public:
	//! get the shader Id
	GLuint shaderId() const;

	//! Get the shader Name
	QString shaderName() const;
//////////////////////////////////////////////////////////////////////
// Public Set Functions
//////////////////////////////////////////////////////////////////////
public:
	//! Update list thumbnails
	void UpdateThumbnailsList();

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////
private:

	//! Take a snapshoot
	QImage takeSnapShoot();
//////////////////////////////////////////////////////////////////////
// private member
//////////////////////////////////////////////////////////////////////
private:
	//! OpenGl view
	OpenglView* m_pOpenglView;

	//! List of usable shader
	ShaderList* m_pShaderList;

	//! Size of the icon in shader list
	QSize m_IconSize;


};

#endif /* CHOOSESHADERDIALOG_H_ */
