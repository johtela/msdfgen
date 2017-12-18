#include "msdfgen.h"
#include "msdfgen-ext.h"

#ifdef _WIN32
#pragma warning(disable:4996)
#endif

using namespace msdfgen;

typedef Bitmap<FloatRGB>* BITMAP;

BITMAP createCharBitmap(FontHandle *font, char character)
{
	Shape shape;
	if (loadGlyph(shape, font, character)) {
		shape.normalize();
		edgeColoringSimple(shape, 3.0); // max. angle
		BITMAP msdf = new Bitmap<FloatRGB>(32, 32); // image width, height 
		generateMSDF(*msdf, shape, 4.0, 1.0, Vector2(4.0, 4.0)); // range, scale, translation
		return msdf;
	}
	return NULL;
}

int main(int argc, const char* const *argv) {
	if (argc != 3)
		printf("Usage: msdfgen <fontpath> <charmap>");
	const char* fontpath = argv[1];
	const char* charmap = argv[2];

	FreetypeHandle *ft = initializeFreetype();
	if (ft) {
		FontHandle *font = loadFont(ft, fontpath);
		if (font) {
			FILE *cmapFile = fopen(charmap, "r");
			if (cmapFile) {
				std::vector<BITMAP> bitmaps;
				char character;
				while ((character = fgetc(cmapFile)) >= 0) {
					BITMAP bitmap = createCharBitmap(font, character);
					if (bitmap)
						bitmaps.push_back(bitmap);
					else
						break;
				}
				for (int i = 0; i < bitmaps.size(); i++)
					delete bitmaps[i];
				fclose(cmapFile);
			}
			destroyFont(font);
		}
		deinitializeFreetype(ft);
	}
	return 0;
}