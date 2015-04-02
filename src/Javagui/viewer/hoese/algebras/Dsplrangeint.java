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

import  sj.lang.ListExpr;
import  viewer.*;
import viewer.hoese.*;
import  java.util.*;


/**
 * A displayclass for the rangeint-type (spatiotemp algebra), alphanumeric only
 */
public class Dsplrangeint extends DsplGeneric {
  Vector ranges = new Vector(10, 5);

  /**
   * Scans the representation of a rangeint datatype, and constructs the ranges Vector 
   * @param v A list of integer intervals
   * @see sj.lang.ListExpr
   * @see <a href="Dsplrangeintsrc.html#ScanValue">Source</a>
   */
  private boolean ScanValue (ListExpr v) {
    int i = 0;
    while (!v.isEmpty()) {
      ListExpr le = v.first();
      if (le.listLength() != 4)
        return  false;
      if ((le.first().atomType() != ListExpr.INT_ATOM) || (le.second().atomType()
          != ListExpr.INT_ATOM) || (le.third().atomType() != ListExpr.BOOL_ATOM)
          || (le.fourth().atomType() != ListExpr.BOOL_ATOM))
        return  false;
      boolean leftcl = le.third().boolValue();
      boolean rightcl = le.fourth().boolValue();
      String left = (leftcl) ? "[" : "]";
      String right = (rightcl) ? "]" : "[";
      int firstval = le.first().intValue();
      int secondval = le.second().intValue();
      //ranges[i++]
      String s = new String(left + firstval + "," + secondval + right);
      ranges.add(s);
      v = v.rest();
    }
    return  true;
  }

  public void init (String name, int nameWidth, int indent, ListExpr type, ListExpr value, QueryResult qr) {
    name = extendString(name, nameWidth, indent);
    String e = "";
    if ((!ScanValue(value)) || value.isEmpty())
      qr.addEntry("(" + name + ":[?,?]"); 
    else {
      //qr.addEntry(type.symbolValue()+": "+ranges); //.elementAt(0));
      for (int i = 0; i < ranges.size(); i += 1)
        e = e + " " + (String)ranges.elementAt(i);
      qr.addEntry(name + ":" + e);
    }
    return;
  }
}



