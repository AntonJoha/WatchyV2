#ifndef NETWORK_H
#define NETWORK_H

class Network{

public:
	void handleNetwork(char * (*webpage)(void), void (*handleResponse)(char *));

};

#endif
