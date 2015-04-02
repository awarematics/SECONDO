/*
---- 
This file is part of SECONDO.

Copyright (C) 2004, University in Hagen, Department of Computer Science, 
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

1 SocketRuleSet

February 2002 Ulrich Telle

1.1 Overview

This class implements a set of access rules based on IP adresses and IP masks.

*/

using namespace std;

#include "SecondoConfig.h"
#include "SocketIO.h"
#include <iostream>
#include <fstream>

char SocketRule::delimiter = '/';

SocketRule::SocketRule()
  : allowDeny( ALLOW )
{
  ipAddr.s_addr = ipMask.s_addr = 0;
}

SocketRule::SocketRule( const string& strIpAddr,
                        const string& strIpMask,
                        const Policy setAllowDeny /* = ALLOW */ )
   : allowDeny( setAllowDeny) 
{
  ipMask.s_addr = inet_addr( strIpMask.c_str() );
  ipAddr.s_addr = inet_addr( strIpAddr.c_str() ) & ipMask.s_addr;
}

bool
SocketRule::Match( const SocketAddress& host )
{
  return ((inet_addr( host.GetIPAddress().c_str() ) & ipMask.s_addr) == ipAddr.s_addr);
}

bool
SocketRule::Allowed( const SocketAddress& host )
{
  return (Match( host ) && allowDeny == ALLOW);
}

bool
SocketRule::Denied( const SocketAddress& host )
{
  return (Match( host ) && allowDeny == DENY);
}
   
void
SocketRule::SetDelimiter( const char newDelimiter /* = '/' */ )
{
  delimiter = newDelimiter;
}

char
SocketRule::GetDelimiter()
{
  return (delimiter);
}

ostream&
operator <<( ostream& os, SocketRule& r )
{
  os << inet_ntoa( r.ipAddr ) << SocketRule::delimiter;
  os << inet_ntoa( r.ipMask ) << SocketRule::delimiter << r.allowDeny;
  return (os);
}

SocketRuleSet::SocketRuleSet( SocketRule::Policy initDefaultPolicy )
  : defaultPolicy( initDefaultPolicy )
{
}

void
SocketRuleSet::AddRule( const string& strIpAddr,
                        const string& strIpMask,
                        SocketRule::Policy allowDeny /* = SocketRule::ALLOW */ )
{
  SocketRule rule( strIpAddr, strIpMask, allowDeny );
  rules.insert( rules.end(), rule);
}

bool
SocketRuleSet::Ok( const SocketAddress& host )
{
  unsigned int i;
 
  if ( defaultPolicy == SocketRule::DENY )
  {
    for ( i = 0; i < rules.size(); i++ )
    {
      if( rules[i].Allowed( host ) )
      {
        return (true);
      }
    }
    return (false);
  }

  if ( defaultPolicy == SocketRule::ALLOW )
  {
    for ( i = 0; i < rules.size(); i++ )
    {
      if ( rules[i].Denied( host ) )
      {
        return (false);
      }
    }
    return (true);
  }
  return (false);
}

bool
SocketRuleSet::LoadFromFile( const string& ruleFileName )
{
  ifstream ruleFile( ruleFileName.c_str() );
  if( !ruleFile )
  {
    return (false);
  }

  while (1)
  {
    bool inAllowDeny;
    string inIpAddr, inIpMask;
    ruleFile >> inIpAddr >> inIpMask >> inAllowDeny;
    if ( !ruleFile )
    {
      break;
    }
    AddRule( inIpAddr, inIpMask,
             inAllowDeny ? SocketRule::ALLOW : SocketRule::DENY );
  }
  ruleFile.close();
  return (true);
}

bool
SocketRuleSet::StoreToFile( const string& ruleFileName )
{
  ofstream ruleFile( ruleFileName.c_str() );
  if( !ruleFile )
  {
    return (false);
  }

  char delim = SocketRule::GetDelimiter();
  SocketRule::SetDelimiter( ' ' );
  ruleFile << *this;
  SocketRule::SetDelimiter( delim );
  ruleFile.close();

  return (true);
}

ostream&
operator <<( ostream& os, SocketRuleSet& r )
{
  unsigned int i;
  for ( i = 0; i < r.rules.size(); i++ )
  {
    os << r.rules[i] << endl;
  }
  return (os);
}

// --- End of source ---

