#include "GlyphBitmap.h"

namespace msdfgen {

GlyphBitmap::GlyphBitmap() : Bitmap<FloatRGB>()
{
}

GlyphBitmap::GlyphBitmap(int width, int height) : Bitmap<FloatRGB>(width, height)
{
}

GlyphBitmap::~GlyphBitmap()
{
	if (metrics)
		delete metrics;
}

bool GlyphBitmap::canFit(Bitmap<FloatRGB>& target)
{
	return x + width() <= target.width() && y + height() <= target.height();
}

bool GlyphBitmap::bitBlit(Bitmap<FloatRGB> &target) {
	if (!canFit(target))
		return false;
	for (int line = 0, src = 0; line < height(); line++) {
		int dest = (y + line) * target.width() + x;
		for (int pixel = 0; pixel < width(); pixel++)
			target.pixels()[dest++] = content[src++];
	}
	return true;
}


}