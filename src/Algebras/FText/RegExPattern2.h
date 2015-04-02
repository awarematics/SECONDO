

/*
----
This file is part of SECONDO.

Copyright (C) 2007, 
Faculty of Mathematics and Computer Science,
Database Systems for New Applications.

SECONDO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

SECONDO is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SECONDO; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
----


1 class RegExPattern2

This class is the implementation of a finite automaton. 

In contrast to RegExPattern, this class is a pure main memory
implementation which is faster than RegExPattern but cannot be 
use as an attribute data type


*/


#ifndef REGEXPATTERN2_H
#define REGEXPATTERN2_H


#define NUMCHARS 256

#include "IntNfa.h"
#include "NestedList.h"
#include "ListUtils.h"
#include "SecondoSMI.h"


int parseRegEx(const char* argument, IntNfa** T);

class RegExPattern2{

 public:

/*
1.1 Constructors

*/  
   RegExPattern2() {}
   RegExPattern2(bool _defined): defined(_defined),
                               numOfStates(0),
                               transitions(),
                               finalStates(),
                               src(""),
                               srcDefined(false) {}

   RegExPattern2(const RegExPattern2& p): defined(p.defined),
                                       numOfStates(p.numOfStates),
                                       transitions(p.transitions),
                                       finalStates(p.finalStates),
                                       src(p.src), 
                                       srcDefined(p.srcDefined){
  }

  bool hasSource() const{
     return srcDefined;
  }
  std::string getSource() const{
     return src;
  }
  void setSource(const std::string& src){
     this->src = src;
     srcDefined = true;
  }
               
  RegExPattern2& operator=(const RegExPattern2& p){
     defined = p.defined;
     numOfStates = p.numOfStates;
     transitions = p.transitions;
     finalStates = p.finalStates;
     src = p.src;
     srcDefined = p.srcDefined;
     return *this;
  }


/*
1.3 Destructor

*/
  ~RegExPattern2() {}


/*
1.4 Clone function

*/

   RegExPattern2* Clone() const{
     return new RegExPattern2(*this);
   }


/*
1.5 SetDefined

Sets the defined flag and possible (in false case) 
clears the contained vectors.

*/
   void SetDefined(const bool def){
      defined = def;
      if(!def){
         numOfStates = 0;
         transitions.clear();
         finalStates.clear();
         src = "";
         srcDefined = false; 
      } 
   }

   void SetDefinedKeepSource(const bool def){
      defined = def;
      if(!def){
         numOfStates = 0;
         transitions.clear();
         finalStates.clear();
      }
   }


/*
1.6 IsDefined

Checks whether this automaton is a defined one.

*/

   inline bool IsDefined() const{
      return defined;
   }


/*
1.7 constructFrom

Creates a dfa from a regular expression.

*/
   bool constructFrom(const std::string& regex){
      IntNfa* nfa;
      setSource(regex);
      if(parseRegEx(regex.c_str(),&nfa)!=0){
         SetDefinedKeepSource(false);
         return false;
      } 
      nfa->nfa.makeDeterministic();
      nfa->nfa.minimize();        
      nfa->nfa.bringStartStateToTop();
      transitions.clear();
      finalStates.clear();
      numOfStates = nfa->nfa.numOfStates();
      for(int i=0;i<numOfStates;i++){
         bool isFinal = (nfa->nfa.isFinalState(i));
         finalStates.push_back(isFinal);
      }
      for(int i=0;i<numOfStates;i++){
        State<int> s = nfa->nfa.getState(i);
        for(int c=0;c<NUMCHARS;c++){
           std::set<int> t = s.getTransitions(c);
           if(t.empty()){
              transitions.push_back(-1);
           } else {
              assert(t.size()==1);
              transitions.push_back(*t.begin());
           }
        } 
      }
      delete nfa;
      SetDefined(true);
      return true;
   }


/*
1.8 matches

Checks whether the given string is a word of the
language represented by this dfa.

*/
   bool matches(const std::string& text){
      if(!IsDefined()){
        return false;
      }
      if((finalStates.size()==0) || (numOfStates==0)){
        return false;
      }
      int state = 0;
      const char* tc = text.c_str();
      for(size_t i=0;i<text.length();i++){
         state = transitions[state*NUMCHARS+(unsigned char)tc[i]];
         if(state<0){
           return false;
         }
      }
      return finalStates[state];
   }

/*
1.9 starts

Checks whether the argument starts with  any word of the language
represented by this automaton.

*/

