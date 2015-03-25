/**********************************************************
 * Version $Id: tool_counter.cpp 2332 2014-11-07 14:12:16Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      dev_tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   tool_counter.cpp                    //
//                                                       //
//                 Copyright (C) 2015 by                 //
//                     Olaf Conrad                       //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute for Geography                //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "tool_counter.h"

//---------------------------------------------------------
#include <wx/dir.h>
#include <wx/filename.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTool_Counter::CTool_Counter(void)
{
	//-----------------------------------------------------
	Set_Name		("Count Tools");

	Set_Author		("O. Conrad (c) 2015");

	Set_Description	(
		"Extracts SAGA tool instantiations directly from source by analyzing MLB_Interface.cpp files. "
		"Helps to correctly find the number of tools in a certain SAGA distribution, which became "
		"necessary due to a tool counting bug in the module library interface function after introducing "
		"the 'skip module' option for maintainance of module identifiers. "
	);

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "TARGET"		, SG_T("Tools"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath(
		NULL	, "DIRECTORY"	, SG_T("Sources Directory"),
		"",
		NULL, SG_T("H:/saga/saga_2/trunk/saga-gis/src/modules"), false, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTool_Counter::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	&Elements	= *Parameters("TARGET")->asTable();

	Elements.Destroy();
	Elements.Set_Name("Tools Instantiations");

	Elements.Add_Field(SG_T("LIB" ), SG_DATATYPE_String);
	Elements.Add_Field(SG_T("ID"  ), SG_DATATYPE_String);
	Elements.Add_Field(SG_T("TOOL"), SG_DATATYPE_String);

	int	nFiles	= Read_Directory(Parameters("DIRECTORY")->asString(), Elements);

	if( nFiles <= 0 )
	{
		Error_Set(SG_T("no source code files found"));

		return( false );
	}

	Message_Add(CSG_String::Format("\n%s: %d", _TL("number of scanned files"), nFiles), false);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTool_Counter::Read_Directory(const SG_Char *Directory, CSG_Table &Elements)
{
	int		nFiles	= 0;

	wxString	Name, s;
	wxFileName	File;
	wxDir		Dir;

	if( !Dir.Open(Directory) )
	{
		return( nFiles );
	}

	File.AssignDir(Directory);

	if(	Dir.GetFirst(&Name, "MLB_Interface.cpp", wxDIR_FILES|wxDIR_HIDDEN) )
	{
		do
		{
			File.SetFullName(Name);

			nFiles	+= Read_File(s = File.GetFullPath(), Elements);
		}
		while( Dir.GetNext(&Name) );
	}

	if(	Dir.GetFirst(&Name, "*"    , wxDIR_DIRS |wxDIR_HIDDEN) )
	{
		do
		{
			File.AssignDir(Directory);
			File.AppendDir(Name);

			nFiles	+= Read_Directory(s = File.GetFullPath(), Elements);
		}
		while( Dir.GetNext(&Name) );
	}

	return( nFiles );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTool_Counter::Read_File(const SG_Char *File, CSG_Table &Elements)
{
	Process_Set_Text(CSG_String::Format("%s: %s", _TL("scanning"), File));

	//-----------------------------------------------------
	CSG_String	Lib	= SG_File_Get_Path(File).BeforeLast('\\').AfterLast('\\');

	if( !Lib.CmpNoCase("dev_tools") || !Lib.CmpNoCase("template") )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(File, SG_FILE_R, false) )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	CSG_String	String;

	if( !Stream.Read(String, Stream.Length()) )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	int	i;

	i	= String.Find("Create_Module");	if( i < 0 )	return( false );

	String	= String.Right(String.Length() - i);

	i	= String.Find("switch");	if( i < 0 )	return( false );

	String	= String.Right(String.Length() - i);

	String	= String.AfterFirst('{').BeforeFirst('}');

	while( !String.is_Empty() )
	{
		CSG_String	Text	= String.BeforeFirst(';');

		if( Text.Find("new") >= 0 )
		{
			Text.Replace("\t", " "); Text.Replace("\n", " "); Text.Trim();

			if( Text.Find("case") == 0 )
			{
				Text	= Text.Right(Text.Length() - 4);
				
				CSG_String	ID	= Text.BeforeFirst(':'); ID.Trim();

				Text	= Text.AfterFirst(':');	Text.Trim();

				CSG_Table_Record	*pRecord	= Elements.Add_Record();

				pRecord->Set_Value(0, Lib);
				pRecord->Set_Value(1, ID);
				pRecord->Set_Value(2, Text);
			}
		}

		String	= String.AfterFirst(';');
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTool_Counter::Read_Text(const SG_Char *String, CSG_String &Text)
{
	int			n, Level;

	Text.Clear();

	for(n=0, Level=-1; String[n]!='\0' && Level<2; n++)
	{
		if( Level < 0 )
		{
			if( String[n] == '(' )
			{
				Level	= 0;
			}
		}
		else if( Level == 0 )
		{
			switch( String[n] )
			{
			case '\"':
				Level	= 1;
				break;

			case ')':
				Level	= 2;
				break;
			}
		}
		else switch( String[n] )
		{
		case '\"':
			Level	= 0;
			break;

		case '\\':
			Text.Append(String[n++]);
			Text.Append(String[n]);
			break;

		default:
			Text.Append(String[n]);
			break;
		}
	}

	return( n );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
