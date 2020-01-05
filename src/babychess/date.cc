#include "date.hh"

//#include "deref.hh"
//#include "dbg.hh"

#ifndef DATE_TIME_T

#else /* DATE_TIME_T */

int Date_Diff_ms(Date a, Date b)
{
	return (b - a) * 1000;
}

#endif /* DATE_TIME_T */


String
Format_Date(Date date,
	    const char *format)
{
#ifndef DATE_TIME_T
	time_t tm= date.tv_sec;
#else
	time_t tm= date;
#endif

	struct tm *t= localtime(& tm);  /* gm */ 
	if (t == NULL)
		return "";
	struct tm &stm= *t;

	enum {size= 0x100};
	char result[size];
	if (0 == strftime(result,
			  size,
			  format,
			  &stm))
		return "";
	return result; 
}

String Date_Get_Week_Day_Name(int i)
{
	struct tm brokentime;
	brokentime.tm_wday= (i + 1) % 7;
	char rv[0x40];
	int s= strftime(rv, 0x40, "%A", & brokentime);
	return s == 0x40 ? "day" : rv;
}

String
Date_Get_Month_Name(int i)
{
	struct tm brokentime;
	brokentime.tm_mon= i;
	char rv[0x40];
	int s= strftime(rv, 0x40, "%B", & brokentime);
	return s == 0x40 ? "month" : rv;
}

Date Parse_Date(String text, 
		const char *format)
{
	{
		if (! text.Is())
			goto error;

		struct tm tm;
		memset(& tm, '\0', sizeof (tm));
		const char *st=strptime(text(),
					format,
					& tm);
		if (st == NULL)
			goto error;

		time_t result= mktime(& tm);
	
		return Make_Date(result);
	}

 error:
	return DATE_NO_DATE; 
}
