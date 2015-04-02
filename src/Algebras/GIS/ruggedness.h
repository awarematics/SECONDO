/*
This file is part of SECONDO.

Copyright (C) 2011, University in Hagen, Department of Computer Science,
Database Systems for New Applications.

SECONDO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

SECONDO is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SECONDO; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef GIS_RUGGEDNESS_H
#define GIS_RUGGEDNESS_H

/*
SECONDO includes

*/
#include <NList.h>

/*
Raster2 includes

*/
#include "../Raster2/stype.h"

/*
declaration of namespace GISAlgebra

*/
namespace GISAlgebra 
{
/*
declaration of ruggednessFuns array

*/
        extern ValueMapping ruggednessFuns[];

/*
Type Mapping

*/
        ListExpr ruggednessTypeMap(ListExpr args);

/*
Value Mapping

*/
        int ruggednessSelectFun(ListExpr args);

/*
Struct ruggednessInfo describes name, syntax, meaning and signature
of operator ruggedness.

*/
        struct ruggednessInfo : OperatorInfo 
        {
          ruggednessInfo()
          { 
            name      = "ruggedness";
            signature = "sType -> sType";
            appendSignature("stream(tType) x double -> stream(tType)");
            syntax    = "_ ruggedness";
            meaning   = "Creates ruggedness for raster or stream of tiles";
           }              
        };
}

#endif /* #define GIS_RUGGEDNESS_H */

