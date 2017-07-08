#include <unistd.h>
#include <limits.h>
#include <iostream>
#include <stdlib.h>

#include <messagehub/messagehub.h>
#include <messagehub/message.h>

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 64
#endif

#define PORT 56789


int main(int argc, char ** argv) {
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    MessageControl msgctl(hostname, argv[1], PORT);
    if (argc > 2)
        for (int i = 2; i < argc; i++)
            msgctl.connect(argv[i], PORT, std::to_string(i));
    while (true) {}
    return EXIT_SUCCESS;
}
