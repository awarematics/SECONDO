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

//paragraph [1] Title: [{\Large \bf \begin {center}] [\end {center}}]
//[TOC] [\tableofcontents]
//[_] [\_]

[1] Implementation of the OSM Algebra

June-November, 2011. Thomas Uchdorf

[2] operator fullosmimport added

Jan-Feb 2012, Fabio Vald\'{e}s

[3] operator divide\_osm added

July 2013, Fabio Vald\'{e}s

[4] operator convertstreets added

October 2013, Fabio Vald\'{e}s

[TOC]

1 Overview

This implementation file contains the implementation of the class ~OsmAlgebra~.

For more detailed information see OsmAlgebra.h.

2 Defines and Includes

*/
#undef __TRACE__
//#define __TRACE__ cout <<  __FILE__ << "::" << __LINE__;
#define __TRACE__

/*
Including header-files

*/

using namespace std;
#include "OsmAlgebra.h"
#include "AlgebraManager.h"
#include "NestedList.h"
#include "QueryProcessor.h"
#include "Symbols.h"
#include "RelationAlgebra.h"
#include "../Spatial/Geoid.h"
#include "../Spatial/SpatialAlgebra.h"
#include "../FText/FTextAlgebra.h"
#include "ShpFileReader.h"
#include "ConnCodeFinder.h"
#include "ScalingEngine.h"
#include "OsmImportOperator.h"
#include "SecondoCatalog.h"
#include "XmlFileReader.h"
#include "XmlParserInterface.h"
#include "Element.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <iostream>
#include <string>
#include <fstream>


// --- Enabling global pointer variables
extern NestedList* nl;
extern QueryProcessor* qp;

// --- Announcing global functions from ImExAlgebra.cpp
//extern string getShpType (const string fname, bool& correct,
//                          string& errorMessage);
//template<int filePos>
//extern int shpimportVM (Word* args, Word& result, int message,
//                        Word& local, Supplier s);

// --- shpimport3-operator
// Specification of operator shpimport3
const string shpimport3Spec  =
    "( ( \"Signature\" \"Syntax\" \"Meaning\" \"Example\" ) "
    "(<text> text -> stream(T), T in {point, points, line, region}</text--->"
    "<text>shpimport3(_)</text--->"
    "<text>Produces a stream of spatial objects from a shapefile. "
    "The spatial result stream element type T is determined "
    "automatically by inspecting the import file.</text--->"
    "<text> query shpimport3('kinos.shp') count</text--->) )";

/*
Value-mapping-function of operator shpimport3

*/
template<int filePos>
int shpimport3ValueMap(Word* args, Word& result, int message,
        Word& local, Supplier s){
    //return shpimportVM<filePos> (args, result, message, local, s);
    ShpFileReader* reader = NULL;
    FText* fname = NULL;
    ListExpr type;
    int ret = 0;
    switch(message){
        case OPEN:
            if(local.addr){
                delete (ShpFileReader*)local.addr;
            }
            fname = static_cast<FText*>(args[filePos].addr);
            type = nl->Second(qp->GetType(s));
            local.setAddr(new ShpFileReader(type,fname));
            ret = 0;
            break;
        case REQUEST:
            if(!local.addr){
                ret = CANCEL;
            } else {
                reader = static_cast<ShpFileReader*>(local.addr);
                if(!reader){
                    ret = CANCEL;
                } else {
                    Attribute* next = reader->getNext();
                    result.addr = next;
                    ret = next?YIELD:CANCEL;
                }
            }
            break;
        case CLOSE:
            reader = static_cast<ShpFileReader*>(local.addr);
            if(reader){
                reader->close();
                delete reader;
                local.addr = 0;
            }
            ret = 0;
            break;
        default:
            ret = 0;
            break;
    }
    return ret;
}

/*
Type-mapping-function of operator shpimport3

*/
ListExpr shpimport3TypeMap(ListExpr args){
   if(nl->ListLength(args)!=1){
      return listutils::typeError("one argument expected");
   }
   ListExpr arg = nl->First(args);
   if(nl->ListLength(arg) !=2){
      return listutils::typeError("Error, argument has to consist of 2 parts");
   }
   ListExpr type = nl->First(arg);
   ListExpr value = nl->Second(arg);

   if(!listutils::isSymbol(type,FText::BasicType())){
       return listutils::typeError("text expected");
   }

   // get the value if possible

   Word res;
   bool success = QueryProcessor::ExecuteQuery(nl->ToString(value),res);
   if(!success){
     return listutils::typeError("could not evaluate the value of  " +
                                  nl->ToString(value) );
   }

   FText* resText = static_cast<FText*>(res.addr);

   if(!resText->IsDefined()){
      resText->DeleteIfAllowed();
       return listutils::typeError("filename evaluated to be undefined");
   }

   string name = resText->GetValue();
   resText->DeleteIfAllowed();

   string shpType;
   bool correct;
   string errmsg;

   shpType = ShpFileReader::getShpType(name, correct, errmsg);
   if(!correct){
      return listutils::typeError(errmsg);
   }
   return nl->TwoElemList(nl->SymbolAtom(Symbol::STREAM()),
                          nl->SymbolAtom(shpType));
}

/*
Instance of operator shpimport3

*/
Operator shpimport3( "shpimport3",
                    shpimport3Spec,
                    shpimport3ValueMap<0>,
                    Operator::SimpleSelect,
                    shpimport3TypeMap);

// --- osmimport-operator
// Specification of operator osmimport
const string osmimportSpec  =
    "( ( \"Signature\" \"Syntax\" \"Meaning\" \"Example\" ) "
    "(<text> text x ('node','way','restriction') -> stream (tuple(...)) "
    "</text--->"
    "<text> osmimport(_,_) </text--->"
    "<text> Produces a stream with the nodes, ways or restrictions from the "
    "specified OSM-file. </text--->"
    "<text> query osmimport('city.osm','node') count </text--->) )";

// Value-mapping-function of operator osmimport
int osmimportValueMap(Word* args, Word& result, int message,
        Word& local, Supplier s)
{
    assert (args != NULL);
    OsmImportOperator* importOperator = NULL;
    FText *arg1 = NULL;
    FText *arg2 = NULL;
    result = qp->ResultStorage (s);
    int ret = 0;
    switch (message) {
        case OPEN:
            arg1 = static_cast<FText *>(args[0].addr);
            arg2 = static_cast<FText *>(args[1].addr);
            if (arg1->IsDefined () && arg2->IsDefined ()) {
                importOperator = new OsmImportOperator (arg1->GetValue (),
                    arg2->GetValue (), qp->GetType (s));
            }
            local.setAddr (importOperator);
            ret = 0; 
            break;
        case REQUEST:
            importOperator = static_cast<OsmImportOperator *>(local.addr);
            if (!importOperator) {
                ret = CANCEL;
            } else  {
                result.addr = importOperator->getNext (); 
                ret = (result.addr)? YIELD : CANCEL;
            }
            break;
        case CLOSE:
            importOperator = static_cast<OsmImportOperator *>(local.addr);
            if (importOperator)  {
                delete importOperator;
                local.setAddr (0);
            }
            break;
        default:
            break;
    }
    return ret;
}

