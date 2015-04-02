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

1 Includes

*/

#include "JUnit.h"

using namespace jnetwork;

/*
1 Implementation of class JUnit

1.1 Constructors and Deconstructors

*/

  JUnit::JUnit() : Attribute()
  {}

  JUnit::JUnit(const bool def) :
      Attribute(def),
      timeInter(Interval<Instant>(instanttype,instanttype,false,false)),
      routeInter(false)
  {}

  JUnit::JUnit(const JUnit& other) :
      Attribute(other.IsDefined()),
      timeInter(Interval<Instant>(instanttype,instanttype,false,false)),
      routeInter(false)
  {
    if (other.IsDefined())
    {
      timeInter = other.GetTimeInterval();
      routeInter = other.GetRouteInterval();
    }
  }

  JUnit::JUnit(const Interval<Instant>& inst, const JRouteInterval& rint) :
    Attribute(inst.IsDefined() && rint.IsDefined()),
    timeInter(Interval<Instant>(instanttype,instanttype,false,false)),
    routeInter(false)
  {
    if (inst.IsDefined() && rint.IsDefined())
    {
      timeInter = inst;
      routeInter = rint;
    }
  }

  JUnit::~JUnit()
  {}

/*
1.1.1 Getter and Setter for private Attributes

*/

  Interval<Instant> JUnit::GetTimeInterval() const
  {
    return timeInter;
  }

  JRouteInterval JUnit::GetRouteInterval() const
  {
    return routeInter;
  }

  RouteLocation* JUnit::GetStartRLoc() const
  {
    return new RouteLocation(routeInter.GetRouteId(),
                             routeInter.GetStartPosition(),
                             routeInter.GetSide());
  }

  RouteLocation* JUnit::GetEndRLoc() const
  {
    return new RouteLocation(routeInter.GetRouteId(),
                             routeInter.GetEndPosition(),
                             routeInter.GetSide());
  }

  double JUnit::GetSpeed() const
  {
    return routeInter.GetLength() /
               ((timeInter.end.ToDouble() - timeInter.start.ToDouble())/
                 0.00001157);
  }

double JUnit::GetLength() const
{
  return routeInter.GetLength();
}


  void JUnit::SetTimeInterval(const Interval<Instant>& inst)
  {
    timeInter = inst;
  }

  void JUnit::SetRouteInterval(const JRouteInterval& ri)
  {
    routeInter = ri;
  }

/*
1.1.1 Override Methods from Attribute

*/

  void JUnit::CopyFrom(const Attribute* right)
  {
    *this = *((JUnit*)right);
  }

  Attribute::StorageType JUnit::GetStorageType() const
  {
    return Default;
  }

  size_t JUnit::HashValue() const
  {
    return routeInter.HashValue() +
           (size_t) (timeInter.start + timeInter.end).ToDouble() +
           (size_t) (timeInter.lc + timeInter.rc);
  }

  JUnit* JUnit::Clone() const
  {
    return new JUnit(*this);
  }

  bool JUnit::Adjacent(const Attribute* attrib) const
  {
    return false;
  }

  int JUnit::Compare(const void* ls, const void* rs)
  {
    JUnit lp(*(JUnit*) ls);
    JUnit rp(*(JUnit*) rs);
    return lp.Compare(rp);
  }

  int JUnit::Compare(const Attribute* rhs) const
  {
    return Compare(*((JUnit*)rhs));
  }

  int JUnit::Compare(const JUnit& rhs) const
  {
    if (!IsDefined() && !rhs.IsDefined()) return 0;
    if (IsDefined() && !rhs.IsDefined()) return 1;
    if (!IsDefined() && rhs.IsDefined()) return -1;
    int test = timeInter.CompareTo(rhs.GetTimeInterval());
    if (test != 0) return test;
    return routeInter.Compare(rhs.GetRouteInterval());
  }

    size_t JUnit::Sizeof() const
    {
      return sizeof(JUnit);
    }

    ostream& JUnit::Print(ostream& os) const
    {
      if (IsDefined())
      {
        os << "time: ";
        timeInter.Print(os);
        os << "positions: ";
        routeInter.Print(os);
      }
      else
        os << Symbol::UNDEFINED() << endl;
      return os;
    }

    const string JUnit::BasicType()
    {
      return "junit";
    }

    const bool JUnit::checkType(const ListExpr type)
    {
      return listutils::isSymbol(type, BasicType());
    }

/*
1.1.1 Standard Operators

*/

JUnit& JUnit::operator=(const JUnit& other)
{
  SetDefined(other.IsDefined());
  if (other.IsDefined())
  {
    timeInter = other.GetTimeInterval();
    routeInter = other.GetRouteInterval();
  }
  return *this;
}

