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

[1] Header File of the Time Sequence Algebra

November, 2010 Mahmoud Sakr

[TOC]

1 Overview

This algebra provides functions for measuring the similarity between time 
sequence. The main purpose for implementing this algebra, is to exend SECONDO
with a languge for measuring the trajectory similarity

2 Defines and includes

*/

#ifndef STPATTERNALGEBRA_H_
#define STPATTERNALGEBRA_H_
#include "Algebra.h"

#include "NestedList.h"

#include "QueryProcessor.h"

#include "StandardTypes.h"
#include "LogMsg.h"
#include "NList.h"
#include "TemporalAlgebra.h"
#include "GPatternAlgebra.h"
using namespace datetime;
typedef DateTime Instant;
extern NestedList *nl;
extern QueryProcessor* qp;  


namespace TSeq {



} // namespace TSeq



#endif /* STPATTERNALGEBRA_H_ */