// Type-mapping-function of operator osmimport
ListExpr osmimportTypeMap(ListExpr args)
{
    Word res;
    bool success (false);
    std::string elementType;
    ListExpr fileNameArg;
    ListExpr fileNameType;
    ListExpr fileNameValue;
    ListExpr elementTypeArg;
    ListExpr elementTypeType;
    ListExpr elementTypeValue;
    ListExpr attrList;
    ListExpr ret;
    FText* fileName (NULL);

    if(nl->ListLength(args) != 2){
        return listutils::typeError("two arguments expected");
    }

    // --- Testing if a valid file name was passed
    fileNameArg = nl->First(args);
    if(nl->ListLength(fileNameArg) != 2){
        return listutils::typeError(
            "Error, argument has to consists of 2 parts");
    }
    fileNameType = nl->First(fileNameArg);
    fileNameValue = nl->Second(fileNameArg);

    if(!listutils::isSymbol(fileNameType,FText::BasicType())){
        return listutils::typeError("text expected");
    }

    success = QueryProcessor::ExecuteQuery(nl->ToString(fileNameValue),res);
    if(!success){
        return listutils::typeError("could not evaluate the value of  " +
                nl->ToString(fileNameValue) );
    }

    fileName = static_cast<FText*>(res.addr);

    if(!fileName->IsDefined()){
        fileName->DeleteIfAllowed();
        return listutils::typeError("filename evaluated to be undefined");
    }

    fileName->DeleteIfAllowed();

    // --- Checking the element type ("node", "way" or "restriction")
    elementTypeArg = nl->Second(args);
    if(nl->ListLength(elementTypeArg) != 2){
        return listutils::typeError(
            "Error, argument has to consists of 2 parts");
    }
    elementTypeType = nl->First(elementTypeArg);
    elementTypeValue = nl->Second(elementTypeArg);

    if(!listutils::isSymbol(elementTypeType,FText::BasicType())){
        return listutils::typeError("text expected");
    }

    elementType = nl->ToString(elementTypeValue);

    // Creating a list of attributes
    if (elementType == "'node'")  {
        attrList = OsmImportOperator::getOsmNodeAttrList ();
    } else if (elementType == "'way'")  {
        attrList = OsmImportOperator::getOsmWayAttrList ();
    } else if (elementType == "'restriction'")  {
        attrList = OsmImportOperator::getOsmRestrictionAttrList (); 
    } else {
        return listutils::typeError (
            "Error: The passed element type is unknown.");
    }

    ret = nl->TwoElemList(nl->SymbolAtom(Symbol::STREAM()),
            nl->TwoElemList(
                nl->SymbolAtom(Tuple::BasicType()),
                attrList));
    return ret;
}

// Instance of operator osmimport
Operator osmimport( "osmimport",
                    osmimportSpec,
                    osmimportValueMap,
                    Operator::SimpleSelect,
                    osmimportTypeMap);

// --- getconnectivitycode-operator
// Specification of operator getconnectivitycode
const string getconnectivitycodeSpec  =
    "( ( \"Signature\" \"Syntax\" \"Meaning\" \"Example\" ) "
    "(<text> d1 x d2 x d3 x d4 x o1 x o2 x o3 x o4 -> c"
    ", d1, d2, d3, d4, c int, o1, o2, o3, o4 bool</text--->"
    "<text>getconnectivitycode(_)</text--->"
    "<text>Computes and returns the connectivity code as integer value "
    "from the directions and the one-way data of four crossing "
    "sections</text--->"
    "<text> query getconnectivitycode(sec1, sec2, sec3, sec4, ow1, ow2,"
    " ow3, ow4)</text--->))";

/*
Value-mapping-function of operator getconnectivitycode

*/
int getconnectivitycodeValueMap(Word* args, Word& result, int message,
        Word& local, Supplier s){
   assert (args != NULL);
   result = qp->ResultStorage (s);
   CcInt *res = static_cast<CcInt*>(result.addr);;
   int dir[4] = {0, 0, 0, 0};
   bool ow[4] = {false, false, false, false};
   CcInt *direction = NULL;
   CcBool *oneWay = NULL;
   int iDir = 0;
   int iOw = 0;
   bool foundUndefined = false;
   for (iDir = 0; !foundUndefined && iDir < 4; ++iDir)  {
      direction = static_cast<CcInt *>(args[iDir].addr);
      if (!direction->IsDefined()) {
         foundUndefined = true;
      } else  {
         dir[iDir] = direction->GetValue ();
      }
   }
   for (iOw = 0; !foundUndefined && iOw < 4; ++iOw)  {
      oneWay = static_cast<CcBool *>(args[4 + iOw].addr);
      if (!oneWay->IsDefined()) {
         foundUndefined = true;
      } else  {
         ow[iOw] = oneWay->GetValue ();
      }
   }
   if (foundUndefined)  {
      res->SetDefined(false);
   } else  {
      res->Set (true, ConnCodeFinder::getConnectivityCode (
         dir[0],dir[1],dir[2],dir[3],ow[0],ow[1],ow[2],ow[3]));
   }
   return 0;
}

// Type-mapping-function of operator getconnectivitycode
ListExpr getconnectivitycodeTypeMap(ListExpr args){
   if(nl->ListLength(args) != 8){
      return listutils::typeError("eight arguments expected");
   }
   ListExpr rest = args;
   int i = 0;
   int val = 0;
   while (!nl->IsEmpty (rest)) {
      ListExpr current = nl->First (rest);
      rest = nl->Rest (rest);
      if (nl->ListLength (current) != 2){
         return listutils::typeError("argument has to consists of 2 parts");
      }
      if (i >= 0 && i < 4)  {
         if (!listutils::isSymbol (nl->First(current), CcInt::BasicType ())) {
            return listutils::typeError("int expected");
         }
         //TODO Reconsider the following line!
         val = (nl->Second(current));
         if (val >= 0 && val < 3)  {
            return listutils::typeError("value between zero and two expected");
         }
      } else if ((i >= 4 && i < 8 &&
         !listutils::isSymbol (nl->First(current), CcBool::BasicType ()))){
         return listutils::typeError("bool expected");
      }
      ++i;
   }
   return nl->SymbolAtom(CcInt::BasicType());
}

// Instance of operator getconnectivitycode
Operator getconnectivitycode( "getconnectivitycode",
                    getconnectivitycodeSpec,
                    getconnectivitycodeValueMap,
                    Operator::SimpleSelect,
                    getconnectivitycodeTypeMap);

// --- binor-operator
// Specification of operator binor
const string binorSpec  =
    "( ( \"Signature\" \"Syntax\" \"Meaning\" \"Example\" ) "
    "(<text> d1 x d2 -> c, d1, d2, c int</text--->"
    "<text>binor(_,_)</text--->"
    "<text>Represents the binary-or-operation of two interger values</text--->"
    "<text> query binor(0,1)</text--->))";

