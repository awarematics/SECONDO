/*
This file is part of SECONDO.

Copyright (C) 2013, University in Hagen, Department of Computer Science,
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

*/

/*
TileAlgebra includes

*/

#include "atrange.h"
#include "../t/tint.h"
#include "../t/treal.h"
#include "../t/tbool.h"
#include "../t/tstring.h"
#include "../mt/mtint.h"
#include "../mt/mtreal.h"
#include "../mt/mtbool.h"
#include "../mt/mtstring.h"

/*
declaration of namespace TileAlgebra

*/

namespace TileAlgebra
{

/*
Template method atrangeFunctiont calls atrange method of specific t datatype
and returns the result of this call.

author: Dirk Zacher
parameters: pArguments - a pointer to the arguments of atrange operator
            rResult - reference to a Word containing the result
            message - message to distinguish call modes of atrangeFunctiont
            rLocal - reference to a Word to store local method information
            supplier - an Address to a supplier of information of operator tree
return value: 0 if atrangeFunctiont successfully executed, otherwise FAILURE
exceptions: -

*/

template <typename Type>
int atrangeFunctiont(Word* pArguments,
                     Word& rResult,
                     int message,
                     Word& rLocal,
                     Supplier supplier)
{
  int nRetVal = FAILURE;

  if(qp != 0 &&
     pArguments != 0)
  {
    Type* pType = static_cast<Type*>(pArguments[0].addr);
    Rectangle<2>* pRectangle = static_cast<Rectangle<2>*>(pArguments[1].addr);

    if(pType != 0 &&
       pRectangle != 0)
    {
      rResult = qp->ResultStorage(supplier);

      if(rResult.addr != 0)
      {
        Type* pResult = static_cast<Type*>(rResult.addr);

        if(pResult != 0)
        {
          if(pType->IsDefined() &&
             pRectangle->IsDefined())
          {
            pType->atrange(*pRectangle, *pResult);
          }

          else
          {
            pResult->SetDefined(false);
          }

          nRetVal = 0;
        }
      }
    }
  }

  return nRetVal;
}

/*
Template method atrangeFunctionmt calls atrange method of specific mt datatype
and returns the result of this call.

author: Dirk Zacher
parameters: pArguments - a pointer to the arguments of atrange operator
            rResult - reference to a Word containing the result
            message - message to distinguish call modes of atrangeFunctionmt
            rLocal - reference to a Word to store local method information
            supplier - an Address to a supplier of information of operator tree
return value: 0 if atrangeFunctionmt successfully executed, otherwise FAILURE
exceptions: -

*/

template <typename Type>
int atrangeFunctionmt(Word* pArguments,
                      Word& rResult,
                      int message,
                      Word& rLocal,
                      Supplier supplier)
{
  int nRetVal = FAILURE;

  if(qp != 0 &&
     pArguments != 0)
  {
    Type* pType = static_cast<Type*>(pArguments[0].addr);
    Rectangle<2>* pRectangle = static_cast<Rectangle<2>*>(pArguments[1].addr);

    if(pType != 0 &&
       pRectangle != 0)
    {
      if(qp->GetNoSons(supplier) == 2)
      {
        rResult = qp->ResultStorage(supplier);

        if(rResult.addr != 0)
        {
          Type* pResult = static_cast<Type*>(rResult.addr);

          if(pResult != 0)
          {
            if(pType->IsDefined() &&
               pRectangle->IsDefined())
            {
              pType->atrange(*pRectangle, *pResult);
            }

            else
            {
              pResult->SetDefined(false);
            }

            nRetVal = 0;
          }
        }
      }

      else
      {
        Instant* pInstant1 = static_cast<Instant*>(pArguments[2].addr);
        Instant* pInstant2 = static_cast<Instant*>(pArguments[3].addr);

        if(pInstant1 != 0 &&
           pInstant2 != 0)
        {
          rResult = qp->ResultStorage(supplier);

          if(rResult.addr != 0)
          {
            Type* pResult = static_cast<Type*>(rResult.addr);

            if(pResult != 0)
            {
              if(pType->IsDefined() &&
                 pRectangle->IsDefined() &&
                 pInstant1->IsDefined() &&
                 pInstant2->IsDefined())
              {
                pType->atrange(*pRectangle,
                               pInstant1->ToDouble(),
                               pInstant2->ToDouble(),
                               *pResult);
              }

              else
              {
                pResult->SetDefined(false);
              }

              nRetVal = 0;
            }
          }
        }
      }
    }
  }

  return nRetVal;
}

/*
definition of atrangeFunctions array.

*/

ValueMapping atrangeFunctions[] =
{
  atrangeFunctiont<tint>,
  atrangeFunctiont<treal>,
  atrangeFunctiont<tbool>,
  atrangeFunctiont<tstring>,
  atrangeFunctionmt<mtint>,
  atrangeFunctionmt<mtreal>,
  atrangeFunctionmt<mtbool>,
  atrangeFunctionmt<mtstring>,
  0
};

/*
Method atrangeSelectFunction returns the index of specific atrange function
in atrangeFunctions array depending on the arguments.

author: Dirk Zacher
parameters: arguments - arguments of atrange operator
return value: index of specific atrange function in atrangeFunctions
exceptions: -

*/

int atrangeSelectFunction(ListExpr arguments)
{
  int functionIndex = -1;

  if(arguments != 0)
  {
    NList argumentsList(arguments);
    NList argument1 = argumentsList.first();
    NList argument2 = argumentsList.second();

    if(argument2.isSymbol(Rectangle<2>::BasicType()))
    {
      const int TYPE_NAMES = 8;
      const std::string TYPE_NAMES_ARRAY[TYPE_NAMES] =
      {
        tint::BasicType(),
        treal::BasicType(),
        tbool::BasicType(),
        tstring::BasicType(),
        mtint::BasicType(),
        mtreal::BasicType(),
        mtbool::BasicType(),
        mtstring::BasicType()
      };

      for(int i = 0; i < TYPE_NAMES; i++)
      {
        if(argument1.isSymbol(TYPE_NAMES_ARRAY[i]))
        {
          functionIndex = i;
          break;
        }
      }
    }
  }

  return functionIndex;
}

/*
Method atrangeTypeMappingFunction returns the return value type
of atrange operator in the form of a ListExpr.

author: Dirk Zacher
parameters: arguments - arguments of atrange operator
return value: return value type of atrange operator
exceptions: -

*/

ListExpr atrangeTypeMappingFunction(ListExpr arguments)
{
  ListExpr type = NList::typeError("Operator atrange expects "
                                   "a t type or a mt type "
                                   "and a rectangle or "
                                   "a rectangle, an instant and an instant.");

  NList argumentsList(arguments);

  if(argumentsList.hasLength(2))
  {
    std::string argument1 = argumentsList.first().str();
    std::string argument2 = argumentsList.second().str();

    if((IstType(argument1) ||
        IsmtType(argument1)) &&
        argument2 == Rectangle<2>::BasicType())
    {
      type = NList(argument1).listExpr();
    }
  }

  else if(argumentsList.hasLength(4))
  {
    std::string argument1 = argumentsList.first().str();
    std::string argument2 = argumentsList.second().str();
    std::string argument3 = argumentsList.third().str();
    std::string argument4 = argumentsList.fourth().str();

    if(IsmtType(argument1) &&
       argument2 == Rectangle<2>::BasicType() &&
       argument3 == Instant::BasicType() &&
       argument4 == Instant::BasicType())
    {
      type = NList(argument1).listExpr();
    }
  }

  return type;
}

}
