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

//paragraph [1] Title: [{\Large \bf \begin {center}] [\end {center}}]
//[TOC] [\tableofcontents]

Started March 2012, Fabio Vald\'{e}s

[TOC]

\section{Overview}
This is the lex file that does the lexical analysis for the Symbolic Trajectory
Algebra.

*/
%{
#include "PatternParser.tab.h"
#include <iostream>
#include <string>
using namespace std;

void deleteCurrentPatternBuffer() {
    yy_delete_buffer(YY_CURRENT_BUFFER);
}

%}
%option prefix="pattern"
%option nounput

%x condition
%x result
%x assignment

si [+-]?[0-9]+
ui [0-9]+
real {si}+(\.{ui}+)?
star \*
plus \+
setrelation disjoint|superset|equal|intersect
wildcard {star}|{plus}
date {si}(-{ui}(-{ui}(-{ui}(\:{ui})(\:{ui}(\.{ui})?)?)?)?)?
time {ui}\:{ui}(\:{ui}(\.{ui})?)?
interval ({date}(\~{date})?)|\~{date}|{date}\~|([a-z_][a-zA-Z0-9_]*)|\~{time}|{time}\~|{time}\~{time}
intervalset {interval}|(\{{interval}(\,[ ]*{interval})*\})
label ([a-zA-Z_\-][a-zA-Z0-9_\-]*)|(\"|\')[a-zA-Z0-9\. \�\�\�\�\�\�\�\/\(\)\[\]\{\}_\-\*\:]*(\"|\')
place \({label}[\ ]*{ui}\)
labelset {label}|(({setrelation})?\{{label}(\,[ ]*{label})*\})
placeset {place}|(({setrelation})?\{{place}(\,[ ]*{place})*\})
contents {intervalset}[\ ]*({labelset}|{placeset})
variable [A-Z][a-zA-Z0-9_\-]*
doubleslash \/{2}
conditiontype label|place|time|start|end|leftclosed|rightclosed|card|labels|places
assigntype label|place|time|start|end|leftclosed|rightclosed|labels|places
vardottype {variable}\.{conditiontype}
vardottype2 {variable}\.{assigntype}
rightarrow =>
const_op ([^A-Z\(\)\[\]\,\t\n ][^\(\)\[\]\,\t\n ]*)|TRUE|FALSE|\"[a-zA-Z0-9\. \�\�\�\�\�\�\�\/\(\)\[\]\{\}_\-]*\"
assign \:=
openregex \[
closeregex (\]\+)|(\]\*)|(\]\?)|\]

%option yylineno
%option noyywrap
%%
<INITIAL>[ \a\b\f\t\v\r]+ {}
<INITIAL>\( {return '(';}
<INITIAL>\) {return ')';}
<INITIAL>{openregex} {patternlval.text = strdup(patterntext); return ZZOPENREGEX;}
<INITIAL>{closeregex} {patternlval.text = strdup(patterntext); return ZZCLOSEREGEX;}
<INITIAL>{variable} {patternlval.text = strdup(patterntext); return ZZVARIABLE;}
<INITIAL>{wildcard} {patternlval.text = strdup(patterntext); return ZZWILDCARD;}
<INITIAL>{contents} {patternlval.text = strdup(patterntext); return ZZCONTENTS;}
<INITIAL>\| {return '|';}
<INITIAL>{rightarrow} {BEGIN(result); return ZZRIGHTARROW;}
<INITIAL>\n {return ZZEND;}
<INITIAL>{doubleslash} {BEGIN(condition); return ZZDOUBLESLASH;}
<condition>{vardottype} {patternlval.text = strdup(patterntext); return ZZVAR_DOT_TYPE;}
<condition>{rightarrow} {BEGIN(result); return ZZRIGHTARROW;}
<condition>{const_op} {patternlval.text = strdup(patterntext); return ZZCONST_OP;}
<condition>\, {return ',';}
<condition>\( {return '(';}
<condition>\) {return ')';}
<condition>\[ {return '[';}
<condition>\] {return ']';}
<condition>\n {BEGIN(INITIAL); return ZZEND;}
<result>{variable} {patternlval.text = strdup(patterntext); return ZZVARIABLE;}
<result>\( {return '(';}
<result>\) {return ')';}
<result>{doubleslash} {BEGIN(assignment); return ZZDOUBLESLASH;}
<result>\n {BEGIN(INITIAL); return ZZEND;}
<assignment>[ \a\b\f\t\v\r]+ {}
<assignment>{vardottype2} {patternlval.text = strdup(patterntext); return ZZVAR_DOT_TYPE;}
<assignment>{assign} {return ZZASSIGN;}
<assignment>{const_op} {patternlval.text = strdup(patterntext); return ZZCONST_OP;}
<assignment>\, {return ',';}
<assignment>\( {return '(';}
<assignment>\) {return ')';}
<assignment>\[ {return '[';}
<assignment>\] {return ']';}
<assignment>\n {BEGIN(INITIAL); return ZZEND;}
<INITIAL>. {patternlval.text = strdup(patterntext); BEGIN(INITIAL); return ZZERROR;}
%%
namespace stj {
void patternFlushBuffer() {
  BEGIN(INITIAL);
}
}