   bool starts(const std::string& text){
      if(!IsDefined()){
        return false;
      }
      if((finalStates.size()==0) || (numOfStates==0)){
        return false;
      }
      int state = 0;
      const char* tc = text.c_str();
      for(size_t i=0;i<text.length();i++){
         if(finalStates[state]){
            return true;
         }   
         state = transitions[state*NUMCHARS+(unsigned char)tc[i]];
         if(state<0){
           return false;
         }
      }
      return finalStates[state];
   }


/*
1.8 starts2

This function checks whether text at position offset starts with
this pattern. The length of the substring is returned.

*/
   bool starts2(const string& text, const int offset, int& length, 
                const bool findMax, const bool allowEmpty){
       if(!IsDefined()){
         return false;
       }  
      if((finalStates.size()==0) || (numOfStates==0)){
        return false;
      }
      length=0;
      int state = 0;
      bool isFinal;
      int  lastFinal = -1;
      for(size_t i=offset; i<text.length();i++){
          isFinal = finalStates[state];
          if(isFinal){
            if(findMax){
              lastFinal = length;
            } else if((length>0) || allowEmpty){
                return true;
            }
          }
          state = transitions[state*NUMCHARS+(unsigned char)text[i]];
          length++;
          if(state<0){
             if(findMax){
               if((lastFinal>0) || ((lastFinal>=0) && allowEmpty)){
                 length = lastFinal;
                 return true;
               } else {
                 return false;
               }
             } else {
                 return false;
             }
          }  
      } // end of text reached

      isFinal = finalStates[state];
      if(findMax){
          if(isFinal){
            return   allowEmpty || (length>0);
          } else if((lastFinal>0) || ((lastFinal>=0)&&allowEmpty)){
             length = lastFinal;
             return true;  
          } else {
             return false;
          }
      } else {
         return isFinal && ((length>0) || allowEmpty);
      }
   }

/*
1.10 readFrom

Reads this automaton from a nested list representation.

*/
   bool ReadFrom(const ListExpr value, const ListExpr typeInfo){

     if(listutils::isSymbolUndefined(value)){
        SetDefined(false);
        return true;
     }

     if(nl->AtomType(value)==StringType){
        return constructFrom(nl->StringValue(value));
     }

     if(nl->AtomType(value)==TextType){
        return constructFrom(nl->Text2String(value));
     }

     transitions.clear();
     finalStates.clear();
     SetDefined(true);      
     if(!nl->HasLength(value,3) && !nl->HasLength(value,4)){
       SetDefined(false);
       return false;
     }     
     if(nl->HasLength(value,4)){
       ListExpr sourceList = nl->Fourth(value);
       if( (nl->AtomType(sourceList)==StringType) || 
           (nl->AtomType(sourceList)==TextType)){
         setSource(listutils::stringValue(sourceList));
       } else {
           SetDefined(false);
           return false;
       }
     }
     if(nl->AtomType(nl->First(value))!=IntType){
        SetDefined(false);
        return false;
     }
     int ns = nl->IntValue(nl->First(value));
     numOfStates=ns;
     for(int i=0;i<ns*NUMCHARS;i++){
         transitions.push_back(-1);
     } 
     for(int i=0;i<ns;i++){
       finalStates.push_back(false);
     }
     ListExpr T = nl->Second(value);
     while(!nl->IsEmpty(T)){
       ListExpr t = nl->First(T);
       T = nl->Rest(T);
       if(!nl->HasLength(t,3)){
         SetDefined(false);
         return false;
       }
       if((nl->AtomType(nl->First(t))!=IntType) ||
          (nl->AtomType(nl->Second(t))!=IntType) ||
          (nl->AtomType(nl->Third(t))!=IntType)){
          SetDefined(false);
          return false;
       }
       int source = nl->IntValue(nl->First(t));
       int arg   = nl->IntValue(nl->Second(t));
       int target = nl->IntValue(nl->Third(t));
       if(arg<0 || arg >= NUMCHARS){
          SetDefined(false);
          return false;
       }
       if(source <0 || source >= numOfStates || target<0 || target >=NUMCHARS){
         SetDefined(false);
         return  false;
       }
       transitions[source*NUMCHARS+arg] = target;
     }
     ListExpr F = nl->Third(value);
     while(!nl->IsEmpty(F)){
       ListExpr f = nl->First(F);
       F = nl->Rest(F);
       if(nl->AtomType(f)!=IntType){
          SetDefined(false);
          return false;
       }
       int fi = nl->IntValue(f);
       if(fi<0 || fi>=ns){
          SetDefined(false);
          return false;
       }
       finalStates[fi] = true;
     }
     return true;
   }

/*

1.11 ToListExpr

Converts this dfa into it's nested list representation.

*/

