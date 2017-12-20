#include "msdfgen.h"
#include "msdfgen-ext.h"
#include "GlyphBitmap.h"
#include <algorithm>
#include <string>
#include <ft2build.h>

#ifdef _WIN32
#pragma warning(disable:4996)
#endif

using namespace msdfgen;

GlyphBitmap* createCharBitmap(FontHandle *font, char character)
{
	Shape shape;
	GlyphMetrics metrics;
	metrics.range = 4.;
	if (loadGlyph(shape, font, character, &metrics)) {
		shape.normalize();
		edgeColoringSimple(shape, 3.0); // max. angle
		double l, b, r, t;
		shape.bounds(l, b, r, t);
		double width = r - l;
		double height = t - b;
		GlyphBitmap *msdf = new GlyphBitmap(metrics.width, metrics.height); // image width, height 
		generateMSDF(*msdf, shape, metrics.range, 1.0, Vector2(metrics.range, metrics.range));
		return msdf;
	}
	return NULL;
}

bool compare(GlyphBitmap* a, GlyphBitmap* b) {
	if (a->height() == b->height())
		return a->width() > b->width();
	return a->height() > b->height();
}

Bitmap<FloatRGB> *packBitmaps(std::vector<GlyphBitmap*> &bitmaps, int targetWidth) {
	std::sort(bitmaps.begin(), bitmaps.end(), compare);
	int height = 0, ly = 0, lw = targetWidth;
	for (size_t i = 0; i < bitmaps.size(); i++) {
		GlyphBitmap* curr = bitmaps[i];
		if (lw + curr->width() > targetWidth)
		{
			ly = height;
			height += curr->height();
			lw = 0;
		}
		curr->x = lw;
		curr->y = ly;
		lw += curr->width();
	}
	Bitmap<FloatRGB> *result = new Bitmap<FloatRGB>(targetWidth, height);
	for (size_t i = 0; i < bitmaps.size(); i++) {
		if (!bitmaps[i]->bitBlit(*result))
		{
			delete result;
			return NULL;
		}
	}
	return result;
}

int main(int argc, const char* const *argv) {
	if (argc != 3)
		printf("Usage: msdfgen <fontpath> <charmap>");
	std::string fontpath(argv[1]);
	std::string charmap(argv[2]);
	char* fontname = new char[fontpath.size()];
	_splitpath(fontpath.c_str(), NULL, NULL, fontname, NULL);
	std::string outputpath(fontname);
	std::string extension(".png");
	outputpath += extension;

	FreetypeHandle *ft = initializeFreetype();
	if (ft) {
		FontHandle *font = loadFont(ft, fontpath.c_str());
		if (font) {
			FILE *cmapFile = fopen(charmap.c_str(), "r");
			if (cmapFile) {
				std::vector<GlyphBitmap*> bitmaps;
				char character;
				while ((character = fgetc(cmapFile)) >= 0) {
					GlyphBitmap* bitmap = createCharBitmap(font, character);
					if (bitmap)
						bitmaps.push_back(bitmap);
					else
						break;
				}
				Bitmap<FloatRGB> *fontAtlas = packBitmaps(bitmaps, 512);
				savePng(*fontAtlas, outputpath.c_str());
				delete fontAtlas;
				for (size_t i = 0; i < bitmaps.size(); i++)
					delete bitmaps[i];
				fclose(cmapFile);
			}
			destroyFont(font);
		}
		deinitializeFreetype(ft);
	}
	return 0;
}