bool JUnit::operator==(const JUnit& other) const
{
  return (Compare(other) == 0);
}

bool JUnit::operator!=(const JUnit& other) const
{
  return (Compare(other) != 0);
}

bool JUnit::operator<(const JUnit& other) const
{
  return (Compare(other) < 0);
}

bool JUnit::operator<=(const JUnit& other) const
{
  return (Compare(other) < 1);
}

bool JUnit::operator>(const JUnit& other) const
{
  return (Compare(other) > 0);
}

bool JUnit::operator>=(const JUnit& other) const
{
  return (Compare(other) > -1);
}

/*
1.1.1 Operators for Secondo Integration

*/

ListExpr JUnit::Out(ListExpr typeInfo, Word value)
{
  JUnit* in = (JUnit*) value.addr;
  if (!in->IsDefined())
  {
    return nl->SymbolAtom(Symbol::UNDEFINED());
  }
  else
  {
    Interval<Instant> time(in->GetTimeInterval());
    Instant start =  time.start;
    Instant end = time.end;
    JRouteInterval rint(in->GetRouteInterval());
    return nl->TwoElemList(
      nl->FourElemList(OutDateTime(nl->TheEmptyList(), SetWord(&start)),
                       OutDateTime(nl->TheEmptyList(), SetWord(&end)),
                       nl->BoolAtom(time.lc), nl->BoolAtom(time.rc)),
      JRouteInterval::Out(nl->TheEmptyList(), SetWord((void*) &rint)));
  }
}

Word JUnit::In(const ListExpr typeInfo, const ListExpr instance,
               const int errorPos, ListExpr& errorInfo, bool& correct)
{
  correct = true;
  if ( listutils::isSymbolUndefined( instance ) )
  {
    JUnit* p = new JUnit(false);
    correct = true;
    return SetWord( p );
  }
  else
  {
    if (nl->ListLength(instance) != 2)
    {
      correct = false;
      cmsg.inFunError("list length should be 1 or 2");;
      return SetWord(Address(0));
    }

    ListExpr timeList = nl->First(instance);
    ListExpr rintList = nl->Second(instance);

    if (nl->ListLength(timeList) != 4 ||
       !nl->IsAtom(nl->First(timeList)) || !nl->IsAtom(nl->Second(timeList)) ||
       !nl->IsAtom(nl->Third(timeList)) || !nl->IsAtom(nl->Fourth(timeList)) ||
       !nl->AtomType(nl->Third(timeList) == BoolType) ||
       !nl->AtomType(nl->Fourth(timeList) == BoolType))
    {
      cmsg.inFunError("timeintervallist must have length 4.");
      correct = false;
      return SetWord(Address(0));
    }

    Instant* start = (Instant*) InInstant(nl->TheEmptyList(),
                                          nl->First(timeList), errorPos,
                                          errorInfo, correct).addr;

    Instant* end = (Instant*) InInstant(nl->TheEmptyList(),
                                        nl->Second(timeList), errorPos,
                                        errorInfo, correct).addr;

    bool lc = nl->BoolValue(nl->Third(timeList));
    bool rc = nl->BoolValue(nl->Fourth(timeList));

    if (!correct)
    {
      cmsg.inFunError("At least one time instant is invalid.");
      delete start;
      delete end;
      return SetWord(Address(0));
    }
    Interval<Instant> interval(*start,*end, lc, rc);
    if (!interval.IsValid())
    {
      cmsg.inFunError("Invalid TimeInterval");
      delete start;
      delete end;
      correct = false;
      return SetWord(Address(0));
    }

    delete start;
    delete end;

    JRouteInterval* rint =
      (JRouteInterval*) JRouteInterval::In(nl->TheEmptyList(), rintList,
                                           errorPos, errorInfo, correct).addr;
    if (!correct)
    {
      cmsg.inFunError("Error in route interval list.");
      return SetWord(Address(0));
    }

    JUnit* out = new JUnit(interval, *rint);
    rint->DeleteIfAllowed();
    return SetWord(out);
  }
}

Word JUnit::Create(const ListExpr typeInfo)
{
  return SetWord(new JUnit(true));
}

void JUnit::Delete( const ListExpr typeInfo, Word& w )
{
  ((JUnit*) w.addr)->DeleteIfAllowed();
  w.addr = 0;
}

void JUnit::Close( const ListExpr typeInfo, Word& w )
{
  ((JUnit*) w.addr)->DeleteIfAllowed();
  w.addr = 0;
}

Word JUnit::Clone( const ListExpr typeInfo, const Word& w )
{
  return SetWord(new JUnit(*(JUnit*) w.addr));
}

void* JUnit::Cast( void* addr )
{
  return (new (addr) JUnit);
}

