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
	/// an ordered sequence of variable digit length unsigned integer
	/// coded in 15 bits in uint16 arrays, start word with 1<<15 flag,
	/// in little-endian 16bit-word order

	typedef vector<uint16_t>::size_type position_type;

	vector<uint16_t> uintseq;
	position_type count;

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

	VarLenUint16Sequence() { uintseq.clear(); count = 0; }

	position_type size() const { return count; }

	iterator begin() { return uintseq.begin(); }
	iterator end() { return uintseq.end(); }

private:

	iterator const_find(const uint64_t & key) const {
	const_iterator const_find(const uint64_t & key) const {
		const_iterator left= uintseq.begin();
		const_iterator right = uintseq.end();
		const_iterator pos;
		while ( left != right ) {
			pos = left + ((right - left)>>1);
			while ( pos != uintseq.end() && (*pos & 0x8000) == 0 )
				++pos;
			if ( pos == right ) {
				--pos;
				while ( (*pos & 0x8000) == 0 )
					--pos;
			}
			uint64_t val;
			val = read(pos);
			if ( val < key ) {
				if ( left == pos )
					next(pos);
				left = pos;
			} else {
				right = pos;
			}
		}
		return right;
	}

/*
	iterator find_linear(const uint64_t & key) {
		iterator itr = uintseq.begin();
		iterator t_itr;
		while ( itr != uintseq.end() ) {
			t_itr = itr;
			if ( key <= next(t_itr) )
				return itr;
			itr = t_itr;
		}
		return itr;
	}
*/

	void insert(const iterator & pos, const uint64_t & orgval) {
		uint64_t val = orgval;
		uint8_t digits = signifbits(val);
		uint8_t len = ((digits ? digits : 1) - 1)/15 + 1;
		position_type offset = pos - uintseq.begin();
		uintseq.insert(pos, len, (uint16_t) ((1<<15) | (val & 0x7fff)) );
		val >>= 15;
		iterator it = uintseq.begin() + offset + 1;
		while ( val ) {
			*(it) = (uint16_t) (val & 0x7fff);
			++it;
			val >>= 15;
		}
		++count;
	}

public:
	bool includes(const uint64_t & key) {
		const_iterator itr = find(key);
		return (itr != uintseq.end() && key == read(itr));
	}

	void add(const uint64_t & val) {
		iterator itr = find(val);
		insert(itr, val);
	}

	uint64_t at(const position_type & index) const {
		const_iterator it = uintseq.begin();
		skip(it, index);
		return next(it);
	}

	position_type skip(const_iterator & pos, const position_type & count = 1) const {
		position_type i;
		for(i = 0; i < count; ++i ) {
			if ( pos == uintseq.end() )
				return i;
			++pos;
			while ( pos != uintseq.end() && (*pos & 0x8000) == 0 ) {
				++pos;
			}
		}
		return i;
	}

	uint64_t read(const const_iterator & it) const {
		position_type len = 1;
		if ( it == uintseq.end() )
			return (uint64_t)-1;
		while ( (it+len) != uintseq.end() && (*(it+len) & 0x8000) == 0 ) {
			++len;
		}
		uint64_t val = 0;
		for(position_type i = len; i > 0; ) {
			--i;
			val <<= 15;
			val |= *(it+i) & 0x7fff;
		}
		return val;
	}

	uint64_t next(iterator & it) const { return next((const_iterator&) it); }
	uint64_t next(const_iterator & it) const {
		position_type len = 1;
		if ( it == uintseq.end() )
			return (uint64_t)-1;
		while ( (it+len) != uintseq.end() && (*(it+len) & 0x8000) == 0 ) {
			++len;
		}
		uint64_t val = 0;
		//cout << "len = " << len << endl;
		for(position_type i = len; i > 0; ) {
			--i;
			val <<= 15;
			val |= *(it+i) & 0x7fff;
		}
		it += len;
		return val;
	}

	ostream & printOn(ostream & out) const {
		for (const_iterator it = uintseq.begin(); it != uintseq.end(); ) {
			if ( it != uintseq.begin() ) {
				cout << ", ";
			}
			out << (uint64_t) next(it);
		}
		out << ". ";
		return out;
	}

	friend std::ostream & operator<<(std::ostream & out, const VarLenUint16Sequence & seq) {
		return seq.printOn(out);
	}
};

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	VarLenUint16Sequence seq;
	seq.add(2337659);
	seq.add(13);
	seq.add(884753);
	seq.add(384);
	seq.add(753);

	cout << "size = " << seq.size() << endl;

	seq.printOn(cout);
	cout << endl;

	uint64_t val = 23694;
	if ( seq.includes(val) ) {
		cout << "found." << endl;
	} else {
		cout << "not found." << endl;
		seq.add(val);
		seq.printOn(cout);
	}
	return 0;
}
