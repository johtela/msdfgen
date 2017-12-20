#pragma once

#include "msdfgen.h"

namespace msdfgen {

class GlyphBitmap : public Bitmap<FloatRGB>
{
public:
	GlyphBitmap();
	GlyphBitmap(int width, int height);

	bool canFit(Bitmap<FloatRGB> &other);
	bool bitBlit(Bitmap<FloatRGB> &other);
	int x;
	int y;

};

}

