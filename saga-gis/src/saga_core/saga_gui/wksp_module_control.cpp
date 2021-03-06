/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                WKSP_Module_Control.cpp                //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/image.h>
#include <wx/imaglist.h>

#include "res_controls.h"
#include "res_images.h"

#include "helper.h"

#include "wksp_module_control.h"
#include "wksp_module_manager.h"
#include "wksp_module_library.h"
#include "wksp_module_menu.h"
#include "wksp_module.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	IMG_MANAGER		= 1,
	IMG_GROUP,
	IMG_LIBRARY,
	IMG_CHAIN,
	IMG_MODULE
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CWKSP_Module_Control, CWKSP_Base_Control)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CWKSP_Module_Control, CWKSP_Base_Control)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Module_Control	*g_pModule_Ctrl	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Module_Control::CWKSP_Module_Control(wxWindow *pParent)
	: CWKSP_Base_Control(pParent, ID_WND_WKSP_MODULES)
{
	g_pModule_Ctrl	= this;

	//-----------------------------------------------------
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_MODULE_MANAGER)
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_MODULE_GROUP);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_MODULE_LIBRARY);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_MODULE_CHAIN);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_MODULE);

	//-----------------------------------------------------
	_Set_Manager(new CWKSP_Module_Manager);

	Get_Manager()->Initialise();
}

//---------------------------------------------------------
CWKSP_Module_Control::~CWKSP_Module_Control(void)
{
//	Get_Manager()->Finalise();

	g_pModule_Ctrl	= NULL;

	_Del_Item(m_pManager, true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Module_Control::On_Execute(wxCommandEvent &event)
{
	Get_Manager()->On_Execute(event);
}

//---------------------------------------------------------
void CWKSP_Module_Control::On_Execute_UI(wxUpdateUIEvent &event)
{
	Get_Manager()->On_Execute_UI(event);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Module_Control::Add_Group(CWKSP_Module_Group *pGroup)
{
	_Add_Item(pGroup, IMG_GROUP, IMG_GROUP);
}

//---------------------------------------------------------
void CWKSP_Module_Control::Add_Library(const wxTreeItemId &Group, CWKSP_Module_Library *pLibrary)
{
	if( pLibrary != NULL )
	{
		wxString	Name	= pLibrary->Get_Name().AfterFirst('-');

		if( Name.IsEmpty() )
		{
			Name	= pLibrary->Get_Name();
		}
		else
		{
			Name.Trim(false);
		}

		AppendItem(Group, Name, IMG_LIBRARY, IMG_LIBRARY, pLibrary);

		for(int i=0; i<pLibrary->Get_Count(); i++)
		{
			Add_Module(pLibrary->GetId(), pLibrary->Get_Module(i));
		}

		SortChildren(pLibrary->GetId());
	}

	SortChildren(Group);
}

//---------------------------------------------------------
void CWKSP_Module_Control::Add_Module(const wxTreeItemId &Library, CWKSP_Module *pModule)
{
	if( pModule != NULL )
	{
		AppendItem(Library, pModule->Get_Name(), IMG_MODULE, IMG_MODULE, pModule);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
