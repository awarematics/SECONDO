//This file is part of SECONDO.

//Copyright (C) 2005, University in Hagen, Department of Computer Science,
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


import java.io.*;
import java.util.*;

public class RTreePortal2MPoint{


/** Reads the dataset from a file. 
  * This method ensures that the array is ordered.
  * If problems in reading of the files occur, the result will be
  * null.
**/
private static Entry[] readDataSetFromFile(File F, boolean oldFormat){

 boolean sortRequired = false;
 // first we scan the file, count the entries and
 // check whether the entries are already sorted.
 // in the secondo scan, we create the array 
  Entry e1 = new Entry();
  Entry e2 = new Entry();
  int count = 0;
  int lineno =0;
  boolean first = true;
  String line;
  try {
      BufferedReader in = new BufferedReader(new FileReader(F));
      while(in.ready()){
         line = in.readLine();
         if(line!=null){
            lineno++;
            line = line.trim();
            if(line.length()>0){
                if(!e2.readFrom(line, oldFormat)){
                    System.err.println("Error in dataset at line "+lineno);
                    return null;
                }
                count ++;
                if(!first){
                    int cmp = e1.compareTo(e2);
                    if(cmp>0){
                      sortRequired=true;
                    }
                    if(cmp==0){
                       System.err.println("Error in dataset at line "+lineno);
                       return null;
                    }
                }
                first=false;
                e1.equalize(e2);
            }
         }      

      }
      in.close();
  } catch(Exception e){
    e.printStackTrace();
    return null;
  }

  System.err.println("File has" + count + "entries") ;

  Entry[] result = new Entry[count];
  for(int i=0;i<result.length;i++)
       result[i]=null;
  count = 0;
 
  try {
      BufferedReader in = new BufferedReader(new FileReader(F));
      while(in.ready()){
         line = in.readLine();
         if(line!=null){
            lineno++;
            line = line.trim();
            if(line.length()>0){
                if(!e1.readFrom(line, oldFormat)){
                    System.err.println("Error in dataset at line "+lineno);
                    return null;
                }
                result[count] = e1;
                e1 = new Entry();
                count ++;
            }
         }      

      }
      in.close();
  } catch(Exception e){
    e.printStackTrace();
    return null;
  }
  HeapSort.heapSort(result);

  return result;

}


public static void main(String[] args){
   int start=0;
   boolean oldStyle=false;
   boolean splitDays = false;
   boolean oldFormat = false;

   while(start < args.length && args[start].startsWith("--")){

     if(args[start].equals("--oldstyle")){
      oldStyle=true;
      start++;
     } else if( args[start].equals("--splitdays")){
       splitDays = true;
       start++;
     } else if(args[start].equals("--oldformat")){
       oldFormat = true;
       start++;
     } else {
        System.err.println("unknown option found " + args[start]);
        System.exit(1);
     }
   }

   if(start>=args.length){
      System.err.println("Missing input file name");
      System.exit(1);
   }

   File F = new File(args[start]);

   String name = F.getName();
   name = name.replaceAll("\\.","_");
   Entry[] data = readDataSetFromFile(F, oldFormat);
   long lastobjid = -1; // 
   long lasttrjid = -1;
   long lastDate = -1; 

   /* write the type */
   out.println(" ( OBJECT " + name + "  ()");
   out.println("    ( rel ( tuple ( ");
   out.println("                    (Objid int)");
   out.println("                    (Trjid int)");
   out.println("                    (Trip mpoint)");
   out.println("                  )))");
   out.println(" ("); // open value list

   UnitWriter uw=null;

   for(int i=0;i<data.length;i++){
      Entry E = data[i];
      if(i==0){ // the very first entry
        out.println(" ( " + E.objid + " " + E.trjid + " (");  // open tuple , write ids and open mpoint
        lastobjid = E.objid;
        lasttrjid = E.trjid;
        lastDate = E.date;
        uw = new UnitWriter(); 
      }
      if( lastobjid!=E.objid || lasttrjid!=E.trjid ||
          (splitDays && E.date != lastDate) ){     // 
         uw.write();  // write the last unit
         out.println(" ) )"); // close mpoint and tuple
         uw = new UnitWriter();
         lastobjid = E.objid;
         lasttrjid = E.trjid;
         lastDate = E.date;
         out.println(" ( " + E.objid + " " + E.trjid+ "("); // open the next tuple
      }
      uw.appendPoint(E.x,E.y,TimeUnit*E.time + E.date);
   }
   out.println(" ) ) "); // close clast mpoint and the last tuple
   if(oldStyle)
       out.println(" ) () ) "); // close valuelist and object
   else
       out.println(" ) ) ");  // do the same thinmg without model


}

private static PrintStream out = System.out;
private static double TimeUnit = 1.0/86400.0; // 1 second 
private static long NULLDAY = 730003; 

private static class Entry implements Comparable{

/** Constructor taking the values for the entry 
  **/
Entry(long objid, long trjid, long time, long x, long y, long unknown){
   this.objid = objid;
   this.trjid = trjid;
   this.time = time;
   this.date = 0;
   this.x = x;
   this.y = y;
   this.unknown = unknown;
}

Entry(){
   objid = 0;
   trjid = 0;
   date = 0;
   time = 0;
   x = 0;
   y = 0;
   unknown=0;
}

/** analyses the given string and constructs an entry
  * from it. If the format is not correct, the result will
  * be false, otherwise true.
  **/
boolean readFrom(String content, boolean oldstyle){

   if(oldstyle){
      StringTokenizer ST = new StringTokenizer(content,",",false);
      try{
         
         String Id = ST.nextToken().trim();
         String Time = ST.nextToken().trim();
         String X = ST.nextToken().trim();
         String Y = ST.nextToken().trim();
         String Unknown = ST.nextToken().trim();
         objid = 0;
         date = 0;
         trjid = Integer.parseInt(Id);
         time = Integer.parseInt(Time);
         x = Integer.parseInt(X);
         y = Integer.parseInt(Y);
         unknown = Integer.parseInt(Unknown);
         return true;
      } catch(Exception e){
         return false;
      }
   } else { // newer format
      StringTokenizer st = new StringTokenizer(content,";",false);
      try{
         String ObjId = st.nextToken().trim();
         String TrjId = st.nextToken().trim();
         String Date =  st.nextToken().trim();
         String Time = st.nextToken().trim();
         String Lat = st.nextToken().trim();
         String Lon = st.nextToken().trim();
         String X = st.nextToken().trim();
         String Y = st.nextToken().trim();
         objid = Integer.parseInt(ObjId);
         trjid = Integer.parseInt(TrjId);
         x = Double.parseDouble(X);
         y = Double.parseDouble(Y);
         StringTokenizer st2 = new StringTokenizer(Time,":",false);
         String HH = st2.nextToken().trim();
         String MM = st2.nextToken().trim();
         String SS = st2.nextToken().trim();
         int ss  = Integer.parseInt(SS);
         int mm = Integer.parseInt(MM);
         int hh = Integer.parseInt(HH);
         time = (hh*60 + mm)*60 + ss;

         StringTokenizer st3 = new StringTokenizer(Date,"/",false);
         int d = (Integer.parseInt(st3.nextToken().trim()));
         int m = (Integer.parseInt(st3.nextToken().trim()));
         int y = (Integer.parseInt(st3.nextToken().trim()));
         date = y* 365 + Month[m-1] + d - NULLDAY;
         return true;
      } catch (Exception e){
           e.printStackTrace();
           return false;
      } 
   }
}

public String toString(){

  return "" + objid+ ", " + trjid + "-"+date+ ", "+time+"-("+x+","+y+") - "+ unknown;
}

/** reads the content from e**/
void equalize(Entry e){
   objid = e.objid;
   trjid = e.trjid;
   time = e.time;
   date = e.date;
   x = e.x;
   y = e.y;
   unknown = e.unknown;
}


public int compareTo(Object o){
   if(! (o instanceof  Entry))
        return -1;

   Entry entry = (Entry)o;
   if(objid<entry.objid)
      return -1;
   if(objid>entry.objid)
      return 1;
   if(trjid<entry.trjid)
      return -1;
   if(trjid>entry.trjid)
      return 1;
   if(date<entry.date)
       return -1;
   if(date>entry.date)
      return 1;
   if(time<entry.time )
      return -1;
   if(time>entry.time)
     return 1;
   // check for validity. 
   // if id and time are equal, all other values must also be equual
   if(x!=entry.x){
       System.err.println("Error in dataset found: " + this.toString() + " <-> " + entry.toString() );
     
   }
   if(y!=entry.y){
       System.err.println("Error in dataset found" + this.toString() + " <-> " + entry.toString() );
   }
   if(unknown!=entry.unknown){
      System.err.println("Error in dataset found" + this.toString() + " <-> " + entry.toString() );
   }
   return 0;
}


long objid;
long trjid;
long date;
long time;
double x;
double y;
long unknown;

static int[] Month = {0 ,  //J
                31,  // F
                59,  // M
                90,  // A
                120,  // M
                151,  // J
                181 , // J
                212,  // A
                243,  // S
                273,  // O
                303,   // N
                334   // D
             };

}

private static class UnitWriter{


/** Appends a new point in time
  **/
boolean appendPoint(double x, double y, double t){
  if(!initialized){ // the first point
    x1 = x;
    y1 = y;
    t1 = t;
    initialized = true;
    return true; 
  }

  if(t<=t1) // time must be more than before
     return false;

  if(!isComplete){ // only one point is stored
    x2 = x;
    y2 = y;
    t2 = t;
    double dt = (t2-t1);
    // we compute the change of the coordinates within 1000 milliseconds
    dX = (x2-x1)/dt;
    dY = (y2-y1)/dt;
    isComplete=true;
    return true;
  }
  // check whether the new points are a extension
  // of the existing unit
  double dt = (t-t1);
  // compute the expected position of the points  
  double ex = x1 + dX*dt;
  double ey = y1 + dY*dt;
  //System.err.println("*********************");
  //System.err.println("x =" +x +"   ex = "+ex);
  //System.err.println("y =" +y +"   ey = "+ey);
  //System.err.println("*********************");
  


  if(Math.abs(ex-x)<=EPSILON && Math.abs(ey-y)<=EPSILON){ 
     // the expected points a near enough to ignore 
     // intermediate points
     t2 = t;
     x2 = x;
     y2 = y;
     skippedPoints++;
     return true;
  }
  // the new point is not a extension of the movement
  // write the old unit
  write();
  // and build a new one
  x1 = x2;
  y1 = y2;
  t1 = t2;
  x2 = x;
  y2 = y;
  t2 = t;
  written = false;
  dt = (t2-t1);
  dX = (x2-x1)/dt;
  dY = (y2-y1)/dt;
  isComplete=true;
  return true;
}





/** Writes this unit to the standard output **/
boolean write(){
   if(!isComplete)
      return false;
    System.out.print("    ("); // open unit
    System.out.print("("); // open interval
    System.out.print(t1+" "+t2);
    System.out.print(" TRUE FALSE"); // closeness of interval
    System.out.print(")"); // close interval
    System.out.print("("); // open map
    System.out.print(x1+" "+y1+" "+x2+" "+y2); // the map
    System.out.print(")"); // close map
    System.out.println(")"); // close unit
    written = true;
    writtenUnits++;
    return true;
}


private static void printStatistic(){
   System.err.println("written Units  : " + writtenUnits );
   System.err.println("skipped Points : " + skippedPoints);
}

// is true if the first point in time is stored
private boolean initialized = false;
private boolean isComplete = false;
private double lastTime=0.0;
private double x1 = 0.0;
private double y1 = 0.0;
private double t1 = 0.0;
private double x2 = 0.0;
private double y2 = 0.0;
private double t2 = 0.0;
private double dX = 0.0;
private double dY = 0.0;
private boolean written = false;

private static double EPSILON=0.00000;

private static long writtenUnits = 0;
private static long skippedPoints = 0;

}






}
