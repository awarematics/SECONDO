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

[1] Header File of the OSM Algebra

June-November, 2011. Thomas Uchdorf

[TOC]

1 Overview

This header file essentially contains the definition of the classes ~OsmAlgebra~.

2 Defines and includes

*/
// [...]
#ifndef __OSM_ALGEBRA_H__
#define __OSM_ALGEBRA_H__

// --- Including header-files
#include "Algebra.h"
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlmemory.h>
#include "RelationAlgebra.h"
#include "LongInt.h"
#include "Stream.h"
#include "../Spatial/SpatialAlgebra.h"
#include <string>

enum entityKind {NODE, WAY, RELATION};

class FullOsmImport {
  public:
     FullOsmImport(const string& fileName, const string& prefix);
     FullOsmImport(const string& fileName, const string& _subFileName,
                   const int _size, const string& prefix);
     FullOsmImport(const string& prefix);
    ~FullOsmImport();
    
    bool initRelations(const string& prefix, const string& suffix, bool all);
    bool openFile(const string&fileName);
    void defineRelations(bool all);
    void fillRelations();
    void processNode(xmlTextReaderPtr reader);
    void processWay(xmlTextReaderPtr reader);
    void processWayNodeRef(xmlTextReaderPtr reader);
    void processRel(xmlTextReaderPtr reader);
    void processRelMemberRef(xmlTextReaderPtr reader);
    void processTag(xmlTextReaderPtr reader, entityKind kind);
    void storeRelations(bool all);
    void storeRel(string name, ListExpr type, Relation *rel);
    void divideOSMfile(const string& fileName);
    const char* getFileName(LongInt dest);
    string trim(const string &s);
    bool isWhitespace(const char c);
    bool isFileSwitchAllowed(const string& line);
    void insertNodes(list<Point> &points, LongInt &wayId, Tuple *tuple,
                     Word *args);
    void insertWayTags(LongInt &wayId, Tuple *tuple, Word *args);
    void processStream(Stream<Tuple> &stream, int attrNo, Word *args);
    
    SecondoCatalog* sc;
    bool isTemp, relationsInitialized, fileOk;
    xmlTextReaderPtr reader;
    Relation *nodeRel, *nodeTagRel, *wayRel, *wayTagRel, *relRel,
        *relTagRel;
    TupleType *nodeType, *nodeTagType, *wayType, *wayTagType, *relType,
        *relTagType;
    ListExpr nodeTypeInfo, nodeTagTypeInfo, wayTypeInfo, wayTagTypeInfo,
        relTypeInfo, relTagTypeInfo, numNodeTypeInfo, numNodeTagTypeInfo,
        numWayTypeInfo, numWayTagTypeInfo, numRelTypeInfo, numRelTagTypeInfo;
    string relNames[6];
    string subFileName;
    unsigned int tupleCount[6];
    Tuple *node, *tag, *way, *rel;
    int read, next, refCount, size;
    LongInt currentId;
    bool tagged, newWay;
    set<Point> storedPts;
    map<Point, LongInt> pt2Id;
};

namespace osm {

// OSM-algebra
class OsmAlgebra : public Algebra
{
    public:
        // --- Constructors
        // Constructor
        OsmAlgebra();
        // Destructor
        ~OsmAlgebra ();
};

} // end of namespace osm

#endif /* __OSM_ALGEBRA_H__ */

