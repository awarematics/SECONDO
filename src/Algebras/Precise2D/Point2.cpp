/*
 ----
 This file is part of SECONDO.

 Copyright (C) 2009, University in Hagen, Faculty of Mathematics and
 Computer Science, Database Systems for New Applications.

 SECONDO is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published
 by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 SECONDO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SECONDO; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ----

 //paragraph [1] Title: [{\Large \bf \begin{center}] [\end{center}}]
 //paragraph [10] Footnote: [{\footnote{] [}}]
 //[TOC] [\tableofcontents]

 [TOC]

0 Overview

1 Includes and defines

*/

#ifndef _POINT2_CPP
#define _POINT2_CPP

#include "Point2.h"
#include <stdio.h>
#include <iostream>

namespace p2d {

/*
3 Class PointData

*/

/*
3.1 Constructors

*/
PointData::PointData(const int xGrid, const int yGrid) :
  x(xGrid), y(yGrid), xStartPos(0), yStartPos(0),
  xNumOfChars(0), yNumOfChars(0) {
}

PointData::PointData(const PointData& pd) :
  x(pd.x), y(pd.y), xStartPos(pd.xStartPos), yStartPos(pd.yStartPos),
  xNumOfChars(0), yNumOfChars(0) {
}

/*
3.2 Member functions

 These methods fetch the chars representing the given coordinate from the
 Flob using the indices given in this instance's private attributes, and
 convert them to the correct instance of type mpq\_class, representing the value
 of the given coordinate.

*/

/*
3.2.1 ~=~

*/
PointData& PointData::operator=(const PointData& p){
 x = p.x;
 y = p.y;
 xStartPos = p.xStartPos;
 yStartPos = p.yStartPos;
 xNumOfChars = p.xNumOfChars;
 yNumOfChars = p.yNumOfChars;
 return *this;
}

/*
3.2.1 ~getPreciseX~

*/
void PointData::getPreciseX(const Flob* preciseCoordinates,
  mpq_class& result) const {

 if (xNumOfChars == 0) {
  mpq_class theValue(0);
  result = theValue;
  return;
 }
 SmiSize sz = preciseCoordinates->getSize();

 char* s;
 if (sz == 0) {
  s = new char[1];
  s[0] = 0;
 } else {
  s = new char[xNumOfChars + 1];

  bool ok = preciseCoordinates->read(s, xNumOfChars, xStartPos);
  assert(ok);
  s[xNumOfChars] = 0;

 }
 mpq_class theValue(s);
 theValue.canonicalize();
 result = theValue;
 delete s;
}

/*
3.2.1 ~getPreciseY~

*/
void PointData::getPreciseY(const Flob* preciseCoordinates,
  mpq_class& result) const {
 if (yNumOfChars == 0) {
  mpq_class theValue(0);
  result = theValue;
  return;
 }
 SmiSize sz = preciseCoordinates->getSize();
 char* s;
 if (sz == 0 || sz == yStartPos) {
  s = new char[1];
  s[0] = 0;
 } else {
  s = new char[yNumOfChars + 1];

  bool ok = preciseCoordinates->read(s, yNumOfChars, yStartPos);
  assert(ok);

  s[yNumOfChars] = 0;

 }
 mpq_class theValue(s);
 theValue.canonicalize();
 result = theValue;
 delete s;
}

/*
3.2.1 ~getPreciseXAsString~

*/
char* PointData::getPreciseXAsString(const Flob* preciseCoordinates) const {
 if (xNumOfChars == 0) {
  char* s = new char[2];
  s[0] = '0';
  s[1] = 0;
  return s;
 }
 size_t sz = preciseCoordinates->getSize();
 assert(sz >= xStartPos + xNumOfChars);
 char* s = new char[xNumOfChars + 1];
 s[xNumOfChars] = 0;
 bool ok = preciseCoordinates->read(s, xNumOfChars, xStartPos);
 assert(ok);
 assert(s[xNumOfChars] == 0);
 return s;
}

/*
3.2.1 ~getPreciseYAsString~

*/
char* PointData::getPreciseYAsString(const Flob* preciseCoordinates) const {
 if (yNumOfChars == 0) {
  char* s = new char[2];
  s[0] = '0';
  s[1] = 0;
  return s;
 }
 size_t sz = preciseCoordinates->getSize();
 assert(sz >= yStartPos + yNumOfChars);
 char* s = new char[yNumOfChars + 1];
 s[yNumOfChars] = 0;
 bool ok = preciseCoordinates->read(s, yNumOfChars, yStartPos);
 assert(ok);
 assert(s[yNumOfChars] == 0);
 return s;
}

/*
3.2.1 ~setGrid...~

*/
void PointData::SetGridX(int xp) {
 x = xp;
}

void PointData::SetGridY(int yp) {
 y = yp;
}

/*
3.2.1 ~setPrecise...~

 These methods take the argument of type mpq\_class, convert it to a
 number of chars and stores these chars in the given Flob. The
 private attributes representing the array indices to restore the coordinates
 are of course also updated.

*/
void PointData::SetPreciseX(Flob* preciseCoordinates, mpq_class x) {
 if (cmp(x, 0) == 0) {
  xStartPos = 0;
  xNumOfChars = 0;
  return;
 }
 SmiSize sz = preciseCoordinates->getSize();

 string str = x.get_str();
 char* s = new char[str.length() + 1];
 std::strcpy(s, str.c_str());

 xNumOfChars = strlen(s);

 if (sz == 0) {
  xStartPos = 0;
  // We have to allocate one byte for the ending-0 in the FLOB.
  preciseCoordinates->resize(xNumOfChars + 1);
 } else {
  xStartPos = sz - 1;
  // The FLOB already ends with 0.
  preciseCoordinates->resize(sz + xNumOfChars);
 }
 bool ok = preciseCoordinates->write(s, xNumOfChars + 1, xStartPos);
 assert(ok);

 delete s;
}

void PointData::SetPreciseY(Flob* preciseCoordinates, mpq_class y) {
 if (cmp(y, 0) == 0) {
  yStartPos = 0;
  yNumOfChars = 0;
  return;
 }
 SmiSize sz = preciseCoordinates->getSize();

 string str = y.get_str();
 char* s = new char[str.length() + 1];
 std::strcpy(s, str.c_str());

 yNumOfChars = strlen(s);

 if (sz == 0) {
  yStartPos = 0;
  preciseCoordinates->resize(yNumOfChars + 1);
 } else {
  yStartPos = sz - 1;
  preciseCoordinates->resize(sz + yNumOfChars);
 }
 bool ok = preciseCoordinates->write(s, yNumOfChars + 1, yStartPos);
 assert(ok);

 delete s;
}

/*
3.2.1 ~CopyPreciseCoordinates~

*/
void PointData::CopyPreciseCoordinates(Point2* p, Flob* preciseCoordinates) {
 SmiSize sz = preciseCoordinates->getSize();

 char* x = p->getPreciseXAsString();
 if (x[0] == '0') {
  xStartPos = 0;
  xNumOfChars = 0;
 } else {
  xStartPos = sz;
  xNumOfChars = strlen(x);
 }
 char* y = p->getPreciseYAsString();
 if (y[0] == '0') {
  yStartPos = 0;
  yNumOfChars = 0;
 } else {
  yStartPos = sz + xNumOfChars;
  yNumOfChars = strlen(y);
 }
 if ((xNumOfChars + yNumOfChars) > 0) {
  SmiSize newSize;
  if (sz == 0) {
   newSize = (xNumOfChars + yNumOfChars) + 1;
  } else {
   newSize = sz + (xNumOfChars + yNumOfChars);
  }
  preciseCoordinates->resize(newSize);

  if (xNumOfChars > 0) {
   preciseCoordinates->write(x, xNumOfChars, xStartPos);
  }
  if (yNumOfChars > 0) {
   preciseCoordinates->write(y, yNumOfChars, yStartPos);
  }
 }
 delete x;
 delete y;
}

void PointData::CopyPreciseCoordinates(Point2& p, Flob* preciseCoordinates) {
 SmiSize sz = preciseCoordinates->getSize();

 char* x = p.getPreciseXAsString();
 if (x[0] == '0') {
  xStartPos = 0;
  xNumOfChars = 0;
 } else {
  xStartPos = sz;
  xNumOfChars = strlen(x);
 }
 char* y = p.getPreciseYAsString();
 if (y[0] == '0') {
  yStartPos = 0;
  yNumOfChars = 0;
 } else {
  yStartPos = sz + xNumOfChars;
  yNumOfChars = strlen(y);
 }
 if ((xNumOfChars + yNumOfChars) > 0) {
  SmiSize newSize;
  if (sz == 0) {
   newSize = (xNumOfChars + yNumOfChars) + 1;
  } else {
   newSize = sz + (xNumOfChars + yNumOfChars);
  }
  preciseCoordinates->resize(newSize);

  if (xNumOfChars > 0) {
   preciseCoordinates->write(x, xNumOfChars, xStartPos);
  }
  if (yNumOfChars > 0) {
   preciseCoordinates->write(y, yNumOfChars, yStartPos);
  }
 }
 delete x;
 delete y;
}

/*
3.2.1 ~Intersects~

*/
bool PointData::Intersects(const Rectangle<2>& rect,
  const Geoid* geoid/*=0*/, const Flob* preciseCoordinates) const {
 mpq_class x, y;
 getPreciseX(preciseCoordinates, x);
 getPreciseY(preciseCoordinates, y);
 return     ((cmp(x, rect.MinD(0))>=0 ) && (cmp(x,rect.MaxD(0))<=0)
         && (cmp(y, rect.MinD(1))>=0 ) && (cmp(y, rect.MaxD(1))<=0));
}

/*
4 Class Point2

*/

/*
4.1 Constructors

*/
Point2::Point2(const bool def, const int xCoord, int yCoord, mpq_class preciseX,
  mpq_class preciseY) :
  StandardSpatialAttribute<2>(def), preciseCoordinates(0), pointData(xCoord,
    yCoord) {
 pointData.SetPreciseX(&preciseCoordinates, preciseX);
 pointData.SetPreciseY(&preciseCoordinates, preciseY);
}

Point2::Point2(const Point2& cp) :
  StandardSpatialAttribute<2>(cp.IsDefined()), preciseCoordinates(
    cp.preciseCoordinates.getSize()), pointData(cp.pointData) {
 preciseCoordinates.copyFrom(cp.preciseCoordinates);
}

Point2::Point2(bool def) :
  StandardSpatialAttribute<2>(def), preciseCoordinates(0), pointData(0, 0) {
}

Point2& Point2::operator=(const Point2& p) {
  return *(new Point2(p));
 }


/*
4.2 Read access methods

*/
int Point2::getGridX() const {
 return pointData.getGridX();
}

int Point2::getGridY() const {
 return pointData.getGridY();
}

mpq_class Point2::getPreciseX() const {
 mpq_class value;
 pointData.getPreciseX(&preciseCoordinates, value);
 return value;
}

mpq_class Point2::getPreciseY() const {
 mpq_class value;
 pointData.getPreciseY(&preciseCoordinates, value);
 return value;
}

char* Point2::getPreciseXAsString() const {
 return pointData.getPreciseXAsString(&preciseCoordinates);
}

char* Point2::getPreciseYAsString() const {
 return pointData.getPreciseYAsString(&preciseCoordinates);
}

/*
4.3 Comparison methods

*/
bool Point2::operator==(const Point2& p) const {
 if (!IsDefined() && !p.IsDefined()) {
  return true;
 }
 if (!IsDefined() || !p.IsDefined()) {
  return false;
 }
 if (getGridX() != p.getGridX() || getGridY() != p.getGridY()) {
  return false;
 }
 if (cmp(getPreciseX(), p.getPreciseX()) != 0) {
  return false;
 }
 if (cmp(getPreciseY(), p.getPreciseY()) != 0) {
  return false;
 }
 return true;
}

bool Point2::operator<=(const Point2& p) const {
 if (!IsDefined()) {
  return true;
 }
 if (!p.IsDefined()) {
  return !IsDefined();
 }
 if (getGridX() < p.getGridX()) {
  return true;
 } else {
  if (getGridX() > p.getGridX()) {
   return false;
  } else {
   // both points have the same x-coordinate for the
   // grid-value
   mpq_class px1 = getPreciseX();
   mpq_class px2 = p.getPreciseX();
   int cmpX = cmp(px1, px2);
   if (cmpX < 0) {
    return true;
   }
   if (cmpX > 0) {
    return false;
   }
   //both points have the same x-coordinate
   if (getGridY() < p.getGridY()) {
    return true;
   }
   if (getGridY() > p.getGridY()) {
    return false;
   }
   // both points lay in the same grid-point
   mpq_class py1 = getPreciseY();
   mpq_class py2 = p.getPreciseY();
   if (cmp(py1, py2) <= 0) {
    return true;
   }
  }
 }
 return false;
}

bool Point2::operator<(const Point2& p) const {
 if (!p.IsDefined()) {
  return false;
 }
 if (!IsDefined()) {
  return p.IsDefined();
 }
 if (getGridX() < p.getGridX()) {
  return true;
 } else {
  if (getGridX() > p.getGridX()) {
   return false;
  } else {
   // both points have the same gridX-value
   mpq_class px1 = getPreciseX();
   mpq_class px2 = p.getPreciseX();
   int cmpX = cmp(px1, px2);
   if (cmpX < 0) {
    return true;
   }
   if (cmpX > 0) {
    return false;
   }
   //both points have the same x-coordinate
   if (getGridY() < p.getGridY()) {
    return true;
   }
   if (getGridY() > p.getGridY()) {
    return false;
   }
   // both points lay in the same grid-point
   mpq_class py1 = getPreciseY();
   mpq_class py2 = p.getPreciseY();
   if (cmp(py1, py2) < 0) {
    return true;
   } else {
    return false;
   }
  }
 }
}

bool Point2::operator>=(const Point2& p) const {
 return !(*this < p);
}

bool Point2::operator>(const Point2& p) const {
 return !(*this <= p);
}

/*
4.4 functions required by Secondo

*/

/*
4.4.1 SizeOf-functions

*/
int Point2::SizeOfPoint2() {
 return sizeof(Point2);
}

inline size_t Point2::Sizeof() const {
 return (sizeof(*this));
}

/*
4.4.2 Copy-function

*/
inline void Point2::CopyFrom(const Attribute* right) {
 const Point2* p = (const Point2*) right;
 SetDefined(p->IsDefined());
 if (IsDefined()) {
  pointData.SetGridX(p->getGridX());
  pointData.SetGridY(p->getGridY());
  pointData.SetPreciseX(&preciseCoordinates, p->getPreciseX());
  pointData.SetPreciseY(&preciseCoordinates, p->getPreciseY());
  bbox = p->BoundingBox();
 }
}

/*
4.4.3 Compare-function

*/
inline int Point2::Compare(const Attribute *arg) const {
 const Point2* p = (const Point2*) arg;
 if (!IsDefined() && !p->IsDefined())
  return 0;
 if (!IsDefined())
  return -1;
 if (!p->IsDefined())
  return 1;
 if (*this > *p)
  return 1;
 if (*this < *p)
  return -1;
 return 0;
}

/*
4.4.4 ~Clone~-functions

*/
Word Point2::ClonePoint2(const ListExpr typeInfo, const Word& w) {
 return SetWord(new Point2(*((Point2 *) w.addr)));
}

/*
4.4.5 ~Cast~-function

*/
void* Point2::CastPoint2(void* addr) {
 return (new (addr) Point2());
}

/*
4.4.6 ~Create~-function

*/
Word Point2::CreatePoint2(const ListExpr typeInfo) {
 return SetWord(new Point2(false));
}

/*
4.4.7 ~Delete~-function

*/
void Point2::DeletePoint2(const ListExpr typeInfo, Word& w) {
 Point2 *p = (Point2 *) w.addr;
 (p->GetFLOB(0))->destroy();
 p->SetDefined(false);
 delete p;
 w.addr = 0;
}

/*
4.4.8 ~Close~-function

*/
void Point2::ClosePoint2(const ListExpr typeInfo, Word& w) {
 delete (Point2*) w.addr;
 w.addr = 0;
}

/*
4.4.9 ~Point2Property~

 Function describing the signature of the type constructor

*/
ListExpr Point2::Point2Property() {
 return nl->TwoElemList(
   nl->FourElemList(nl->StringAtom("Signature"),
     nl->StringAtom("Example Type List"), nl->StringAtom("List Rep"),
     nl->StringAtom("Example List")),
   nl->FourElemList(nl->StringAtom("-> DATA"),
     nl->StringAtom(Point2::BasicType()),
     nl->StringAtom("(x y ( preciseX preciseY))"),
     nl->StringAtom("(10 5 ( '1/4' '1/4'))")));
}

/*
4.4.10 ~CheckPoint2~

*/
bool Point2::CheckPoint2(ListExpr type, ListExpr& errorInfo) {
 return (nl->IsEqual(type, Point2::BasicType()));
}

/*
4.4.11 ~Distance~

*/
double Point2::Distance(const Rectangle<2>& rect,
  const Geoid* geoid/*=0*/) const {
 //TODO Impl. Point2::Distance
 return numeric_limits<double>::max();
}

/*
4.4.13 ~IsEmpty~

*/
bool Point2::IsEmpty() const {
 return !IsDefined();
}

/*
4.4.14 ~Intersects~

*/
bool Point2::Intersects(const Rectangle<2>& rect,
  const Geoid* geoid/*=0*/) const {
 assert(IsDefined());
 assert(rect.IsDefined());
 assert(!geoid); // not implemented yet
 return pointData.Intersects(rect, geoid, &preciseCoordinates);
}

} // end of namespace p2d

#endif/* _POINT2_CPP*/
