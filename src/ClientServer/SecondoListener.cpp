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

#include <string>
#include <algorithm>

#include "Application.h"
#include "Processes.h"
#include "SocketIO.h"
#include "Profiles.h"
#include "CharTransform.h"
#include "WinUnix.h"

const int EXIT_LISTENER_OK       = 0;
const int EXIT_LISTENER_NOPF     = 1;
const int EXIT_LISTENER_NOHOST   = 2;
const int EXIT_LISTENER_NOSERVER = 3;
const int EXIT_LISTENER_NOSOCKET = 4;
const int EXIT_LISTENER_FAIL     = 5;

using namespace std;

class SecondoListener : public Application
{
 public:
  SecondoListener( const int argc, const char** argv ) : 
    Application( argc, argv ) 
  {};
  virtual ~SecondoListener() {};
  int  Execute();
  bool ClientAllowed();
  bool AbortOnSignal( int sig );
  void LogMessage( const string msg );
 private:
  string parmFile;
  Socket* gate;
  Socket* client;
};

bool
SecondoListener::AbortOnSignal( int sig )
{
  if ( gate != 0 )
  {
    gate->CancelAccept();
  }
  return (true);
}

void
SecondoListener::LogMessage( const string msg )
{
}

int
SecondoListener::Execute()
{
  int rc = EXIT_LISTENER_OK;
  SetAbortMode( true );

  // --- Get configuration file
  parmFile = (GetArgCount() > 1) ? GetArgValues()[1] : "SecondoConfig.ini";

  // --- Load ruleSet
  string rulePolicy = SmiProfile::GetParameter( "Environment", 
                                                "RulePolicy", 
                                                "ALLOW", parmFile );
  
  transform( rulePolicy.begin(), rulePolicy.end(), rulePolicy.begin(), 
             ToUpperProperFunction );
  
  SocketRule::Policy policy = (rulePolicy == "ALLOW") ? SocketRule::ALLOW 
                                                      : SocketRule::DENY;
  SocketRuleSet ipRules( policy );
  string ruleSetFile = SmiProfile::GetParameter( "Environment", 
                                                 "RuleSetFile", "", parmFile );
  if ( ruleSetFile != "" )
  {
    ipRules.LoadFromFile( ruleSetFile );
  }

  // --- Get host and port of Secondo server
  string host = SmiProfile::GetParameter( "Environment", 
                                          "SecondoHost", "", parmFile );
  string port = SmiProfile::GetParameter( "Environment", 
                                          "SecondoPort", "", parmFile );

  if ( host.length() == 0 || port.length() == 0 )
  {
    return (EXIT_LISTENER_NOHOST);
  }

  // --- Get name of client server program
  string section = (GetArgCount() > 2) ? GetArgValues()[2] : "BerkeleyDB";
  string server  = SmiProfile::GetParameter( section, "ServerProgram", 
                                             "", parmFile );
  if ( server.length() == 0 )
  {
    return (EXIT_LISTENER_NOSERVER);
  }

  // --- Start up process factory
  if ( !ProcessFactory::StartUp() )
  {
    return (EXIT_LISTENER_NOPF);
  }

  // --- Create listener socket

  gate = Socket::CreateGlobal( host, port );
  if ( gate && gate->IsOk() )
  {
    while (!ShouldAbort())
    {
      client = gate->Accept();
      if ( client && client->IsOk() )
      {
        if ( ipRules.Ok( SocketAddress( client->GetPeerAddress() ) ) )
        {
          // --- Spawn server for client
          int pidServer;
          string pgmArgs = string( "\"-srv\" \"" ) + parmFile + "\""; 
          if ( !ProcessFactory::SpawnProcess( server, pgmArgs,
                                              pidServer, true, client ) )
          {
            // --- Start of server failed
            iostream& ss = client->GetSocketStream();
            ss << "<SecondoError>" << endl
               << "SECONDO-0002: Server not available. Try again later." << endl
               << "</SecondoError>" << endl;
            client->Close();
            delete client;
            LogMessage( "Start of client server failed" );
          }
          WinUnix::sleep( 0 );
        }
        else
        {
          // --- Reject client
          iostream& ss = client->GetSocketStream();
          ss << "<SecondoError>" << endl
             << "SECONDO-0001: Connection rejected." << endl
             << "</SecondoError>" << endl;
          client->Close();
          delete client;
          string errmsg = string( "Client '" ) + client->GetPeerAddress() 
                          + "' not allowed.";
          LogMessage( errmsg );
        }
      }
    }
    ProcessFactory::WaitForAll();
  }
  else
  {
    string errbuf = gate->GetErrorText();
    delete gate;
    LogMessage( "Failed to create global socket: " + errbuf );
    rc = EXIT_LISTENER_NOSOCKET;
  }
  gate->Close();
  delete gate;
  ProcessFactory::ShutDown();
  return (rc);
}

int main( const int argc, const char* argv[] )
{
  SecondoListener* appPointer = new SecondoListener( argc, argv );
  int rc = appPointer->Execute();
  delete appPointer;
  return (rc);
}
