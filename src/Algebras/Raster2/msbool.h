/*
This file is part of SECONDO.

Copyright (C) 2011, University in Hagen, Department of Computer Science,
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

#ifndef RASTER2_MSBOOL_H_
#define RASTER2_MSBOOL_H_

#include "mstype.h"
#include "sbool.h"

namespace raster2 {

    struct msbool_helper;
    typedef mstype<char, msbool_helper> msbool;

    struct msbool_helper
    {
        typedef msbool implementation_type;
        typedef MBool moving_type;
        typedef CcBool wrapper_type;
        typedef UBool unit_type;
        typedef sbool spatial_type;
        static const char* name;
        static bool check(const NList& nl) { return nl.isBool(); }
        static char parse(const NList& nl) { return char(nl.boolval()); }
        static NList print(const char& c)
            { return isUndefined(c)
                   ? NList(Symbol::UNDEFINED())
                   : NList(bool(c), true); }
        static bool isUndefined(const char& t) { return t == UNDEFINED_BOOL; }
        static char getUndefined() { return UNDEFINED_BOOL; }
        static std::string BasicType() { return CcBool::BasicType(); }
        static wrapper_type wrap(const char& t) {
            return CcBool(!isUndefined(t), int(t));
        }
        static char unwrap(const CcBool& i) {
            if (i.IsDefined()) {
              return i.GetValue();
            } else {
              return getUndefined();
            }
        }
    };

}

namespace std
{
    template<> inline void swap<raster2::msbool>
        (raster2::msbool& a, raster2::msbool& b)
    {
        raster2::swap(a, b);
    }
}

#endif /* SBOOL_H_ */
