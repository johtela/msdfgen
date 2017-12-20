#include "msdfgen.h"
#include "msdfgen-ext.h"
#include "GlyphBitmap.h"
#include <algorithm>
#include <string>
#include <ft2build.h>

#ifdef _WIN32
#pragma warning(disable:4996)
#endif
#define LARGE_VALUE 1e240

using namespace msdfgen;

GlyphBitmap* createGlyphBitmap(FontHandle *font, char character)
{
	Shape shape;
	GlyphMetrics *metrics = new GlyphMetrics();
	double range = 4.;
	if (!loadGlyph(shape, font, character, metrics))
	{
		delete metrics;
		return NULL;
	}
	shape.normalize();
	edgeColoringSimple(shape, 3.0); // max. angle
	double l = LARGE_VALUE, b = LARGE_VALUE, r = -LARGE_VALUE, t = -LARGE_VALUE;
	shape.bounds(l, b, r, t);
	GlyphBitmap *result = l == LARGE_VALUE || b == LARGE_VALUE ?
		new GlyphBitmap(0, 0) :
		new GlyphBitmap((int)round(r - l + (2 * range)), (int)round(t - b + (2 * range)));
	if (result->width() != 0 && result->height() != 0)
		generateMSDF(*result, shape, range, 1.0, Vector2(-l + range, -b + range));
	result->character = character;
	result->metrics = metrics;
	return result;
}

bool bySize(GlyphBitmap* a, GlyphBitmap* b) {
	if (a->height() == b->height())
		return a->width() > b->width();
	return a->height() > b->height();
}

bool byCharacter(GlyphBitmap* a, GlyphBitmap* b) {
	return a->character < b->character;
}

Bitmap<FloatRGB> *packBitmaps(std::vector<GlyphBitmap*> &bitmaps, int targetWidth, FILE *csvfile) {
	std::sort(bitmaps.begin(), bitmaps.end(), bySize);
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
	std::sort(bitmaps.begin(), bitmaps.end(), byCharacter);
	Bitmap<FloatRGB> *result = new Bitmap<FloatRGB>(targetWidth, height);
	for (size_t i = 0; i < bitmaps.size(); i++) {
		GlyphBitmap *bm = bitmaps[i];
		if (!bm->bitBlit(*result))
		{
			delete result;
			return NULL;
		}
		char csvline[200];
		sprintf(csvline, "%c|%i|%i|%i|%i|%f|%f|%f|%f|%f\n", 
			bm->character, bm->x, bm->y, bm->width(), bm->height(),
			bm->metrics->width, bm->metrics->height, 
			bm->metrics->offsetX, bm->metrics->offsetY, bm->metrics->advance);
		fputs(csvline, csvfile);
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
	std::string pngfilepath = outputpath + ".png";
	std::string csvfilepath = outputpath + ".csv";

	FreetypeHandle *ft = initializeFreetype();
	if (ft) {
		FontHandle *font = loadFont(ft, fontpath.c_str());
		if (font) {
			FILE *cmapFile = fopen(charmap.c_str(), "r");
			if (cmapFile) {
				std::vector<GlyphBitmap*> bitmaps;
				char character;
				while ((character = fgetc(cmapFile)) >= 0) {
					GlyphBitmap* bitmap = createGlyphBitmap(font, character);
					if (bitmap)
						bitmaps.push_back(bitmap);
					else
						break;
				}
				FILE *csvfile = fopen(csvfilepath.c_str(), "w");
				fputs("char|tx|ty|tw|th|gw|gh|goffsx|goffsy|gadv\n", csvfile);
				Bitmap<FloatRGB> *fontAtlas = packBitmaps(bitmaps, 256, csvfile);
				fclose(csvfile);
				savePng(*fontAtlas, pngfilepath.c_str());
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