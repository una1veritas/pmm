//============================================================================
// Name        : VarintSequence.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
using namespace std;

/*
uint32 nlz32_IEEEFP(uint32 x)
{
	// Hacker's Delight 2nd by H. S. Warren Jr., 5.3, p. 104 --
	double d = (double)x + 0.5;
	uint32 *p = ((uint32*) &d) + 1;
	return 0x41e - (*p>>20);  // 31 - ((*(p+1)>>20) - 0x3FF)
}

uint32 nlz32_ABM(uint32 x)
{
    int ret;
    __asm__ volatile ("lzcnt %1, %0" : "=r" (ret) : "r" (x) );
    return ret;
}
 *
 */

class VarintSequence {
	vector<uint8_t> byteseq;

	uint32_t sigbits(const uint32_t & x) {
		// nlz32_IEEEFP(x)
		// Hacker's Delight 2nd by H. S. Warren Jr., 5.3, p. 104 --
		double d = (double)x + 0.5;
		uint32_t *p = ((uint32_t*) &d) + 1;
		return 32 - (0x41e - (*p>>20));  // 31 - ((*(p+1)>>20) - 0x3FF)
	}

public:
	VarintSequence() { byteseq.clear(); }

	void insert(const uint32_t & pos, const uint64_t & val) {
		uint32_t cval = val;
		uint8_t len = sigbits(cval);
		len = len < 5 ? 0 : (len+3)>>3;
		uint8_t bval = (len<<4) | (uint8_t)(cval & 0x0f);
		vector<uint8_t>::iterator it = byteseq.begin()+pos;
		byteseq.insert(it, len+1, bval);
		cval >>= 4;
		it = byteseq.begin()+pos+1;
		while (cval != 0) {
			*(it) = (uint8_t) cval & 0xff;
			cval >>= 8;
			++it;
		}
	}

	void append(const uint32_t & val) {
		uint32_t cval = val;
		uint8_t len = sigbits(cval);
		len = len < 5 ? 0 : (len+3)>>3;
		byteseq.push_back( (len<<4) | (uint8_t)(cval & 0x0f));
		cval >>= 4;
		while (cval != 0) {
			byteseq.push_back(cval & 0xff);
			cval >>= 8;
		}
	}

	ostream & printOn(ostream & out) {
		for (vector<uint8_t>::iterator it = byteseq.begin();
				it != byteseq.end(); ++it) {
			if ( it != byteseq.begin() ) {
				cout << ", ";
			}
			out << hex << (unsigned int) *it;
		}
		out << " ";
		return out;
	}
};

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	cout << "sizeof(uint64_t) = " << sizeof(uint64_t) << endl;

	VarintSequence seq;
	seq.insert(0,753);
	seq.insert(0,13);
	seq.append(136534);

	seq.printOn(cout);
	return 0;
}
