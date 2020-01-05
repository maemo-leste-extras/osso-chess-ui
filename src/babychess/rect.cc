#include "rect.hh"

Rect &
Rect::operator |= (Rect re)
{
	if (! re.Full())
		return *this;

	if (! Full())
	{
		(*this)= re;
		return *this;
	}

	int r= origin.x + size.x;
	int b= origin.y + size.y;
	origin.x= min(origin.x, re.origin.x);
	origin.y= min(origin.y, re.origin.y);
	r= max(r, re.origin.x + re.size.x);
	b= max(b, re.origin.y + re.size.y);
	size.x= r - origin.x;
	size.y= b - origin.y;
	return *this;
}

Rect
Rect::Normalize() const
{
	Rect r= *this;

	if (r.size.x < 0)
	{
		r.origin.x= r.origin.x + r.size.x;
		r.size.x= - r.size.x;
	}

	if (r.size.y < 0)
	{
		r.origin.y= r.origin.y + r.size.y;
		r.size.y= - r.size.y;
	}
			
	return r;
}
