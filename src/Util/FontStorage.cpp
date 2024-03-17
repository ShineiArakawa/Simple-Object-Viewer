#include <Util/FontStorage.hpp>

namespace simview {
namespace util {
namespace fonts {

TrueTypeFontRegistry::TrueTypeFontRegistry(const unsigned int* ttfFontData,
                                           const unsigned int ttfFontDataSize,
                                           const unsigned int pixelSize,
                                           const unsigned int padding)
    : _library(),
      _face(),
      _padding(padding) {
  bool isOK = true;

  // =========================================================================
  // Initialize
  // =========================================================================
  if (FT_Init_FreeType(&_library) != 0) {
    isOK = false;
  }

  // =========================================================================
  // Load from memory
  // =========================================================================
  if (isOK && FT_New_Memory_Face(_library,                     // FT_Library      library
                                 (const FT_Byte*)ttfFontData,  // const FT_Byte*  file_base
                                 ttfFontDataSize,              // FT_Long         file_size
                                 0,                            // FT_Long         face_index
                                 &_face                        // FT_Face*        aface
                                 ) != 0) {
    isOK = false;
  }

  // =========================================================================
  // Set pixel size
  // =========================================================================
  if (isOK && FT_Set_Char_Size(_face, 0, pixelSize * pixelSize, 300, 300)) {
    isOK = false;
  }

  if (!isOK) {
    LOG_ERROR("Failed to load TTF fonts");
  }
}

TrueTypeFontRegistry::~TrueTypeFontRegistry() {
  FT_Done_Face(_face);
  FT_Done_FreeType(_library);
}

Bitmap_t TrueTypeFontRegistry::getBitmap(const char* text, int& width, int& height) const {
  std::u32string u32str = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(text);
  std::vector<int> lineHightOffsets = {0};
  std::vector<int> lineHights = {0};

  {
    // Calculate target bitmap size
    int lineWidth = 0;   // Width of each line
    int lineHeight = 0;  // Height of each line
    int maxWidth = 0;    // Width of canvas
    int maxHeight = 0;   // Height of canvas
    int currentLine = 0;

    for (int iChar = 0; iChar < (int)u32str.size(); iChar++) {
      if (u32str[iChar] == '\n') {
        lineWidth = 0;   // Reset line width
        lineHeight = 0;  // Reset line height

        currentLine++;                          // Next line
        lineHightOffsets.push_back(maxHeight);  // Init line height offset position

        lineHights.push_back(0);
      } else {
        if (FT_Load_Char(_face, u32str[iChar], FT_LOAD_RENDER)) {
          continue;
        }

        FT_GlyphSlot& slot = _face->glyph;

        // Per charactor
        const int ascender = _face->size->metrics.ascender >> 6;
        const int descender = _face->size->metrics.descender >> 6;

        const int charWidth = slot->advance.x >> 6;
        const int charHeight = ascender - descender;

        // Per line
        lineWidth += charWidth;
        lineHeight = std::max(lineHeight, charHeight);
        lineHights[currentLine] = lineHeight;

        // Per bitmap
        maxWidth = std::max(maxWidth, lineWidth);
        maxHeight = std::max(maxHeight, lineHightOffsets[currentLine] + lineHeight);
      }
    }

    width = maxWidth + _padding * 2;    // Add pagging
    height = maxHeight + _padding * 2;  // Add pagging
  }

  Bitmap_t distBitmap = std::make_shared<Bitmap>(width, height);

  {
    // Draw
    int penX = _padding;                  // left position
    int penY = lineHights[0] + _padding;  // bottom position
    int currentLine = 0;                  // Current drawing line

    for (int iChar = 0; iChar < (int)u32str.size(); iChar++) {
      if (u32str[iChar] == '\n') {
        currentLine++;
        penX = _padding;
        penY += lineHights[currentLine];
      } else {
        if (FT_Load_Char(_face, u32str[iChar], FT_LOAD_RENDER)) {
          continue;
        }

        FT_GlyphSlot& slot = _face->glyph;

        drawBitmap(slot->bitmap,
                   penX + slot->bitmap_left,
                   penY - slot->bitmap_top + (int)(_face->size->metrics.descender >> 6),
                   distBitmap);

        penX += slot->advance.x >> 6;
      }
    }
  }

#if defined(DEBUG_FONT_STORAGE_SAVE_BITMAP_IMAGE)
  {
    // For test
    const auto bytes = distBitmap->getPointer();
    stb::saveImage(distBitmap->width, distBitmap->height, 1, bytes, "text.png");
  }
#endif

  return distBitmap;
}

void TrueTypeFontRegistry::drawBitmap(const FT_Bitmap& srcBitmap,
                                      const int& cursorX,
                                      const int& cursorY,
                                      Bitmap_t& distBitmap) {
  const int maxX = cursorX + srcBitmap.width;
  const int maxY = cursorY + srcBitmap.rows;

  LOG_DEBUG("##### Draw char");
  LOG_DEBUG("from: (" + std::to_string(cursorX) + ", " + std::to_string(cursorY) + ")");
  LOG_DEBUG("to  : (" + std::to_string(maxX - 1) + ", " + std::to_string(maxY - 1) + ")");
  LOG_DEBUG("shape  : (" + std::to_string(maxX - cursorX) + ", " + std::to_string(maxY - cursorY) + ")");

  for (int i = cursorX, p = 0; i < maxX; i++, p++) {
    for (int j = cursorY, q = 0; j < maxY; j++, q++) {
      if (i < 0 || j < 0 || i >= distBitmap->width || j >= distBitmap->height) {
        continue;
      }

      distBitmap->data[j * distBitmap->width + i] = srcBitmap.buffer[q * srcBitmap.width + p];
    }
  }
}

}  // namespace fonts
}  // namespace util
}  // namespace simview