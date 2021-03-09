// The module name is configured in the Makefile.

// Include the necessary boiler-plate for C++ mods.
%include <od/glue/mod.cpp.swig>

// Code between %{ and %} is inserted verbatim into the wrapper.
%{

// Add header files necessary for compilation of the interfaces added further down.
#undef SWIGLUA

#include <EuclideanSequencer.h>
	
#define SWIGLUA

%}

// Here we define what classes should have lua interfaces generated.
// Usually just %include'ing the header file is enough.
// Code bracked by #ifndef SWIGLUA will be ignored here.

%include <EuclideanSequencer.h>