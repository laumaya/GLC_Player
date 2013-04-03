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

#ifndef OPENALBUMOPTION_H_
#define OPENALBUMOPTION_H_

#include "ui_OpenAlbumOption.h"

#include <QDialog>

class OpenAlbumOption : public QDialog, private Ui::OpenAlbumOption
{
	Q_OBJECT
public:
	OpenAlbumOption(QWidget *parent);
	~OpenAlbumOption();
public:
	//! Return true if the current Album have to be saved
	inline bool currentAlbumHaveToBeSaved() const {return m_SaveCurrentAlbum;}

	//! Return true if the current album have to be replaced
	inline bool currentAlbumHaveToBeReplaced() const {return m_ReplaceCurrentAlbum;}

	//! Accept modifications
	void accept();

private slots:
	//!Update label
	void updateLabel();
//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! Current Album have to be saved
	bool m_SaveCurrentAlbum;

	//! Current Album have to be replaced
	bool m_ReplaceCurrentAlbum;

};

#endif /* OPENALBUMOPTION_H_ */
