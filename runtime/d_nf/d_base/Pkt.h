// This module is where we poll input/output port and schedule
// execution_context actors.

#ifndef PKT_H
#define PKT_H
#define PKT_SIZE 500


struct Pkt{
	bool empty;
	char pkt[PKT_SIZE];
};


#endif
