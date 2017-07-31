# messagehub
[![Build Status](https://travis-ci.org/taran96/messagehub.svg?branch=master)](https://travis-ci.org/taran96/messagehub)


This project uses specific message formats to pass information between different programs. This also includes programs on different systems. It uses ZeroMQ to create connections between the processes. This project is pretty simple and more of a learning experience overall.
## Components
### MessageHub
This component is responsible for handling the messages and initiating responses to correct hosts / processes. This represents a node in the communications array, which can receive and send messages. It contains two sub components:
- **Receiver:**
  - The receiver is what accepts the messages from other processes.
- **Sender:**
  - The sender is given an address to connect to and a message to send.
### HubMonitor
This component is responsible for keeping track of all nodes in the connections array. It will send requests for statuses of all other nodes. It also will provide an outlet to get the status data so that it can be accessed by some sort of GUI interface for visual results.
## Contributions
All contributions are welcomed via pull requests, as long as they abide to the following:
- There **must** be documentation on the feature(s) added. 
- Comments in the code **must** follow the format required to generate documentation.  *(Currently there is no way to generate documentation)*
- Unit tests and if needed integration tests must also be provided for new features.
- Small bugfixes do not have to include all the above, but the pull request should reference the issue if there is one related.
## Todo
- [ ] Set up doxygen or something of the likes
- [x] Create main documentations
- [ ] Have user provide logger (Potentially support multiple loggers)
