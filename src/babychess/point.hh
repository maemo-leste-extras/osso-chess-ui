#ifndef POINT_INCLUDED
#define POINT_INCLUDED

#include "inline.hh"

class Point
{
public:

	int x, y;

	Point() 
		/* raw */
		{
		}
	Point(int new_x)
		:  x(new_x), y(new_x)  
		{
		}
	Point(int new_x, int new_y)
		:  x (new_x), y (new_y)  
		{
		}

	bool operator == (Point p) const
		{
			return x == p.x && y == p.y;
		}

	bool operator != (Point p) const
		{
			return x != p.x || y != p.y;
		}

	Point operator+(Point p) const
		{
			return Point(x + p.x, y + p.y);
		}

	Point operator-(Point p) const
		{
			return Point(x - p.x, y - p.y);
		}

	Point operator - () const
		{
			return Point(-x, -y);
		}

	Point operator * (int n) const
		{
			return Point(x * n, y * n);
		}

	Point &operator+=(Point p)
		{
			x += p.x;
			y += p.y;
			return *this;
		}
	Point &operator -= (int p)
		{
			x -= p;
			y -= p;
			return *this;
		}
};

Inline Point operator * (int n, Point point)
{
	return Point(n * point.x, n * point.y);
}

#endif /* ! POINT_INCLUDED */


