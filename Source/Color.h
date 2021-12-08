#ifndef __COLOR_H__
#define __COLOR_H__

#include "Warnings.h"

struct Color
{
public:
	Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
	{}
	Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a)
	{}

	void Set(float r, float g, float b, float a = 1.0f)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	float* operator & ()
	{
		return (float*)this;
	}

	bool operator == (const Color& c)
	{
		return (this->r == c.r && this->g == c.g && this->b == c.b && this->a == c.a);
	}

public:
	float r, g, b, a;
};

extern Color red;
extern Color green;
extern Color blue;
extern Color black;
extern Color white;

#endif // !__COLOR_H__