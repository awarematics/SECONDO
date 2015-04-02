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

package extern.shapedbf;

import extern.*;
import extern.shapereader.*;
import extern.dbfreader.*;
import sj.lang.ListExpr;
import java.util.*;
import tools.Reporter;

public class ShapeDbf implements SecondoImporter{

public ListExpr getList(String FileName){
  LastError = "no error";
  if(!FileName.endsWith(".dbf") && ! FileName.endsWith(".shp")){
    LastError ="unknow file extension";
    return null;
  }
  String ShapeFileName = FileName.substring(0,FileName.length()-3)+"shp";
  String DBFName = FileName.substring(0,FileName.length()-3)+"dbf";
  ShapeReader SR = new ShapeReader();
  ListExpr L1 = SR.getList(ShapeFileName);

  if(L1==null){
     LastError = "shapereader : "+SR.getErrorString();
     return null;
  }

  Dbf3Reader DR = new Dbf3Reader();
  ListExpr L2 = DR.getList(DBFName);

  if(L2==null){
     LastError = "db3reader: "+DR.getErrorString();
     return null;
  }

  ListExpr T1 = L1.first();
  ListExpr T2 = L2.first();
  ListExpr V1 = L1.second();
  ListExpr V2 = L2.second();
  if(V1.listLength()!=V2.listLength()){
    Reporter.writeWarning("different number of tuples in dbf and shape file:"+
                          V1.listLength()+"<->"+V2.listLength());
    return null;
  }
  ListExpr TupleList = T2.second().second();
  int length = TupleList.listLength();
  
  Vector UsedNames = new Vector(TupleList.listLength());
  while(!TupleList.isEmpty()){
     UsedNames.add(TupleList.first().first().symbolValue());
     TupleList = TupleList.rest();
  }
  String NumberName = "No";
  int i = 0;
  if(UsedNames.contains(NumberName)){
     while(UsedNames.contains(NumberName+"_"+i))
         i++;
     NumberName = NumberName+"_"+i;
  }
  String ShapeName = "Shape";
  i = 0;
  if(UsedNames.contains(ShapeName)){
     while(UsedNames.contains(ShapeName+"_"+i))
         i++;
     ShapeName = ShapeName+"_"+i;
  }

  ListExpr TupleNumber = ListExpr.twoElemList(ListExpr.symbolAtom(NumberName),
                                              ListExpr.symbolAtom("int"));
  ListExpr TupleShape = ListExpr.twoElemList(ListExpr.symbolAtom(ShapeName),
                                             T1.second().second().second().second());

  TupleList = T2.second().second();
  ListExpr Last = TupleList;

  // find the Last Tuple in DBFList
  while(Last.listLength()!=1)
     Last = Last.rest();

  Last = ListExpr.append(Last,TupleNumber);
  Last = ListExpr.append(Last,TupleShape);

  while(!V2.isEmpty()&&!V1.isEmpty()){
     ListExpr Tupel = V2.first();
     Last = Tupel;
     int alength = Last.listLength();
     while(alength!=1){
        Last = Last.rest();
        alength--;
     }
     Last = ListExpr.append(Last,V1.first().first());
     Last = ListExpr.append(Last,V1.first().second());
     V2 = V2.rest();
     V1 = V1.rest();
  }

  return L2;


}

public String getErrorString(){
  return LastError;
}

private String LastError;

}
