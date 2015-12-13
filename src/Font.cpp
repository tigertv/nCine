#include <cstring>
#include "Font.h"
#include "FontGlyph.h"
#include "IFile.h"
#include "ServiceLocator.h"
#include "Texture.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructs a font class from a texture and a FNT file (from AngelCode's Bitmap Font Generator)
Font::Font(const char *texFilename, const char *fntFilename)
	: Object(FONT_TYPE, fntFilename), texture_(NULL), lineHeight_(0), base_(0), width_(0), height_(0), numGlyphs_ (0), numKernings_(0), glyphs_(NULL)
{
	texture_ = new Texture(texFilename);
	glyphs_ = new FontGlyph[MaxGlyphs];

	IFile *fileHandle = IFile::createFileHandle(fntFilename);
	fileHandle->open(IFile::MODE_READ);
	parseFntFile(fileHandle);
	delete fileHandle;
}

Font::~Font()
{
	delete[] glyphs_;
	delete texture_;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns a constant pointer to a glyph
const FontGlyph* Font::glyph(unsigned int glyphId) const
{
	if (glyphId < MaxGlyphs)
	{
		return &glyphs_[glyphId];
	}
	else
	{
		return NULL;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Loads an AngelCode's FNT file in a RAM buffer then parses it
void Font::parseFntFile(IFile *fileHandle)
{
	int fileLine = 0;

	int glyphId;
	unsigned int x, y, width, height;
	int xOffset, yOffset;
	unsigned int xAdvance;
	int secondGlyphId;
	int kerningAmount;

	char *fileBuffer = new char[fileHandle->size()];
	fileHandle->read(fileBuffer, fileHandle->size());

	char *buffer = fileBuffer;
	do
	{
		fileLine++;

		// skipping entirely the "info" line
		if (strncmp(buffer, "info", 4) == 0)
		{
			continue;
		}
		else if (strncmp(buffer, "common", 6) == 0)
		{
			sscanf(buffer, "common lineHeight=%u base=%u scaleW=%u scaleH=%u", &lineHeight_, &base_, &width_, &height_);
			if (static_cast<int>(width_) != texture_->width() || static_cast<int>(height_) != texture_->height())
			{
				LOGF_X("FNT texture has a different size: (%u, %u)", width_, height_);
				exit(EXIT_FAILURE);
			}
		}
		// skipping entirely the "page" line
		else if (strncmp(buffer, "page", 4) == 0)
		{
			continue;
		}
		else if (strncmp(buffer, "chars", 5) == 0)
		{
			sscanf(buffer, "chars count=%u", &numGlyphs_);
		}
		else if (strncmp(buffer, "char", 4) == 0)
		{
			sscanf(buffer, "char id=%d x=%u y=%u width=%u height=%u xoffset=%d yoffset=%d xadvance=%u", &glyphId, &x, &y, &width, &height, &xOffset, &yOffset, &xAdvance);
			glyphs_[glyphId].set(x, y, width, height, xOffset, yOffset, xAdvance);
		}
		else if (strncmp(buffer, "kernings", 8) == 0)
		{
			sscanf(buffer, "kernings count=%u", &numKernings_);
		}
		else if (strncmp(buffer, "kerning", 7) == 0)
		{
			sscanf(buffer, "kerning first=%d second=%d amount=%d ", &glyphId, &secondGlyphId, &kerningAmount);
			glyphs_[glyphId].addKerning(secondGlyphId, kerningAmount);
		}
	} while (strchr(buffer, '\n') && (buffer = strchr(buffer, '\n')+1) < fileBuffer + fileHandle->size());

	LOGI_X("FNT file parsed: %u glyphs and %u kernings", numGlyphs_, numKernings_);
	delete[] fileBuffer;
}

}
