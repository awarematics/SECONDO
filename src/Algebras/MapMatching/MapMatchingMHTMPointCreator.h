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

[1] Header File of the MapMatching Algebra

April, 2012. Matthias Roth

[TOC]

1 Overview

This header file essentially contains the definition of the class
~MPointCreator~.

2 Defines and includes

*/

#ifndef __MAPMATCHINGMHTMPOINTCREATOR_H_
#define __MAPMATCHINGMHTMPOINTCREATOR_H_

#include "MapMatchingMHT.h"
#include "MHTRouteCandidate.h"
#include <TemporalAlgebra.h>
#include <vector>

class SimpleLine;


namespace mapmatch {

/*
3 class MPointCreator

*/
class MPointCreator : public IMapMatchingMHTResultCreator
{
public:

    MPointCreator(MPoint* pResMPoint, double dNetworkScale);
    virtual ~MPointCreator();

    virtual bool CreateResult(const std::vector<MHTRouteCandidate*>&
                                                           rvecRouteCandidates);

private:

    void ProcessPoints(const MHTRouteCandidate::PointData& rData1,
                       const MHTRouteCandidate::RouteSegment& rSegment1,
                       const MHTRouteCandidate::PointData& pData2,
                       std::vector<const SimpleLine*>& vecCurvesBetweenPoints);

    void ProcessCurve(const SimpleLine& rCurve,
                      const Interval<Instant> TimeInterval,
                      double dCurveLength = -1.0);

    bool Init(void);
    void Finalize(void);

    MPoint* m_pResMPoint;
    double m_dNetworkScale;
};


} // end of namespace mapmatch


#endif /* __MAPMATCHINGMHTMPOINTCREATOR_H_ */
