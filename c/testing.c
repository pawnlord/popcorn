#include "display.h"
#include "stdlib.h"
extern char press_flag;

void kasserteq(uint32_t val1, uint32_t val2, const char *msg){
  if(val1 == val2){
    return;
  }
  char temp = press_flag;
  println("Error: asserteq failed");
  print("Message: ");
  println(msg);
  print("         ");
  print_int(val1);
  print(" does not equal ");
  print_int(val2);
  while(temp == press_flag){} // TODO: Move to IO, make it work
}



void kassertneq(uint32_t val1, uint32_t val2, const char *msg){
  if(val1 != val2){
    return;
  }
  char temp = press_flag;
  println("Error: assertneq failed");
  print("Message: ");
  println(msg);
  print("         ");
  print_int(val1);
  print(" equals ");
  print_int(val2);
  while(temp == press_flag){} // TODO: Move to IO, make it work
}

