#include "ksh.h"

#include "display.h"
#include "kio.h"
#include "stdlib.h"

/*
  Primary interface for kernel. Should  shoot off other processes
 */
void sh() {
    char *prompt = "ksh>";
    char *buf = malloc(250);
    while (1) {
        print(prompt);
        get_line(buf, 250);
        println(buf);
    }
}
