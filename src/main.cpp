#include <unistd.h>
#include <limits.h>
#include <iostream>
#include <stdlib.h>
#include <messagehub/messagehub.h>

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 64
#endif

int main(int argc, char ** argv) {
    if (argc != 3){
        std::cout << "[Error] Usage: ./main [HOST_IP] [REMOTE_IP]\n";
        return EXIT_FAILURE;
    }
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    std::cout << "Got hostname\n";
    MessageHub hub(hostname, argv[1], 5555);
    std::cout << "initialized hub\n";
    hub.run();
    sleep(3);
    hub.connect(argv[2], "Raspi");
    while (true) {}
    return EXIT_SUCCESS;
}
