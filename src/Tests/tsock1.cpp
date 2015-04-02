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

*/
using namespace std;
#include "SocketIO.h"
#include <time.h>
#include <string>
#include <cstdlib>

#define HDR_SIZE  16
#define BODY_SIZE 240
#define N_LOOPS   10000

int main( int argc, char* argv[] )
{
  string errbuf;
  char buf[HDR_SIZE+BODY_SIZE];	
  Socket* sock;
  bool server = false;
  int i;

  if ( argc < 4 )
  {
    cout << "Socket test program 1 (Performance test)" << endl
         << "Usage: tsock1 [(local-server|global-server)|client] host port"
         << endl;
    return (EXIT_FAILURE);
  }

  if ( *argv[1] == 'l' || *argv[1] == 'L' )  // local server
  {
    Socket* gate = Socket::CreateLocal( argv[2] );
    if ( !gate->IsOk() )
    {
      errbuf = gate->GetErrorText();
      cout << "Failed to create local socket: " << errbuf << endl;
    }
    sock = gate->Accept();
    if ( sock == 0 )
    {
      errbuf = gate->GetErrorText();
      cout << "Failed to accept socket: " << errbuf << endl;
    }
    delete gate;
    server = true;
  }
  else if ( *argv[1] == 'g' || *argv[1] == 'G' )  // global server
  {
    Socket* gate = Socket::CreateGlobal( argv[2], argv[3] );
    if (!gate->IsOk())
    {
      errbuf = gate->GetErrorText();
      delete gate;
      cout << "Failed to create global socket: " << errbuf << endl;
    }
    sock = gate->Accept();
    if ( sock == 0 )
    {
      errbuf = gate->GetErrorText();
      delete gate;
      cout << "Failed to accept socket: " << errbuf << endl;
    }
    delete gate;
    server = true;
  }
  else   // client
  {
    sock = Socket::Connect( argv[2], argv[3] );
    if ( sock == 0 )
    {
      cout << "Failed to connect to server" << endl;
    }
    else if ( !sock->IsOk() )
    {
      errbuf = sock->GetErrorText();
      cout << "Connection to server failed: " << errbuf << endl;
    }
    server = false;
  }
  time_t start_time = time( NULL );

  if ( server )
  {
    i = 0;
    while ( sock->Read( buf, HDR_SIZE ) && 
            sock->Read( buf, sizeof(buf) - HDR_SIZE ) ) 
    {
      i += 1;
      if ( !sock->Write( buf, sizeof(buf) ) )
      {
        errbuf = sock->GetErrorText();
        cout << "Write to socket failed: " << errbuf << endl;
      }
    } 	
    cout << "Handle " << i << " requests" << endl;
  }
  else
  {
    for ( i = 0; i < N_LOOPS; i++ )
    {
      if ( !sock->Write( buf, sizeof(buf) ) ||
           !sock->Read( buf, HDR_SIZE ) ||
           !sock->Read( buf, sizeof(buf) - HDR_SIZE ) )
      {
        errbuf = sock->GetErrorText();
        cout << "Write to socket failed: " << errbuf << endl;
      }
      if ( i % 1000 == 0)
      {
        cout << i << endl;
      }
    }
  } 
  cout << "Elapsed time " << time(NULL) - start_time << " seconds" << endl;
  return (EXIT_SUCCESS);
}

