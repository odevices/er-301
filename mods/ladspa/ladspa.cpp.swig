%module ladspa_libladspa
%include <od/glue/mod.cpp.swig>

%{

#undef SWIGLUA

// objects

#include <ladspa/Adapter.h>
#include <ladspa/GVerb.h>
	
#define SWIGLUA

%}

// objects

%include <ladspa/Adapter.h>
%include <ladspa/GVerb.h>
