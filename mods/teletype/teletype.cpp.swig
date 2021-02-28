%module teletype_libteletype
%include <od/glue/mod.cpp.swig>

%{

#undef SWIGLUA

// tasks

#include <teletype/Dispatcher.h>

// objects

#include <teletype/CV.h>
#include <teletype/TR.h>
	
#define SWIGLUA

%}

// tasks

%include <teletype/Dispatcher.h>

// objects

%include <teletype/CV.h>
%include <teletype/TR.h>