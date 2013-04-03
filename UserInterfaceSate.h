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

#ifndef USERINTERFACESATE_H_
#define USERINTERFACESATE_H_

// GLC_Player UI Mode
enum GlobalState
{
	NORMAL_STATE= 1, INSTANCE_STATE
};

class UserInterfaceSate
{
private:
	UserInterfaceSate();
public:
	~UserInterfaceSate();
//////////////////////////////////////////////////////////////////////
// Public Get Methods
//////////////////////////////////////////////////////////////////////
public:
	//! Get user interface global state
	inline static GlobalState globalState()
	{ return m_GlobalSate;}
	//! Get Album manager visibility
	inline static bool albumManagerIsVisible()
	{ return m_AlbumManagerVisibility;}
	//! Get Camera property visibility
	inline static bool cameraPropertyIsVisible()
	{ return m_CameraPropertyVisibility;}
	//! Get Selection property visibility
	inline static bool selectionPropertyIsVisible()
	{ return m_SelectionPropertyVisibility;}
	
//////////////////////////////////////////////////////////////////////
// Public Set Methods
//////////////////////////////////////////////////////////////////////
public:
	//! Set User interface global state
	inline static void SetGlobalState(const GlobalState state)
	{ m_GlobalSate= state;}
	//! Set Album manager visibility
	inline static void albumMangerVisibility(const bool vis)
	{ m_AlbumManagerVisibility= vis;}
	//! Set Camera property visibility
	inline static void cameraPropertyVisibility(const bool vis)
	{ m_CameraPropertyVisibility= vis;}
	//! Set Selection property visibility
	inline static void selectionPropertyVisibility(const bool vis)
	{ m_SelectionPropertyVisibility= vis;}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! Global User Interface state
	static GlobalState m_GlobalSate;
	//! Album Manager window visibility
	static bool m_AlbumManagerVisibility;
	//! Camera property visibility
	static bool m_CameraPropertyVisibility;
	//! Selection Property visibility
	static bool m_SelectionPropertyVisibility;
};

#endif /*USERINTERFACESATE_H_*/
