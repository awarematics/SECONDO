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

2012, May Simone Jandt

1 Includes

*/

#ifndef JNETWORK_H
#define JNETWORK_H

#include <map>
#include <string>
#include <vector>
#include "../Tools/Flob/DbArray.h"
#include "NestedList.h"
#include "NList.h"
#include "StandardTypes.h"
#include "RelationAlgebra.h"
#include "OrderedRelationAlgebra.h"
#include "BTreeAlgebra.h"
#include "RTreeAlgebra.h"
#include "SpatialAlgebra.h"
#include "TemporalAlgebra.h"
#include "JList.h"
#include "PQManagement.h"
#include "RectangleAlgebra.h"
#include "SectionInterval.h"
#include "PosJNetSpatial.h"
#include "InterSP.h"
#include "PairIntDouble.h"

namespace jnetwork {

/*
1 Forward declarations of network data types

*/

class JPoint;
class JLine;
class JPath;
class JUnit;
class MJPoint;

/*
1 Class ~JNetwork~

Hybrid representation for street networks.

The representation consists of an defined flag (bool), an double value telling
which error can be accepted in map matching algorithms, an network id (string),
three relations, one ordered relation, and some BTree and RTree indices on this
relations providing faster access to the data stored in the network.

The defined flag tells if the network data is well defined. The network id
corresponds to the objectname of the network in the secondo database.

The first relation stores the network informations about street crossings, and
dead ends of streets. The relation is called junctions. The attributes of this
relation and their meanings are:

Attributname | Datatype | Meaning
==============================================================================
Id           | int      | unifique identifier of the street crossing
Position     | point    | spatial position of the crossing
ListRoutePos | listrloc | list of network (route) positions of the crossing
ListSectIn   | listint  | list of section identifiers for sections incoming to
             |          | this crossing
ListSectOut  | listint  | list of section identifiers for sections outgoing from
             |          | this crossing

The attribut Id is indexed by an BTree (junctionBTree) and the attribut
Position by an RTree (junctionRTree).

The second relation stores the network informations about the sections of the
street network. A section is the street part between two crossings resp. the
street part between a crossing and the end of a street.

Attributname      | Datatype  | Meaning
==============================================================================
Id                | int       | unifique identifier of the street section
Curve             | SimpleLine| spatial representation of the section
StartJuncId       | int       | identifier of the crossing at the start of the
                  |           | section
EndJuncId         | int       | identifier of the crossing at the end of the
                  |           | section
Side              | jdirection| tells if the section can be used upwards,
                  |           | downwards or in both directions
VMax              | real      | maximum allowed speed on this section in km/h
Length            | real      | length of the section in meter
ListRoadInter     | listjrint | list of road intervals represented by this
                  |           | section (sections may belong to more than one
                  |           | road, for example parts of main roads have often
                  |           | additional street names inside of towns)
ListAdjSectUp     | listint   | list of the identifiers of adjacent sections at
                  |           | the end of the section
ListAdjSectDow    | listint   | list of the identifiers of adjacent sections at
                  |           | the end of the section
ListRevAdjSectUp  | listint   | list of the identifiers of adjacent sections at
                  |           | the end of the section
ListRevAdjSectDown| listint   | list of the identifiers of adjacent sections at
                  |           | the end of the section

The attribut Id is again indexed by an BTree (sectionBTree) and the attribut
Curve by an RTree(sectionRTree).

In fact the complete network data is given in this two relations but men are
used into roads and positions on roads not in sections and junctions therefore
we maintain a third relation, called routes, which connects the junction and
section information to the road definition we normal use.

Attributname      | Datatype  | Meaning
===============================================================================
Id                | int       | unifique identifier of the road
ListJunctions     | listint   | list of junction identifiers for the crossings
                  |           | of this road
ListSections      | listint   | list of section identifiers for the sections
                  |           | belonging to this road
Length            | real      | length of the road in meter

The attribut Id is indexed by BTree routeBTree.

Last we define an ordered relation to be part of jnetwork which is stores the
netdistance values for already computed paths. The attributes are:

Attributename     | Datatype  | Meaning
===============================================================================
Source            | int       | identifier of source junction
Target            | int       | identifier of junction at the end of the path
NextJunction      | int       | identifier of next junction on the path
NextSection       | int       | identifier of section, which leads to the next
                  |           | junction on the path
NetworkDistance   | real      | network distance from source to target

The composite key is defined by source and target identifier.
At the start the relation is empty it is filled step by step at each network
distance computation.

*/


class JNetwork
{

/*
1.1. Public Declarations

*/
public:

/*
1.1.1 Constructors and Deconstructors

The first constructor expects the Relations in internal representation. It just
copies the given relations into the internal data structure and builds the
network indexes over the copied relations.

*/

