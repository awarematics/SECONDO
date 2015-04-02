/*
This file is part of SECONDO.

Copyright (C) 2013, University in Hagen, Department of Computer Science,
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

#ifndef TILEALGEBRA_BBOX_H
#define TILEALGEBRA_BBOX_H

/*
SECONDO includes

*/

#include "AlgebraTypes.h"
#include "Operator.h"
#include "QueryProcessor.h"
#include "RectangleAlgebra.h"

/*
TileAlgebra includes

*/

#include "../Constants.h"
#include "../Types/Types.h"

/*
declaration of namespace TileAlgebra

*/

namespace TileAlgebra
{

/*
Struct bboxInfo describes name, syntax, meaning and signature
of TileAlgebra operator bbox.

author: Dirk Zacher

*/

struct bboxInfo : OperatorInfo
{
  bboxInfo()
  {
    name      = "bbox";
    syntax    = "bbox(_)";
    meaning   = "Returns the bounding box rectangle.";

    std::vector<std::string> typeParameterT;
    typeParameterT.push_back("T");
    std::vector<std::string> valueWrapperTypes;
    GetValueWrapperTypes(valueWrapperTypes);

    signature = std::string(TYPE_NAME_PREFIX_T) + "T" +
                RIGHT_ARROW + Rectangle<2>::BasicType() +
                FOR + GetTypeParametersDomain(typeParameterT,
                                              valueWrapperTypes);
    appendSignature(std::string(TYPE_NAME_PREFIX_MT) + "T" +
                    RIGHT_ARROW + Rectangle<3>::BasicType() +
                    FOR + GetTypeParametersDomain(typeParameterT,
                                                  valueWrapperTypes));
  }
};

/*
declaration of bboxFunctions array.

*/

extern ValueMapping bboxFunctions[];

/*
Method bboxSelectFunction returns the index of specific bbox function
in bboxFunctions array depending on the arguments.

author: Dirk Zacher
parameters: arguments - arguments of bbox operator
return value: index of specific bbox function in bboxFunctions
exceptions: -

*/

int bboxSelectFunction(ListExpr arguments);

/*
Method bboxTypeMappingFunction returns the return value type
of bbox operator in the form of a ListExpr.

author: Dirk Zacher
parameters: arguments - arguments of bbox operator
return value: return value type of bbox operator
exceptions: -

*/

ListExpr bboxTypeMappingFunction(ListExpr arguments);

}

#endif // TILEALGEBRA_BBOX_H