/*
Value-mapping-function of operator binor

*/
int binorValueMap(Word* args, Word& result, int message,
        Word& local, Supplier s){
   assert (args != NULL);
   result = qp->ResultStorage (s);
   CcInt *res = static_cast<CcInt*>(result.addr);;
   CcInt *arg1 = NULL;
   CcInt *arg2 = NULL;
   int a = 0;
   int b = 0;
   bool foundUndefined = false;
   arg1 = static_cast<CcInt *>(args[0].addr);
   if (!arg1->IsDefined()) {
      foundUndefined = true;
   }
   a = arg1->GetValue ();
   arg2 = static_cast<CcInt *>(args[1].addr);
   if (!arg2->IsDefined()) {
      foundUndefined = true;
   }
   b = arg2->GetValue ();
    
   if (foundUndefined)  {
      res->SetDefined(false);
   } else  {
      res->Set (true,(a|b));
   }
   return 0;
}

// Type-mapping-function of operator binor
ListExpr binorTypeMap(ListExpr args){
   if(nl->ListLength(args) != 2){
      return listutils::typeError("two arguments expected");
   }
   ListExpr a = nl->First (args);
   ListExpr b = nl->Second (args);
   if (nl->ListLength (a) != 2){
      return listutils::typeError("argument has to consists of 2 parts");
   }
   if (nl->ListLength (b) != 2){
      return listutils::typeError("argument has to consists of 2 parts");
   }
   if (!listutils::isSymbol (nl->First(a), CcInt::BasicType ())) {
      return listutils::typeError("int expected");
   }
   if (!listutils::isSymbol (nl->First(b), CcInt::BasicType ())) {
      return listutils::typeError("int expected");
   }
   return nl->SymbolAtom(CcInt::BasicType());
}

// Instance of operator binor
Operator binor( "binor",
                binorSpec,
                binorValueMap,
                Operator::SimpleSelect,
                binorTypeMap);

// --- getscalefactorx-operator
// Specification of operator getscalefactorx
const string getscalefactorxSpec  =
    "( ( \"Signature\" \"Syntax\" \"Meaning\" \"Example\" ) "
    "(<text> -> int</text--->"
    "<text>getscalefactorx()</text--->"
    "<text>Returns the currently set scale factor for x coordinates.</text--->"
    "<text> query getscalefactorx()</text--->))";

/*
Value-mapping-function of operator getscalefactorx

*/
int getscalefactorxValueMap(Word* args, Word& result, int message,
        Word& local, Supplier s){
   assert (args != NULL);
   result = qp->ResultStorage (s);
   CcInt *res = static_cast<CcInt*>(result.addr);;
   res->Set (true,ScalingEngine::getInstance ().getScaleFactorX ());
   return 0;
}

// Type-mapping-function of operator getscalefactorx
ListExpr getscalefactorxTypeMap(ListExpr args){
   if(args){
      return listutils::typeError("no arguments expected");
   }
   return nl->SymbolAtom(CcInt::BasicType());
}

// Instance of operator getscalefactorx
Operator getscalefactorx( "getscalefactorx",
                getscalefactorxSpec,
                getscalefactorxValueMap,
                Operator::SimpleSelect,
                getscalefactorxTypeMap);

// --- getscalefactory-operator
// Specification of operator getscalefactory
const string getscalefactorySpec  =
    "( ( \"Signature\" \"Syntax\" \"Meaning\" \"Example\" ) "
    "(<text> -> int</text--->"
    "<text>getscalefactory()</text--->"
    "<text>Returns the currently set scale factor for y coordinates.</text--->"
    "<text> query getscalefactory()</text--->))";

// Value-mapping-function of operator getscalefactory
int getscalefactoryValueMap(Word* args, Word& result, int message,
        Word& local, Supplier s){
   assert (args != NULL);
   result = qp->ResultStorage (s);
   CcInt *res = static_cast<CcInt*>(result.addr);;
   res->Set (true,ScalingEngine::getInstance ().getScaleFactorY ());
   return 0;
}

// Type-mapping-function of operator getscalefactory
ListExpr getscalefactoryTypeMap(ListExpr args){
   if(args){
      return listutils::typeError("no arguments expected");
   }
   return nl->SymbolAtom(CcInt::BasicType());
}

// Instance of operator getscalefactory
Operator getscalefactory( "getscalefactory",
                getscalefactorySpec,
                getscalefactoryValueMap,
                Operator::SimpleSelect,
                getscalefactoryTypeMap);

// --- setscalefactorx-operator
// Specification of operator setscalefactorx
const string setscalefactorxSpec  =
    "( ( \"Signature\" \"Syntax\" \"Meaning\" \"Example\" ) "
    "(<text> int -> int</text--->"
    "<text>setscalefactorx(_)</text--->"
    "<text>Sets the scale factor for x coordinates and returns the new "
    "value.</text--->"
    "<text> query setscalefactorx(2000)</text--->))";

// Value-mapping-function of operator setscalefactorx
int setscalefactorxValueMap(Word* args, Word& result, int message,
        Word& local, Supplier s){
   assert (args != NULL);
   result = qp->ResultStorage (s);
   CcInt *res = static_cast<CcInt*>(result.addr);;
   CcInt *arg = static_cast<CcInt *>(args[0].addr);
   if (!arg->IsDefined()) {
      res->SetDefined(false);
   } else  {
      ScalingEngine::getInstance ().setScaleFactorX (arg->GetValue ());
      res->Set (true, ScalingEngine::getInstance ().getScaleFactorX ());
   }
   return 0;
}

// Type-mapping-function of operator setscalefactorx
ListExpr setscalefactorxTypeMap(ListExpr args){
   if(nl->ListLength(args) != 1){
      return listutils::typeError("one argument expected");
   }
   ListExpr arg = nl->First (args);
   if (nl->ListLength (arg) != 2 ||
       !listutils::isSymbol (nl->First (arg), CcInt::BasicType ())) {
      return listutils::typeError("wrapped int expected");
   }
   return nl->SymbolAtom(CcInt::BasicType());
}

// Instance of operator setscalefactorx
Operator setscalefactorx( "setscalefactorx",
                setscalefactorxSpec,
                setscalefactorxValueMap,
                Operator::SimpleSelect,
                setscalefactorxTypeMap);

// --- setscalefactory-operator
// Specification of operator setscalefactory
const string setscalefactorySpec  =
    "( ( \"Signature\" \"Syntax\" \"Meaning\" \"Example\" ) "
    "(<text> int -> int</text--->"
    "<text>setscalefactory(_)</text--->"
    "<text>Sets the scale factor for y coordinates and returns the new "
    "value.</text--->"
    "<text> query setscalefactory(2000)</text--->))";

