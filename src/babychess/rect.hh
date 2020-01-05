#ifndef RECT_INCLUDED
#define RECT_INCLUDED

#include "point.hh"
#include "pointer.hh"

/* Operators <? and >? are depricated */
template <class T> inline const T& min(const T& a, const T& b)
{
	return (a < b ? a : b);
}

template <class T> inline const T& max(const T& a, const T& b)
{
	return (a > b ? a : b);
}


class Rect
	:  public Pointed
{
public:

	Point origin, size;

	Rect() 
		{
		}
	Rect(Point p): origin(0, 0), size(p) {}
	Rect(Point _org, Point _s)
		: origin(_org), size(_s) 
		{
		}
	Rect(int x, int y, int w, int h)
		: origin(x, y), size(w, h) {} 

	bool Full() const
		{
			return size.x > 0 && size.y > 0; 
		}

	bool operator != (Rect r)
		{
			return origin != r.origin || size != r.size;
		}

	Rect operator+(Point p) const
		{
			return Rect(origin + p, size);
		}

	Rect operator += (Point p)
		{
			origin += p;
			return *this;
		}

	Rect operator-(Point p) const
		{
			return Rect(origin - p, size);
		}

	Rect operator & (Rect r) const
		{
			Rect v;
			v.origin.x = max(origin.x, r.origin.x);
			v.origin.y = max(origin.y, r.origin.y);
			v.size.x   = min(size.x + origin.x, r.size.x + r.origin.x) - v.origin.x;
			if (v.size.x < 0)
				v.size.x= 0;
			v.size.y= min(size.y + origin.y, r.size.y + r.origin.y) - v.origin.y; 
			if (v.size.y < 0)
				v.size.y= 0;
			return v;
		}

	Rect &operator &= (Rect r)
		{
			*this= *this & r;
			return *this;
		}

	Rect &operator |= (Rect re);

	bool Has(Point p) const
		{
			return 
				origin.x <= p.x
				&& p.x < (origin.x + size.x)
				&& origin.y <= p.y
				&& p.y < (origin.y + size.y);
		}

	Rect Normalize() const;

	/* region operations.  negative size yields empty rect. 
	 */
	void operator += (Rect rect)
	{
		Rect result(min(origin.x, rect.origin.x),
			    min(origin.y, rect.origin.y),
			    max(origin.x + size.x, rect.origin.x + rect.size.x),
			    max(origin.y + size.x, rect.origin.y + rect.size.y));

		*this= Rect(result.origin,
			   result.size - result.origin);
	}

	void operator -= (Rect)
	{
	}
};

#endif /* ! RECT_INCLUDED */
