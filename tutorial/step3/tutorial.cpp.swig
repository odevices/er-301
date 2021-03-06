%module tutorial_libFoo
%include <od/glue/mod.cpp.swig>

%{

#undef SWIGLUA

#include <EuclideanSequencer.h>
#include <CatCircle.h>
	
#define SWIGLUA

%}

%include <EuclideanSequencer.h>
%include <CatCircle.h>