bool JUnit::KindCheck ( ListExpr type, ListExpr& errorInfo )
{
  return checkType(type);
}

int JUnit::SizeOf()
{
  return sizeof(JUnit);
}

ListExpr JUnit::Property()
{
  return nl->TwoElemList(
    nl->FourElemList(
      nl->StringAtom("Signature"),
      nl->StringAtom("Example Type List"),
      nl->StringAtom("List Rep"),
      nl->StringAtom("Example List")),
      nl->FourElemList(
        nl->StringAtom("-> " + Kind::TEMPORAL()),
        nl->StringAtom(BasicType()),
        nl->TextAtom("( <timeInterval> " +  JRouteInterval::BasicType() +
        "), describes the positions of an mjpoint within the time interval."),
        nl->TextAtom(Example())));
}

/*
1.1.1 Other Operations

*/
string JUnit::Example()
{
  return "(((instant 0.5) (instant 0.6) TRUE FALSE)" +
           JRouteInterval::Example() + ")";
}

/*
1.1.1.1 ExtendedBy

*/


bool JUnit::ExtendBy(const JUnit& other)
{
  if (CanBeExtendedBy(other))
  {
    JRouteInterval otherRint = other.GetRouteInterval();
    Interval<Instant> otherTime = other.GetTimeInterval();
    SetTimeInterval(Interval<Instant> (timeInter.start,
                                       otherTime.end,
                                       timeInter.lc,
                                       otherTime.rc));
    Direction compD(Down);
    if (routeInter.GetSide() == compD)
    {
      SetRouteInterval(JRouteInterval(routeInter.GetRouteId(),
                                      otherRint.GetFirstPosition(),
                                      routeInter.GetLastPosition(),
                                      routeInter.GetSide()));
    }
    else
    {
      SetRouteInterval(JRouteInterval(routeInter.GetRouteId(),
                                      routeInter.GetFirstPosition(),
                                      otherRint.GetLastPosition(),
                                      routeInter.GetSide()));
    }
    return true;
  }
  else
   return Contains(other);
}

/*
1.1.1.1 Boxes

*/

Rectangle<3> JUnit::TempNetBox() const
{
  if (IsDefined())
    return Rectangle<3>(true,
                       (double) GetRouteInterval().GetRouteId(),
                       (double) GetRouteInterval().GetRouteId(),
                       GetRouteInterval().GetFirstPosition(),
                       GetRouteInterval().GetLastPosition(),
                       GetTimeInterval().start.ToDouble(),
                       GetTimeInterval().end.ToDouble());
  else
    return Rectangle<3>(false,0.0,0.0,0.0,0.0,0.0,0.0);
}

Rectangle<2> JUnit::NetBox() const
{
  if (IsDefined())
    return routeInter.NetBox();
  else
    return Rectangle<2>(false,0.0,0.0,0.0,0.0);
}

/*
1.1.1.1.1 AtInstant

*/

IJPoint JUnit::AtInstant(const Instant* inst, const string netId) const
{
  if (!IsDefined() || inst == 0 || !inst->IsDefined() ||
      !timeInter.Contains(*inst,true))
    return IJPoint(false);
  else
  {
    if (*inst == timeInter.start)
      return IJPoint(*inst, JPoint(netId, routeInter.GetStartLocation(),
                                   false));
    else
    {
      if (*inst == timeInter.end)
        return IJPoint(*inst, JPoint(netId, routeInter.GetEndLocation(),
                                     false));
      else
        return IJPoint(*inst,
                       JPoint(netId,
                              RouteLocation(routeInter.GetRouteId(),
                                            PosAtTime(inst),
                                            routeInter.GetSide()),
                              false));
    }
  }
}

/*
1.1.1.1 Initial

*/

IJPoint JUnit::Initial(const string netId) const
{
  if (IsDefined())
    return IJPoint(timeInter.start,
                   JPoint(netId, routeInter.GetStartLocation(), false));
  else
    return IJPoint(false);
}

/*
1.1.1.1 AtPos

*/

JUnit* JUnit::AtPos(const JPoint* jp) const
{
  if (IsDefined() && jp != 0 && jp->IsDefined() &&
      routeInter.Contains(jp->GetLocation()))
  {
    if (routeInter.GetFirstPosition() != routeInter.GetLastPosition())
    {
      Instant time = TimeAtPos(jp->GetLocation().GetPosition());
      return new JUnit(Interval<Instant> (time, time, true, true),
                       JRouteInterval(routeInter.GetRouteId(),
                                  jp->GetLocation().GetPosition(),
                                  jp->GetLocation().GetPosition(),
                                  routeInter.GetSide()));
    }
    else
    {
      if (timeInter.start != timeInter.end)
      {
        return new JUnit(Interval<Instant> (timeInter.start, timeInter.end,
                                            false, false),
                         JRouteInterval(routeInter.GetRouteId(),
                                        jp->GetLocation().GetPosition(),
                                        jp->GetLocation().GetPosition(),
                                        routeInter.GetSide()));
      }
      else
      {
        return new JUnit(Interval<Instant> (timeInter.start, timeInter.end,
                                            true, true),
                         JRouteInterval(routeInter.GetRouteId(),
                                        jp->GetLocation().GetPosition(),
                                        jp->GetLocation().GetPosition(),
                                        routeInter.GetSide()));
      }
    }
  }
  else
    return 0;
}

