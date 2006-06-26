
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                                                       //
//                   contrib_t_wutzler                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     RGBOverlay.cpp                    //
//                                                       //
//            Copyright (C) 2003 Your Name               //
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
//    e-mail:     your@e-mail.abc                        //
//                                                       //
//    contact:    Your Name                              //
//                And Address                            //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "RGBOverlay.h"

#define MAX(A,B) ( (A)>(B)?(A):(B) )
#define VALUEBASE 10
#define COLORBASE 10

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRGBOverlay::CRGBOverlay(void)
{
	//-----------------------------------------------------
	// Place information about your module here...

	Set_Name	("RGBOverlay");

	Set_Author	("Copyrights (c) 2004 by Thomas Wutzler");

	Set_Description(
		"RGBOverlay\n"
		"calculates an Red-Green-Blue Image of given input grids.\n"
	);


	//-----------------------------------------------------

	CParameter *parentPar;
	parentPar = Parameters.Add_Grid(	NULL, "RED"	, "Red"	, ""	, PARAMETER_INPUT);
	Parameters.Add_Value( parentPar, "InputWeightRed","weighting factor","weighting factor (0-100%)",  PARAMETER_TYPE_Double, 100 );
	parentPar = Parameters.Add_Grid(	NULL, "GREEN"	, "Green"	, ""	, PARAMETER_INPUT);
	Parameters.Add_Value( parentPar, "InputWeightGreen","weighting factor","weighting factor (0-100%)",  PARAMETER_TYPE_Double, 100 );
	parentPar = Parameters.Add_Grid(	NULL, "BLUE"	, "Blue"	, ""	, PARAMETER_INPUT);
	Parameters.Add_Value( parentPar, "InputWeightBlue","weighting factor","weighting factor (0-100%)",  PARAMETER_TYPE_Double, 100 );
	
	Parameters.Add_Grid(	NULL, "OUTPUT"	, "RGB-Output"	, "Grid, that combines three grids"	, PARAMETER_OUTPUT);
}

//---------------------------------------------------------
CRGBOverlay::~CRGBOverlay(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// The only thing left to do is to realize your ideas whithin
// the On_Execute() function (which will be called automatically
// by the framework). But that's really your job :-)

bool CRGBOverlay::On_Execute(void)
{

	int		x, y;
	double valueCenter = VALUEBASE/2;
	double	r,g,b;
	long rc, gc, bc, rgbc;
	double  rmin, gmin, bmin, rrange, grange, brange;
	double	rw, gw, bw;

	CGrid	*pRGrid, *pGGrid, *pBGrid, *pRGBGrid;

	//-----------------------------------------------------
	// Get user inputs from the 'Parameters' object...

	pRGrid		= Parameters("RED")->asGrid();
	pGGrid		= Parameters("GREEN")->asGrid();
	pBGrid		= Parameters("BLUE")->asGrid();
	pRGBGrid	= Parameters("OUTPUT")->asGrid();

	rw	= Parameters("InputWeightRed")->asDouble()/100.0;
	gw	= Parameters("InputWeightGreen")->asDouble()/100.0;
	bw	= Parameters("InputWeightBlue")->asDouble()/100.0;

	if( rw > 1 || rw < 0 ){
		Error_Set("Weight for Red Grid out of range (0..100) - Cannot execute module");
		return (false);
	}
	if( rw > 1 || rw < 0 ){
		Error_Set("Weight for Red Grid out of range (0..100) - Cannot execute module");
		return (false);
	}
	if( rw > 1 || rw < 0 ){
		Error_Set("Weight for Red Grid out of range (0..100) - Cannot execute module");
		return (false);
	}

	//----------------- init boundaries  ------------
	rmin = pRGrid->Get_ZMin();
	gmin = pGGrid->Get_ZMin();
	bmin = pBGrid->Get_ZMin();


	// -1 deminishes step, so that on r = rmax , valueBase is not reached
	rrange = (VALUEBASE-1)/(pRGrid->Get_ZMax() - rmin);
	grange = (VALUEBASE-1)/(pGGrid->Get_ZMax() - gmin);
	brange = (VALUEBASE-1)/(pBGrid->Get_ZMax() - bmin);

	// ---------------- calculate values ------------
	// 
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			r = pRGrid->asDouble(x,y);
			g = pGGrid->asDouble(x,y);
			b = pBGrid->asDouble(x,y);
			if( pRGBGrid->is_NoData_Value(r)  || pRGBGrid->is_NoData_Value(r) || pRGBGrid->is_NoData_Value(r) ){
				pRGBGrid->Set_NoData(x,y);
			}else{
				rc = (rw > 0) ? (long) (valueCenter + ((r - rmin) * rrange -valueCenter) * rw) : 0;
				gc = (gw > 0) ? (long) (valueCenter + ((g - gmin) * grange -valueCenter) * gw) : 0;
				bc = (bw > 0) ? (long) (valueCenter + ((b - bmin) * brange -valueCenter) * bw) : 0;
				rgbc = VALUEBASE*VALUEBASE*rc + VALUEBASE*gc + bc;
				pRGBGrid->Set_Value(x,y,rgbc);
			}
		}
	}

	//------------- adjust ColorRamp -----------
	CColors colors;
	double colFac = 256.0 / COLORBASE;
	double colFacStrech = colFac / COLORBASE;
	DataObject_Get_Colors( pRGBGrid, colors );
	colors.Set_Count(COLORBASE*COLORBASE*COLORBASE);
	int ri, gi, bi;
	for( ri = 0; ri < COLORBASE; ri++ ){
		rc = (int)( ri * colFac + ri * colFacStrech);
		for( gi = 0; gi < COLORBASE; gi++ ){
			gc = (int)( gi * colFac  + gi * colFacStrech);
			for( bi = 0; bi < COLORBASE; bi++ ){
				bc = (int)( bi * colFac + bi * colFacStrech);
				rgbc = (long) COLOR_GET_RGB(rc,gc,bc);
				colors.Set_Color( COLORBASE*COLORBASE*ri + COLORBASE*gi + bi, rgbc );
			}
		}
	}
	DataObject_Set_Colors( pRGBGrid, colors );
	//prescribe minimum and maximum value render workaround
	pRGBGrid->Set_Value(0,0,0);
	pRGBGrid->Set_Value(0,1,VALUEBASE*VALUEBASE*VALUEBASE-1);

	return( true );
}
