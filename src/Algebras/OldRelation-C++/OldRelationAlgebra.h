/*
----
This file is part of SECONDO.

Copyright (C) 2004, University in Hagen, Department of Computer Science,
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
----

//paragraph [1] Title: [{\Large \bf \begin{center}] [\end{center}}]
//paragraph [10] Footnote: [{\footnote{] [}}]
//[TOC] [\tableofcontents]

[1] Header file  of the Old Relational Algebra

March 2003 Victor Almeida created the new Relational Algebra organization

December 2005, Victor Almeida deleted the deprecated algebra levels
(~executable~, ~descriptive~, and ~hibrid~). Only the executable
level remains. Models are also removed from type constructors.

September 2007, M. Spiekermann. Implementation of operator mconsume moved from
module RelationAlgebra into this module. This removes linker dependencies and
allows to switch off this module. Moreover, the tuples are organized in class
vector instead of class CTable.

1 Defines, includes, and constants

*/
#ifndef OLD_RELATION_ALGEBRA__H
#define OLD_RELATION_ALGEBRA_H

#include "Algebra.h"
#include "AlgebraManager.h"
#include "SecondoSystem.h"
#include "SecondoCatalog.h"
#include "NestedList.h"
#include "QueryProcessor.h"
#include "StandardTypes.h"
#include "ListUtils.h"

#include <iostream>
#include <string>
#include <deque>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <sstream>
#include <typeinfo>
#include "OldRelationAlgebraInfo.h"

enum CcRelationType { mrel, mtuple, mstream, mmap, mbool, merror };

const int MaxSizeOfAttr = 35;  //changed by DZM, original value: 20

int CcFindAttribute( ListExpr list, string attrname,
		     ListExpr& attrtype, NestedList* nl);
bool CcIsTupleDescription(ListExpr a, NestedList* nl);

ListExpr CcTupleProp ();

class CcTuple
{
  private:

    int NoOfAttr;
    Attribute* AttrList [MaxSizeOfAttr];

    /* if a tuple is free, then a stream receiving the tuple can delete or
       reuse it */
    bool isFree;
    SmiRecordId id;

  public:

    CcTuple ();

    virtual ~CcTuple ();

    Attribute* Get (int);
    void  Put (int, Attribute*);
    void  SetNoAttrs (int);
    int   GetNoAttrs ();
    bool IsFree();
    void SetFree(bool);

    CcTuple* CloneIfNecessary();

    CcTuple* Clone();

    void DeleteIfAllowed();

    SmiRecordId GetId();
    void SetId(SmiRecordId id);

    friend
    ostream& operator<<(ostream& s, CcTuple t);

    static const string BasicType() { return "mtuple"; }
    static const bool checkType(const ListExpr type){
      return listutils::isTupleDescription(type, true);
    }
};

class LexicographicalCcTupleCmp
{

public:

  bool operator()(const CcTuple*, const CcTuple*) const;
};

string ReportCcTupleStatistics();

ListExpr OutCcTuple (ListExpr, Word);

ListExpr SaveToListCcTuple (ListExpr, Word);

Word InCcTuple(ListExpr typeInfo, ListExpr value,
          int errorPos, ListExpr& errorInfo, bool& correct);

Word RestoreFromListCcTuple(ListExpr typeInfo, ListExpr value,
          int errorPos, ListExpr& errorInfo, bool& correct);

void DeleteCcTuple(Word&);

bool CheckCcTuple(ListExpr, ListExpr&);

void* CastCcTuple(void*);

Word CreateCcTuple(const ListExpr);

ListExpr CcRelProp ();

/*

1.3.1 Main memory representation

(Figure needs to be redrawn, doesn't work.)

Figure 2: Main memory representation of a relation (~vector~) [relation.eps]

*/
typedef vector<CcTuple*> CcRep;
typedef CcRep* CcRelation;

class CcRel;

class CcRelIT
{
  CcRep::iterator rs;
  CcRel* r;
  public :

  CcRelIT (CcRep::iterator rs, CcRel* r);
  ~CcRelIT ();
  CcRelIT& operator=(CcRelIT& right);

  CcTuple* GetTuple();
  void Next();
  bool EndOfScan();
  CcTuple* GetNextTuple();

};

class CcRel
{
  friend class CcRelIT;

  private:

    int NoOfTuples;
    CcRelation TupleList;
    SmiRecordId currentId;

  public:

    CcRel ();
    ~CcRel ();

    CcTuple* GetTupleById(SmiRecordId id);
    void    AppendTuple (CcTuple*);
    void Empty();

    CcRelIT* MakeNewScan();

    void    SetNoTuples (int);
    int     GetNoTuples ();

    static const string BasicType() { return "mrel"; }
    static const bool checkType(const ListExpr type){
      return listutils::isRelDescription2(type, BasicType());
    }
};

ListExpr OutCcRel(ListExpr, Word);

ListExpr SaveToListCcRel(ListExpr, Word);

Word CreateCcRel(const ListExpr);

Word InCcRel(ListExpr, ListExpr,
          int, ListExpr&, bool&);

Word RestoreFromListCcRel(ListExpr, ListExpr,
          int, ListExpr&, bool&);

void DeleteCcRel(Word&);

bool CheckCcRel(ListExpr, ListExpr&);

void* CastCcRel(void*);

bool OpenCcRel( SmiRecord&, size_t&,
                const ListExpr, Word& );

bool SaveCcRel( SmiRecord&, size_t&,
                const ListExpr, Word& );

#endif /* OLD_RELATION_ALGEBRA_H */
