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

2013, May Simone Jandt

1 Defines and includes

*/

#ifndef JPATH_H
#define JPATH_H

#include <ostream>
#include <string>
#include "Attribute.h"
#include "StandardTypes.h"
#include "../Tools/Flob/DbArray.h"
#include "JNetwork.h"
#include "SpatialAlgebra.h"
#include "JLine.h"

namespace jnetwork {

/*
1 class JPath

Consists of a network id and a set of ~jrint~ sorted by their position in
a path in the ~jnetwork~.

*/


class JPath : public Attribute
{

public:

/*
1.1 public declarations

1.1.1 Constructors and Deconstructor

*/

  explicit JPath(const bool defined);
  explicit JPath(const string netId, const DbArray<JRouteInterval>& inpath,
                 const bool check = true);
  explicit JPath(const JPath& other);

  ~JPath();

/*
1.1.1 Getter and Setter for private attributes

*/

  const STRING_T* GetNetworkId() const;
  const DbArray<JRouteInterval>& GetPath() const;

  void SetNetworkId(const STRING_T& nid);
  void SetPath(const DbArray<JRouteInterval>& setri, const bool check = true,
               const JNetwork* jnet = 0);

/*
1.1.1 Override Methods from Attribute

*/
  void CopyFrom(const Attribute* right);
  size_t HashValue() const;
  JPath* Clone() const;
  bool Adjacent(const Attribute* attrib) const;
  static int Compare(const void* ls, const void* rs);
  int Compare(const Attribute* rhs) const;
  int Compare(const JPath& rhs) const;
  size_t Sizeof() const;
  int NumOfFLOBs() const;
  Flob* GetFLOB(const int i);
  void Destroy();
  void Clear();
  ostream& Print(ostream& os) const;
  Attribute::StorageType GetStorageType() const;
  static const string BasicType();
  static const bool checkType(const ListExpr type);

/*
1.1.1 Standard Operators

*/

  JPath& operator=(const JPath& other);

  bool operator==(const JPath& other) const;
  bool operator!=(const JPath& other) const;
  bool operator<(const JPath& other) const;
  bool operator<=(const JPath& other) const;
  bool operator>(const JPath& other) const;
  bool operator>=(const JPath& other) const;

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
1.1.1 Manage Bulkload of PathEntries

*/

  void StartBulkload();
  void EndBulkload();

  JPath& Add(const JRouteInterval& rint);

/*
1.1.1 Other helpful operators

1.1.1.1 ~Example~

Returns an example of the data type.

*/

  static string Example();

/*
1.1.1.1 GetNoComponents

Returns the number of path entries of the path.

*/

  int GetNoComponents() const;

/*
1.1.1.1 IsEmpty

Returns true if the JPath is defined and has no entries.

*/

  bool IsEmpty() const;

/*
1.1.1.1 Get

Returns the routeinterval at the given position

*/

  void Get(const int i, JRouteInterval& ri) const;

/*
1.1.1.1 ToJLine

Builds a jline from the current object.

*/
  void ToJLine(JLine* result);

/*
1.1.1.1 Length

Returns the total length of the path.

*/

 double Length() const;


private:

/*
1.1 private Deklaration part

1.1.1 Attributes

*/
  STRING_T nid;                 //network identifier
  DbArray<JRouteInterval> path; //set of JRouteIntervals sorted by position
                                //in the path
  bool activBulkload;           //Flag for Bulkload of elements

/*

1.1.1 Default Constructor

The default constructor may only be used in the cast-function therefore we
declare it to be private.

*/

 JPath();

/*
1.1.1 Methods

1.1.1.1 FillIntervalList

Fills the list of route intervals from the given DbArray. Ignores
RouteIntervals which are not in the network.

*/

void FillIntervalList(const DbArray<JRouteInterval>* rintList,
                      const JNetwork* jnet);

/*
1.1.1.1 CheckAndFillIntervallList

Calls FillIntervalList if jnet is 0 the jnetwork is opened and closed around
the FillIntervalList call.

*/

void CheckAndFillIntervallList(const DbArray<JRouteInterval>* setri,
                                     const JNetwork* jnet /*= 0*/);

/*
1.1.1.1 Compress

Reduces number of route intervals if possible.

*/

 void Simplify();


};

} // end of namespace jnetwork

/*
1 Overwrite output operator

*/

using namespace jnetwork;
ostream& operator<<(ostream& os, const JPath& l);


#endif // JPATH_H
