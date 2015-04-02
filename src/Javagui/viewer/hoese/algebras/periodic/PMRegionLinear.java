//This file is part of SECONDO.

//Copyright (C) 2004, University in Hagen, Department of Computer Science, 
//Database Systems for New Applications.

//SECONDO is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.

//SECONDO is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with SECONDO; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

package viewer.hoese.algebras.periodic;

import viewer.hoese.*;
import sj.lang.ListExpr;
import java.awt.geom.*;
import java.util.Vector;
import tools.Reporter;

public class PMRegionLinear extends LinearMove{

  public PMRegionLinear(){
    Faces=new Vector();
    interval = new RelInterval();
    defined = false;
  }

  public String getName(){
    return "pmregion";
  }
  /** returns a Point2D.Double */
  public Object getObjectAt(Time T){
     if(!defined){
        Reporter.debug("PMRegionLinear.getObjectAt called on an undefined instance");
        return null;
     }
     if(!interval.contains(T)){
        return null;
     }
     double p = interval.where(T);
     GeneralPath GP = new GeneralPath(GeneralPath.WIND_EVEN_ODD,numberOfPoints+3);
     Vector CurrentFace;
     Vector CurrentCycle;
     Point CurrentPoint;
     double cx,cy;
     for(int face =0;face<Faces.size();face++){
         CurrentFace = (Vector) Faces.get(face);
	 for(int cycle=0;cycle<CurrentFace.size();cycle++){
            CurrentCycle = (Vector) CurrentFace.get(cycle);
	    for(int vertex=0;vertex<CurrentCycle.size();vertex++){
               CurrentPoint = (Point) CurrentCycle.get(vertex);
               cx = CurrentPoint.getX(p);
	       cy = CurrentPoint.getY(p);
	       if(vertex==0) // first node of a cycle
	          GP.moveTo((float)cx,(float)cy);
	       else
	          GP.lineTo((float)cx,(float)cy);
	    }
	    GP.closePath();
	 }
     }
     return new Area(GP);
  }

  protected boolean readMap(ListExpr map){
    if(map.listLength()!=2)
       return false;
    ListExpr start = map.first();
    ListExpr end = map.second();
    numberOfPoints=0;
    isstatic=true;
    bounds = new BBox(); // create an undefined BBox
    int sLength = start.listLength();
    int eLength = end.listLength();
    if(sLength!=eLength || sLength<1){
        Reporter.debug("different ListLength or empty Region");
        defined = false;
        return false;
    }
    Faces.ensureCapacity(sLength);
    while(!start.isEmpty()){ // for every face
        ListExpr CurrentStartFace = start.first();
        ListExpr CurrentEndFace = end.first();
        start = start.rest();
        end = end.rest();
        int sfl = CurrentStartFace.listLength();
	int sel = CurrentEndFace.listLength();
	if(sfl!=sel || sfl<1){
       Reporter.debug("different face length or face without an outer cycle");
	   defined = false;
	   return false;
	}
	Vector CurrentFace = new Vector(sfl);
	while(!CurrentStartFace.isEmpty()){ // for every cycle in the face
           ListExpr CurrentStartCycle = CurrentStartFace.first();
	   ListExpr CurrentEndCycle = CurrentEndFace.first();
	   CurrentStartFace = CurrentStartFace.rest();
	   CurrentEndFace = CurrentEndFace.rest();
	   int scl = CurrentStartCycle.listLength();
	   int ecl = CurrentEndCycle.listLength();
	   if(scl!=ecl || scl<3){
        Reporter.debug("different cycle length or too less vertices in cycle\n"+
		                     "StartCycleLength ="+scl+"\n"+
                         "EndCycleLength ="+ecl);
	      return false;
	   }
	   Vector CurrentCycle = new Vector(scl+1);
           while(!CurrentStartCycle.isEmpty()){
               ListExpr CurrentStartPoint = CurrentStartCycle.first();
	       ListExpr CurrentEndPoint = CurrentEndCycle.first();
	       CurrentStartCycle=CurrentStartCycle.rest();
	       CurrentEndCycle=CurrentEndCycle.rest();
	       Point P = new Point();
	       if(!P.readFrom(CurrentStartPoint,CurrentEndPoint))
	          return false;
	       CurrentCycle.add(P);
	       bounds.unionInternal(P.startX,P.startY);
	       bounds.unionInternal(P.endX,P.endY);
	       numberOfPoints++;
	       isstatic = isstatic && P.isstatic();
	   }
	   numberOfPoints++; // to close the cycle
	   CurrentFace.add(CurrentCycle);
	}
        Faces.add(CurrentFace);
    }
    defined = true;
    return true;
  }

  public BBox getBoundingBox(){
      if(!defined){
         Reporter.debug("PMRegionLinear.getBoundingBox called with an undefined instance");
         return null;
      }
      if(bounds==null){
           Reporter.debug("PMRegionLinear.getBoundingBox called without a bounding box");
      }
      return bounds;
  }

  Vector Faces;
  int numberOfPoints = 0;
  private BBox bounds;

  private class Point{

     Point(){
       this(0,0,0,0);
     }
     Point(double sx,double sy,double ex, double ey){
       startX=sx;
       startX=sy;
       endX=ex;
       endY=ey;
     }
     boolean readFrom(ListExpr start, ListExpr end){
        if(start.listLength()!=2 || end.listLength()!=2){
           Reporter.debug("Wrong ListLength for a point");
	         return false;
        }
	Double X1 = LEUtils.readNumeric(start.first());
	Double Y1 = LEUtils.readNumeric(start.second());
        Double X2 = LEUtils.readNumeric(end.first());
	Double Y2 = LEUtils.readNumeric(end.second());
	if(X1==null || X2==null || Y1==null || Y1==null){
         Reporter.debug("found a non numeric for a coordinate of a point");
	    return false;
	}
        startX = X1.doubleValue();
	startY = Y1.doubleValue();
	endX = X2.doubleValue();
        endY = Y2.doubleValue();
	return true;
     }

     double startX;
     double startY;
     double endX;
     double endY;
     double getX(double pos){
        return startX+pos*(endX-startX);
     }
     double getY(double pos){
        return startY+pos*(endY-startY);
     }
     boolean isstatic(){
       return startX==endX && startY==endY;
     }
  }

}
