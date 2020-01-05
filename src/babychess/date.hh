#ifndef DATE_INCLUDED
#define DATE_INCLUDED

/* Wall clock time, with millisecond precision. 
 */

#include "string.hh"
#include "inline.hh"

#include <sys/types.h>
#include <time.h>

#include <sys/time.h>

typedef struct timeval Date;

/* Date diffs in milliseconds. 
 */

Inline Date Date_No_Date()
{
	struct timeval tm;
	tm.tv_sec= (time_t) -1;
	return tm;
}

#define DATE_NO_DATE Date_No_Date()

Inline Date Get_Date()
{
	struct timeval rv;
	if (0 != gettimeofday(& rv, NULL))
		return Date_No_Date();
	return rv;
}

Inline int ms_To_Date_Diff(int ms)
{
	return ms;
}

Inline int Date_Diff_ms(Date d1, 
		 Date d2)
/* The number of milliseconds between DATE_1 and DATE_2.  DATE_1 is before
 * DATE_2. 
 */
{
	return ((d2.tv_sec - d1.tv_sec) * 1000
		+ (d2.tv_usec - d1.tv_usec) / 1000);
}

Inline int Date_Diff_s(Date d1, Date d2)
/* The same in seconds.  Using milliseconds limits the time span on 32 bit machines.
 */
{
	return d2.tv_sec - d1.tv_sec;
}

Inline Date operator + (Date tv, int dd)
{
	tv.tv_sec += dd / 1000;
	tv.tv_usec += dd % 1000 * 1000;
	if (tv.tv_usec > 1000000)
	{
		tv.tv_usec -= 1000000;
		++ tv.tv_sec;
	}
	return tv;
}

Inline Date Make_Date(time_t t)
{
	struct timeval tv;
	tv.tv_sec= t;
	tv.tv_usec= 0;
	return tv;
}

Inline bool Date_Is(Date d)
{
	return d.tv_sec != (time_t)-1;
}

Inline void operator += (Date &date, int date_diff)
{
	date= date + date_diff;
}

String Format_Date(Date, const char *format= "%c");
Date Parse_Date(String, const char *format= "%c");
/* Both functions operate on the local time, rather than UTC.  Use
 * format strings of Libc.
 */

String Date_Get_Week_Day_Name(int);
/* monday-based
 */

String Date_Get_Month_Name(int);

#endif /* ! DATE_INCLUDED */
