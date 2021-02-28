BEGIN{ 
  print "#include <hal/dynload/reflect.h>"; 
  print "#include \"symtab.h\""; 
  print "struct sym_table_t gbl_sym_table[]={" 
} { 
  if(NF==1){
    print "{\"" $1 "\", (uintptr_t)&" $1 "},"
  }
} 
END{
  print "{0,0} };"
}