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

2012, November Simone Jandt

1 Defines and Includes

*/

#include "JNetUtil.h"
#include "RouteLocation.h"
#include "JRouteInterval.h"
#include "BTreeAlgebra.h"
#include "RelationAlgebra.h"
#include "StandardTypes.h"
#include "JList.h"
#include "OrderedRelationAlgebra.h"
#include "RTreeAlgebra.h"

using namespace jnetwork;

/*
1 Implementation of class ~JNetUtil~

1.1.1. Often used strings

*/

string JNetUtil::GetJunctionsBTreeTypeInfo()
{
  return "("+ BTree::BasicType() + JNetUtil::GetJunctionsTupleTypeInfo() +
         CcInt::BasicType() + ")";
}

string JNetUtil::GetJunctionsRelationTypeInfo()
{
  return "("+ Relation::BasicType() +
         JNetUtil::GetJunctionsTupleTypeInfo() + ")";
}

string JNetUtil::GetJunctionsRTreeTypeInfo()
{
  return "("+ R_Tree<2,TupleId>::BasicType() +
         JNetUtil::GetJunctionsTupleTypeInfo() + Point::BasicType() +
         " FALSE)";
}

string JNetUtil::GetJunctionsTupleTypeInfo()
{
  return "(" + Tuple::BasicType() + "((Id " + CcInt::BasicType() + ") (Pos " +
         Point::BasicType() + ") (Listjuncpos " +  JListRLoc::BasicType() +
         ") (Listinsections " + JListInt::BasicType() + ") (Listoutsections " +
         JListInt::BasicType() + ")))";
}

string JNetUtil::GetNetdistancesRelationTypeInfo()
{
  return "("+ OrderedRelation::BasicType() +
         JNetUtil::GetNetdistancesTupleTypeInfo() +" (Source))";
}

string JNetUtil::GetNetdistancesTupleTypeInfo()
{
  return  "("+ Tuple::BasicType() + "((Source " + CcInt::BasicType() +
          ")(Target " + CcInt::BasicType() + ")(NextJunct " +
          CcInt::BasicType() + ")(ViaSect " + CcInt::BasicType() +
          ")(NetDist " + CcReal::BasicType() +")))";
}

string JNetUtil::GetRoutesBTreeTypeInfo()
{
  return "("+ BTree::BasicType() + JNetUtil::GetRoutesTupleTypeInfo() +
         CcInt::BasicType() + ")";
}

string JNetUtil::GetRoutesRelationTypeInfo()
{
  return "("+ Relation::BasicType() + JNetUtil::GetRoutesTupleTypeInfo() + ")";
}

string JNetUtil::GetRoutesTupleTypeInfo()
{
  return "(" + Tuple::BasicType() + "((Id " + CcInt::BasicType() +
         ") (ListJunctions " + JListInt::BasicType() + ") (ListSections " +
         JListInt::BasicType() + ") (Lenth " + CcReal::BasicType() + ")))";
}

string JNetUtil::GetSectionsBTreeTypeInfo()
{
  return "("+ BTree::BasicType() + JNetUtil::GetSectionsTupleTypeInfo() +
          CcInt::BasicType() + ")";

}

string JNetUtil::GetSectionsRelationTypeInfo()
{
  return "("+ Relation::BasicType() + JNetUtil::GetSectionsTupleTypeInfo() +
         ")";
}

string JNetUtil::GetSectionsRTreeTypeInfo()
{
  return  "("+ R_Tree<2, TupleId>::BasicType() +
          JNetUtil::GetSectionsTupleTypeInfo() + SimpleLine::BasicType() +
          " FALSE)";
}

string JNetUtil::GetSectionsTupleTypeInfo()
{
  return "(" + Tuple::BasicType() + "((Id " + CcInt::BasicType() + ") (Curve " +
         SimpleLine::BasicType() + ") (StartJunctionId " + CcInt::BasicType() +
         ") (EndJunctionId " + CcInt::BasicType() + ") (Direction " +
         Direction::BasicType() + ") (VMax " + CcReal::BasicType() +
         ") (Lenth " + CcReal::BasicType() + ") (ListSectRouteIntervals " +
         JListRInt::BasicType() + ") (ListAdjSectUp " + JListInt::BasicType() +
         ") (ListAdjSectDown " + JListInt::BasicType() +
         ") (ListRevAdjSectUp " + JListInt::BasicType() +
         ") (ListRevAdjSectDown " + JListInt::BasicType() + ")))";
}

/*
1.1.1. DbArray Methods

*/

int JNetUtil::GetIndexOfJRouteIntervalForJRInt(
    const DbArray< JRouteInterval >& list, const JRouteInterval rint,
    const int startindex, const int endindex)
{
  if (rint.IsDefined() && startindex > -1 && endindex < list.Size() &&
      startindex <= endindex)
  {
    JRouteInterval ri;
    int mid = (endindex + startindex)/ 2;
    list.Get(mid, ri);
    if (ri.Overlaps(rint, false))
    {
      return mid;
    }
    else
    {
      switch(ri.Compare(rint))
      {
        case -1:
        {
          if (mid != startindex)
            return JNetUtil::GetIndexOfJRouteIntervalForJRInt(list,rint, mid,
                                                              endindex);
          else
            return JNetUtil::GetIndexOfJRouteIntervalForJRInt(list,rint, mid+1,
                                                              endindex);
          break;
        }

        case 1:
        {
          if (mid != endindex)
            return JNetUtil::GetIndexOfJRouteIntervalForJRInt(list,rint,
                                                              startindex, mid);
          else
            return JNetUtil::GetIndexOfJRouteIntervalForJRInt(list,rint,
                                                              startindex,
                                                              mid-1);

          break;
        }

        default: //should never been reached
        {
          assert(false);
          return -1;
          break;
        }
      }
    }
  }
  return -1;
}