// Value-mapping-function of operator setscalefactory
int setscalefactoryValueMap(Word* args, Word& result, int message,
        Word& local, Supplier s){
   assert (args != NULL);
   result = qp->ResultStorage (s);
   CcInt *res = static_cast<CcInt*>(result.addr);;
   CcInt *arg = static_cast<CcInt *>(args[0].addr);
   if (!arg->IsDefined()) {
      res->SetDefined(false);
   } else  {
      ScalingEngine::getInstance ().setScaleFactorY (arg->GetValue ());
      res->Set (true, ScalingEngine::getInstance ().getScaleFactorY ());
   }
   return 0;
}

// Type-mapping-function of operator setscalefactory
ListExpr setscalefactoryTypeMap(ListExpr args){
   if(nl->ListLength(args) != 1){
      return listutils::typeError("one argument expected");
   }
   ListExpr arg = nl->First (args);
   if (nl->ListLength (arg) != 2 ||
       !listutils::isSymbol (nl->First (arg), CcInt::BasicType ())) {
      return listutils::typeError("wrapped int expected");
   }
   return nl->SymbolAtom(CcInt::BasicType());
}

/*
Instance of operator setscalefactory

*/
Operator setscalefactory( "setscalefactory",
                setscalefactorySpec,
                setscalefactoryValueMap,
                Operator::SimpleSelect,
                setscalefactoryTypeMap);

/*
1 Operator fullosmimport

*/

/*
1.1 Specification of operator fullosmimport

*/
const string fullosmimportSpec =
    "( ( \"Signature\" \"Syntax\" \"Meaning\" \"Example\" ) "
    "(<text> text x string -> bool</text--->"
    "<text>fullosmimport(_)</text--->"
    "<text>imports an osm file into six new relations</text--->"
    "<text>query fullosmimport('dortmund.osm', \"dortmund\") count</text--->))";

/*
1.2 Type mapping function for operator fullosmimport

*/
ListExpr fullosmimportTypeMap(ListExpr args) {
  if (!nl->HasLength(args, 2))
    return listutils::typeError("two arguments expected");
  if (!listutils::isSymbol(nl->First(args), FText::BasicType()))
    return listutils::typeError("type text required for the first argument");
  if (!listutils::isSymbol(nl->First(nl->Rest(args)), CcString::BasicType()))
    return listutils::typeError("type string required for the 2nd argument");  
  return nl->SymbolAtom(CcBool::BasicType());
}

/*
1.3 Constructor and functions of class FullOsmImport

*/
FullOsmImport::FullOsmImport(const string& fileName, const string& prefix) {
  sc = SecondoSystem::GetCatalog();
  isTemp = false;
  reader = 0;
  relationsInitialized = initRelations(prefix, "", true);
  if(!relationsInitialized) {
    cout << "relations could not be initialized" << endl;
    return;
  }
  fileOk = openFile(fileName);
  if (!fileOk) {
    cout << "file could not be initialized" << endl;
    return;
  }
  defineRelations(true);
  node = 0;
  tag = 0;
  tagged = false;
  way = 0;
  rel = 0;
  fillRelations();
  storeRelations(true);
}

FullOsmImport::~FullOsmImport() {}

bool FullOsmImport::initRelations(const string& prefix, const string& suffix,
                                  bool all) {
  relNames[0] = prefix + "Nodes" + suffix;
  relNames[1] = prefix + "NodeTags" + suffix;
  relNames[2] = prefix + "Ways" + suffix;
  relNames[3] = prefix + "WayTags" + suffix;
  relNames[4] = prefix + "Relations" + suffix;
  relNames[5] = prefix + "RelationTags" + suffix;
  for (int i = 0; i < 6; i++) { // check the new relations' names
    if (all || (i == 0 || i == 2 || i == 3)) {
      if (sc->IsObjectName(relNames[i])) {
        cout << relNames[i] << " is already defined" << endl;
        return false;
      }
      string errMsg = "error";
      if (!sc->IsValidIdentifier(relNames[i], errMsg, true)) {
        cout << errMsg << endl;
        return false;
      }
      if (sc->IsSystemObject(relNames[i])) {
        cout << relNames[i] << " is a reserved name" << endl;
        return false;
      }
    }
  }
  
  return true;
} 

bool FullOsmImport::openFile(const string& fileName) {
  // check whether the file can be opened and is an osm file
  reader = xmlReaderForFile(fileName.c_str(), NULL, 0);
  if (reader == NULL) {
    cout << "file " << fileName << " cannot be opened" << endl;
    return false;
  }
  int read = xmlTextReaderRead(reader);
  if (read < 1) {
    cout << "file " << fileName << " is empty" << endl;
    xmlFreeTextReader(reader);
    return false;
  }
  // strcmp(x1, x2) == 0  <==> x1 == x2
  if (strcmp((char *)xmlTextReaderConstName(reader), "osm")) {
    cout << "root node of " << fileName << " has to be \"osm\"" << endl;
    xmlFreeTextReader(reader);
    return false;
  }
  cout << "file " << fileName << " opened successfully" << endl;
  return true;
}

