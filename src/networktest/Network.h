#ifndef NETWORK_OWN_H
#define NETWORK_OWN_H
//Yes bad name I know, but it's just in case somebody else decides to use NETWORK_H


class Network{

public:
	static void handleNetwork(char * (*webpage)(void), void (*handleResponse)(char *));

};

#endif
