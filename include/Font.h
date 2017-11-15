#ifndef CLASS_NCINE_FONT
#define CLASS_NCINE_FONT

#include "Object.h"
#include "Vector2.h"

namespace ncine {

class FontGlyph;
class Texture;
class IFile;

/// A class holding every information needed to correctly render text
class DLL_PUBLIC Font : public Object
{
  public:
	/// Constructs the object from a texture and a `FNT` file (from <em>AngelCode's Bitmap Font Generator</em>)
	Font(const char *texFilename, const char *fntFilename);
	virtual ~Font();

	/// Gets the texture object
	inline const Texture *texture() const { return texture_; }
	/// Sets the texture object
	void setTexture(Texture *texture);

	/// Returns font line height
	inline unsigned int lineHeight() const { return lineHeight_; }
	/// Returns font base
	inline unsigned int base() const { return base_; }
	/// Returns texture atlas size
	inline Vector2i textureSize() const { return Vector2i(width_, height_); }
	/// Returns number of glyphs
	inline unsigned int numGlyphs() const { return numGlyphs_; }
	/// Returns a constant pointer to a glyph
	const FontGlyph *glyph(unsigned int glyphId) const;

	inline static ObjectType sType() { return FONT_TYPE; }

  private:
	/// The font texture
	Texture *texture_;
	/// Font line height
	unsigned int lineHeight_;
	/// Font base
	unsigned int base_;
	/// Texture atlas width
	unsigned int width_;
	/// Texture atlas height
	unsigned int height_;
	/// Number of glyphs for this font
	unsigned int numGlyphs_;
	/// Number of kernings for this font
	unsigned int numKernings_;

	/// Maximum number of glyphs in a font
	static const unsigned int MaxGlyphs = 256;
	/// Array of font glyphs
	FontGlyph *glyphs_;

	/// Private copy constructor
	Font(const Font &);
	/// Private assignment operator
	Font &operator=(const Font &);

	/// Loads an <em>AngelCode's</em> `FNT` file in a memory buffer then parses it
	void parseFntFile(IFile *fileHandle);
};

}

#endif
