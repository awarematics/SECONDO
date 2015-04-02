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

package  viewer.hoese.algebras;

import java.awt.geom.*;
import java.awt.*;
import viewer.*;
import viewer.hoese.*;
import sj.lang.ListExpr;
import java.util.*;
import gui.Environment;
import tools.Reporter;


/**
 * A displayclass for the movingpoint-type (spatiotemp algebra), 2D with TimePanel
 */
public class Dsplmovingpoint extends DisplayTimeGraph implements LabelAttribute, RenderAttribute {
  Point2D.Double point;
  Vector PointMaps;
  Rectangle2D.Double bounds;
  double minValue = Integer.MAX_VALUE;
  double maxValue = Integer.MIN_VALUE;
  boolean defined;
  static java.text.DecimalFormat format = new java.text.DecimalFormat("#.#####");


  public int numberOfShapes(){
     return 1;
  }

  

  /** Returns a short text usable as label **/
  public String getLabel(double time){
    if(Intervals==null || PointMaps==null){
       return null;
    }
    int index = IntervalSearch.getTimeIndex(time,Intervals);
    if(index<0){
      return null; 
    }
    PointMap pm = (PointMap) PointMaps.get(index);
    Interval in = (Interval)Intervals.get(index);
    double t1 = in.getStart();
    double t2 = in.getEnd();
    double Delta = (time-t1)/(t2-t1);
    double x = pm.x1+Delta*(pm.x2-pm.x1);
    double y = pm.y1+Delta*(pm.y2-pm.y1);
    return "("+format.format(x)+", "+ format.format(y)+")";

  }


  /**
   * Gets the shape of this instance at the ActualTime
   * @param at The actual transformation, used to calculate the correct size.
   * @return Rectangle or Circle Shape if ActualTime is defined otherwise null.
   * @see <a href="Dsplmovingpointsrc.html#getRenderObject">Source</a>
   */
  public Shape getRenderObject (int num,AffineTransform at) {
    if(num!=0){
        return null;
    }
    if(Intervals==null || PointMaps==null){
       return null;
    }
    if(RefLayer==null){
       return null;    
    }
    double t = RefLayer.getActualTime();
    int index = IntervalSearch.getTimeIndex(t,Intervals);
    if(index<0){
       return null;
    }

    PointMap pm = (PointMap) PointMaps.get(index);
    Interval in = (Interval)Intervals.get(index);
    double t1 = in.getStart();
    double t2 = in.getEnd();
    double Delta = (t-t1)/(t2-t1);
    double x = pm.x1+Delta*(pm.x2-pm.x1);
    double y = pm.y1+Delta*(pm.y2-pm.y1);

    point = new Point2D.Double(x, y);
    double ps = Cat.getPointSize(renderAttribute,CurrentState.ActualTime);
    double pixy = Math.abs(ps/at.getScaleY());
    double pix = Math.abs(ps/at.getScaleX());
    Shape shp;
    if (Cat.getPointasRect())
      shp = new Rectangle2D.Double(point.getX()- pix/2, point.getY() - pixy/2, pix, pixy);
    else {
      shp = new Ellipse2D.Double(point.getX()- pix/2, point.getY() - pixy/2, pix, pixy);
    }
    return  shp;
  }

  /**
   * Reads the coefficients out of ListExpr for a map
   * @param le ListExpr of four reals.
   * @return The PointMap that was read.
   * @see <a href="Dsplmovingpointsrc.html#readPointMap">Source</a>
   */
  private PointMap readPointMap (ListExpr le) {
    Double value[] =  {
      null, null, null, null
    };
    if (le.listLength() != 4)
      return  null;
    for (int i = 0; i < 4; i++) {
      value[i] = LEUtils.readNumeric(le.first());
      if (value[i] == null)
        return  null;
      le = le.rest();
    }
    double x1, y1;
    
    double v0 = value[0].doubleValue();
    double v1 = value[1].doubleValue();
    double v2 = value[2].doubleValue();
    double v3 = value[3].doubleValue();
    if(minValue>v0) minValue=v0;
    if(maxValue<v0) maxValue=v0;
    if(minValue>v2) minValue=v2;
    if(maxValue<v2) maxValue=v2;
   
    if(!ProjectionManager.project(value[0].doubleValue(),value[1].doubleValue(),aPoint)){
        return null;
    }
    x1 = aPoint.x;
    y1 = aPoint.y;
    if(!ProjectionManager.project(value[2].doubleValue(),value[3].doubleValue(),aPoint)){
      return null;
    }
    return  new PointMap(x1,y1,aPoint.x,aPoint.y);
  }

