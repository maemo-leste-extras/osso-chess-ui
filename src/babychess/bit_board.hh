#ifndef BIT_BOARD_INCLUDED
#define BIT_BOARD_INCLUDED

//#include "dbg.hh"
#include "int.hh"

//#include <stdint.h>

/* Assume that LL represents 64 bits. 
 */

class Bit_Board
{
public:

	int Get_Rank(int rank) const
		{
			return (bits >> (rank << 3)) & 0xff;
		}
	void Set()
		{
			bits= 0xffffffffffffffffLL; 
		}
	void Set(int square)
		{
			//Dbg("set %c%c", 'a'+File(square), '1'+Rank(square));
			bits |= (1LL << (File(square) | (Rank(square) << 3)));
		}
	void Empty() {bits= 0; }

private:
	
	uint_fast64_t bits;
};

#endif /* ! BIT_BOARD_INCLUDED */ 
