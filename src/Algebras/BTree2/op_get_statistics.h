/*
//paragraph [1] Title: [{\Large \bf \begin{center}] [\end{center}}]
//paragraph [10] Footnote: [{\footnote{] [}}]
//[TOC] [\tableofcontents]

[1] Header of the createbtree2 Operator 

[TOC]

0 Overview

*/

#ifndef _BTREE2_OP_GET_STATISTICS_H
#define _BTREE2_OP_GET_STATISTICS_H

#include "Operator.h"
#include "NestedList.h"
#include "AlgebraTypes.h"

#include "BTree2.h"

namespace BTree2Algebra {
namespace Operators {

class get_statistics {
  public:
  static string Specification();
  static ListExpr TypeMapping(ListExpr args);
  static int Select( ListExpr args );

  static int ValueMappingEx(Word* args, Word& result, int message,
                        Word& local, Supplier s);

  static Operator def;
  static ValueMapping valueMappings[];
  static int numberOfValueMappings;
};

} // end namespace Operators
} // end namespace BTree2Algebra

#endif