   ListExpr ToListExpr(const ListExpr typeInfo){
     if(!IsDefined()){
        return listutils::getUndefined();
     }
     ListExpr ns = nl->IntAtom(numOfStates);
     ListExpr tr = nl->TheEmptyList();
     ListExpr last = nl->TheEmptyList();
     bool first = true;
     for(unsigned int i=0;i<transitions.size();i++){
         int t = transitions[i];
         if(t>=0){
           ListExpr trans = nl->ThreeElemList(nl->IntAtom(i/NUMCHARS),
                                              nl->IntAtom(i%NUMCHARS),
                                              nl->IntAtom(t));
           if(first){
             tr = nl->OneElemList(trans);
             last = tr;
             first = false;
           } else {
             last = nl->Append(last,trans);
           }
         }
     } 
     ListExpr fin = nl->TheEmptyList();
     first = true;
     for(unsigned int i=0;i<finalStates.size();i++){
        bool isf = finalStates[i];
        if(isf){
          if(first){
             fin = nl->OneElemList(nl->IntAtom(i));
             last = fin;
             first = false;
          } else {
             last = nl->Append(last,nl->IntAtom(i));
          }
        }
     }
     if(!srcDefined){
        return nl->ThreeElemList(ns,tr,fin);
     } else {
        return nl->FourElemList(ns,tr,fin, nl->TextAtom(src));
     }
   }

/*
Secondo specific operators

*/

   static const std::string BasicType(){
     return "regex2";
   }

   static bool checkType(ListExpr type){
      return listutils::isSymbol(type, BasicType());
   }

    
   static ListExpr Property(){
      return (nl->TwoElemList(
            nl->FourElemList(nl->StringAtom("Signature"),
                             nl->StringAtom("Example Type List"),
                             nl->StringAtom("List Rep"),
                             nl->StringAtom("Example List")),
            nl->FourElemList(nl->StringAtom("-> Simple"),
                             nl->StringAtom(BasicType()),
                             nl->StringAtom("(numStates (transitions)"
                                            " (finalstates)"),
                             nl->StringAtom("(2 ( (0 97 1)) (1)) "))));
   }

   static bool Check(const ListExpr type, ListExpr& errorInfo){
      return nl->IsEqual(type,BasicType());
   }

   static ListExpr Out(const ListExpr typeInfo, const Word value){
      RegExPattern2* p = (RegExPattern2*) value.addr;
      return p->ToListExpr(typeInfo);
   }
   
   static Word In(const ListExpr typeInfo, const ListExpr value,
                  const int errorPos, ListExpr& errorInfo, bool& correct){
      RegExPattern2* p = new RegExPattern2(false);
      correct = p->ReadFrom(value,typeInfo);
      Word res;
      if(!correct){
        delete p;
        res.addr = 0;
      } else {
        res.addr = p;
      }
      return res;
   }

   static Word Create(const ListExpr typeInfo){
      Word res;
      res.addr = new RegExPattern2(false);
      return res;
   }
  
   static void Delete(const ListExpr typeInfo, Word& value){
      delete (RegExPattern2*) value.addr;
      value.addr = 0;
   }

   static void Close(const ListExpr typeInfo, Word& value){
      delete (RegExPattern2*) value.addr;
      value.addr = 0;
   }

   static Word Clone(const ListExpr typeInfo,const Word& w){
      Word res;
      res.addr = ((RegExPattern2*)w.addr)->Clone();
      return res;
   }
   static int SizeOf(){
       return sizeof(RegExPattern2);
   }

