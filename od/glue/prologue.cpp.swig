// Import this at the beginning of a CPP swig module.

%import <od/glue/prologue.c.swig>
%include "std_string.i"
%include "std_vector.i"

%feature("ref") ReferenceCounted "$this->attachLua();"
%feature("unref") ReferenceCounted "$this->releaseLua();"
%immutable;