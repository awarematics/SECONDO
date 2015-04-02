import sj.lang.ListExpr;
import viewer.hoese.*;
import java.io.*;
import java.util.Vector;

/**
  * This class creates a moving Region like described in the article
  * External Representation of Spatial and Spatio-Temporal Values
**/

public class RegTransLinear{
  

static int points;
static boolean firstRun;
static PrintStream out;
static boolean init;
static boolean firstUnit;
static java.text.DecimalFormat Format;
static String FormatString = "#.##########";
static boolean oldStyle=false;
static boolean triangles=false;


/** returns a point in nested list format which results by application of AT
  * to the point coded by AT. 
  **/
private static ListExpr processPoint(ListExpr PL, ATransform AT){
   Point P = new Point();
   if(!P.readFrom(PL)){
       System.err.println("error in reading point");
       System.exit(65); // data format error
   }
   AT.transformPoint(P);
   if(firstRun) points++;
   return ListExpr.twoElemList(ListExpr.realAtom(P.getX()),ListExpr.realAtom(P.getY()));
}

/**
  * transforms a single Cycle of a region
  **/
private static ListExpr processCycle(ListExpr Cycle,ATransform AT){
		if(Cycle.isEmpty())
			 return  new ListExpr();

		ListExpr Res = ListExpr.oneElemList(processPoint(Cycle.first(),AT));
		ListExpr Last = Res;
		Cycle = Cycle.rest();
		while(!Cycle.isEmpty()){
			 Last = ListExpr.append(Last,processPoint(Cycle.first(),AT));
			 Cycle = Cycle.rest();
		}
		return Res;
}

/*
 * transforms a single face of a region
*/
private static ListExpr processFace(ListExpr Face, ATransform AT){
   if(Face.isEmpty())
      return new ListExpr();

   ListExpr Res = ListExpr.oneElemList(processCycle(Face.first(),AT));
   Face = Face.rest();
   ListExpr Last = Res;

   while(!Face.isEmpty()){
      Last = ListExpr.append(Last,processCycle(Face.first(),AT));
      Face = Face.rest();
   }
   return Res;
}


/**
 * Transforms Region with the given transformation.
 * The Result will be the transformed region in nested list format.
**/
private static ListExpr processValueList(ListExpr Region, ATransform AT){
  if(Region.isEmpty())
     return new ListExpr();

  ListExpr Res = ListExpr.oneElemList(processFace(Region.first(),AT));
  ListExpr Last = Res;
  Region = Region.rest();
  while(!Region.isEmpty()){
     Last = ListExpr.append(Last,processFace(Region.first(),AT));
     Region = Region.rest();
  }
  return Res;
}



private static void writePointMap(ListExpr Start, ListExpr End){
   Double X1 = LEUtils.readNumeric(Start.first());
   Double Y1 = LEUtils.readNumeric(Start.second());
   Double X2 = LEUtils.readNumeric(End.first());
   Double Y2 = LEUtils.readNumeric(End.second());
   if(X1==null || X2==null || Y1==null || Y2 == null){
      System.err.println("Error in writing PointMap");
      System.err.println(" The Lists are :");
      System.err.println(" Start ="+Start.writeListExprToString());
      System.err.println(" End ="+End.writeListExprToString());
      System.exit(65); // data format error
   }
//   out.println("("+X1.doubleValue()+" "+Y1.doubleValue()+" "+
//                          X2.doubleValue()+" "+Y2.doubleValue()+")");

   double x1 = X1.doubleValue();
   double x2 = X2.doubleValue();
   double y1 = Y1.doubleValue();
   double y2 = Y2.doubleValue();
   out.println("("+Format.format(x1)+" "+Format.format(y1)+" "+
                          Format.format(x2)+" "+Format.format(y2)+")");
}



/** writes a moving segment-map resulting from the parameters 
  * to out. All movements are represented by triangles. Note that no
  * check is performed for ensuring the correct reprentations of the 
  * points.
  * @param fs: startpoint of one point of the segment
  * @param fe: the endpoint corresponding to fs
  * @param ss: second start points of this segment
  * @param se: location of ss at the end of this unit
  **/

private static void writeSegmentTriangles(ListExpr fs,
                                   ListExpr fe,
                                   ListExpr ss,
                                   ListExpr se){

     double fs_x = LEUtils.readNumeric(fs.first()).doubleValue();
     double fs_y = LEUtils.readNumeric(fs.second()).doubleValue();
     double fe_x = LEUtils.readNumeric(fe.first()).doubleValue();
     double fe_y = LEUtils.readNumeric(fe.second()).doubleValue();
     
     double ss_x = LEUtils.readNumeric(ss.first()).doubleValue();
     double ss_y = LEUtils.readNumeric(ss.second()).doubleValue();
     double se_x = LEUtils.readNumeric(se.first()).doubleValue();
     double se_y = LEUtils.readNumeric(se.second()).doubleValue();

     if(fs_x==ss_x && fs_y==ss_y){ // common start point
        if(fe_x==se_x && fe_y==se_y){ // common end point
          // write the map for a single point
          out.println("("+Format.format(fs_x)+" "+Format.format(fs_y)+" "
                         +Format.format(fe_x)+" "+Format.format(fe_y)+" )");
        } else{ // endpoints differa
          out.println("("+Format.format(fs_x)+" "+Format.format(fs_y)+" "
                         +Format.format(fe_x)+" "+Format.format(fe_y)+" )");
          out.println("("+Format.format(fs_x)+" "+Format.format(fs_y)+" "
                         +Format.format(se_x)+" "+Format.format(se_y)+" )");
        }
     }else{ // different start points
        if(fe_x==se_x && fe_y==se_y){ // common end point
          // write the map for a single point
          out.println("("+Format.format(fs_x)+" "+Format.format(fs_y)+" "
                         +Format.format(fe_x)+" "+Format.format(fe_y)+" )");
          out.println("("+Format.format(ss_x)+" "+Format.format(ss_y)+" "
                         +Format.format(fe_x)+" "+Format.format(fe_y)+" )");
        } else{ // all differ
          out.println("("+Format.format(fs_x)+" "+Format.format(fs_y)+" "
                         +Format.format(fe_x)+" "+Format.format(fe_y)+" )");
          out.println("("+Format.format(fs_x)+" "+Format.format(fs_y)+" "
                         +Format.format(se_x)+" "+Format.format(se_y)+" )");
        }
     }     
}
                      


/** write a single cyclemap to the standard output **/
private static void writeCycleMap(ListExpr Start, ListExpr End){
  if(!triangles){
     out.println("("); // open map
     while(!Start.isEmpty()){
         writePointMap(Start.first(),End.first());
         Start = Start.rest();
         End = End.rest();
     }
     out.println(")"); // close map
  } else{
   /* in the triangle mode we have to process whole segments */

   // ensure to havce at least 3 points whithin the cycle 
   if(Start.isEmpty() || Start.rest().isEmpty() || Start.rest().rest().isEmpty() ){
       return;
   }
   out.println("("); // open map
   ListExpr firstStart = Start.first(); 
   ListExpr firstEnd = End.first();
   
   ListExpr fs = firstStart;
   ListExpr fe = firstEnd;
   Start = Start.rest();
   End = End.rest();
   ListExpr ss=null;
   ListExpr se=null;
   while(!Start.isEmpty()){
      ss = Start.first();
      se = End.first();
      writeSegmentTriangles(fs,fe,ss,se);
      Start=Start.rest();
      End = End.rest();
      fs = ss;
      fe = se;
      if(!Start.isEmpty()){
        ss = Start.first();
        se = End.first();
      }
   }
   // close the cycle
   writeSegmentTriangles(ss,se,firstStart,firstEnd); 
   out.println(")"); // close map
  }
}

/** write a single facemap to the standard output **/
private static void writeFaceMap(ListExpr Start, ListExpr End){
  out.println("("); // open map
  while(!Start.isEmpty()){
      writeCycleMap(Start.first(),End.first());
      Start = Start.rest();
      End = End.rest();
  }
  out.println(")"); // close map
}

/** write the Region map to the standardoutput */
private static void writeRegMap(ListExpr Start, ListExpr End){
  out.println("("); // open map
  while(!Start.isEmpty()){
      writeFaceMap(Start.first(),End.first());
      Start = Start.rest();
      End = End.rest();
  }
  out.println(")"); // close map
}

/**This function writes a single unit computed from the arguments to the
  * out
  **/
private static void writeUnit(double startTime, double moveTime,ListExpr Start, ListExpr End){
     out.println("(");  // open unit
     out.print("  ("); // open interval
     out.print("\""+DateTime.getString(startTime)+"\"");
     out.print(" \""+DateTime.getString(startTime+moveTime)+"\"");
     out.print(" TRUE FALSE ");
     out.println(" )"); // close interval
     writeRegMap(Start,End);  // write the map
     out.println(")"); // close unit
}

/* write the value of a moving region resulting from the
 * region in the list to the standard output.
 */
private static void writeMRegionValue(ListExpr RValue,ListExpr TransformList,double startTime, double moveTime){
  ListExpr NextValue;
  out.println("("); // open value
  firstUnit =true;
  ATransform AT = new ATransform();
  
  while(!TransformList.isEmpty()){
     if(!AT.readFrom(TransformList.first())){
       System.err.println("error in TransformationList");
       System.exit(65);
     }
     TransformList = TransformList.rest();
     NextValue = processValueList(RValue,AT);
     firstRun=false;
     if(!init || !firstUnit){ 
        writeUnit(startTime,moveTime,RValue,NextValue);
     } else{
        firstUnit=false;
     }
     startTime += moveTime;
     RValue = NextValue;
  }
    out.println(")"); // close value 
}

private static void processRelation(ListExpr Type, ListExpr Value,ListExpr TransformList,
                                    double startTime, double moveTime ){
   Vector V = new Vector(5); // store all attributnumbers containing regions
   ListExpr Tuple = Type.second();
   if(Tuple.atomType()!=ListExpr.NO_ATOM && Tuple.listLength()!=2){
        System.err.println("error in tuple definition");
        System.exit(65);
   } 
   ListExpr AttrList = Tuple.second();
   int number=1;
   out.print(" (rel(tuple (");
   while(!AttrList.isEmpty()){
      ListExpr First = AttrList.first();
      if(First.listLength()!=2){
         System.err.println("error in analyse of attribute list"); 
      }
      ListExpr TNList = First.second();

      if(TNList.atomType()==ListExpr.SYMBOL_ATOM && TNList.symbolValue().equals("region")){
         V.add(new Integer(number));
         out.print("(");
         First.first().writeTo(out,false);
         out.print(" movingregion)");
      } else{
         First.writeTo(out,false); 
      }
      number++;
      AttrList = AttrList.rest();
   }
   out.println(")))");
   out.println("("); // open value list
   while(!Value.isEmpty()){
      out.println("( ");
      ListExpr CV = Value.first(); // the current tuple
      number = 1;
      while(!CV.isEmpty()){
        Integer currentInt=new Integer(number);
        ListExpr Attr=CV.first();
        if(V.contains(currentInt)){ // a region
         writeMRegionValue(Attr,TransformList,startTime,moveTime);  
        } else{
             Attr.writeTo(out,false);
             out.print(" ");
        }
        CV = CV.rest();
        number++;
      } 
      out.println(")");
      Value=Value.rest();
   }

   out.println(")"); // close relation

}


public static void main(String[] args){

  int start=0;
  if(args.length>0 && args[0].equals("--oldstyle")){
     oldStyle=true;
     start++;
  }
  if(args.length>start && args[start].equals("--triangle")){
     triangles=true;
     start++;
  }


  java.text.DecimalFormatSymbols dfs = new java.text.DecimalFormatSymbols();
  dfs.setDecimalSeparator('.');
  Format = new java.text.DecimalFormat(FormatString,dfs);

  // process arguments
  if(args.length<start+3){
     System.err.println ("missing argument");
     System.err.println("usage:java RegTransformer [--oldstyle] {RegionFile|RelationFile} TransformFile outFile [init]");
     System.exit(64); // command line usage error
  }
  ListExpr RegList = new ListExpr();
  if(RegList.readFromFile(args[start])!=0){
    System.err.println(" error in reading RegionFile");
    System.exit(65);
  }
  ListExpr TransformList = new ListExpr();
  if(TransformList.readFromFile(args[start+1])!=0){
     System.err.println("error in reading transformation file");
     System.exit(65);
  }

  try{
    out = new PrintStream(new FileOutputStream(args[start+2]));
  }catch(Exception e){
    System.err.println("error in opening output file"); 
    System.exit(73); // can't create user output file
  }
  init = false;
  if(args.length>start+3 && args[start+3].equals("init"))
    init = true;
 
  // initialize  
  double startTime = 0.0;
  double moveTime = 0.25; // this means 6 hours
  boolean isObject = false;
  ListExpr Sixth=null;
  int listlength=RegList.listLength();

  // old styled object format
  if(RegList.listLength()==6 && RegList.first().atomType()==ListExpr.SYMBOL_ATOM &&
     RegList.first().symbolValue().equals("OBJECT")){
     Sixth=RegList.sixth();
     isObject=true;
     out.print(" ( OBJECT ");
     RegList.second().writeTo(out,false);
     out.print(" ");
     RegList.third().writeTo(out,false);
     out.println(" ");
     RegList=ListExpr.twoElemList(RegList.fourth(),RegList.fifth());
  } else if(RegList.listLength()==5 && RegList.first().atomType()==ListExpr.SYMBOL_ATOM &&
     RegList.first().symbolValue().equals("OBJECT")){
     Sixth=new ListExpr();;
     isObject=true;
     out.print(" ( OBJECT ");
     RegList.second().writeTo(out,false);
     out.print(" ");
     RegList.third().writeTo(out,false);
     out.println(" ");
     // change reglist to the desired format
     RegList=ListExpr.twoElemList(RegList.fourth(),RegList.fifth());
  } else{
     out.println("("); // open object 
  }

  if(RegList.first().atomType()==RegList.SYMBOL_ATOM && RegList.first().symbolValue().equals("region")){
  // process a single region
      ListExpr RValue = RegList.second();
      firstRun=true; // only needed to count the number of
                 // contained points
      out.println(" movingregion "); // write type 

      writeMRegionValue(RValue,TransformList,startTime,moveTime);
      System.err.println("processed "+points+" points");

  } else{
      ListExpr Type = RegList.first();
      if(Type.atomType()==ListExpr.NO_ATOM && Type.listLength()==2 && Type.first().atomType()==ListExpr.SYMBOL_ATOM &&
         Type.first().symbolValue().equals("rel")){
           processRelation(Type,RegList.second(),TransformList,startTime,moveTime);
      } else{
         System.err.println("No relation and no single region");
     }
  }
  if(isObject){
     out.print(" ");
     if(oldStyle)
        Sixth.writeTo(out,false);
  }
  out.println(" )"); // close object


    try{
       out.close();
    }catch(Exception e){
       System.err.println("Warning: Error in Closing output file");
    }
}
  
}
