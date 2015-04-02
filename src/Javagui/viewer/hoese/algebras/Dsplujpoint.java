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

//2012, July Simone Jandt

package  viewer.hoese.algebras;

import viewer.hoese.algebras.jnet.*;
import java.awt.geom.*;
import java.awt.*;
import viewer.*;
import viewer.hoese.*;
import sj.lang.ListExpr;
import gui.Environment;
import javax.swing.JPanel;
import tools.Reporter;
import java.util.*;
import gui.SecondoObject;

/**
 * A displayclass for a single unit of the moving point type
 */
public class Dsplujpoint extends DisplayTimeGraph{

  private UJPoint ujp;
  private String label;


  public Dsplujpoint() {
    super();
  }

  public void init (String name, int nameWidth, int indent, ListExpr type,
                    ListExpr value, QueryResult qr) {
    AttrName = extendString(name, nameWidth, indent);
    try{
      ujp = new UJPoint (value);
      label = ujp.toString();
      qr.addEntry(this);
      TimeBounds = ujp.getBoundingInterval();
    } catch (JNetworkNotAvailableException ex){
      err = true;
      Reporter.writeError(ex.getMessage());
      qr.addEntry(AttrName + ex.getMessage());
    } catch (Exception ex) {
      ex.printStackTrace();
      err = true;
      Reporter.writeError("Error in ListExpr :parsing aborted");
      qr.addEntry(AttrName + "error");
    }
    return;
  }

  public boolean isPointType(int num){
    return ujp.isPointType(num);
  }

  public boolean isLineType(int num){
    return ujp.isLineType(num);
  }
  public int numberOfShapes(){
      return ujp.numOfShapes();
  }

  public Rectangle2D.Double getBounds(){
    return ujp.getBounds();
  }

  public Shape getRenderObject(int no, AffineTransform af){
    double t = RefLayer.getActualTime();
    double pointSize = Cat.getPointSize(renderAttribute,CurrentState.ActualTime);
    boolean asRect = Cat.getPointasRect();
    Point2D.Double actPos = ujp.getPointAtTime(t);
    if(actPos == null)
      return null;
    if(!ProjectionManager.project(actPos.x,actPos.y,actPos))
      return null;
    double pointSizeX = Math.abs(pointSize/af.getScaleX());
    double pointSizeY = Math.abs(pointSize/af.getScaleY());
    Shape shape;
    if (asRect) {
      shape = new Rectangle2D.Double(actPos.getX()- pointSizeX/2,
                                     actPos.getY()- pointSizeY/2,
                                     pointSizeX,
                                     pointSizeY);
    } else {
      shape = new Ellipse2D.Double(actPos.getX() - pointSizeX/2,
                                   actPos.getY() - pointSizeY/2,
                                   pointSizeX,
                                   pointSizeY);
    }
    return  shape;
  }

/** A method of the Timed-Interface
*
* @return the global time boundaries [min..max] this instance is defined at
* @see <a href="Dsplintimepointsrc.html#getTimebounds">Source</a>
*/
 public Interval getBoundingInterval () {
   return  ujp.getBoundingInterval();
 }

/** A method of the Timed-Interface
* @return The Vector representation of the time intervals this instance is defined at
* @see <a href="Dsplintimepointsrc.html#getIntervals">Source</a>
*/
public Vector getIntervals(){
  return ujp.getIntervals();
}

/**
* A method of the Timed-Interface to render the content of the TimePanel
* @param PixelTime pixel per hour
* @return A JPanel component with the renderer
* @see <a href="Dsplintimepointsrc.html#getTimeRenderer">Source</a>
*/
public JPanel getTimeRenderer (double PixelTime) {
  return ujp.getTimeRenderer(PixelTime);
}

};



