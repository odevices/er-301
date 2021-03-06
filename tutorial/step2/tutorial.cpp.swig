%module tutorial_libFoo
%include <od/glue/mod.cpp.swig>

%{

#undef SWIGLUA

#include <EuclideanSequencer.h>
	
#define SWIGLUA

%}

%include <EuclideanSequencer.h>