void FullOsmImport::defineRelations(bool all) {
  nodeTypeInfo = nl->TwoElemList(nl->SymbolAtom(Tuple::BasicType()),
      nl->ThreeElemList(nl->TwoElemList(nl->SymbolAtom("NodeId"), 
                                      nl->SymbolAtom(LongInt::BasicType())),
                      nl->TwoElemList(nl->SymbolAtom("Lat"),
                                      nl->SymbolAtom(CcReal::BasicType())),
                      nl->TwoElemList(nl->SymbolAtom("Lon"),
                                      nl->SymbolAtom(CcReal::BasicType()))));
  numNodeTypeInfo = sc->NumericType(nodeTypeInfo);
  nodeType = new TupleType(numNodeTypeInfo);
  nodeRel = new Relation(nodeType, isTemp);
  
  if (all) {
    nodeTagTypeInfo = nl->TwoElemList(nl->SymbolAtom(Tuple::BasicType()),
        nl->ThreeElemList(nl->TwoElemList(nl->SymbolAtom("NodeIdInTag"),
                                          nl->SymbolAtom(LongInt::BasicType())),
                          nl->TwoElemList(nl->SymbolAtom("NodeTagKey"),
                                          nl->SymbolAtom(FText::BasicType())),
                          nl->TwoElemList(nl->SymbolAtom("NodeTagValue"),
                                          nl->SymbolAtom(FText::BasicType()))));
    numNodeTagTypeInfo = sc->NumericType(nodeTagTypeInfo);
    nodeTagType = new TupleType(numNodeTagTypeInfo);
    nodeTagRel = new Relation(nodeTagType, isTemp);
  }
  
  wayTypeInfo = nl->TwoElemList(nl->SymbolAtom(Tuple::BasicType()),
      nl->ThreeElemList(nl->TwoElemList(nl->SymbolAtom("WayId"), 
                                        nl->SymbolAtom(LongInt::BasicType())),
                        nl->TwoElemList(nl->SymbolAtom("NodeCounter"),
                                        nl->SymbolAtom(CcInt::BasicType())),
                        nl->TwoElemList(nl->SymbolAtom("NodeRef"),
                                        nl->SymbolAtom(LongInt::BasicType()))));
  numWayTypeInfo = sc->NumericType(wayTypeInfo);
  wayType = new TupleType(numWayTypeInfo);
  wayRel = new Relation(wayType, isTemp);
  
  wayTagTypeInfo = nl->TwoElemList(nl->SymbolAtom(Tuple::BasicType()),
      nl->ThreeElemList(nl->TwoElemList(nl->SymbolAtom("WayIdInTag"), 
                                        nl->SymbolAtom(LongInt::BasicType())),
                        nl->TwoElemList(nl->SymbolAtom("WayTagKey"),
                                        nl->SymbolAtom(FText::BasicType())),
                        nl->TwoElemList(nl->SymbolAtom("WayTagValue"),
                                        nl->SymbolAtom(FText::BasicType()))));
  numWayTagTypeInfo = sc->NumericType(wayTagTypeInfo);
  wayTagType = new TupleType(numWayTagTypeInfo);
  wayTagRel = new Relation(wayTagType, isTemp);
  
  if (all) {
    relTypeInfo = nl->TwoElemList(nl->SymbolAtom(Tuple::BasicType()),
        nl->FiveElemList(nl->TwoElemList(nl->SymbolAtom("RelId"),
                                         nl->SymbolAtom(LongInt::BasicType())),
                         nl->TwoElemList(nl->SymbolAtom("RefCounter"),
                                         nl->SymbolAtom(CcInt::BasicType())),
                         nl->TwoElemList(nl->SymbolAtom("MemberType"),
                                         nl->SymbolAtom(FText::BasicType())),
                         nl->TwoElemList(nl->SymbolAtom("MemberRef"),
                                         nl->SymbolAtom(LongInt::BasicType())),
                         nl->TwoElemList(nl->SymbolAtom("MemberRole"),
                                         nl->SymbolAtom(FText::BasicType()))));
    numRelTypeInfo = sc->NumericType(relTypeInfo);
    relType = new TupleType(numRelTypeInfo);
    relRel = new Relation(relType, isTemp);
  
    relTagTypeInfo = nl->TwoElemList(nl->SymbolAtom(Tuple::BasicType()),
        nl->ThreeElemList(nl->TwoElemList(nl->SymbolAtom("RelIdInTag"),
                                          nl->SymbolAtom(LongInt::BasicType())),
                          nl->TwoElemList(nl->SymbolAtom("RelTagKey"),
                                          nl->SymbolAtom(FText::BasicType())),
                          nl->TwoElemList(nl->SymbolAtom("RelTagValue"),
                                          nl->SymbolAtom(FText::BasicType()))));
    numRelTagTypeInfo = sc->NumericType(relTagTypeInfo);
    relTagType = new TupleType(numRelTagTypeInfo);
    relTagRel = new Relation(relTagType, isTemp);
  }
}

void FullOsmImport::processNode(xmlTextReaderPtr reader) {
  int attrCount = 0;
  currentId = 0;
  xmlChar *id, *lat, *lon, *subNameXml;
  id = xmlTextReaderGetAttribute(reader, (xmlChar *)"id");
  if (id != NULL) {
    currentId.ReadFrom((const char*)id);
    node->PutAttribute(0, new LongInt(currentId));
    xmlFree(id);
    lat = xmlTextReaderGetAttribute(reader, (xmlChar *)"lat");
    if (lat != NULL) {
      node->PutAttribute(1, new CcReal(true, OsmImportOperator::convStrToDbl
          ((const char*)lat)));
      attrCount++;
      xmlFree(lat);
    }
    lon = xmlTextReaderGetAttribute(reader, (xmlChar *)"lon");
    if (lon != NULL) {
      node->PutAttribute(2, new CcReal(true, OsmImportOperator::convStrToDbl
          ((const char *)lon)));
      attrCount++;
      xmlFree(lon);
    }
    // ~tagged~ solves the node duplication problem. Otherwise, every tagged
    // node would be read twice
    if (attrCount == 2 && !tagged) {
      nodeRel->AppendTuple(node);
      tupleCount[0]++;
    }
    tagged = false;
    read = xmlTextReaderRead(reader);
    next = xmlTextReaderNext(reader);
    subNameXml = xmlTextReaderLocalName(reader);
    string subName = (char *)subNameXml;
    xmlFree(subNameXml);
    while (subName == "tag") {
      processTag(reader, NODE);
      subNameXml = xmlTextReaderLocalName(reader);
      subName = (char *)subNameXml;
      xmlFree(subNameXml);
    }
  }
}

void FullOsmImport::processWay(xmlTextReaderPtr reader) {
  currentId = 0;
  refCount = 0;
  xmlChar *id, *subNameXml;
  id = xmlTextReaderGetAttribute(reader, (xmlChar *)"id");
  if (id != NULL) {
    currentId.ReadFrom((const char*)id);
    xmlFree(id);
    read = xmlTextReaderRead(reader);
    next = xmlTextReaderNext(reader);
    subNameXml = xmlTextReaderLocalName(reader);
    string subName = (char *)subNameXml;
    xmlFree(subNameXml);
    while (subName == "nd") {
      processWayNodeRef(reader);
      subNameXml = xmlTextReaderLocalName(reader);
      subName = (char *)subNameXml;
      xmlFree(subNameXml);
    }
    while (subName == "tag") {
      processTag(reader, WAY);
      subNameXml = xmlTextReaderLocalName(reader);
      subName = (char *)subNameXml;
      xmlFree(subNameXml);
    }
  }
}

void FullOsmImport::processWayNodeRef(xmlTextReaderPtr reader) {
  int attrCount = 0;
  xmlChar *nodeRef;
  way->PutAttribute(0, new LongInt(currentId));
  nodeRef = xmlTextReaderGetAttribute(reader, (xmlChar *)"ref");
  if (nodeRef != NULL) {
    way->PutAttribute(1, new CcInt(true, refCount));
    way->PutAttribute(2, new LongInt(string((const char*)nodeRef)));
    refCount++;
    attrCount++;
    xmlFree(nodeRef);
  }
  if (attrCount) {
    wayRel->AppendTuple(way);
    tupleCount[2]++;
  }
  read = xmlTextReaderRead(reader);
  next = xmlTextReaderNext(reader);
}

void FullOsmImport::processRel(xmlTextReaderPtr reader) {
  currentId = 0;
  refCount = 0;
  xmlChar *id, *subNameXml;
  id = xmlTextReaderGetAttribute(reader, (xmlChar *)"id");
  if (id != NULL) {
    currentId.ReadFrom((const char*)id);
    xmlFree(id);
    read = xmlTextReaderRead(reader);
    next = xmlTextReaderNext(reader);
    subNameXml = xmlTextReaderLocalName(reader);
    string subName = (char *)subNameXml;
    xmlFree(subNameXml);
    while (subName == "member") {
      processRelMemberRef(reader);
      subNameXml = xmlTextReaderLocalName(reader);
      subName = (char *)subNameXml;
      xmlFree(subNameXml);
    }
    while (subName == "tag") {
      processTag(reader, RELATION);
      subNameXml = xmlTextReaderLocalName(reader);
      subName = (char *)subNameXml;
      xmlFree(subNameXml);
    }
  }
}

