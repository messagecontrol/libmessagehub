#ifndef MESSAGEHUB_H
#define MESSAGEHUB_H

// TODO: We probably don't need to import all of these ... they were there
//       since planning and learning
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <thread>
#include <memory>
#include <zmq.hpp>

#include "messagehub/message.h"

// The listening port
// TODO: Get from a config
#define DEFAULT_PORT 5555



class MessageHub {

protected:
    
    // this is the identity of the hub ... usually the hostname
    std::string identity;

    // These are the main threads
    // TODO: Add a manager
    std::unique_ptr<std::thread> manager, msgProcessor, receiver, sender;
    
    // This is the inQueue of messages
    // TODO: Add feature for priorities
    std::queue<Message> inQueue;

    // The outQueue of destination ip w/ port and zmq::message_t pairs
    // The destination ip is not including the protocol to be used.
    std::queue<std::pair<std::string, zmq::message_t> > outQueue;
    
    // DEPRECIATED: The threads create new contexts and sockets
    zmq::context_t context;
    zmq::socket_t inSock, outSock;

    // Hold information of the in port ... defaults to DEFAULT_PORT
    int port;
    
    // This the host ip address. It is needed to prevent the process of
    // determining which ip we are using.
    // TODO: Provide support for systems with dynamic ip by getting the 
    //       network interface from a config file
    std::string hostAddr;

    // These are control booleans for the while loops
    bool still_process, still_receive, still_send;

    // Message Processor
    // TODO: Remove this and try to provide a way for custom processors
    void _run();

    // The function run by the sender
    void _run_sender();

    // The function run by the receiver
    // FIXME: The loop contains blocking code ('inSock.recv()') so the control
    //        boolean is ineffective until a message is recevied
    //        Use a poller for this
    void _run_recevier();

    // TODO: Need to removed or allow custom processor
    void process(std::string s);
    void process(Message &msg);
    // The map will be in the format of:
    //      "IP ADDRESS:POST": ("HOSTNAME", Bool for active or not)
    std::map<std::string, std::pair<std::string, bool> > connections;
    void addConnection(const std::string & ipaddr, const int port, const std::string & name);
    void addConnection(const std::string & ipaddrWithPort, const std::string & name);

    bool waitingOnShake;
    void _timer(int time, bool *flag);
    bool handshake(std::string ip, int port);

public:

    MessageHub(std::string id, std::string hostip, int listenPort = DEFAULT_PORT);
    ~MessageHub();

    // Runs all the threads
    void run();

    // Gives the full address including the identity in the format of:
    // [IDENTITY]::192.145.234.43:4555
    // The above can change in future message formats
    std::string fullAddr();

    // Allow messages to be sent without a message received with a response
    // required
    void send(const std::string msg, const std::string dst);

    bool connect(const std::string &ipaddr, const int &port, const std::string &name);
};

#endif
