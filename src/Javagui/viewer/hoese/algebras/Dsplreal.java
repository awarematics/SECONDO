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


/**
 * A displayclass for the real-type, alphanumeric only
 */
public class Dsplreal extends DsplGeneric 
       implements LabelAttribute, RenderAttribute{

  // this value is represented
  private double value;
  private String entry;
  private boolean defined;


  private String computeValue(ListExpr value){
    if(isUndefined(value)){
       defined = false;
       this.value = 0.0;
       return "undefined";
    } else{
       Double v = LEUtils.readNumeric(value);
       if(v==null){
          this.value =0.0;
          defined = false;
          return "<error>"; 
       } 
       else {
           defined=true;
           this.value = v.doubleValue();
           return ""+this.value;
       }
    }
  }
  
  public void init (String name, int nameWidth, int indent,ListExpr type,
                    ListExpr value, QueryResult qr)
  {
     String T = name;
     String V = computeValue(value);
     T=extendString(T, nameWidth, indent);
     entry = (T + " : " + V);
     qr.addEntry(this);
  }

  public String toString(){
     return entry;
  }

  /** returns the maximum value **/
  public double getMaxRenderValue(){
     return value;
  }
  /** returns the minimum value **/
  public double getMinRenderValue(){
     return value;
  }
  /** returns the current value **/
  public double getRenderValue(double time){
     return value;
  }
  /** returns true if this int is defined **/
  public boolean mayBeDefined(){
      return defined;
  }
  /** returns tre if this integer is defined **/
  public boolean isDefined(double time){
     return defined;
  }
 /** returns the label **/
  public String getLabel(double time){
    return defined?""+value:"undefined";
  }


}