   static void* Cast(void* w){
      return new(w) RegExPattern2();
   }

   static bool Open(SmiRecord& valueRecord,
                    size_t& offset,
                    const ListExpr typeInfo,
                    Word& value ) {
       RegExPattern2* p = new RegExPattern2(true);
       valueRecord.Read(&(p->defined), sizeof(bool), offset);
       offset += sizeof(bool);
       valueRecord.Read(&(p->numOfStates),sizeof(int), offset);
       offset += sizeof(int);
       for(int i=0;i<p->numOfStates*NUMCHARS;i++){
          int target;
          valueRecord.Read(&target,sizeof(int),offset);
          offset+=sizeof(int);
          p->transitions.push_back(target);
       }   
       for(int i=0;i<p->numOfStates;i++){
          bool isFinal;
          valueRecord.Read(&isFinal,sizeof(bool), offset);
          offset += sizeof(bool);
          p->finalStates.push_back(isFinal);
       }
       int srcLength;
       valueRecord.Read(&srcLength,sizeof(int), offset);
       offset += sizeof(int);
       if(srcLength<0){
         p->srcDefined = false;
         p->src = "";
       } else {
          p->srcDefined = true;
          if(srcLength==0){
             p->src = "";
          } else {
             char* buffer = new char[srcLength];
             valueRecord.Read(buffer,srcLength,offset);
             offset += srcLength;
             p->src = std::string(buffer,srcLength);
             delete buffer; 
          }
       }
       value.addr = p;
       return true;
   }

   static bool Save(SmiRecord& valueRecord,
                    size_t& offset,
                    const ListExpr typeInfo,
                    Word& value ) {
       RegExPattern2* p = (RegExPattern2*) value.addr;
       valueRecord.Write(&(p->defined),sizeof(bool),offset);
       offset+=sizeof(bool);
       valueRecord.Write(&(p->numOfStates),sizeof(int),offset);
       offset += sizeof(int);
       assert((int) p->transitions.size() == NUMCHARS*p->numOfStates);
       for(unsigned int i = 0; i< p->transitions.size();i++){
           int target = p->transitions[i];
           valueRecord.Write(&target,sizeof(int),offset);
           offset += sizeof(int);
       }  
       assert((int) p->finalStates.size() == p->numOfStates);
       for(int i=0;i<p->numOfStates;i++){
          bool isFinal = p->finalStates[i];
          valueRecord.Write(&isFinal,sizeof(bool), offset);
          offset += sizeof(bool);
       } 
       if(!p->srcDefined){
         int srcLength = -1;
         valueRecord.Write(&srcLength, sizeof(int), offset);
         offset += sizeof(int);
       } else {
         int srcLength = strlen(p->src.c_str());
         valueRecord.Write(&srcLength, sizeof(int), offset);
         offset += sizeof(int);
         valueRecord.Write(p->src.c_str(), srcLength, offset);
         offset += srcLength;   
       }
       return true;
   }


   bool computeRegEx(stringstream& ss ){
      regexcreator::DeaRegEx automaton;
      for(int s=0;s<numOfStates;s++){
         for(int c=0;c<NUMCHARS;c++){
             int state = nextState(s,c);
             if(state>=0){
                automaton.addEdge(s,state,c);
             }     
         }
      }
      automaton.setStart(0);
      for(int i=0;i<numOfStates;i++){
         if(isFinal(i)){
            automaton.addFinal(i);
         }
      }
      regexcreator::RegEx* re = automaton.computeRegEx(true);
      if(re==0){
          return false;
      }
      re->printTo(ss);
      delete re;
      return true;
   }
   

   bool isFinal(int i){
      if(i<0 || i>numOfStates) return false;
      return  finalStates[i];
   }

 private: 
   bool defined;
   int numOfStates;
   std::vector<int>  transitions;
   std::vector<bool> finalStates;
   std::string src;
   bool srcDefined;


   int nextState(const int state, const int c) const{
      assert(state >= 0 && state < numOfStates);
      assert(c>=0 && c<=NUMCHARS);
      int pos = state*NUMCHARS+c;
      return transitions[pos];
   }


};


#endif

