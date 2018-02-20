#pragma once

#include "msdfgen.h"
#include "msdfgen-ext.h"

namespace msdfgen {

template<typename T>
class GlyphBitmap : public Bitmap<T>
{
public:
	GlyphBitmap();
	GlyphBitmap(int width, int height);
	~GlyphBitmap();

	bool canFit(Bitmap<T> &target);
	bool bitBlit(Bitmap<T> &target);
	int character;
	int x;
	int y;
	GlyphMetrics *metrics;
};

}