void FullOsmImport::processRelMemberRef(xmlTextReaderPtr reader) {
  int attrCount = 0;
  xmlChar *memberRef, *type, *role;
  rel->PutAttribute(0, new LongInt(currentId));
  memberRef = xmlTextReaderGetAttribute(reader, (xmlChar *)"ref");
  if (memberRef != NULL) {
    rel->PutAttribute(1, new CcInt(true, refCount));
    rel->PutAttribute(3, new LongInt(string((const char*)memberRef)));
    refCount++;
    attrCount++;
    xmlFree(memberRef);
  }
  type = xmlTextReaderGetAttribute(reader, (xmlChar *)"type");
  if (type != NULL) {
    rel->PutAttribute(2, new FText(true, (char *)type));
    attrCount++;
    xmlFree(type);
  }
  role = xmlTextReaderGetAttribute(reader, (xmlChar *)"role");
  if (role != NULL) {
    rel->PutAttribute(4, new FText(true, (char *)role));
    attrCount++;
    xmlFree(role);
  }
  if (attrCount == 3) {
    relRel->AppendTuple(rel);
    tupleCount[4]++;
  }
  read = xmlTextReaderRead(reader);
  next = xmlTextReaderNext(reader);
}


void FullOsmImport::processTag(xmlTextReaderPtr reader, entityKind kind) {
  int attrCount = 0;
  xmlChar *key, *value;
  key = xmlTextReaderGetAttribute(reader, (xmlChar *)"k");
  Relation *currentRel = 0;
  int counterPos = -1;
  switch (kind) {
    case NODE:
      currentRel = nodeTagRel;
      counterPos = 1;
      break;
    case WAY:
      currentRel = wayTagRel;
      counterPos = 3;
      break;
    case RELATION:
      currentRel = relTagRel;
      counterPos = 5;
      break;
    default:
      assert(false);
  }  
  tag->PutAttribute(0, new LongInt(currentId));
  if (key != NULL) {
    tag->PutAttribute(1, new FText(true, (char *)key));
    attrCount++;
    xmlFree(key);
    value = xmlTextReaderGetAttribute(reader, (xmlChar *)"v");
    if (value != NULL) {
      tag->PutAttribute(2, new FText(true, (char *)value));
      attrCount++;
      xmlFree(value);
    }
    if (attrCount == 2) {
      currentRel->AppendTuple(tag);
      tagged = true;
      tupleCount[counterPos]++;
    }
  }
  read = xmlTextReaderRead(reader);
  next = xmlTextReaderNext(reader);
}

void FullOsmImport::fillRelations() {  
  xmlChar *subNameXml;
  node = new Tuple(nodeType);
  tag = new Tuple(nodeTagType);
  way = new Tuple(wayType);
  rel = new Tuple(relType);
  memset(tupleCount, 0 , 6 * sizeof(int));
  string currentName = "undefined string";
  read = xmlTextReaderRead(reader);
  next = xmlTextReaderNext(reader);
  
  while ((read == 1) && (next == 1)) {
    subNameXml = xmlTextReaderLocalName(reader);
    currentName = (char *)subNameXml;
    xmlFree(subNameXml);
    if (currentName == "node") {
      processNode(reader);
    }
    else if (currentName == "way") {
      processWay(reader);
    }
    else if (currentName == "relation") {
      processRel(reader);
    }
    else {
      read = xmlTextReaderRead(reader);
      next = xmlTextReaderNext(reader);
    }
  }
  // clean up the memory
  xmlFreeTextReader(reader);
  xmlCleanupParser();
  node->DeleteIfAllowed();
  tag->DeleteIfAllowed();
  way->DeleteIfAllowed();
  rel->DeleteIfAllowed();
  nodeType->DeleteIfAllowed();
  nodeTagType->DeleteIfAllowed();
  wayType->DeleteIfAllowed();
  wayTagType->DeleteIfAllowed();
  relType->DeleteIfAllowed();
  relTagType->DeleteIfAllowed();
}

void FullOsmImport::storeRelations(bool all) {
  storeRel(relNames[0], nodeTypeInfo, nodeRel);
  storeRel(relNames[2], wayTypeInfo, wayRel);
  storeRel(relNames[3], wayTagTypeInfo, wayTagRel);
  if (all) {
    storeRel(relNames[1], nodeTagTypeInfo, nodeTagRel);
    storeRel(relNames[4], relTypeInfo, relRel);
    storeRel(relNames[5], relTagTypeInfo, relTagRel);
  }
  for (int i = 0; i < 6; i++) {
    if (all || (i == 0 || i == 2 || i == 3)) {
      cout << "relation " << relNames[i] << " with " << tupleCount[i]
           << " tuples stored" << endl;
    }
  }
}

void FullOsmImport::storeRel(string name, ListExpr typeInfo, Relation *rel) {
  ListExpr type = nl->TwoElemList(nl->SymbolAtom(Relation::BasicType()),
                                  typeInfo);
  Word relWord;
  relWord.setAddr(rel);
  sc->InsertObject(name, "", type, relWord, true);
}
/*
1.4 Value mapping function for operator fullosmimport

*/
int fullosmimportValueMap(Word* args, Word& result, int message, Word& local,
                           Supplier s) {
  FText *file = (FText *)(args[0].addr);
  string fileName = file->GetValue();
  string prefix = ((CcString *)args[1].addr)->GetValue();
  FullOsmImport fullOsmImport(fileName, prefix);
  result = qp->ResultStorage(s);
  CcBool* res = (CcBool*) result.addr;    
  if (!(fullOsmImport.relationsInitialized && fullOsmImport.fileOk)) {
    res->Set(true, false);
  }
  else { // success
    res->Set(true, true);
  }
  return 0;                           
}

/*
1.5 Operator instance

*/
Operator fullosmimport( "fullosmimport",
                fullosmimportSpec,
                fullosmimportValueMap,
                Operator::SimpleSelect,
                fullosmimportTypeMap);

