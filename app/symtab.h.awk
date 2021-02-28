BEGIN{ 
  print "#pragma once"
} { 
  if(NF==1){
    print "extern uintptr_t " $1 ";"
  }
} 
END{
  
}