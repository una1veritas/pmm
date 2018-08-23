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

uint32_t nlz32_IEEEFP(uint32_t x)
{
	// Hacker's Delight 2nd by H. S. Warren Jr., 5.3, p. 104 --
	double d = (double)x + 0.5;
	uint32_t *p = ((uint32_t *) &d) + 1;
	return 0x41e - (*p>>20);  // 31 - ((*(p+1)>>20) - 0x3FF)
}
/*
uint32 nlz32_ABM(uint32 x)
{
    int ret;
    __asm__ volatile ("lzcnt %1, %0" : "=r" (ret) : "r" (x) );
    return ret;
}
 */

class VarintSequence {
	vector<uint8_t> byteseq;

public:
	static uint8_t signifbits(const uint64_t & x) {
		struct LowHigh64 {
			uint32_t low, high;
		};
		LowHigh64 * lh = & (LowHigh64 &) x;
		const uint8_t nlz_high = nlz32_IEEEFP(lh->high);
		return (nlz_high == 32 ? 32 - nlz32_IEEEFP(lh->low) : 64 - nlz_high );
	}

public:
	VarintSequence() { byteseq.clear(); }

	void insert(const uint32_t & pos, const uint64_t & val) {
		uint64_t cval = val;
		uint8_t len = signifbits(cval>>32);
		len = len == 0 ? signifbits(cval) : len;
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

	void append(const uint64_t & val) {
		insert(byteseq.size(), val);
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

	VarintSequence seq;
	seq.append(753);
	seq.append(13);
	seq.append(1365344459);

	seq.printOn(cout);

	return 0;
}