/*
\section{Operator ~convertstreets~}

\subsection{Type Mapping}

*/
ListExpr convertstreetsTM(ListExpr args) {
  string err = "Expected syntax: stream(tuple(..., x: line, ...)) x IDENT x "
               "string";
  if (!nl->HasLength(args, 3)) {
    return listutils::typeError(err + " (wrong number of arguments)");
  }
  ListExpr stream = nl->First(args);
  ListExpr anlist = nl->Second(args);
  if (!CcString::checkType(nl->Third(args))) {
    return listutils::typeError(err);
  }
  if (!Stream<Tuple>::checkType(stream) || !listutils::isSymbol(anlist)){
    return listutils::typeError(err);
  }
  string attr = nl->SymbolValue(anlist);
  ListExpr attrlist = nl->Second(nl->Second(stream));
  ListExpr type;
  int index = listutils::findAttribute(attrlist, attr, type);
  if (!index) {
    return listutils::typeError("attribute " + attr + " not found in tuple");
  }
  if (!Line::checkType(type)) {
    return listutils::typeError("wrong type " + nl->ToString(type)
                                + " of attritube " + attr);
  }
  ListExpr attrs = nl->OneElemList(nl->StringAtom(nl->SymbolValue
                                  (nl->First(nl->First(attrlist)))));
  ListExpr lastAttr = attrs;
  ListExpr types = nl->OneElemList(nl->TextAtom( nl->SymbolValue
                                  (nl->Second(nl->First(attrlist)))));
  ListExpr lastType = types;
  attrlist = nl->Rest(attrlist);
  while (!nl->IsEmpty(attrlist)) {
    ListExpr first = nl->First(attrlist);
    ListExpr firstfirst = nl->First(first);
    ListExpr firstsecond = nl->Second(first);
    lastAttr = nl->Append(lastAttr, nl->StringAtom
                                    (nl->SymbolValue(firstfirst)));
    lastType = nl->Append(lastType, nl->TextAtom(nl->ToString(firstsecond)));
    attrlist = nl->Rest(attrlist);
  }
  ListExpr infolist1 = nl->OneElemList(nl->IntAtom(index - 1));
  ListExpr infolist2 = listutils::concat(infolist1, attrs);
  ListExpr infolist = listutils::concat(infolist2, types);
  return nl->ThreeElemList(nl->SymbolAtom(Symbol::APPEND()), infolist,
                           nl->SymbolAtom(CcBool::BasicType()));
}

/*
subsection{Specification}

*/
const string convertstreetsSpec =
    "( ( \"Signature\" \"Syntax\" \"Meaning\" \"Example\" ) "
    "(<text> stream(tuple(..., x:line, ...)) x IDENT x string -> bool</text--->"
    "<text>_ convertstreets[ _ , _ ]</text--->"
    "<text>Converts BerlinMOD streets into Node and Way relations.</text--->"
    "<text>query strassen feed convertstreets[GeoData, \"Berlin\"]</text--->))";

FullOsmImport::FullOsmImport(const string& prefix) :
         isTemp(false), reader(0), nodeRel(0), wayRel(0), wayTagRel(0), node(0),
         tag(0), way(0), rel(0), currentId(0), tagged(false), newWay(true) {
  for (int i = 0; i < 6; i++) {
    tupleCount[i] = 0;
  }
  sc = SecondoSystem::GetCatalog();
  if(!initRelations(prefix, "", false)) {
    cout << "relations could not be initialized" << endl;
    return;
  }
  defineRelations(false);
}

void FullOsmImport::insertNodes(list<Point> &points, LongInt &wayId,
                                Tuple *tuple, Word *args) {
  if (points.empty()) {
    return;
  }
  int refCounter = 0;
  for (list<Point>::iterator it = points.begin(); it != points.end(); it++) {
    way = new Tuple(wayType);
    way->PutAttribute(0, new LongInt(wayId));
    if (storedPts.find(*it) != storedPts.end()) { // point found
      way->PutAttribute(2, new LongInt(pt2Id[*it]));
    }
    else { // insert new point
      storedPts.insert(*it);
      pt2Id[*it] = currentId;
      way->PutAttribute(2, new LongInt(currentId));
      node = new Tuple(nodeType);
      node->PutAttribute(0, new LongInt(currentId));
      currentId++;
      node->PutAttribute(1, new CcReal(true, it->GetY() / 100000.0 + 52.0));
      node->PutAttribute(2, new CcReal(true, it->GetX() / 100000.0 + 13.0));
      nodeRel->AppendTuple(node);
      tupleCount[0]++;
    }
    way->PutAttribute(1, new CcInt(true, refCounter));
    wayRel->AppendTuple(way);
    tupleCount[2]++;
    refCounter++;
  }
  insertWayTags(wayId, tuple, args);
  wayId++;
}

void FullOsmImport::insertWayTags(LongInt &wayId, Tuple *tuple, Word *args) {
  TupleType *tt = tuple->GetTupleType();
  int noAttrs = tt->GetNoAttributes();
  for (int i = 0; i < noAttrs; i++) {
    FText *type = static_cast<FText*>(args[i + 4 + noAttrs].addr);
    if ((type->GetValue() == "string") || (type->GetValue() == "text")) {
      CcString *attrname = static_cast<CcString*>(args[i + 4].addr);
      tag = new Tuple(wayTagType);
      tag->PutAttribute(0, new LongInt(wayId));
      tag->PutAttribute(1, new FText(true, attrname->GetValue()));
      if (type->GetValue() == "string") {
        tag->PutAttribute(2,
         new FText(true, ((CcString*)(tuple->GetAttribute(i)))->GetValue()));
      }
      else {
        tag->PutAttribute(2,
               new FText(true, ((FText*)(tuple->GetAttribute(i)))->GetValue()));
      }
      
      wayTagRel->AppendTuple(tag);
      tupleCount[3]++;
    }
  }
}

void FullOsmImport::processStream(Stream<Tuple> &stream, int attrNo,
                                  Word *args) {
  stream.open();
  Tuple *tuple = stream.request();
  list<Point> points; // store segments in correct order
  LongInt wayId = 0;
  while (tuple) {
    Line *line = (Line*)(tuple->GetAttribute(attrNo));
    LineSplitter *ls = new LineSplitter(line, true, false);
    do {
      points.clear();
      ls->NextLine(&points);
      insertNodes(points, wayId, tuple, args);
    } while (!points.empty());
    delete ls;
    deleteIfAllowed(tuple);
    tuple = stream.request();
  }
  stream.close();
}
    
/*
\subsection{Value Mapping}

*/
int convertstreetsVM(Word* args, Word& result, int message, Word& local,
                     Supplier s) {
  result = qp->ResultStorage(s);
  CcBool* res = (CcBool*)result.addr;
  CcInt* attrNo = static_cast<CcInt*>(args[3].addr);
  CcString* prefix = static_cast<CcString*>(args[2].addr);
  if (!attrNo->IsDefined() || !prefix->IsDefined()) {
    cout << "undefined parameter" << endl;
    res->SetDefined(false);
    return 0;
  }
  FullOsmImport *osm = new FullOsmImport(prefix->GetValue());
  if (osm->nodeRel && osm->wayRel && osm->wayTagRel) {
    Stream<Tuple> str = static_cast<Stream<Tuple>* >(args[0].addr);
    osm->processStream(str, attrNo->GetValue(), args);
    osm->storeRelations(false);
    res->Set(true, true);
  }
  if (osm) {
    delete osm;
    osm = 0;
  }
  return 0;
}

/*
\subsection{Operator instance}

*/
Operator convertstreets( "convertstreets",
                convertstreetsSpec,
                convertstreetsVM,
                Operator::SimpleSelect,
                convertstreetsTM);

