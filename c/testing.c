#include "display.h"
#include "stdlib.h"
extern char press_flag;

void kasserteq(uint32_t val1, uint32_t val2){
  if(val1 == val2){
    return;
  }
  char temp = press_flag;
  print("Error: asserteq failed");
  print_nl();
  print("        ");
  print_int(val1);
  print(" does not equal ");
  print_int(val2);
  while(temp == press_flag){} // TODO: Move to IO
}
