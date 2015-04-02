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

package tools;

import extern.*;
import sj.lang.ListExpr;
import java.util.TreeSet;
import java.io.*;

public class xxx2secdb{

private static ImportManager IM;
private static boolean oldStyle=false;


private static void AppendObject(PrintStream out,String FileName,TreeSet ONames) throws IOException{

  ListExpr LE;
  System.out.println("convert File: "+FileName);
  LE = IM.importFile(FileName);
  if(LE==null){
     System.err.println("error in converting File: "+FileName);
     return;
  }
  int length = LE.listLength();
  if(length!=2){
    if(length!=6 && length!=5){ // accept old an new styled object lists
       System.err.println("Result is not an object");
       return;
    }
    if(LE.first().atomType()!=ListExpr.SYMBOL_ATOM ||
       !LE.first().symbolValue().equals("OBJECT")){
       System.err.println("Result is not an object");
       return;
    }
  }

  String ObjectName;
  int ExtPos = FileName.lastIndexOf(".");
  if(ExtPos>0)
      ObjectName = FileName.substring(0,ExtPos).trim();
  else
      ObjectName = FileName;
  int PathPos;
  PathPos = Math.max(ObjectName.lastIndexOf("/"),ObjectName.lastIndexOf("\\"));
  if(PathPos>=0)
     ObjectName = ObjectName.substring(PathPos+1);
  //remove not allowed characters
  ObjectName = ObjectName.replace(' ','_');
  ObjectName = ObjectName.replace('.','_');
  ObjectName = ObjectName.replace('-','_');

  if(ONames.contains(ObjectName)){
     String tmpName;
     int number = 1;
     tmpName = ObjectName+"_"+number;
     while(ONames.contains(tmpName)){
        number++;
        tmpName = ObjectName+"_"+number;
     }
     ObjectName = tmpName;
  }
  ONames.add(ObjectName);

  ListExpr outList;
  if(LE.listLength()!=2)
     outList = LE;
  else{
     if(oldStyle){
          outList = ListExpr.sixElemList(ListExpr.symbolAtom("OBJECT"),
                                         ListExpr.symbolAtom(ObjectName),
	 			         ListExpr.theEmptyList(),
    				     LE.first(),
		    		     LE.second(),
				         ListExpr.theEmptyList());
      } else {
          outList = ListExpr.fiveElemList(ListExpr.symbolAtom("OBJECT"),
                                         ListExpr.symbolAtom(ObjectName),
	 			         ListExpr.theEmptyList(),
    				     LE.first(),
		    		     LE.second());
      }
   }
  
 //  out.print(outList.writeListExprToString());
    outList.writeTo(out,false);

}


private static void error(String Message){
    System.err.println(Message);
    System.exit(-1);
}

private static void showUsage(String Message){
    System.err.println(Message);
    System.err.println("java [--oldstyle] DBName SourceFiles");
    System.err.println("xxx2secdb creates a Secondo Database with the ");
    System.err.println("given name");
    System.err.println("The Database is written into a file with the same Name.");
    System.err.println("like the Database.");
    System.err.println("With the option --oldstyle, also the model mapping is part of an object");
    System.exit(-1);
}

public static void main(String[] args){
   ListExpr.initialize(500000);
   IM = new ImportManager();
   int start=0;
   if(args.length>0 && args[0].equals("--oldstyle")){
      oldStyle=true;
      start=1;
   }


   if(args.length<2+start){
       showUsage("Missing Parameter");
   }

  try{
     File F = new File(args[start]);
     if(F.exists()){
        System.out.println("File "+args[start]+ " exists. Overwrite it ? (y/n)");
        int i = System.in.read();
        if(i<0){
            error("no input");
        }
        if((char)i != 'y' && (char)i !='Y')
            System.exit(0);
     }
  }
  catch(Exception e){
      error("error while checking file");
  }

  TreeSet ONames = new TreeSet();
  try{
    PrintStream out = new PrintStream(new FileOutputStream(args[start]));
    // first print the header of a Database
    out.println("(DATABASE "+args[start]);
    if(oldStyle){
       out.println("   (DESCRIPTIVE ALGEBRA)");
       out.println("      (TYPES)");
       out.println("      (OBJECTS)");
       out.println("   (EXECUTABLE ALGEBRA)");
    }
    out.println("      (TYPES)");
    out.println("      (OBJECTS ");
    for(int i=start+1;i<args.length;i++){
       AppendObject(out,args[i],ONames);
    }
    out.println("))"); // close OBJECTS, DATABASE
  }
  catch(Exception e){
      e.printStackTrace();
      error("eror in processing files");
  }

}

}