/*
\section{Operator ~divide_osm~}

Randomly divides an OSM file into a number of sub-files.

\subsection{Type Mapping}

*/
ListExpr divide_osmTM(ListExpr args) {
  const string errMsg = "Expecting text x string x int x string";
  if (nl->HasLength(args, 4)) {
    if (FText::checkType(nl->First(args))
     && CcString::checkType(nl->Second(args))
     && CcInt::checkType(nl->Third(args))
     && CcString::checkType(nl->Fourth(args))) {
      return nl->SymbolAtom(CcBool::BasicType());
    }
  }
  return listutils::typeError(errMsg);
}

/*
subsection{Specification}

*/
const string divide_osmSpec =
    "( ( \"Signature\" \"Syntax\" \"Meaning\" \"Example\" ) "
    "(<text> text x string x int x string -> bool</text--->"
    "<text>divide_osm(_ _ _ _)</text--->"
    "<text>Randomly divides an OSM file into a number of sub-files.</text--->"
    "<text>query divide_osm('dortmund.osm', \"do\", 1909, \"DO\")</text--->))";

FullOsmImport::FullOsmImport(const string& fileName, const string& _subFileName,
                             const int _size, const string& prefix) :
         isTemp(false), reader(0), subFileName(_subFileName), node(0), tag(0),
         way(0), rel(0), size(_size), tagged(false) {
  for (int i = 0; i < 6; i++) {
    tupleCount[i] = 0;
  }
  sc = SecondoSystem::GetCatalog();
  relationsInitialized = initRelations(prefix, "_type", true);
  if(!relationsInitialized) {
    cout << "relations could not be initialized" << endl;
    return;
  }
  defineRelations(true);
  storeRelations(true);
  divideOSMfile(fileName);
}

const char* FullOsmImport::getFileName(LongInt dest) {
  stringstream result;
  result << subFileName << "_" << dest;
  return result.str().c_str();
}

bool FullOsmImport::isWhitespace(const char c) {
  return c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == 11;
}

string FullOsmImport::trim(const string &s) {
  size_t first = s.find_first_not_of(" \n\t\r");
  if (first == string::npos) {
    return string();
  }
  return s.substr(first);
}

bool FullOsmImport::isFileSwitchAllowed(const string& line) {
  if ((line.substr(0,5) == "<node") && (line.substr(line.length() -2) == "/>")){
    return true;
  }
  if ((line.substr(0, 7) == "</node>") || (line.substr(0, 6) == "</way>")
   || (line.substr(0, 11) == "</relation>")) {
    return true;
  }
  return false;
}

void FullOsmImport::divideOSMfile(const string& fileName) {
  ifstream source;
  ofstream dest;
  source.open(fileName.c_str(), ios::in);
  if(!source.good()){
     cerr << "Problem in open file " << fileName << endl;
     return;
  }
  string line;
  LongInt numOfChars(0), charCounter(0), destId(-1), nextLimit(0);
  source.seekg(0, ios::end);
  numOfChars = (LongInt)source.tellg();
  source.close();
  source.open(fileName.c_str(), ios::in);
  getline(source, line);
  for (LongInt i = 0; i < size; i++) { // clear destination files if existing
    dest.open(getFileName(i), ios::trunc);
    dest.close();
  }
  line = trim(line);
  while (!source.eof() && source.good() &&
         (trim(line).substr(0, 5) != "<node")) { // copy head
    for (LongInt file = 0; file < size; file++) {
      dest.open(getFileName(file), ios::app);
      dest << line << endl;
      dest.close();
    }
    getline(source, line);
    line = trim(line);
    charCounter += line.length();
  }
  if(!source.good()){
     cerr << "problem in reading file(2)" << fileName << endl;
     return;
  }
  charCounter -= line.length();
  nextLimit = charCounter;
  LongInt partSize = (numOfChars - source.tellg() - 1) / size + 1;
  dest.open(getFileName(0), ios::app);
  dest << line << endl;
  dest.close();
  while (!source.eof() && source.good()) { // copy rest
    if (charCounter >= nextLimit && isFileSwitchAllowed(line)) {
      if (dest.is_open()) {
        dest << "</osm>" << endl;
        dest.close();
      }
      nextLimit += partSize;
      destId++;
      dest.open(getFileName(destId), ios::app);
    }
    getline(source, line);
    charCounter += line.length();
    line = trim(line);
    dest << line << endl;
  }
  source.close();
  dest.close();
  fileOk = true;
  nodeType->DeleteIfAllowed();
  nodeTagType->DeleteIfAllowed();
  wayType->DeleteIfAllowed();
  wayTagType->DeleteIfAllowed();
  relType->DeleteIfAllowed();
  relTagType->DeleteIfAllowed();
}

/*
\subsection{Value Mapping}

*/
int divide_osmVM(Word* args, Word& result, int message, Word& local,Supplier s){
  result = qp->ResultStorage(s);
  CcBool* res = (CcBool*)result.addr;
  for(int i=0;i<4;i++){
     if(!((Attribute*)args[i].addr)->IsDefined()){
        res->Set(true,false);
        return 0;
     }
  }
  string fileName = ((FText*)args[0].addr)->GetValue();
  string subFileName = ((CcString*)args[1].addr)->GetValue();
  int size = ((CcInt*)args[2].addr)->GetValue();
  string prefix = ((CcString*)args[3].addr)->GetValue();
  FullOsmImport osm(fileName, subFileName, size, prefix);  
  if (!(osm.relationsInitialized && osm.fileOk)) {
    res->Set(true, false);
  }
  else { // success
    res->Set(true, true);
  }
  return 0;
}

/*
\subsection{Operator instance}

*/
Operator divide_osm( "divide_osm",
                divide_osmSpec,
                divide_osmVM,
                Operator::SimpleSelect,
                divide_osmTM);

// --- Constructors
// Constructor
osm::OsmAlgebra::OsmAlgebra () : Algebra ()
{
    AddOperator(&shpimport3);
    shpimport3.SetUsesArgsInTypeMapping();;
    AddOperator(&getconnectivitycode);
    getconnectivitycode.SetUsesArgsInTypeMapping();;
    AddOperator(&binor);
    binor.SetUsesArgsInTypeMapping();;
    AddOperator(&getscalefactorx);
    getscalefactorx.SetUsesArgsInTypeMapping();;
    AddOperator(&getscalefactory);
    getscalefactory.SetUsesArgsInTypeMapping();;
    AddOperator(&setscalefactorx);
    setscalefactorx.SetUsesArgsInTypeMapping();;
    AddOperator(&setscalefactory);
    setscalefactory.SetUsesArgsInTypeMapping();;
    AddOperator(&osmimport);
    osmimport.SetUsesArgsInTypeMapping();;
    AddOperator(&fullosmimport);
    AddOperator(&divide_osm);
//     AddOperator(&convertstreets);
}

// Destructor
osm::OsmAlgebra::~OsmAlgebra ()
{
    // empty
}

// --- Global initialization function
extern "C"
Algebra*
InitializeOsmAlgebra( NestedList* nlRef, QueryProcessor* qpRef )
{
  nl = nlRef;
  qp = qpRef;
  return (new osm::OsmAlgebra());
}
