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

1 Defines and includes

*/

#ifndef JPOINTS_H
#define JPOINTS_H

#include <ostream>
#include <string>
#include "Attribute.h"
#include "RouteLocation.h"
#include "StandardTypes.h"
#include "../Tools/Flob/DbArray.h"
#include "JList.h"
#include "JLine.h"
#include "JPath.h"
#include "JNetwork.h"


namespace jnetwork {

  class JPoint;
/*
1 class JPoints

Consists of a network id and a set of ~RouteLocations~ describing an set of
single positions in an ~jnetwork~. The ~RouteLocations~ are stored sorted by
their route identifiers and positions.

*/

class JPoints : public Attribute
{

/*
1.1 public declarations

*/

public:

/*
1.1.1 Constructors and Deconstructor

*/

  explicit JPoints(const bool defined);
  JPoints(const string netId, const DbArray<RouteLocation>& rlocList,
          const bool check = true, const bool issorted = false);
  JPoints(const JPoints& other);

  ~JPoints();

/*
1.1.1 Getter and Setter for private attributes

*/

  const STRING_T* GetNetworkId() const;
  const DbArray<RouteLocation>& GetRouteLocations() const;

  void SetNetworkId(const STRING_T& nid);
  void SetRouteLocations(const DbArray<RouteLocation>& setri,
                                const bool check = true,
                                const bool issorted = false,
                                const JNetwork* jnet = 0);
  void SetRouteLocations(const JListRLoc& rlocs,
                         const bool check =true,
                         const bool issorted =false,
                         const JNetwork* jnet =0);

/*
1.1.1 Override Methods from Attribute

*/
  void CopyFrom(const Attribute* right);
  size_t HashValue() const;
  JPoints* Clone() const;
  bool Adjacent(const Attribute* attrib) const;
  static int Compare(const void* ls, const void* rs);
  int Compare(const Attribute* rhs) const;
  int Compare(const JPoints& rhs) const;
  size_t Sizeof() const;
  int NumOfFLOBs() const;
  Flob* GetFLOB(const int i);
  void Destroy();
  ostream& Print(ostream& os) const;
  Attribute::StorageType GetStorageType() const;
  static const string BasicType();
  static const bool checkType(const ListExpr type);

/*
1.1.1 Standard Operators

*/

  JPoints& operator=(const JPoints& other);

  bool operator==(const JPoints& other) const;
  bool operator!=(const JPoints& other) const;
  bool operator<(const JPoints& other) const;
  bool operator<=(const JPoints& other) const;
  bool operator>(const JPoints& other) const;
  bool operator>=(const JPoints& other) const;

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
1.1.1 Other helpful operators

1.1.1.1 ~Example~

Returns an example of the data type.

*/

  static string Example();

/*
1.1.1.1 GetNoComponents

Returns the number of routeintervals of the jline.

*/

  int GetNoComponents() const;

/*
1.1.1.1 IsEmpty

Returns true if the JPoints is defined and has no routeintervals.

*/

  bool IsEmpty() const;

/*
1.1.1.1 Get

Returns the routeinterval at the given position

*/

  void Get(const int i, RouteLocation& ri) const;

/*
1.1.1.1 Managing routeintervals

1.1.1.1.1 ~StartBulkload~

*/

  void StartBulkload();

/*
1.1.1.1.1 ~EndBulkload~

*/

  void EndBulkload();

/*
1.1.1.1.1 ~Add~

Adds the given (set of) RouteLocation(s) to the set of routelocations, whereas
sets of route locations can only can be added in activBulkload.

*/

 JPoints& Add(const RouteLocation& rint);
 JPoints& Add(const JListRLoc& rlocs);

/*
1.1.1.1 Shortest Path

Computes the shortest path to the given target.

*/

void ShortestPath(const JPoint* target, JPath* result) const;
void ShortestPath(const JPoints* target, JPath* result) const;
void ShortestPath(const JLine* target, JPath* result) const;

/*
1.1.1.1 Netdistance

Returns the network distance to the target, which is given by the length of
the shortest path to the target.

*/

void Netdistance(const JPoint* target, CcReal* result) const;
void Netdistance(const JPoints* target, CcReal* result) const;
void Netdistance(const JLine* target, CcReal* result) const;

/*
1.1.1.1 Contains

Returns true if the target is contained.

*/

bool Contains(const JPoint* target) const;

/*

1.1 private Deklaration part

*/

private:

/*
1.1.1 Attributes

*/
  STRING_T nid; //network identifier
  DbArray<RouteLocation> routelocations; //sorted set of RouteLocations
  bool sorted; //true if routelocations are sorted
  bool activBulkload; //only true while bulkload of routelocations runs

/*

1.1.1 Default Constructor

The default constructor may only be used in the cast-function therefore we
declare it to be private.

*/

 JPoints();

/*
1.1.1 Methods

1.1.1.1 Management of RouteLocations

1.1.1.1.1 IsSorted

Checks if the given set of RouteLocations is sorted.

*/

  bool IsSorted() const;

/*
1.1.1.1.1 Sort

Sorts the given set of RouteLocations ascending by route Identifier and
Position.

*/

  void Sort();

/*
1.1.1.1.1 FillLocationList

Fills the location List from an DbArray of RouteLocations. Route Locations
out of the network are ignored.

*/

  void FillLocationList(const DbArray<RouteLocation>& locList,
                        const JNetwork* jnet);

/*
1.1.1.1.1 CheckAndFillLocationList

For jnet != 0 FillLocationList is called. For jnet == 0 the jnetwork is opened
before call of FillLocationList and closed after that.

*/

void CheckAndFillLocationList(const DbArray<RouteLocation>* setri,
                    const JNetwork* jnet = 0);
};

} // end of namespace jnetwork

/*
1 Overwrite output operator

*/



using namespace jnetwork;

ostream& operator<<(ostream& os, const jnetwork::JPoints l);


#endif // JPOINTS_H
