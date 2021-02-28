// Import this at the beginning of a C swig module.

// Disable warning #325: "Nested class not currently supported"
#pragma SWIG nowarn=325
%include "typemaps.i"
%include "stdint.i"

%typemap(in,checkfn="lua_isinteger") uint32_t
%{
	$1 = ($type)lua_tointeger(L, $input);
%}

%typemap(in,checkfn="lua_isinteger") unsigned int
%{
	$1 = ($type)lua_tointeger(L, $input);
%}

%typemap(out) uint32_t
%{
	lua_pushinteger(L, (lua_Unsigned) $1);
	SWIG_arg++;
%}

%typemap(out) unsigned int
%{
	lua_pushinteger(L, (lua_Unsigned) $1);
	SWIG_arg++;
%}


%typemap(out) int32_t
%{
	lua_pushinteger(L, (lua_Integer) $1);
	SWIG_arg++;
%}

%typemap(out) int 
%{
	lua_pushinteger(L, (lua_Integer) $1);
	SWIG_arg++;
%}