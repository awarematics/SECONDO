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

#ifndef TILEALGEBRA_PROPERTIESINT_H
#define TILEALGEBRA_PROPERTIESINT_H

/*
SECONDO includes

*/

#include "StandardTypes.h"
#include "TemporalAlgebra.h"

/*
TileAlgebra includes

*/

#include "Properties.h"
#include "../Constants.h"

/*
declaration of namespace TileAlgebra

*/

namespace TileAlgebra
{

/*
Class Properties<int> represents the properties of base datatype int.

author: Dirk Zacher

*/

template <>
class Properties<int>
{
  public:

  /*
  typedef of PropertiesType

  */

  typedef int PropertiesType;

  /*
  typedef of WrapperType

  */

  typedef CcInt WrapperType;

  /*
  typedef of MType

  */

  typedef MInt MType;

  /*
  typedef of UnitType

  */

  typedef UInt UnitType;

  /*
  Method GetUndefinedValue returns the undefined value of base datatype int.

  author: Dirk Zacher
  parameters: -
  return value: undefined value of base datatype int
  exceptions: -

  */

  static int GetUndefinedValue();

  /*
  Method GetValue returns the value of given NList representation.

  author: Dirk Zacher
  parameters: rNList - reference to a NList object
  return value: value of given NList representation
  exceptions: -

  */

  static int GetValue(const NList& rNList);

  /*
  Method GetUnwrappedValue returns the unwrapped value of given wrapped value.

  author: Dirk Zacher
  parameters: rWrappedValue - reference to a wrapped value
  return value: unwrapped value
  exceptions: -

  */

  static int GetUnwrappedValue(const CcInt& rWrappedValue);

  /*
  Method GetWrappedValue returns the wrapped value of given value.

  author: Dirk Zacher
  parameters: rValue - reference to a value
  return value: wrapped value
  exceptions: -

  */

  static CcInt GetWrappedValue(const int& rValue);

  /*
  Method IsUndefinedValue checks if given value is an undefined value.

  author: Dirk Zacher
  parameters: rValue - reference to a value
  return value: true, if rValue is an undefined value, otherwise false
  exceptions: -

  */

  static bool IsUndefinedValue(const int& rValue);

  /*
  Method IsValidValueType checks if given NList is NList of type int.

  author: Dirk Zacher
  parameters: rNList - reference to a NList object
  return value: true, if given NList is NList of type int, otherwise false
  exceptions: -

  */

  static bool IsValidValueType(const NList& rNList);

  /*
  Method ToNList returns NList representation of given value.

  author: Dirk Zacher
  parameters: rValue - reference to a value
  return value: NList representation of given value
  exceptions: -

  */

  static NList ToNList(const int& rValue);
};

}

#endif // TILEALGEBRA_PROPERTIESINT_H