int JNetUtil::GetIndexOfJRouteIntervalForRLoc(
    const DbArray< JRouteInterval >& list, const RouteLocation& rloc,
    const int startindex, const int endindex)
{
  if (!rloc.IsDefined() || startindex < 0 || endindex > list.Size()-1 ||
      startindex > endindex)
    return -1;
  else
  {
    JRouteInterval ri;
    int mid = (startindex + endindex)/ 2;
    list.Get(mid, ri);
    if (ri.GetRouteId() < rloc.GetRouteId())
    {
      return JNetUtil::GetIndexOfJRouteIntervalForRLoc(list, rloc,
                                                       mid+1,endindex);
    }
    else if (ri.GetRouteId() > rloc.GetRouteId())
    {
      return JNetUtil::GetIndexOfJRouteIntervalForRLoc(list, rloc,
                                                       startindex, mid-1);
    }
    else
    {
      int testmid = mid;
      while (testmid < list.Size() && testmid > -1)
      {
        list.Get(testmid, ri);
        if (ri.GetRouteId() == rloc.GetRouteId())
        {
          if (ri.Contains(rloc))
            return testmid;
          else
            testmid++;
        }
        else
          testmid = list.Size();
      }
      testmid = mid;
      while (testmid > -1 && testmid < list.Size())
      {
        list.Get(testmid, ri);
        if (ri.GetRouteId() == rloc.GetRouteId())
        {
          if (ri.Contains(rloc))
            return testmid;
          else
            testmid--;
        }
        else
          testmid = -1;
      }
      return -1;
    }
  }
}

int JNetUtil::GetIndexOfRouteLocationFor(const DbArray< RouteLocation >& list,
                                         const RouteLocation& rloc,
                                         const int startindex,
                                         const int endindex)
{
  if (!rloc.IsDefined() || startindex < 0 || endindex > list.Size()-1 ||
      startindex > endindex)
    return -1;
  else
  {
    RouteLocation rl;
    int mid = (startindex + endindex)/ 2;
    list.Get(mid, rl);
    if (rl.GetRouteId() < rloc.GetRouteId())
      return JNetUtil::GetIndexOfRouteLocationFor(list, rloc,
                                                   mid+1,endindex);
    else if (rl.GetRouteId() > rloc.GetRouteId())
      return JNetUtil::GetIndexOfRouteLocationFor(list, rloc,
                                                  startindex, mid-1);
    else if (rl.GetPosition()< rloc.GetPosition())
      return JNetUtil::GetIndexOfRouteLocationFor(list, rloc,
                                                    mid+1,endindex);
    else if (rl.GetPosition() > rloc.GetPosition())
      return JNetUtil::GetIndexOfRouteLocationFor(list, rloc,
                                                   startindex, mid-1);
    else if (rl.GetSide().SameSide(rloc.GetSide(),false))
      return mid;
    else
      return -1;
  }
}


bool JNetUtil::ArrayContainIntersections(const DbArray< JRouteInterval >& lhs,
                                         const DbArray< JRouteInterval >& rhs)
{
  int i = 0;
  int j = 0;
  JRouteInterval lhsRInt, rhsRInt;
  while (i < lhs.Size() && j < rhs.Size())
  {
    lhs.Get(i, lhsRInt);
    rhs.Get(j, rhsRInt);
    if (lhsRInt.Overlaps(rhsRInt, false))
      return true;
    switch(lhsRInt.Compare(rhsRInt))
    {
      case 1:
      {
        j++;
        break;
      }
      case -1:
      {
        i++;
        break;
      }
      default: //should never happen
      {
        assert(false);
        i = lhs.Size();
        j = rhs.Size();
        break;
      }
    }
  }
  return false;
}

bool JNetUtil::ArrayContainIntersections(const DbArray< RouteLocation >& lhs,
                                         const DbArray< RouteLocation >& rhs)
{
  RouteLocation lhsRLoc(false);
  RouteLocation rhsRLoc(false);
  int i = 0;
  int j = 0;
  while (i < lhs.Size() && j < rhs.Size())
  {
    lhs.Get(i,lhsRLoc);
    rhs.Get(j,rhsRLoc);
    if (lhsRLoc.IsOnSamePlace(rhsRLoc))
      return true;
    else if (lhsRLoc.GetRouteId() < rhsRLoc.GetRouteId())
      i++;
    else if (lhsRLoc.GetRouteId() > rhsRLoc.GetRouteId())
      j++;
    else if (lhsRLoc.GetPosition() < rhsRLoc.GetPosition())
      i++;
    else if (lhsRLoc.GetPosition() > rhsRLoc.GetPosition())
      j++;
    else
    {
      i++;
      j++;
    }
  }
  return false;
}

bool JNetUtil::ArrayContainIntersections(const DbArray< JRouteInterval >& lhs,
                                         const DbArray< RouteLocation >& rhs)
{
  JRouteInterval rint(false);
  RouteLocation rloc(false);
  int i = 0;
  int j = 0;
  while (i < lhs.Size() && j < rhs.Size())
  {
    lhs.Get(i, rint);
    rhs.Get(j,rloc);
    if (rint.Contains(rloc))
      return true;
    else if (rint.GetRouteId() < rloc.GetRouteId())
      i++;
    else if (rint.GetRouteId() > rloc.GetRouteId())
      j++;
    else if (rint.GetLastPosition() < rloc.GetPosition())
      i++;
    else if (rint.GetFirstPosition() > rloc.GetPosition())
      j++;
    else
    {
      i++;
      j++;
    }
  }
  return false;
}
