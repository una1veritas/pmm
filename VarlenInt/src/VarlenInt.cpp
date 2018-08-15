//============================================================================
// Name        : VarlenInt.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

uint32_t read_varuint(char * &p) {
	uint8_t len = (*p)>>4;
	uint32_t val = (*p) & 0x0f;
	++p;
	while ( len > 0 ) {
		val <<= 8;
		val |= (*p);
		++p;
		--len;
	}
	return val;
}

uint32_t write_varuint(char * &p, uint32_t val) {
	char digits[5];
	int len = 0;
	for(int d = 0; d < 5; ++d) {
		digits[d] = val & 0xff;
		val >>= 8;
		if ( val > 0 )
			len++;
		else
			break;
	}

	while(len > 0) {

	}
	return val;
}

/*
void write_varuint(char * & ptr, const uint32_t & val) {

}
*/

int main() {
	char data[] = { 0x03, 0x0f, 0x12, 0x23, 0x29, 0x7f, 0x43, 0x00, };
	char * p = data;
	uint32_t val;
	do {
		val = read_varuint(p);
		cout << val << endl;
	} while (val != 0);
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
