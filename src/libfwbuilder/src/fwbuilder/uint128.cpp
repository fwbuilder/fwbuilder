/* 

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

*/


#include "uint128.h"

using namespace std;

ostream &operator<<(ostream &o, const uint128 &n) 
{
    o << n.to_string();
	return o;
}
