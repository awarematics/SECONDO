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

2012, July Simone Jandt

1 Defines and Includes

*/

#ifndef UJPOINT_H
#define UJPOINT_H

#include <ostream>
#include <string>
#include "Attribute.h"
#include "StandardTypes.h"
#include "DateTime.h"
#include "TemporalAlgebra.h"
#include "JRouteInterval.h"
#include "JPoint.h"
#include "JUnit.h"
#include "JNetwork.h"

namespace jnetwork{

/*
1 class ~UJPoint~

A ~UJPoint~ consist of an network id, an ~JRouteInterval~, and an time
intervall. It represents the positions of the ~MJPoint~ in the ~JNetwork~ in
this time intervall.

*/

class UJPoint : public Attribute
{

/*
1.1 Public Declarations

*/
public:

/*
1.1.1 Constructors and Deconstructors

The Standard Constructor should not be used without inside the cast function.
It can not be private because UJPoint is used as part of MJPoint.

*/

  UJPoint();
  explicit UJPoint(const bool def);
  UJPoint(const UJPoint& other);
  UJPoint(const string id, const JUnit& u, const bool check = true);
  UJPoint(const string id, const Interval<Instant>& inst,
          const JRouteInterval& r, const bool check = true);
  UJPoint(const JNetwork* jnet, const JRouteInterval* jrint,
          const Interval<Instant>* timeInter, const bool check = true);

  ~UJPoint();

/*
1.1.1 Getter and Setter for private Attributes

*/

  const STRING_T* GetNetworkId() const;
  JUnit GetUnit() const;

  void SetNetworkId(const STRING_T& id);
  void SetUnit (const JUnit& j, const bool check = true,
                const JNetwork* jnet = 0);

/*
1.1.1 Override Methods from Attribute

*/

  void CopyFrom(const Attribute* right);
  StorageType GetStorageType() const;
  size_t HashValue() const;
  UJPoint* Clone() const;
  bool Adjacent(const Attribute* attrib) const;
  static int Compare(const void* ls, const void* rs);
  int Compare(const Attribute* rhs) const;
  int Compare(const UJPoint& rhs) const;
  size_t Sizeof() const;
  ostream& Print(ostream& os) const;
  static const string BasicType();
  static const bool checkType(const ListExpr type);

/*
1.1.1 Standard Operators

*/

  UJPoint& operator=(const UJPoint& other);

  bool operator==(const UJPoint& other) const;
  bool operator!=(const UJPoint& other) const;
  bool operator<(const UJPoint& other) const;
  bool operator<=(const UJPoint& other) const;
  bool operator>(const UJPoint& other) const;
  bool operator>=(const UJPoint& other) const;

/*
1.1.1 Operators for Secondo Integration

*/

  static ListExpr Out(ListExpr typeInfo, Word value);
  static Word In(const ListExpr typeInfo, const ListExpr instance,
                 const int errorPos, ListExpr& errorInfo, bool& correct);
  static Word Create(const ListExpr typeInfo);
  static void Delete( const ListExpr typeInfo, Word& w );
  static void Close( const ListExpr typeInfo, Word& w );
  static Word Clone( const ListExpr typeInfo, const Word& w );
  static void* Cast( void* addr );
  static bool KindCheck ( ListExpr type, ListExpr& errorInfo );
  static int SizeOf();
  static ListExpr Property();

/*
1.1.1 Other Operations

*/
  static string Example();

/*
1.1.1 Bounding Box

*/

Rectangle<3> BoundingBox() const;
Rectangle<3> TempNetBox() const;
Rectangle<2> NetBox() const;

/*
1.1 Private declarations

*/

private:

/*
1.1.1 Attributes

*/

  STRING_T nid;  //network identifier
  JUnit    unit; //time interval and route interval passed within this
                 //time interval

/*
1.1.1 Methods

1.1.1.1 CheckJNetContains

Checks if the jrint is part of the given jnetwork.

*/

  bool CheckJNetContains(const JRouteInterval jrint,
                         const JNetwork* jnet = 0) const;

};

} //end of namespace jnetwork

/*
1 Overwrite output operator

*/

using namespace jnetwork;

ostream& operator<< (const ostream& os, const jnetwork::UJPoint& jp);



#endif // UJPOINT_H
