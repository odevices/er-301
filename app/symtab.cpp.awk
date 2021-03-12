BEGIN{ 
  print "#include \"symtab.h\"" 
  print ""
  print "constexpr unsigned int hash(const char* str, int h = 0)"
  print "{"
  print "  return (!str[h]) ? 5381 : ((hash(str, h+1) * 33) ^ str[h]);"
  print "}"
  print ""
  print "uintptr_t gbl_sym_table_lookup(const char * name) {"
  print "  switch (hash(name)) {"
} { 
  if(NF==1){
    print "    case hash(\"" $1 "\"): return (uintptr_t)&" $1 ";"
  }
} 
END{
  print "    default: return 0;"
  print "  }"
  print "}"
}