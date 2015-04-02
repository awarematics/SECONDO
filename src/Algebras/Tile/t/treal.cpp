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

/*
TileAlgebra includes

*/

#include "treal.h"

/*
declaration of namespace TileAlgebra

*/

namespace TileAlgebra
{

/*
Method GetXDimensionSize returns the size of x dimension of datatype treal.

author: Dirk Zacher
parameters: -
return value: size of x dimension of datatype treal
exceptions: -

*/

int tProperties<double>::GetXDimensionSize()
{
  /*
  According to Prof. Dr. G�ting all Tile Algebra data types should have
  an identical size of x dimension, optimized for data type tint.

  */

  int xDimensionSize = static_cast<unsigned int>
                       (std::pow((WinUnix::getPageSize() -
                                  sizeof(tgrid) -
                                  2 * sizeof(int)) /
                                  sizeof(int),
                                  0.5)
                       );

  return xDimensionSize;
}

/*
Method GetYDimensionSize returns the size of y dimension of datatype treal.

author: Dirk Zacher
parameters: -
return value: size of y dimension of datatype treal
exceptions: -

*/

int tProperties<double>::GetYDimensionSize()
{
  /*
  According to Prof. Dr. G�ting all Tile Algebra data types should have
  an identical size of y dimension, optimized for data type tint.

  */

  int yDimensionSize = static_cast<unsigned int>
                       (std::pow((WinUnix::getPageSize() -
                                  sizeof(tgrid) -
                                  2 * sizeof(int)) /
                                  sizeof(int),
                                  0.5)
                       );

  return yDimensionSize;
}

/*
Method GetFlobElements returns the number of flob elements of datatype treal.

author: Dirk Zacher
parameters: -
return value: number of flob elements of datatype treal
exceptions: -

*/

int tProperties<double>::GetFlobElements()
{
  int flobElements = GetXDimensionSize() * GetYDimensionSize();

  return flobElements;
}

/*
Method GetFlobSize returns the size of the flob of datatype treal.

author: Dirk Zacher
parameters: -
return value: size of the flob of datatype treal
exceptions: -

*/

SmiSize tProperties<double>::GetFlobSize()
{
  SmiSize flobSize = GetFlobElements() * sizeof(double);

  return flobSize;
}

/*
Method GetTypeName returns the typename of datatype treal.

author: Dirk Zacher
parameters: -
return value: typename of datatype treal
exceptions: -

*/

std::string tProperties<double>::GetTypeName()
{
  return TYPE_NAME_TREAL;
}

}
