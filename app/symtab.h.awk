BEGIN{ 
  print "#pragma once"
  print "#include <stdint.h>"
} { 
  if(NF==1){
    print "extern uintptr_t " $1 ";"
  }
} 
END{
  
}