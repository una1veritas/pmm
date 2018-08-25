//============================================================================
// Name        : VarLenUint16Sequence.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
using namespace std;

class VarLenUint16Sequence {
	/// a sequence of variable digit length unsigned integer
	/// coded in 15 bits in uint16 arrays, start word with 1<<15 flag,
	/// in little-endian 16bit-word order
	vector<uint16_t> uintseq;

	static uint8_t nlz32_IEEEFP(uint32_t x) {
		/* Hacker's Delight 2nd by H. S. Warren Jr., 5.3, p. 104 -- */
		double d = (double)x + 0.5;
		uint32_t *p = ((uint32_t*) &d) + 1;
		return 0x41e - (*p>>20);  // 31 - ((*(p+1)>>20) - 0x3FF)
	}

	static uint8_t signifbits(const uint64_t & x) {
		struct LowHigh64 {
			uint32_t low, high;
		};
		LowHigh64 * lh = & (LowHigh64 &) x;
		const uint8_t nlz_high = nlz32_IEEEFP(lh->high);
		return (nlz_high == 32 ? 32 - nlz32_IEEEFP(lh->low) : 64 - nlz_high );
	}

public:
	typedef vector<uint16_t>::iterator iterator;
	typedef vector<uint16_t>::const_iterator const_iterator;

	VarLenUint16Sequence() { uintseq.clear(); }

	void insert(const vector<uint16_t>::size_type & pos, const uint64_t & orgval) {
		uint64_t val = orgval;
		uint8_t digits = signifbits(val);
		uint8_t len = ((digits ? digits : 1) - 1)/15 + 1;
		vector<uint16_t>::iterator it = uintseq.begin() + pos;
		uintseq.insert(it, len, (uint16_t) ((1<<15) | (val & 0x7fff)) );
		val >>= 15;
		it = uintseq.begin()+pos+1;
		while ( val ) {
			*(it) = (uint16_t) (val & 0x7fff);
			++it;
			val >>= 15;
		}
	}

	void append(const uint64_t & val) {
		insert(uintseq.size(), val);
	}

	uint64_t at(const vector<uint16_t>::size_type & pos) {
		uint64_t val;
		vector<uint16_t>::size_type cnt = 0;
		iterator itr = begin();
		while ( itr != end() && cnt < pos) {
			++itr;
			if ( (*itr & 0x8000) != 0 ) {
				++cnt;
			}
		}
		val = *itr & 0x7fff;
		++itr;
		cnt = 1;
		while ( itr != end() && (*itr & 0x8000) == 0 ) {
			val |= ((uint64_t)(*itr & 0x7fff)) << (15*cnt);
			++cnt;
			++itr;
		}
		return val;
	}

	iterator begin() { return uintseq.begin(); }
	iterator end() { return uintseq.end(); }

	ostream & printOn(ostream & out) {
		for (iterator it = begin(); it != end(); ++it) {
			if ( it != begin() && (*it & 0x8000) != 0 ) {
				cout << ", ";
			} else {
				cout << ' ';
			}
			out << hex << (unsigned int) (*it &0x7fff);
		}
		out << " ";
		return out;
	}
};

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	VarLenUint16Sequence seq;
	seq.append(753);
	seq.append(13);
	seq.append(1365768459);

	cout << seq.at(0) << endl;
	cout << seq.at(1) << endl;
	cout << seq.at(2) << endl;

	seq.printOn(cout);

	return 0;
}