  /**
   * Scans the representation of a movingpoint datatype
   * @param v A list of start and end intervals with ax,bx,ay,by values
   * @see sj.lang.ListExpr
   * @see <a href="Dsplmovingpointsrc.html#ScanValue">Source</a>
   */
  private void ScanValue (ListExpr v) {
    err = true;
    if (v.isEmpty()){ //empty point
      Intervals=null;
      PointMaps=null;
      err=false;
      defined = false;
      return;
    }
    while (!v.isEmpty()) {      // unit While maybe empty
      ListExpr aunit = v.first();
      ListExpr tmp = aunit;
      int L = aunit.listLength();
      if(L!=2 && L!=8){
         Reporter.debug("wrong ListLength in reading moving point unit");
         defined = false;
         return;
      }
      // deprecated version of external representation
      Interval in=null;
      PointMap pm=null;
      if (L == 8){
         Reporter.writeWarning("Warning: using deprecated external representation of a moving point !");
         in = LEUtils.readInterval(ListExpr.fourElemList(aunit.first(),
                                   aunit.second(), aunit.third(), aunit.fourth()));
         aunit = aunit.rest().rest().rest().rest();
         pm = readPointMap(ListExpr.fourElemList(aunit.first(), aunit.second(),
                           aunit.third(), aunit.fourth()));
      }
      // the corrected version of external representation
      if(L==2){
         in = LEUtils.readInterval(aunit.first());
         pm = readPointMap(aunit.second());
      }

      if ((in == null) || (pm == null)){
        
         Reporter.debug("Error in reading Unit");
         Reporter.debug(tmp.writeListExprToString());
         if(in==null){
              Reporter.debug("Error in reading interval");
          }
          if(pm==null){
             Reporter.debug("Error in reading Start and EndPoint");
          }
          defined = false;
          return;
      }
      Intervals.add(in);
      PointMaps.add(pm);
      v = v.rest();
    }
    err = false;
    defined = true;
  }

  public boolean isPointType(int num){
     return true;
  }

  public void init (String name, int nameWidth, int indent, ListExpr type, ListExpr value, QueryResult qr) {

    AttrName = extendString(name, nameWidth, indent);
    int length = value.listLength();
    Intervals = new Vector(length+2);
    PointMaps = new Vector(length+2);
    ScanValue(value);

    if (err) {
      Reporter.writeError("Dsplmovingpoint Error in ListExpr :parsing aborted");
      qr.addEntry(new String("(" + AttrName + ": GTA(mpoint))"));
      return;
    }
    else{
      
      qr.addEntry(this);
    }
    //ListIterator li=iv.listIterator();
    bounds = null;
    TimeBounds = null;
    if(Intervals==null){ // empty moving point
       return;
    }
    for (int j = 0; j < Intervals.size(); j++) {
      Interval in = (Interval)Intervals.elementAt(j);
      PointMap pm = (PointMap)PointMaps.elementAt(j);
      Rectangle2D.Double r = new Rectangle2D.Double(pm.x1,pm.y1,0,0);
      r = (Rectangle2D.Double)r.createUnion(new Rectangle2D.Double(pm.x2,pm.y2,0,0));
      if (bounds == null) {
        bounds = r;
        TimeBounds = in;
      }
      else {
        bounds = (Rectangle2D.Double)bounds.createUnion(r);
        TimeBounds = TimeBounds.union(in);
      }
    }
  }

  /**
   * @return The overall boundingbox of the movingpoint
   * @see <a href="Dsplmovingpointsrc.html#getBounds">Source</a>
   */
  public Rectangle2D.Double getBounds () {
    return  bounds;
  }

  /** returns the minimum x value **/
  public double getMinRenderValue(){
    return minValue;
  }
  /** returns the maximum x value **/
  public double getMaxRenderValue(){
    return maxValue;
  }
  /** returns the current x value **/
  public double getRenderValue(double time){
    if(Intervals==null || PointMaps==null){
       return 0;
    }
    int index = IntervalSearch.getTimeIndex(time,Intervals);
    if(index<0){
      return 0; 
    }
    PointMap pm = (PointMap) PointMaps.get(index);
    Interval in = (Interval)Intervals.get(index);
    double t1 = in.getStart();
    double t2 = in.getEnd();
    double Delta = (time-t1)/(t2-t1);
    double x = pm.x1+Delta*(pm.x2-pm.x1);
    return  x;
  }

  public boolean mayBeDefined(){
    return defined;
  }

  public boolean isDefined(double time){
    if(!defined){
      return false;
    }
    int index = IntervalSearch.getTimeIndex(time,Intervals);
    return index>=0;
  }
  

  class PointMap {
    double x1,x2,y1,y2;

    public PointMap (double x1, double y1, double x2, double y2) {
       this.x1 = x1;
       this.y1 = y1;
       this.x2 = x2;
       this.y2 = y2;
    }

    public String toString(){
      return ("[x1,y1 | x2,y2] = ["+x1+","+y1+" <> "+x2+","+y2+"]");
    }
  }
}



