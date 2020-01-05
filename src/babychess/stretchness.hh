#ifndef STRETCHNESS_INCLUDED
#define STRETCHNESS_INCLUDED

enum Stretchness {str_fixed= 0,    /* Widget should only be displayed in its
				      original size.  */
		  str_infinite,    /* Widget can be displayed in any size.  */
		  str_medium,      /* Best displayed in its original size but
				      can be stretched. */

		  str_ignore= -1};

#endif /* ! STRETCHNESS_INCLUDED */
