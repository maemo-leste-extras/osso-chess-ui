#include "game_info.hh"

#include <cstdio>
#include "date.hh"
#include "format.hh"

const char *Game_Info::result_text[4]=
{"1-0",
 "1/2-1/2",
 "0-1",
 "*"};

void Game_Info::Set(const Hash_Table &all_tags)
{
	tags= all_tags; 

	tags.Remove("FEN"); 
	tags.Remove("SetUp"); 

	event= tags.Get("Event");
	tags.Remove("Event");
	if (event == '?')
		event= "";

	site= tags.Get("Site");
	tags.Remove("Site");
	if (site == '?')
		site= "";

	String date= tags.Get("Date");
	tags.Remove("Date");

	Parse_Date(date, date_year, date_month, date_day);

	round= tags.Get("Round");
	tags.Remove("Round");
	if (round == '?')
		round= "";

	white= tags.Get("White");
	tags.Remove("White");
	if (white == '?')
		white= ""; 

	black= tags.Get("Black");
	tags.Remove("Black");
	if (black == '?')
		black= "";  
			
	String new_result= tags.Get("Result");
	tags.Remove("Result");
			
	if (new_result == "1-0")
		result= re_white;
	else if (new_result == "0-1")
		result= re_black;
	else if (new_result == "1/2"  ||
		 new_result == "1/2-1/2")
		result= re_draw;
	else 
		result= re_unfinished; 
}

void Game_Info::Reset()
{
	/* Maintain parallel to RAW_PGN::Reset();
	 */

	//Dbg_Function();

	event= site= round= white= black= ""; 
	result= re_unfinished; 
	tags.Free(); 

//	Date_Val date_val;
//	Get_Date_Val(date_val, Get_Date());
	Date now= Get_Date();
	struct tm *tm= localtime(&now.tv_sec);
	if (tm)
	{
		date_year=  1900 + tm->tm_year;
		date_month= tm->tm_mon;
		date_day=   tm->tm_mday;
	}
	else
		date_year= date_month= date_day= -1; 
	/*Dbg_Int(date_year);
	Dbg_Int(date_month);
	Dbg_Int(date_day);*/
}

String Game_Info::Format_Date() const
{
	/*Dbg_Function();
	Dbg_Int(date_year);
	Dbg_Int(date_month);
	Dbg_Int(date_day);*/

	char buf[]= "????.??.??";

	if (date_year >= 0)
	{
		int year= date_year;
		buf[3]= '0' + year % 10;
		year /= 10;
		buf[2]= '0' + year % 10;
		year /= 10;
		buf[1]= '0' + year % 10;
		year /= 10;
		buf[0]= '0' + year % 10;
	}

	if (date_month >= 0)
	{
		int month= date_month + 1;
		buf[6]= '0' + month % 10;
		buf[5]= '0' + month / 10 % 10;
	}

	if (date_day >= 0)
	{
		buf[9]= '0' + date_day % 10;
		buf[8]= '0' + date_day / 10 % 10;
	}

	//Dbg_Str(buf);

	return buf; 
}

String Game_Info::Format_Date_Locally(int year, int month, int day) 
{
	//Dbg_Function();
	//Dbg("\tyear= %d, month= %d, day= %d", year, month, day); 

	if (year < 0)
		return "";

	if (month < 0)
		return Format("%d", year);
	
	if (day < 0)  day= 1;

	String date_F_text= Format("%d-%d-%d",
				   year,
				   month + 1,
				   day);
	Date date= ::Parse_Date(date_F_text, "%F");
	
	return ::Format_Date(date, "%x");
}

void Game_Info::Parse_Date(String date, int &year, int &month, int &day)
{
	//Dbg_Function(); 
	//Dbg_Str(date()); 

	if ((! date.Is()) || date[0] == '?')
	{
		year= month= day= -1;
	}
	else
	{
		int s= sscanf(date(),
			      "%d.%d.%d",
			      & year,
			      & month,
			      & day);

		switch (s)
		{
		default:  
		case 0:   year= -1;
		case 1:   month= 0;
		case 2:   day= -1;
		case 3:   -- month;
		}
	}

	//Dbg("\tyear= %d, month= %d, day= %d", year, month, day); 
}