  explicit JNetwork(const bool def);
  JNetwork(const string nid, const double t, const Relation* inJuncRel,
           const Relation* inSectRel, const Relation* inRoutesRel);
  JNetwork(const string nid, const double t, const Relation* inJuncRel,
           const Relation* inSectRel, const Relation* inRoutesRel,
           OrderedRelation* inNetDistRel);
  JNetwork(SmiRecord& valueRecord, size_t& offset, const ListExpr typeInfo,
           bool& ok);

  ~JNetwork();

/*
1.1.1. ~Destroy~

Destroy all network data from database

*/

 void Destroy();

/*
1.1.1 Get and Set Network Data

1.1.1.1 ~IsDefined~

Returns defined flag.

*/

  bool IsDefined() const;

/*
1.1.1.1 ~GetId~

Returns the network id.

*/

  const STRING_T* GetId() const;

/*
1.1.1.1 ~GetTolerance~

Returns the tolerance value of the network for map matching.

*/

  double GetTolerance() const;
  void SetTolerance(const double t);

/*
1.1.1.1 Get Relation Type Infos

*/

 static string GetJunctionsRelationType();
 static string GetSectionsRelationType();
 static string GetRoutesRelationType();
 static string GetNetdistancesRelationType();
 static string GetNetdistancesTupleType();

/*
1.1.1.1 ~GetBoundingBox~

Returns the bounding box of the network.

*/

 Rectangle<2> GetBoundingBox() const;

/*
1.1.1.1 ~SetDefined~

Sets the defined flag.

*/

void SetDefined(const bool def);

/*
1.1.1.1 GetRelationCopy

Returns a pointer to a copy of the original network data relation. The pointer
to the origin is not given to the outside to preserve internal network data from
beeing damaged.

*/

  Relation* GetJunctionsCopy() const;
  Relation* GetRoutesCopy() const;
  Relation* GetSectionsCopy() const;
  OrderedRelation* GetNetdistancesCopy() const;

/*
1.1.1 Secondo Integration

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
  static bool Save(SmiRecord& valueRecord, size_t& offset,
                   const ListExpr typeInfo, Word& value );
  bool Save(SmiRecord& valueRecord, size_t& offset,
       const ListExpr  typeInfo);
  static bool Open(SmiRecord& valueRecord, size_t& offset,
                   const ListExpr typeInfo, Word& value );
  static ListExpr Property();

/*
1.1.1 Standard Operations

*/

  ostream& Print(ostream& os) const;
  static const string BasicType();
  static const bool checkType(const ListExpr type);

/*
1.1.1 Operations for other network data types

1.1.1.1 ~Contains~
Returns true if the given position(s) exist in the network.

*/

  bool Contains(const RouteLocation& rloc) const;
  bool Contains(const JRouteInterval& rint) const;

/*
1.1.1 Operations for Translation from Spatial Data Types into Network Data Types

*/