/*
1.1.1.1 AtRint

*/

JUnit* JUnit::AtRint(const JRouteInterval* rint, bool& lastrc) const
{
  if (IsDefined() && rint != 0 && rint->IsDefined())
  {
    Direction compD(Down);
    Instant startTime, endTime;
    double startpos, endpos;
    bool lc = !lastrc;
    bool rc = false;
    lastrc = false;
    if (rint->Contains(routeInter))
    {
      startpos = routeInter.GetStartPosition();
      endpos = routeInter.GetEndPosition();
      startTime = timeInter.start;
      endTime = timeInter.end;
    }
    else
    {
      if (rint->Inside(routeInter))
      {
        if(routeInter.GetSide() != compD)
        {
          startpos = rint->GetFirstPosition();
          endpos = rint->GetLastPosition();
        }
        else
        {
          startpos = rint->GetLastPosition();
          endpos = rint->GetFirstPosition();
        }
        startTime = TimeAtPos(startpos);
        endTime = TimeAtPos(endpos);
      }
      else
      {
        if (rint->Contains(routeInter.GetStartLocation()) &&
            rint->GetEndLocation() != routeInter.GetStartLocation())
        {
          startpos = routeInter.GetStartPosition();
          startTime = timeInter.start;
          if (routeInter.GetSide() != compD)
            endpos = rint->GetLastPosition();
          else
            endpos = rint->GetFirstPosition();
          endTime = TimeAtPos(endpos);
        }
        else
        {
          if (rint->Contains(routeInter.GetEndLocation()) &&
              rint->GetStartLocation() != routeInter.GetEndLocation())
          {
            endpos = routeInter.GetEndPosition();
            endTime = timeInter.end;
            if (routeInter.GetSide() != compD)
              startpos = rint->GetFirstPosition();
            else
              startpos = rint->GetLastPosition();
            startTime = TimeAtPos(startpos);
          }
          else
          {
            assert(false); //should never been reached
            startpos = 0.0;
            endpos = 0.0;
          }
        }
      }
    }
    if (startTime == endTime)
    {
      lc = true;
      rc = true;
      lastrc = true;
    }
    return new JUnit(Interval<Instant>(startTime, endTime, lc, rc),
                     JRouteInterval(routeInter.GetRouteId(),
                                    startpos, endpos, routeInter.GetSide()));
  }
  return 0;
}

/*
1.1.1.1 CanBeExtendedBy

*/

bool JUnit::CanBeExtendedBy(const JUnit& other) const
{
  return (timeInter.end == other.GetTimeInterval().start &&
          routeInter.Adjacent(other.GetRouteInterval()) &&
          AlmostEqual(GetSpeed(), other.GetSpeed()));
}

/*
1.1.1.1 ~PosAtTime~

*/

double JUnit::PosAtTime(const Instant* inst) const
{
  return routeInter.GetStartPosition() +
         ((routeInter.GetEndPosition() - routeInter.GetStartPosition())*
          ((*inst - timeInter.start)/(timeInter.end - timeInter.start)));
}

JRouteInterval* JUnit::PosAtTimeInterval(const Interval< Instant >& time) const
{
  return new JRouteInterval(routeInter.GetRouteId(),
                            PosAtTime(&time.start),
                            PosAtTime(&time.end),
                            routeInter.GetSide());
}

/*
1.1.1.1 ~TimeAtPos~

*/

Instant JUnit::TimeAtPos(const double pos) const
{
  return (timeInter.end - timeInter.start)*
           (fabs(pos - routeInter.GetStartPosition())/
            fabs(routeInter.GetEndPosition()-routeInter.GetStartPosition()))
         + timeInter.start;
}

/*
1.1.1.1 ~Contains~

*/

bool JUnit::Contains(const JUnit& other) const
{
  return (IsDefined() && other.IsDefined() &&
          timeInter.Contains(other.GetTimeInterval(), true) &&
          routeInter.Contains(other.GetRouteInterval()));
}

/*
1 Overwrite output operator

*/

ostream& operator<< (ostream& os, const JUnit& jp)
{
  jp.Print(os);
  return os;
}
