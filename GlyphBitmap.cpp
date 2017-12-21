#include "GlyphBitmap.h"

namespace msdfgen {

template <typename T>
GlyphBitmap<T>::GlyphBitmap() : Bitmap<T>()
{
}

template <typename T>
GlyphBitmap<T>::GlyphBitmap(int width, int height) : Bitmap<T>(width, height)
{
}

template <typename T>
GlyphBitmap<T>::~GlyphBitmap()
{
	if (metrics)
		delete metrics;
}

template <typename T>
bool GlyphBitmap<T>::canFit(Bitmap<T>& target)
{
	return x + width() <= target.width() && y + height() <= target.height();
}

template <typename T>
bool GlyphBitmap<T>::bitBlit(Bitmap<T> &target) {
	if (!canFit(target))
		return false;
	for (int line = 0, src = 0; line < height(); line++) {
		int dest = (y + line) * target.width() + x;
		for (int pixel = 0; pixel < width(); pixel++)
			target.pixels()[dest++] = content[src++];
	}
	return true;
}

template class GlyphBitmap<float>;
template class GlyphBitmap<FloatRGB>;

}