  RouteLocation* GetNetworkValueOf(const Point* p) const;
  RouteLocation* GetNetworkValueOfOn(const Point* p,
                                     const Tuple* sectTup) const;
  JRouteInterval* GetNetworkValueOf(const HalfSegment& hs) const;
  bool GetNetworkValueOf(const Line* in, JLine* result) const;
  bool GetNetworkValueOf(const MPoint* in, MJPoint* result);
  JListRLoc* GetNetworkValuesOf(const Point* p) const;
  JListRLoc* GetNetworkValuesOf(const RouteLocation& rloc) const;
  JListRLoc* GetNetworkValuesOf(const Tuple* actSect,
                                const double distStart) const;

  //used for mapmatching adapter
  void GetSectionTuplesFor(const Rectangle<2> bbox,
                           vector<TupleId>& listSectTupIds) const;
/*
1.1.1 Operation for Translation from network data types into spatial data types

*/

  Point* GetSpatialValueOf(const JPoint& jp) const;
  void GetSpatialValueOf(const JLine* jl, Line& result) const;
  void GetSpatialValueOf(const MJPoint* mjp, MPoint& result) const;

  void GetSpatialValueOf(const JRouteInterval& rint, SimpleLine& result) const;

/*
1.1.1 Spatial and Spatio-Temporal BoundingBoxes of Network DataTypes

*/

Rectangle<3> BoundingBox(const JUnit& ju) const;
Rectangle<2> BoundingBox(const JRouteInterval& rint) const;
Rectangle<3> BoundingBox(const DbArray<JRouteInterval>& traj,
                         const double mintime, const double maxtime) const;

/*
1.1.1 ~SimulateTrip~

Generates a trip on the shortest path from source to target in the given time
interval. jnetwork is not const because ShortestPath computation is included by
the algorithm, which might change the netdistances relation.

*/

void SimulateTrip(const RouteLocation& source, const RouteLocation& target,
                  const Instant& starttime, const Instant& endtime,
                  const bool& lc, const bool& rc, MJPoint* result);

void SimulateTrip(const RouteLocation& source, const RouteLocation& target,
                  const Instant& starttime, const Instant& endtime,
                  MJPoint* result);

void ComputeAndAddUnits(const JPath* path,
                        const Instant& startime, const Instant& endtime,
                        const bool lc, const bool rc, double length,
                        MJPoint* result) const;

/*
1.1.1 ~ShortestPathTree~

Computes the distance to all junctions in the jnet from source. Operation
leaves jnetwork not const, because the relation with the precomputed network
distances might be updated.

*/

void ShortestPathTree(const RouteLocation& source,
                      DbArray<PairIntDouble>* result,
                      const double distlimit);

void ReverseShortestPathTree(const RouteLocation& source,
                      DbArray<PairIntDouble>* result,
                      const double distlimit);

/*
1.1.1 Network Part Around

The circle operations compute the network part in a given network distance
around the location. Whereas incircle computes the part from which the rloc
can be reached within the given distance, outcircle computes the part which
can be reached from the rloc within the given network distance and
circle combines both previous results.

*/

void Circle(const RouteLocation& src, const double distLimit, JLine* result);
void InCircle(const RouteLocation& src, const double distLimit, JLine* result);
void OutCircle(const RouteLocation& src, const double distLimit, JLine* result);

/*
1.1.1 ~ShortestPath~

Returns the shortest path from source to target. Operation leaves jnetwork
not const, because the relation with the precomputed network
distances might be updated.

*/

void ShortestPath(const RouteLocation& source,
                  const RouteLocation& target,
                  JPath* result);

void ShortestPath(const RouteLocation& source,
                  const DbArray<RouteLocation>& target,
                  JPath* result);

void ShortestPath(const DbArray<RouteLocation>& source,
                  const RouteLocation& target,
                  JPath* result);

void ShortestPath(const DbArray<RouteLocation>* sources,
                  const DbArray<RouteLocation>* targets,
                  JPath* result);

/*
1.1.1 ~Netdistance~

Returns the length of the shortest path from source to target.

*/

void Netdistance(const RouteLocation& source,
                 const RouteLocation& target,
                 CcReal* result);

void Netdistance(const RouteLocation& source,
                 const DbArray<RouteLocation>& target,
                 CcReal* result);

void Netdistance(const DbArray<RouteLocation>& source,
                 const RouteLocation& target,
                 CcReal* result);

void Netdistance(const DbArray<RouteLocation>* sources,
                 const DbArray<RouteLocation>* targets,
                 CcReal* result);

/*
1.1.1 Tuple Access on internal relations

The returned tuple must be deleted by the caller.

1.1.1.1 By Identifier

*/

