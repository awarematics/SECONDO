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

import  java.awt.geom.*;
import  java.awt.*;
import  sj.lang.ListExpr;
import  java.util.*;
import  viewer.*;
import viewer.hoese.*;
import tools.Reporter;


/**
 * The displayclass for rectangles
 */
public class Dsplrect extends DisplayGraph {
/** The internal datatype representation */
  Rectangle2D.Double rect;
  /**
   * Scans the numeric representation of a rectangle
   */
  private void ScanValue (ListExpr v) {
    if (v.listLength() != 4) {
      Reporter.writeError("Error: No correct rectangle expression: 4 elements needed");
      err = true;
      return;
    }
    Double X1 = LEUtils.readNumeric(v.first());
    Double X2 = LEUtils.readNumeric(v.second());
    Double Y1 = LEUtils.readNumeric(v.third());
    Double Y2 = LEUtils.readNumeric(v.fourth());
    if(X1==null || X2==null || Y1==null | Y2==null){
       Reporter.writeError("Error: no correct rectangle expression (not a numeric)");
       err =true;
       return;
    }
    try{
       double tx1 = X1.doubleValue();
       double tx2 = X2.doubleValue();
       double ty1 = Y1.doubleValue();
       double ty2 = Y2.doubleValue();
       if(!ProjectionManager.project(tx1,ty1,aPoint)){
          err = true;
       } else{
          double x1 = aPoint.x;
          double y1 = aPoint.y;
          if(!ProjectionManager.project(tx2,ty2,aPoint)){
             err=true;
          }  else{
            double x2 = aPoint.x;
            double y2 = aPoint.y;
            double x = Math.min(x1,x2);
            double w = Math.abs(x2-x1);
            double y = Math.min(y1,y2);
            double h = Math.abs(y2-y1);
            rect = new Rectangle2D.Double(x,y,w,h);
          }        
       }      
    }catch(Exception e){
       err = true;
    }
  }

  public int numberOfShapes(){
     return 1;
  }

  /** Returns the rectangle to display **/
  public Shape getRenderObject(int num,AffineTransform at){
     if(num<1){
       return rect;
     } else{
       return null;
     }
  }

  public void init (String name, int nameWidth,  int indent, ListExpr type, ListExpr value, QueryResult qr) {
    AttrName = extendString(name, nameWidth, indent);
    ScanValue(value);
    if (err) {
      Reporter.writeError("Error in ListExpr :parsing aborted");
      qr.addEntry(new String("(" + AttrName + ": GA(rectangle))"));
      return;
    } 
    else 
      qr.addEntry(this);
  }

}



