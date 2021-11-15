#include "WiFI.h"

class Network{

public:
	static void handleNetwork(char * (*webpage)(void), void (*handleResponse)(char *));

};
