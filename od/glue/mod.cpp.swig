// Import this at the beginning of mods.

%import <od/glue/prologue.cpp.swig>

%{
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

#undef SWIGLUA

#include <od/extras/ReferenceCounted.h>
#include <od/extras/Lockable.h>

#include <od/audio/Sample.h> 
#include <od/audio/Slices.h> 

#include <od/objects/Object.h>  
#include <od/objects/heads/Head.h> 
#include <od/objects/heads/TapeHead.h>
#include <od/objects/heads/RecordHead.h> 
#include <od/objects/heads/SliceHead.h> 

#include <od/tasks/Task.h> 

#include <od/graphics/Graphic.h> 
#include <od/graphics/sampling/HeadDisplay.h> 

#define SWIGLUA
%}

%import(module="app") <od/extras/ReferenceCounted.h>
%import(module="app") <od/extras/Lockable.h>

%import(module="app") <od/audio/Sample.h> 
%import(module="app") <od/audio/Slices.h> 

%import(module="app") <od/objects/Port.h>
%import(module="app") <od/objects/Inlet.h>
%import(module="app") <od/objects/Outlet.h>
%import(module="app") <od/objects/Followable.h>
%import(module="app") <od/objects/Parameter.h>
%import(module="app") <od/objects/Option.h>
%import(module="app") <od/objects/StateMachine.h>
%import(module="app") <od/objects/Object.h> 

%import(module="app") <od/objects/Object.h>  
%import(module="app") <od/objects/heads/Head.h> 
%import(module="app") <od/objects/heads/TapeHead.h>
%import(module="app") <od/objects/heads/RecordHead.h> 
%import(module="app") <od/objects/heads/SliceHead.h> 

%import(module="app") <od/units/Unit.h>

%import(module="app") <od/tasks/Task.h> 

%import(module="app") <od/graphics/Graphic.h> 
%import(module="app") <od/graphics/sampling/HeadDisplay.h> 