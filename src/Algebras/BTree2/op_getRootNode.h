/*
//paragraph [1] Title: [{\Large \bf \begin{center}] [\end{center}}]
//paragraph [10] Footnote: [{\footnote{] [}}]
//[TOC] [\tableofcontents]

[1] Header of the BTree2 getRootNode Operator 

[TOC]

0 Overview

*/ 

#ifndef _BTREE2_OP_GETROOTNODE_H
#define _BTREE2_OP_GETROOTNODE_H

#include "Operator.h"
#include "NestedList.h"
#include "AlgebraTypes.h"

#include "BTree2.h"

namespace BTree2Algebra {
namespace Operators {

class getRootNode {
  public:
    
  static int ValueMapping(Word* args, Word& result, int message,
                        Word& local, Supplier s);

  static ListExpr TypeMapping(ListExpr args);
  
  static Operator def;
};

} // end namespace Operators
} // end namespace BTree2Algebra

#endif
