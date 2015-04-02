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

package viewer.hoese.algebras;

import java.awt.geom.*;
import java.awt.*;
import viewer.*;
import viewer.hoese.*;
import sj.lang.ListExpr;
import java.util.*;
import viewer.hoese.algebras.periodic.*;
import javax.swing.JPanel;
import tools.Reporter;

public class Dsplpmreal extends DsplGeneric implements Function,ExternDisplay,LabelAttribute {

Class linearClass = (new PMRealUnit()).getClass();
Time T = new Time();
TotalMove Move=null;
String Entry=""; // text to display in textual list 

public String toString(){
   return Entry;
}

public void init(String name, int nameWidth,int indent, 
                 ListExpr type,
                 ListExpr value,
                 QueryResult qr){
  AttrName = name;
  Move = new TotalMove();
  String V; // value of this pmreal
  V = "pmreal";
  boolean ok = true;
  if(!Move.readFrom(value,linearClass)){
     V = "wrong list format";
     ok = false;
  }
  if(!Move.isDefined()){
     V = "undefined";
     ok = false;
  }
  AttrName = extendString(AttrName, nameWidth, indent);
  Entry = AttrName + " : "+V;
  if(!ok){
    qr.addEntry(Entry);
    return;
  }

  qr.addEntry(this);
  double StartTime = Move.getStartTime().getDouble();
  RelInterval D = Move.getInterval();
  double EndTime = StartTime;
  EndTime += D.getLength().getDouble();
  TimeBounds = new Interval(StartTime,EndTime,D.isLeftClosed(),D.isRightClosed());
}




/** Returns the interval of this periodic moving real */
 public Interval getInterval(){
    return TimeBounds;
 }

 /** Returns true when the function of this periodic moving real is displayed **/
 public boolean isExternDisplayed(){
      return(functionframe.isVisible() && this.equals(functionframe.getSource()));
  }

public void  displayExtern(){
      if(TimeBounds!=null){
         functionframe.setSource(this);
         functionframe.setVisible(true);
         functionframe.toFront();
      } else{
         Reporter.showInfo("The periodic moving real is empty");
      }
}


public Double getValueAt(double x){
  if(Move==null) return null;
  if(!Move.isDefined()) return null;
  anInstant.readFrom(x);
  return (Double) Move.getObjectAt(anInstant);
}

public String getLabel(double time){
  Double D = getValueAt(time);
  if(D==null){
     return "";
  } else {
     return ""+D;
  }

}


private static Time anInstant = new Time();

private Interval TimeBounds;

private static FunctionFrame functionframe = new FunctionFrame();

}