  Tuple* GetRouteTupleWithId(const int rid) const;
  Tuple* GetSectionTupleWithId(const int sid) const;
  Tuple* GetSectionTupleWithTupleId(const TupleId tid) const;
  Tuple* GetJunctionTupleWithId(const int jid) const;
  Tuple* GetTupleWithId(BTree* tree, const Relation* rel,
                        const int id) const;
  Tuple* GetNetdistanceTupleFor(const int fid, const int tid) const;

/*
1.1.1.1 By Spatial Position

*/

  Tuple* GetSectionTupleFor(const Point* p, double& pos) const;


/*
1.1.1.1 By Single Network Postion

*/

  Tuple* GetSectionTupleFor(const RouteLocation& rloc, double& pos) const;
  Tuple* GetSectionTupleFor(const RouteLocation& rloc, double& pos,
                            const JListInt* sectList, int& index) const;

/*
1.1.1.1 By parts of Network

*/

  void GetCoveredSections(const JRouteInterval& rint,
                          DbArray<SectionInterval>* result) const;

/*
1.1.1 Access to tuple attributes of internal relations

1.1.1.1 Attributes of routes relation

*/

  double GetRouteLength(const int rid) const;
  double GetRouteLength(const Tuple* routeTuple) const;

  JListInt* GetRouteSectionList (const int rid) const;
  JListInt* GetRouteSectionList (const Tuple* routeTuple) const;

/*
1.1.1.1 Attributes of Sections Relation

*/

  SimpleLine* GetSectionCurve(const int sid) const;
  SimpleLine* GetSectionCurve(const Tuple* sectTuple) const;
  SimpleLine* GetSectionCurve(const RouteLocation& rloc, double& relpos) const;

  JListRInt* GetSectionListRouteIntervals(const int sid) const;
  JListRInt* GetSectionListRouteIntervals(const Tuple* sectTuple) const;

  void GetAdjacentSections(const int sid, const Direction* dir,
                           JListInt* result) const;
  void GetReverseAdjacentSections(const int sid, const Direction* dir,
                                  JListInt* result) const;
  JListInt* GetSectionListAdjSectionsUp(const Tuple* sectTuple) const;
  JListInt* GetSectionListAdjSectionsDown(const Tuple* sectTuple) const;
  JListInt* GetSectionListReverseAdjSectionsUp(const Tuple* sectTuple) const;
  JListInt* GetSectionListReverseAdjSectionsDown(const Tuple* sectTuple) const;

  JRouteInterval* GetRouteIntervalFor(const JListRLoc* leftrlocs,
                                      const JListRLoc* rightrlocs,
                                      const bool allowResetSide) const;
  JRouteInterval* GetRouteIntervalFor(const RouteLocation& left,
                                      const RouteLocation& right,
                                      const bool allowResetSide) const;

  JRouteInterval* GetSectionFirstRouteInterval(const Tuple* sectTuple) const;
  JRouteInterval* GetSectionRouteIntervalForRID(int rid,
                                                const Tuple* sectTup) const;
  JRouteInterval* GetSectionRouteIntervalForRLoc(const RouteLocation& rloc,
                                                 const Tuple* sectTup) const;

  Tuple* GetSectionStartJunctionTuple(const Tuple* sectTuple) const;
  Tuple* GetSectionEndJunctionTuple(const Tuple* sectTuple) const;

  JListRLoc* GetSectionStartJunctionRLocs(const Tuple* sectTuple) const;
  JListRLoc* GetSectionEndJunctionRLocs(const Tuple* sectTuple) const;

  int GetSectionStartJunctionID (const Tuple* sectTuple) const;
  int GetSectionEndJunctionID(const Tuple* sectTuple) const;

