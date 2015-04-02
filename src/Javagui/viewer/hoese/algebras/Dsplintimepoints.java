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
import  viewer.*;
import viewer.hoese.*;
import  sj.lang.ListExpr;
import  java.util.*;
import  javax.swing.*;
import tools.Reporter;


/**
 * A displayclass for the intimepoints-type (spatiotemp algebra), 2D with TimePanel
 */
public class Dsplintimepoints extends Dsplpoints implements Timed {
  Interval boundingInterval;

  /** A method of the Timed-Interface
   * 
   * @return the global time boundaries [min..max] this instance is defined at
   */
  public Interval getBoundingInterval () {
    return  boundingInterval;
  }

  /**
   * A method of the Timed-Interface to render the content of the TimePanel
   * @param PixelTime pixel per hour
   * @return A JPanel component with the renderer
   * @see <a href="Dsplintimepointssrc.html#getTimeRenderer">Source</a>
   */
  public JPanel getTimeRenderer (double PixelTime) {
    int start = 0;              
    JLabel label = new JLabel("|"+LEUtils.convertTimeToString(boundingInterval.getStart()).substring(11, 
        16), JLabel.LEFT);
    label.setBounds(start, 15, 100, 15);
    label.setVerticalTextPosition(JLabel.CENTER);
    label.setHorizontalTextPosition(JLabel.RIGHT);
    JPanel jp = new JPanel(null);
    jp.setPreferredSize(new Dimension(100, 25));
    jp.add(label);
    //Add labels to the JPanel. 
    return  jp;
  }

  /**
   * Tests if a given position is contained in any of the points,but only if ActualTime == defined time
   * @param xpos The x-Position to test.
   * @param ypos The y-Position to test.
   * @param scalex The actual x-zoomfactor 
   * @param scaley The actual y-zoomfactor
   * @return true if x-, ypos is contained in this points type
   * @see <a href="Dsplintimepointssrc.html#contains">Source</a>
   */
  public boolean contains (double xpos, double ypos, double scalex, double scaley) {
    double t = RefLayer.getActualTime();
    if (Math.abs(t - boundingInterval.getStart()) < 0.000001)
      return  super.contains(xpos, ypos, scalex, scaley); 
    else 
      return  false;
  }

  /**
   * Scans the representation of an instant datatype 
   * @param v An instant value
   * @see sj.lang.ListExpr
   * @see <a href="Dsplintimepointssrc.html#ScanValue">Source</a>
   */
  public void ScanValue (ListExpr v) {
    Double d;
    if (v.listLength() != 2) {                  //perhaps changes later
      Reporter.writeError("Error: No correct intimepoints expression: 2 elements needed");
      err = true;
      return;
    }
    d = LEUtils.readInstant(v.first());
    if (d == null) {
      err = true;
      return;
    }
    boundingInterval = new Interval(d.doubleValue(), d.doubleValue(), true, true);
    super.ScanValue(v.second());
  }

  /**
   * Init. the Dsplintimepoints instance.
   * @param type The symbol intimepoints
   * @param value The value of an instant and a points-datatype
   * @param qr queryresult to display output.
   * @see generic.QueryResult
   * @see sj.lang.ListExpr
   * @see <a href="Dsplintimepointssrc.html#init">Source</a>
   */
  public void init (String name, int nameWidth, int indent,ListExpr type, ListExpr value, QueryResult qr) {
    AttrName = extendString(name, nameWidth, indent);
    ScanValue(value);
    if (err) {
      Reporter.writeError("Error in ListExpr :parsing aborted");
      qr.addEntry(new String("(" + AttrName + ": GTA(IntimePoints))"));
      return;
    } 
    else 
      qr.addEntry(this);
    
    bounds = null;
    for(int i=0;i<points.length;i++) {
      Point2D.Double p = points[i];
      if (bounds == null)
        bounds = new Rectangle2D.Double(p.getX() - 2, p.getY() - 2, 4, 4); 
      else 
        bounds = (Rectangle2D.Double)bounds.createUnion(new Rectangle2D.Double(
            p.getX() - 2, p.getY() - 2, 4, 4));
    }
  }
  /** A method of the Timed-Interface
   * @return The Vector representation of the time intervals this instance is defined at 
   * @see <a href="Dsplintimepointssrc.html#getIntervals">Source</a>
   */
  public Vector getIntervals(){
    Vector v=new Vector(1,0);
    v.add(boundingInterval);
    return v;
    } 

}



