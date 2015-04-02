/*

$Header: /home/cvsroot/secondo/Optimizer/MemoryAllocation/MemoryOptimization.h,v 1.2 2012/11/19 09:27:51 nvkempen Exp $
@author Nikolai van Kempen

Refer to the file MemoryAllocatin.cpp for more information.

*/

#include "SWI-Prolog.h"

using namespace std;

foreign_t pl_memoryOptimization(term_t t_minOpMemory, term_t t_maxMemory,
    term_t t_constraints, term_t t_dimension, term_t sufficientMemory,
    term_t memoryAssignments);

// eof
