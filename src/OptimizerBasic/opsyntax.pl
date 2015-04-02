

/*
1 Operator Syntax

[File ~opsyntax.pl~]

*/

:-
  op(800, xfx, =>),
  op(800, xfx, <=),
  op(800, xfx, #),
  op(800, xfx, div),
  op(800, xfx, mod),
  op(800, xfx, starts),
  op(800, xfx, contains),
  op(200, xfx, :).

:- op(800, xfx, inside).
:- op(800, xfx, intersects).
:- op(800, xfx, touches).
:- op(800, xfx, passes).
:- op(800, xfx, present).
:- op(800, xfx, or).
:- op(800, fx, not).

/*

----	secondoOp(Op, Syntax, NoArgs) :-
----

~Op~ is a Secondo operator written in ~Syntax~, with ~NoArgs~ arguments.
Currently implemented:

  * postfix, 1 or 2 arguments: corresponds to \_ \# and \_ \_ \#

  * postfixbrackets, 2 or 3 arguments, of which the last one is put into
the brackets: \_ \# [ \_ ] or \_ \_ \# [ \_ ]

  * prefix, 2 arguments: \# (\_, \_)

  * prefix, either 1 or 3 arguments, does not need a rule here, is
translated by default.

  * infix, 2 arguments: does not need a rule, translated by default.

For all other forms, a plan\_to\_atom rule has to be programmed explicitly.

*/

secondoOp(distance, prefix, 2).


secondoOp(feed, postfix, 1).
secondoOp(consume, postfix, 1).
secondoOp(count, postfix, 1).
secondoOp(product, postfix, 2).
secondoOp(filter, postfixbrackets, 2).
secondoOp(loopjoin, postfixbrackets, 2).
secondoOp(exactmatch, postfixbrackets, 3).
secondoOp(leftrange, postfixbrackets, 3).
secondoOp(rightrange, postfixbrackets, 3).
secondoOp(extend, postfixbrackets, 2).
secondoOp(remove, postfixbrackets, 2).
secondoOp(project, postfixbrackets, 2).
secondoOp(sortby, postfixbrackets, 2).
secondoOp(loopsel, postfixbrackets, 2).
secondoOp(sum, postfixbrackets, 2).
secondoOp(min, postfixbrackets, 2).
secondoOp(max, postfixbrackets, 2).
secondoOp(avg, postfixbrackets, 2).
secondoOp(tuplesize, postfix, 1).

/* Khoja define a new syntax */
secondoOp(between, postfixbrackets2, 3).


 
















