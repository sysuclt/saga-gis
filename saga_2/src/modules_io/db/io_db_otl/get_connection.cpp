
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     io_table_odbc                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Get_Connection.cpp                  //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                      Olaf Conrad                      //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "get_connection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGet_Connection::CGet_Connection(void)
{
	Set_Name		(_TL("ODBC Connect"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		"Connect to ODBC source."
	));

	Parameters.Add_Choice(
		NULL	, "SERVERS"		, _TL("Server"),
		_TL(""),
		_TL("")
	);

	Parameters.Add_String(
		NULL	, "USERNAME"	, _TL("User"),
		_TL(""),
		_TL("")
	);

	Parameters.Add_String(
		NULL	, "PASSWORD"	, _TL("Password"),
		_TL(""),
		SG_T(""), false, true
	);

	Parameters.Add_FilePath(
		NULL	, "DIRPATH"		, _TL("Directory"),
		_TL(""),
		NULL, NULL, false, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGet_Connection::On_Before_Execution(void)
{
	CSG_String	Servers;

	if( SG_ODBC_Get_Connection_Manager().Get_Servers(Servers) > 0 )
	{
		Parameters("SERVERS")->asChoice()->Set_Items(Servers);

		return( true );
	}

	Message_Dlg(
		_TW("No ODBC server available!\n"
			"Set up an ODBC server first."),
		_TL("ODBC Database Connection Error")
	);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGet_Connection::On_Execute(void)
{
	CSG_String	Server, User, Password, Directory;

	Server		= Parameters("SERVERS")		->asString();
	User		= Parameters("USERNAME")	->asString();
	Password	= Parameters("PASSWORD")	->asString();
	Directory	= Parameters("DIRPATH")		->asString();

	if( SG_ODBC_Get_Connection_Manager().Add_Connection(Server, User, Password, Directory) )
	{
		Message_Add(CSG_String::Format(SG_T("%s: %s"), Server.c_str(), _TL("ODBC source connected")));

		return( true );
	}

	Message_Add(CSG_String::Format(SG_T("%s: %s"), Server.c_str(), _TL("could not connect ODBC source")));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDel_Connection::CDel_Connection(void)
{
	Set_Name		(_TL("ODBC Disconnect"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		"Disconnect ODBC source."
	));

	Parameters.Add_Choice(
		NULL	, "SERVERS"		, _TL("Server"),
		_TL(""),
		_TL("")
	);

	Parameters.Add_Choice(
		NULL	, "TRANSACT"	, _TL("Transactions"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("rollback"),
			_TL("commit")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDel_Connection::On_Before_Execution(void)
{
	CSG_String	Servers;

	if( SG_ODBC_Get_Connection_Manager().Get_Connections(Servers) > 0 )
	{
		Parameters("SERVERS")->asChoice()->Set_Items(Servers);

		return( true );
	}

	Message_Dlg(
		_TL("No ODBC connection available!"),
		_TL("ODBC Database Connection Error")
	);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDel_Connection::On_Execute(void)
{
	bool		bCommit;
	CSG_String	Server;

	Server	= Parameters("SERVERS") ->asString();
	bCommit	= Parameters("TRANSACT")->asInt() == 1;

	if( SG_ODBC_Get_Connection_Manager().Del_Connection(Server, bCommit) )
	{
		Message_Add(CSG_String::Format(SG_T("%s: %s"), Server.c_str(), _TL("ODBC source connected")));

		return( true );
	}

	Message_Add(CSG_String::Format(SG_T("%s: %s"), Server.c_str(), _TL("could not connect ODBC source")));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTransaction::CTransaction(void)
{
	Set_Name		(_TL("ODBC Commit/Rollback Transaction"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		"Execute a commit or rollback on open transactions with ODBC source."
	));

	Parameters.Add_Choice(
		NULL	, "SERVERS"		, _TL("Server"),
		_TL(""),
		_TL("")
	);

	Parameters.Add_Choice(
		NULL	, "TRANSACT"	, _TL("Transactions"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("rollback"),
			_TL("commit")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTransaction::On_Before_Execution(void)
{
	CSG_String	Servers;

	if( SG_ODBC_Get_Connection_Manager().Get_Connections(Servers) > 0 )
	{
		Parameters("SERVERS")->asChoice()->Set_Items(Servers);

		return( true );
	}

	Message_Dlg(
		_TL("No ODBC connection available!"),
		_TL("ODBC Database Connection Error")
	);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTransaction::On_Execute(void)
{
	CSG_String	Server;

	Server	= Parameters("SERVERS") ->asString();

	CSG_ODBC_Connection	*pConnection	= SG_ODBC_Get_Connection_Manager().Get_Connection(Server);

	if( !pConnection )
	{
		return( false );
	}

	if( Parameters("TRANSACT")->asInt() == 1 )
	{
		if( pConnection->Commit() )
		{
			Message_Add(CSG_String::Format(SG_T("%s: %s"), Server.c_str(), _TL("open transactions committed")));

			return( true );
		}
	}
	else
	{
		if( pConnection->Rollback() )
		{
			Message_Add(CSG_String::Format(SG_T("%s: %s"), Server.c_str(), _TL("open transactions rollbacked")));

			return( true );
		}
	}

	Message_Add(CSG_String::Format(SG_T("%s: %s"), Server.c_str(), _TL("could not commit/rollback transactions.")));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------