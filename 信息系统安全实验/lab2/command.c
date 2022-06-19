#include <stdio.h>
#include <unistd.h>


int main(int argc, char **argv) {
    char cat[] = "cat ";
    char *command;
    size_t commandLength;
    commandLength = strlen(cat) + strlen(argv[1]) + 1;
    command = (char *)malloc(commandLength);
    strncpy(command, cat, commandLength);
    strncat(command, argv[1], commandLength - strlen(cat));

    setuid(0); 
    setgid(0); 
    system(command);
    return 0;
}
