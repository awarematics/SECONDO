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

import sj.lang.ListExpr;
import tools.Reporter;

public abstract class LinearMove implements Move{

/** creates a undefined Linear Move */
public LinearMove(){
  interval = new RelInterval();
  defined = false;
  isstatic = true;
}


/** returns the relative interval */
public RelInterval getInterval(){ return interval; }

/** returns the Object at the relative time positition */
abstract public Object getObjectAt(Time T);


/** reads the object value from LE */
abstract protected boolean readMap(ListExpr map);


/** reads this from the Given ListExpr */
public boolean readFrom(ListExpr LE,Class linearClass){
   if(LE.listLength()!=2){
      Reporter.debug("LinearMove.readFrom :: Wrong ListLength, should be 2, but is :"+LE.listLength());
      return false;
   }
   if(LE.first().atomType()!=ListExpr.SYMBOL_ATOM || !LE.first().symbolValue().equals("linear")){
      Reporter.debug("LinearMove.readFrom :: Typdescriptor 'linear' not found \n"+
      	             "list is "+LE.first().writeListExprToString());
      return false;
   }
   ListExpr Content = LE.second();
   int len = Content.listLength();
   if( (len!=3) && (len!=2) ){
     Reporter.debug("LinearMove.readFrom :: wrong listLength of the value, \n "+
	                   " expected : 3 or 2 ; get :"+Content.listLength());
     return false;
   }
   if(interval==null)
      interval = new RelInterval();
   if(!interval.readFrom(Content.first())){
     Reporter.debug("LinearMove.readFrom :: error in reading interval ");
     return false;
   }
   boolean ok;
   if(len==3)
       ok = readMap(ListExpr.twoElemList(Content.second(),Content.third()));
   else // len = 2
       ok = readMap(Content.second());

   if(!ok){
      defined = false;
   }


   if(!ok)
      Reporter.debug("LinearMove.readFrom :: error in method readStartEnd");
   return ok;

}

/** returns the type identificator for the total move */
abstract public String getName();

/** returns true if this objects is defined */
public boolean isDefined(){
   return defined;
}

/** returns whether this object is'nt changed in this interval */
public boolean isStatic(){
   return isstatic;
}

public BBox getBoundingBox(){
    return null;
}

public String toString(){
   return "LinearMove[ defined = "+defined+"  intervak = "+interval+"   static ="+isstatic+"]";
}

protected RelInterval interval;
protected boolean defined;
protected boolean isstatic;
}
