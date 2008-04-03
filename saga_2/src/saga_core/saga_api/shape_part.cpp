
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    shape_part.cpp                     //
//                                                       //
//          Copyright (C) 2008 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include <memory.h>

#include "shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape_Part::CSG_Shape_Part(CSG_Shape_Points *pOwner)
{
	m_pOwner	= pOwner;

	m_Points	= NULL;
	m_nPoints	= 0;
	m_nBuffer	= 0;

	m_bUpdate	= true;
}

//---------------------------------------------------------
CSG_Shape_Part::~CSG_Shape_Part(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_GROW_SIZE(n)	(n < 128 ? 1 : (n < 2048 ? 32 : 256))

//---------------------------------------------------------
bool CSG_Shape_Part::_Alloc_Memory(int nPoints)
{
	if( m_nPoints != nPoints )
	{
		int		nGrow	= GET_GROW_SIZE(nPoints),
				nBuffer = (nPoints / nGrow) * nGrow;

		while( nBuffer < nPoints )
		{
			nBuffer	+= nGrow;
		}

		if( m_nBuffer != nBuffer )
		{
			m_nBuffer	= nBuffer;

			TSG_Point	*Points	= (TSG_Point *)SG_Realloc(m_Points, m_nBuffer * sizeof(TSG_Point));

			if( Points == NULL )
			{
				return( false );
			}

			m_Points	= Points;
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape_Part::Destroy(void)
{
	if( m_Points != NULL )
	{
		SG_Free(m_Points);
	}

	m_Points	= NULL;
	m_nPoints	= 0;
	m_nBuffer	= 0;

	m_bUpdate	= true;

	_Extent_Invalidate();

	return( true );
}

//---------------------------------------------------------
bool CSG_Shape_Part::Assign(CSG_Shape_Part *pPart)
{
	if( _Alloc_Memory(pPart->Get_Count()) )
	{
		memcpy(m_Points, pPart->m_Points, pPart->m_nPoints * sizeof(TSG_Point));
 
		m_Extent	= pPart->m_Extent;
		m_bUpdate	= pPart->m_bUpdate;

		if( m_pOwner )
		{
			m_pOwner->_Extent_Invalidate();
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Shape_Part::Add_Point(double x, double y)
{
	return( Ins_Point(x, y, m_nPoints) );
}

//---------------------------------------------------------
int CSG_Shape_Part::Ins_Point(double x, double y, int iPoint)
{
	if( iPoint >= 0 && iPoint <= m_nPoints && _Alloc_Memory(m_nPoints + 1) )
	{
		for(int i=m_nPoints; i>iPoint; i--)
		{
			m_Points[i]	= m_Points[i - 1];
		}

		m_nPoints++;

		m_Points[iPoint].x	= x;
		m_Points[iPoint].y	= y;

		_Extent_Invalidate();

		return( m_nPoints );
	}

	return( 0 );
}

//---------------------------------------------------------
int CSG_Shape_Part::Set_Point(double x, double y, int iPoint)
{
	if( iPoint >= 0 && iPoint < m_nPoints )
	{
		m_Points[iPoint].x	= x;
		m_Points[iPoint].y	= y;

		_Extent_Invalidate();

		return( 1 );
	}

	return( 0 );
}

//---------------------------------------------------------
int CSG_Shape_Part::Del_Point(int del_Point)
{
	if( del_Point >= 0 && del_Point < m_nPoints )
	{
		m_nPoints--;

		for(int iPoint=del_Point; iPoint<m_nPoints; iPoint++)
		{
			m_Points[iPoint]	= m_Points[iPoint + 1];
		}

		_Alloc_Memory(m_nPoints);

		_Extent_Invalidate();

		return( 1 );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Shape_Part::_Extent_Invalidate(void)
{
	m_bUpdate	= true;

	if( m_pOwner )
	{
		m_pOwner->_Extent_Invalidate();
	}
}

//---------------------------------------------------------
void CSG_Shape_Part::_Extent_Update(void)
{
	if( m_bUpdate )
	{
		int			iPoint;
		TSG_Point	*pPoint;
		TSG_Rect	r;

		r.xMin	=  1.0;
		r.xMax	= -1.0;

		for(iPoint=0, pPoint=m_Points; iPoint<m_nPoints; iPoint++, pPoint++)
		{
			if( r.xMin > r.xMax )
			{
				r.xMin	= r.xMax	= pPoint->x;
				r.yMin	= r.yMax	= pPoint->y;
			}
			else
			{
				if( r.xMin > pPoint->x )
				{
					r.xMin	= pPoint->x;
				}
				else if( r.xMax < pPoint->x )
				{
					r.xMax	= pPoint->x;
				}

				if( r.yMin > pPoint->y )
				{
					r.yMin	= pPoint->y;
				}
				else if( r.yMax < pPoint->y )
				{
					r.yMax	= pPoint->y;
				}
			}
		}

		m_Extent.Assign(r);

		m_bUpdate	= false;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
