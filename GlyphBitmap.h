#pragma once

#include "msdfgen.h"
#include "msdfgen-ext.h"

namespace msdfgen {

class GlyphBitmap : public Bitmap<FloatRGB>
{
public:
	GlyphBitmap();
	GlyphBitmap(int width, int height);
	~GlyphBitmap();

	bool canFit(Bitmap<FloatRGB> &other);
	bool bitBlit(Bitmap<FloatRGB> &other);
	char character;
	int x;
	int y;
	GlyphMetrics *metrics;
};

}

