/*
//paragraph [1] Title: [{\Large \bf \begin{center}] [\end{center}}]
//paragraph [10] Footnote: [{\footnote{] [}}]
//[TOC] [\tableofcontents]

[1] Implementation of the treeheight Operator

[TOC]

0 Overview

*/

#include "op_set_maxkeysize.h"

#include "ListUtils.h"
#include "QueryProcessor.h"
#include "RelationAlgebra.h"
#include "TupleIdentifier.h"
#include "BTree2.h"

#include <limits>

extern NestedList* nl;
extern QueryProcessor *qp;

namespace BTree2Algebra {
namespace Operators {

ListExpr set_maxkeysize::TypeMapping( ListExpr args){
    if(nl->ListLength(args) != 1){
      return listutils::typeError("Operator expects one argument");
    }
    ListExpr second = nl->First(args);
    if(!nl->IsEqual(second, CcInt::BasicType())){
      return listutils::typeError( "Second argument must be of type int.");
    }
  return (nl->SymbolAtom(CcBool::BasicType()));
}


int
set_maxkeysize::ValueMapping(Word* args, Word& result, int message,
        Word& local, Supplier s)
{
  result = qp->ResultStorage(s);
  unsigned bc = ((CcInt*)args[0].addr)->GetIntval();
  CcBool *res = (CcBool*) result.addr;
  bool sc = BTree2::SetDefaultMaxKeysize(bc);
  res->Set( true, sc );
  return 0;

}

struct setMaxKeysizeInfo : OperatorInfo {

  setMaxKeysizeInfo() : OperatorInfo()
  {
    name =      "set_maxkeysize";
    signature = "int -> bool";
    syntax =    "set_maxkeysize ( _ )";
    meaning =   "Sets the max. keysize of new btree2s.";
    example =   "query set_maxkeysize (16)";
  }
};

Operator set_maxkeysize::def( setMaxKeysizeInfo(),
                               set_maxkeysize::ValueMapping,
                               set_maxkeysize::TypeMapping);

} // end namespace operator

} // end namespace btree2algebra


