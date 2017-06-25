#include <unistd.h>
#include <limits.h>
#include <iostream>
#include <stdlib.h>
#include <messagehub/messagehub.h>

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 64
#endif

int main(int argc, char ** argv) {
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    std::cout << "Got hostname\n";
    MessageHub hub(hostname, "192.168.1.179", 5555);
    std::cout << "initialized hub\n";
    hub.run();
    hub.send("HELLO", "192.168.1.151:5555");
    std::cout << "sent msg\n";
    sleep(3);
    return EXIT_SUCCESS;
}