  Point* GetSectionStartPoint(const Tuple* sectTuple) const;
  Point* GetSectionEndPoint(const Tuple* sectTuple) const;

  double GetSectionMaxSpeed(const Tuple* sectTuple) const;

  double GetSectionLength(const Tuple* sectTuple) const;

  Direction* GetSectionDirection(const Tuple* sectTuple) const;

  int GetSectionId(const Tuple* sectTuple) const;



/*
1.1.1.1 Attributes of Junctions Relation

*/

  int GetJunctionId(const Tuple* juncTup) const;
  JListRLoc* GetJunctionListRLoc(const Tuple* juncTup) const;
  Point* GetJunctionSpatialPos(const Tuple* juncTup) const;
  JListInt* GetJunctionOutSectionList(const Tuple* juncTup) const;
  JListInt* GetJunctionInSectionList(const Tuple* juncTup) const;

/*
1.1.1.1 Attributes of Netdistance Relation

*/

  double GetNetdistanceDistance(const Tuple* netDistTup) const;

/*
1.1.1 Enumerations of coloumns of internal relations

*/

  enum PositionsJunctionsRelation {
     JUNC_ID = 0,
     JUNC_POS,
     JUNC_LIST_ROUTEPOSITIONS,
     JUNC_LIST_INSECTIONS,
     JUNC_LIST_OUTSECTIONS
   };

   enum PositionsSectionsRelation {
     SEC_ID = 0,
     SEC_CURVE,
     SEC_STARTNODE_ID,
     SEC_ENDNODE_ID,
     SEC_DIRECTION,
     SEC_VMAX,
     SEC_LENGTH,
     SEC_LIST_ROUTEINTERVALS,
     SEC_LIST_ADJ_SECTIONS_UP,
     SEC_LIST_ADJ_SECTIONS_DOWN,
     SEC_LIST_REV_ADJ_SECTIONS_UP,
     SEC_LIST_REV_ADJ_SECTIONS_DOWN
   };

   enum PositionsRoutesRelation {
     ROUTE_ID = 0,
     ROUTE_LIST_JUNCTIONS,
     ROUTE_LIST_SECTIONS,
     ROUTE_LENGTH
   };

   enum PositionsNetdistancesRelation {
     NETDIST_FROM_JID = 0,
     NETDIST_TO_JID,
     NETDIST_NEXT_JID,
     NETDIST_NEXT_SID,
     NETDIST_DIST
   };


/*
1.1 Private declarations

*/

 private:

/*
1.1.1 Attributes of Network Object

*/
   bool defined;        //defined Flag
   double tolerance;    //Accectable deviation for map matching algorithms
   STRING_T id;         //network identifier
   Relation* junctions; //data of street crossings and death ends
   Relation* sections;  //data of connections between junctions
   Relation* routes;    //semantic connection of sections and junctions for
                        //street networks
   OrderedRelation* netdistances; //stores the network distances between the
                                  //junctions
   BTree* junctionsBTree; //supports fast access to junctions by junction id
   R_Tree<2,TupleId>* junctionsRTree;  //supports fast access to junction by
                                       //spatial input
   BTree* sectionsBTree;  //supports fast access to sections by sections id
   R_Tree<2,TupleId>* sectionsRTree;   //supports fast access to sections by
                                       //spatial input
   BTree* routesBTree; //supports fast access to routes by route id

/*
1.1.1 Default Constructor

The default constructor should only be used in the cast function and is
therefore declared to be private.

*/

JNetwork();


/*
1.1.1 Return list representation of internal relations

*/

   ListExpr JunctionsToList() const;
   ListExpr SectionsToList() const;
   ListExpr RoutesToList() const;
   ListExpr NetdistancesToList() const;

/*
1.1.1 create internal trees

*/

  void CreateTrees();

/*
1.1.1 Initialize netdistances relation by section lengths between junctions

*/

