#ifndef __COLOR_H__
#define __COLOR_H__

#include "Warnings.h"
#include "Globals.h"

typedef float4 Color;

extern Color red;
extern Color green;
extern Color blue;
extern Color cyan;
extern Color yellow;
extern Color magenta;
extern Color black;
extern Color white;

struct FadeColor
{
	FadeColor() : color(Color()), pos(0.0f), changingColor(false)
	{}

	FadeColor(Color c, float p, bool change) : color(c), pos(p), changingColor(change)
	{}

	FadeColor(float r, float g, float b, float a, float p, bool change) : color(Color(r, g, b, a)), pos(p), changingColor(change)
	{}

	bool operator == (const FadeColor& fc)
	{
		return (this->pos == fc.pos && this->color.x == fc.color.x && this->color.y == fc.color.y && this->color.z == fc.color.z && this->color.w == fc.color.w);
	}

public:
	float pos;
	Color color;
	bool changingColor;
};

#endif // !__COLOR_H__