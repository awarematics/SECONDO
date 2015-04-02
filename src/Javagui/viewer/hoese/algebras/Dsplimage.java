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

import  javax.swing.*;
import  sj.lang.ListExpr;
import  viewer.*;
import  viewer.hoese.*;


/**
 * An example how to implement the DsplBase interface for a class displaying images in a
 * different frame.
 */
public class Dsplimage implements DsplBase {
  protected String AttrName;
  protected boolean selected;
  private boolean visible = true;
  private static ImageViewer TestFrame = new ImageViewer();
  JLabel lab;

  public String getAttrName () {
    return  AttrName;
  }

  public SecondoFrame getFrame () {
    return  TestFrame;
  }

  public void init (String name, int nameWidth, int indent, ListExpr type, ListExpr value, QueryResult qr) {
    AttrName = DsplGeneric.extendString(name,nameWidth, indent);
    lab = new JLabel(new ImageIcon(value.stringValue()));
    lab.setOpaque(true);
    qr.addEntry(this);
    return;
  }

  public void setVisible (boolean b) {
    visible = b;
  }

  public boolean getVisible () {
    return  visible;
  }
  public void setSelected (boolean b) {
    selected = b;
  }

  public boolean getSelected () {
    return  selected;
  }

  public String toString () {
    return  AttrName + ":ImageAttr";
  }

}