   void InitNetdistances();
   void InsertNetdistanceTuple(const int fromjid, const JPQEntry* entry);
   void InsertNetdistanceTuple(const int fromjid, const JPQEntry* entry,
                               DbArray<InterSP>* wayEntries);
   void InsertNetdistanceTuple(const int fromjid, const int tojid,
                               const int viajid, const int viasid,
                               const double dist);

/*
1.1.1.1 Attributes of Netdistance Relation

*/

   int GetNetdistanceNextSID(const Tuple* actNetDistTup) const;

/*
1.1.1.1 DirectConnectionExists

Checks Network for a direct connection between source and target which are
expected to be on the same route and are allocated in the sections given by
sourceSectTup and targetSectTup. If the connection exists true is returned and
the connecting route interval is returned as result.

*/

 bool DirectConnectionExists(const int startSID, const int endSID,
                             const Tuple* sourceSectTup,
                             const Tuple* targetSectTup,
                             const RouteLocation& source,
                             const RouteLocation& target,
                             DbArray<JRouteInterval>* res,
                             double& length) const;

/*
Returns the connecting route interval if an direct connection between source
and target exists.

*/
 JRouteInterval* DirectConnection(const int sectId,
                                  const RouteLocation& source,
                                  const RouteLocation& target) const;

/*
1.1.1.1 AddAdjacentSections

Adds the sections with id from listSID to priority queue.

*/

template<class SpatialTarget>
void AddAdjacentSections(PQManagement* pq, JPQEntry curEntry,
                        const SpatialTarget* targetPos);

template<class SpatialTarget>
void AddAdjacentSections(PQManagement* pq, const JListInt* listSID,
                        JPQEntry curEntry, const SpatialTarget* targetPos);

void AddAdjacentSections(PQManagement* pq, JPQEntry curEntry,
                         JLine* result, const double distLimit);

void AddAdjacentSections(PQManagement* pq, const JListInt* listSID,
                         JPQEntry curEntry, JLine* result,
                         const double distLimit);

void AddReverseAdjacentSections(PQManagement* pq, JPQEntry curEntry);

void AddReverseAdjacentSections(PQManagement* pq, const JListInt* listSID,
                                JPQEntry curEntry);

void AddReverseAdjacentSections(PQManagement* pq, JPQEntry curEntry,
                                JLine* result, const double distLimit);

void AddReverseAdjacentSections(PQManagement* pq, const JListInt* listSID,
                                JPQEntry curEntry , JLine* result,
                                const double distLimit);

/*
1.1.1.1 WriteShortestPath

Writes the route intervals of the shortest path from source to target in res.
Uses netdistance table.

*/

void WriteShortestPath(const DbArray<PosJNetSpatial>* sources,
                      const DbArray<PosJNetSpatial>* targets,
                      const int srcStartPathJID, const int tgtPosInArray,
                      const bool tgtOverStartJunction,
                      DbArray<JRouteInterval>*& result);

/*
1.1.1.1 WriteNetdistance

Computes the network distance from sources to targets and stores it in result.

*/

void WriteNetdistance(const DbArray<PosJNetSpatial>* sources,
                      const DbArray<PosJNetSpatial>* targets,
                      const int srcStartPathJID, const int tgtPosInArray,
                      const bool tgtOverStartJunction,
                      CcReal* res);

/*
1.1.1.1 ExistsCommonRoute

Returns true if src and tgt can be mapped to the same route. In this case
one or both are changed to the values belonging to the same route.

*/

bool ExistsCommonRoute(RouteLocation& src, RouteLocation& tgt) const;

/*
1.1.1.1 GetRLocOfPosOnRouteInterval

Returns the RouteLocation with distance pos from the start of the route
interval actInt.

*/

RouteLocation* GetRLocOfPosOnRouteInterval(
const JRouteInterval* actInt, const double pos) const;

/*
1.1.1.1 SplitJunit

Returns the corresponding spatial mpoint representation of the junit.

*/

void SplitJUnit(const JUnit& ju, JRouteInterval*& lastRint,
                SimpleLine*& lastCurve, bool& endTimeCorrected,
                Instant& lastEndTime, Point*& lastEndPoint, LRS& lrs,
                int& lrspos, MPoint& result) const;

void SplitJUnit(const JUnit& ju, int& curRid, JRouteInterval*& lastRint,
    JListInt*& routeSectList, int& lastRouteSecListIndex,
    bool& endTimeCorrected, Instant& lastEnd,
    SimpleLine*& lastCurve, MPoint& result) const;

/*
1.1.1.1 CheckTupleForRLoc

Returns true if the the route location is allocated within
the given section, false otherwise. If true is returned pos
describes the distance between the first position of the
RouteInterval describing the section and the route location.

*/

bool CheckTupleForRLoc(const Tuple* actSect,
                       const RouteLocation& rloc,
                       double& pos) const;

/*
1.1.1.1 GetSpatialValueOf

Returns the spatial position of the given rloc in the network.

*/

Point* GetSpatialValueOf(const RouteLocation& rloc) const;
Point* GetSpatialValueOf(const RouteLocation& rloc,
                         const JListInt* routeSectList) const;
Point* GetSpatialValueOf(const RouteLocation& rloc, int& curRid,
                         JListInt*& routeSectList,
                         int& index, JRouteInterval*& lastRint,
                         SimpleLine*& lastCurve) const;
Point* GetSpatialValueOf(const RouteLocation& rloc,
                         JRouteInterval*& routeRint,
                         SimpleLine*& routeCurve,
                         LRS& lrs, int& lrspos) const;
Point* GetSpatialValueOf(const RouteLocation& rloc, int& curRid,
                         JListInt*& routeSectList, int& index,
                         JRouteInterval*& lastRint,
                         SimpleLine*& lastCurve,
                         LRS& lrs, int& lrspos) const;
Point* GetSpatialValueOf(const RouteLocation& rloc, double relpos,
                        const Tuple* actSect)const;

void GetSpatialValueOf(const JRouteInterval& rint,
                       const JListInt* sectList,
                       const int fromIndex,
                       const int toIndex,
                       SimpleLine& result) const;

/*
1.1.1.1.1 ConnectTargetsAndSpatialPositions

Connects the route locations of targets with their spatial positions for
A-Star Path Computing, and some other parameters used later in path computing.

*/

void ConnectSpatialPositions(const DbArray<RouteLocation>* targets,
                            DbArray<PosJNetSpatial>* tgtEntries,
                            const bool start,
                            Points* spatialPositions = 0);

/*
1.1.1.1.1 InitPriorityQueue

Initializes the Priority Queue pqueue for shortest path computation with the
values computed from sources. endPositions are needed to compute the priority
value for shortest path computation.

*/

bool InitPriorityQueue(PQManagement* pqueue,
                       const DbArray< PosJNetSpatial>* sources,
                       const Points* endPositions);

void InitPriorityQueue(PQManagement* pqueue, const RouteLocation& source,
                       DbArray<PairIntDouble>* result);

void InitPriorityQueue(PQManagement* pqueue, const RouteLocation& source,
                       const double distLimit, JLine* result);

void InitReversePriorityQueue(PQManagement* pqueue, const RouteLocation& source,
                              DbArray<PairIntDouble>* result);

void InitReversePriorityQueue(PQManagement* pqueue, const RouteLocation& source,
                              const double distLimit, JLine* result);

/*
1.1.1.1.1. Some Helpful Functions for Priority Queue Initialization

*/

void WriteToLists(PQManagement* pqueue, DbArray<PairIntDouble>* result,
                  JPQEntry& junc);

void WriteToLists(PQManagement* pqueue, JLine* result, JPQEntry& junc,
                  const double distLimit, const RouteLocation& src,
                  const bool up, const Direction* sectDir);

void WriteToLists(PQManagement* pqueue, JPQEntry& junc);

void WriteToLists(PQManagement* pqueue,  DbArray<PairIntDouble>* result,
                  JPQEntry& start, JPQEntry& end, const Tuple* sectTup);

void WriteToLists(PQManagement* pqueue,  JLine* result, JPQEntry& start,
                  JPQEntry& end, const Tuple* sectTup, const double distLimit,
                  const RouteLocation& src, const Direction* sectDir);

void WriteToLists(PQManagement* pqueue, JPQEntry& start, JPQEntry& end,
                  const Tuple* sectTup);

void WriteToReverseLists(PQManagement* pqueue,
                         DbArray<PairIntDouble>* result,
                         JPQEntry& junc);

void WriteToReverseLists(PQManagement* pqueue, JLine* result, JPQEntry& junc,
                         const double distLimit, const RouteLocation& src,
                         const bool up, const Direction* sectDir);

void WriteToReverseLists(PQManagement* pqueue, JPQEntry& junc);

void WriteToReverseLists(PQManagement* pqueue,
                         DbArray<PairIntDouble>* result,
                         JPQEntry& start,
                         JPQEntry& end,
                         const Tuple* sectTup);

void WriteToReverseLists(PQManagement* pqueue, JLine* result, JPQEntry& start,
                         JPQEntry& end, const Tuple* sectTup,
                         const double distLimit, const RouteLocation& src,
                         const Direction* sectDir);

void WriteToReverseLists(PQManagement* pqueue, JPQEntry& start,
                         JPQEntry& end, const Tuple* sectTup);

/*
1.1.1.1.1 CheckForSameSections

Checks if their are paris of sources and targets which are on the same section.
If this is the case and a direct connection between the two places exists
the shortest connection is selected, stored as JRouteInterval in sp and true
returnes. Otherwise we return false.

*/

bool CheckForSameSections(const DbArray< PosJNetSpatial >* sources,
                          const DbArray< PosJNetSpatial >* targets,
                          DbArray< JRouteInterval >*& sp);

/*
Returns true if for each pair of srcEntries and tgtEntires a shortest path
exists. The start and end values and the length of the shortest existing path
is returned, independently from the boolean result.

*/

bool CheckForExistingNetdistance(const DbArray<PosJNetSpatial>* srcEntries,
                                 const DbArray<PosJNetSpatial>* tgtEntries,
                                 int& srcStartPathJID,
                                 int& tgtPosInArray,
                                 bool& tgtOverStartJunction,
                                 double& minDist);

bool ExistsNetdistancesFor(const PosJNetSpatial& start,
                           const PosJNetSpatial& end,
                           double& minDist,
                           int& srcStartPathJID,
                           bool& tgtOverStartJunction);

bool ExistsNetdistanceFor(const int startjid, const double startdist,
                          const int endjid, const double enddist,
                          const bool endJIDisStartJunc,
                          double& minDist,
                          int& srcStartPathJID,
                          bool& tgtOverStartJunc);

/*
Returns the shortest path given by the priority queue to the element of
targets with the shortest distance between the sources of the priority queue
as DbArray of jrint.

*/
bool ProcessPriorityQueue(PQManagement* pqueue,
                                       const DbArray< PosJNetSpatial >* targets,
                                       const Points* spatialPosTargets,
                                       int& tgtPosInArray,
                                       bool& tgtOverStartJunc,
                                       int& srcStartPathJID,
                                       double& minDist);

void ProcessPriorityQueue(PQManagement* pqueue,
                          DbArray<PairIntDouble>* result,
                          const double distLimit);

void ProcessPriorityQueue(PQManagement* pqueue,
                          const double distLimit,
                          JLine* result);

void ProcessReversePriorityQueue(PQManagement* pqueue,
                                 DbArray<PairIntDouble>* result,
                                 const double distLimit);

void ProcessReversePriorityQueue(PQManagement* pqueue,
                                 const double distLimit,
                                 JLine* result);

};

} // end of namespace jnetwork
/*
1 Overwrite output operator

*/

using namespace jnetwork;
ostream& operator<< (ostream& os, const jnetwork::JNetwork& n);



#endif // JNETWORK_H
