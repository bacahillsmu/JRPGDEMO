#pragma once


#define MACRO_COMBINE1(X,Y) X##Y  // helper macro
#define MACRO_COMBINE(X,Y) MACRO_COMBINE1(X,Y)

//To do Info
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "
#define PRAGMA(p)  __pragma( p )
#define NOTE( x )  PRAGMA( message(x) )
#define FILE_LINE  NOTE( __FILE__LINE__ )
// the important bits
#define TODO( x )  NOTE( __FILE__LINE__"\n"           \
       " --------------------------------------------------------------------------------------\n" \
       "|  TODO :   " ##x "\n" \
       " --------------------------------------------------------------------------------------\n" )
#define UNIMPLEMENTED()  TODO( "IMPLEMENT: " QUOTE(__FILE__) " (" QUOTE(__LINE__) ")" );

// ASSERT_RECOVERABLE(0, "")