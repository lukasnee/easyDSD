#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

//#include <cstdlib>
#include "hwg.hpp"
#include "gfxfont.h"

//=========SETTINGS============
/* select one COLOR_DATA_ */
//#define COLOR_DATA_R5G6B5	// the usual format, fits TFT_ILI9163C display driver.
#define COLOR_DATA_R3B5R5G3 // format for ssd1306 oled display in 2byte color mode.
//===========================

#define DISP_RED_BIT_DEPTH 5
#define DISP_GREEN_BIT_DEPTH 6
#define DISP_BLUE_BIT_DEPTH 5

#define DISP_A_MAX ((1 << 8) - 1) /* display alpha (transperancy) max value */
#define DISP_R_MAX ((1 << DISP_RED_BIT_DEPTH) - 1) /* display red gray scale max value */
#define DISP_G_MAX ((1 << DISP_GREEN_BIT_DEPTH) - 1) /* display green gray scale max value */
#define DISP_B_MAX ((1 << DISP_BLUE_BIT_DEPTH) - 1) /* display blue gray scale max value */

#ifdef COLOR_DATA_R5G6B5
#define COLOR_DATA_FORMAT(color) 	(((((color & 0x0000FF00) >> 8 ) * DISP_G_MAX) / 0xFF) << 13) | \
									(((((color & 0x000000FF) >> 0 ) * DISP_B_MAX) / 0xFF) << 8 ) | \
									(((((color & 0x00FF0000) >> 16) * DISP_R_MAX) / 0xFF) << 3 ) | \
									(((((color & 0x0000FF00) >> 8 ) * DISP_G_MAX) / 0xFF) >> 3 )
#elif defined(COLOR_DATA_R3B5R5G3)
#define COLOR_DATA_FORMAT(color) 	(((((color & 0x00FF0000) >> 16 ) * DISP_R_MAX) / 0xFF) << 11) | \
									(((((color & 0x0000FF00) >> 8 ) * DISP_G_MAX) / 0xFF) << 5 ) | \
									(((((color & 0x000000FF) >> 0) * DISP_B_MAX) / 0xFF) << 0 )
#endif

/* 16 bit hex RGB color: - 5 bits red (MSBs), 6 bits green, 5 bits blue  (LSBs) */
typedef uint16_t color_rgb16_t;

/* format: 0xAARRGGBB (aplha, red, green, blue).
 * alpha - [0x00 - opaque : 0xFF - fully transperant];
 * red, green, blue - [0x00 - off : 0xFF - fully saturated]. */
typedef enum color_hex32_t_{

	COLOR_AQUA = 			0xFF00FFFF,
	COLOR_BLACK = 			0xFF000000,
	COLOR_BLUE = 			0xFF0000FF,
	COLOR_FUCHSIA = 		0xFFFF00FF,
	COLOR_GRAY = 			0xFF808080,

	COLOR_GREEN_VERY_DARK = 0xFF002000,
	COLOR_GREEN_DARK = 		0xFF004000,
	COLOR_GREEN = 			0xFF008000,
	COLOR_LIME = 			0xFF00FF00,
	COLOR_LIME_BRIGHT = 	0xFF80FF80,

	COLOR_MAROON = 			0xFF800000,
	COLOR_NAVY = 			0xFF000080,
	COLOR_OLIVE = 			0xFF808000,
	COLOR_PURPLE = 			0xFF800080,
	COLOR_RED = 			0xFFFF0000,
	COLOR_SILVER = 			0xFFC0C0C0,
	COLOR_TEAL = 			0xFF008080,
	COLOR_WHITE = 			0xFFFFFFFF,
	COLOR_YELLOW = 			0xFFFFFF00,

}color_hex32_t;

/// A generic graphics superclass that can handle all sorts of drawing. At a minimum you can subclass and provide drawPixel(). At a maximum you can do a ton of overriding to optimize. Used for any/all Adafruit displays!
class Adafruit_GFX : public Print {

 public:

  Adafruit_GFX(int16_t w, int16_t h); // Constructor

#ifdef WUCY_OS

  uint8_t getCharMaxHeight() { return gfxFont->yAdvance; };

  virtual ~Adafruit_GFX() {}; // Deconstructor
#endif/* WUCY_OS */

  // This MUST be defined by the subclass:
  virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;    ///< Virtual drawPixel() function to draw to the screen/framebuffer/etc, must be overridden in subclass. @param x X coordinate.  @param y Y coordinate. @param color 16-bit pixel color.

  // TRANSACTION API / CORE DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void startWrite(void);
  virtual void writePixel(int16_t x, int16_t y, uint16_t color);
  virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h);
  virtual void writeFastVLine(int16_t x, int16_t y, int16_t h);
  virtual void writeFastHLine(int16_t x, int16_t y, int16_t w);
  virtual void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
  virtual void endWrite(void);

  // CONTROL API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void setRotation(uint8_t r);
  virtual void invertDisplay(boolean i);

  // BASIC DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void
    // It's good to implement those, even if using transaction API
    drawFastVLine(int16_t x, int16_t y, int16_t h),
    drawFastHLine(int16_t x, int16_t y, int16_t w),
    fillRect(int16_t x, int16_t y, int16_t w, int16_t h),
    fillScreen(),
    // Optional and probably not necessary to change
    drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1),
    drawRect(int16_t x, int16_t y, int16_t w, int16_t h);

  // These exist only with Adafruit_GFX (no subclass overrides)
  void
    drawCircle(int16_t x0, int16_t y0, int16_t r),
    drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername),
    fillCircle(int16_t x0, int16_t y0, int16_t r),
    fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
      int16_t delta),
    drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
      int16_t x2, int16_t y2),
    fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
      int16_t x2, int16_t y2),
    drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
      int16_t radius),
    fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
      int16_t radius),
    drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
      int16_t w, int16_t h),
    drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
      int16_t w, int16_t h, uint16_t bg),
    drawBitmap(int16_t x, int16_t y, uint8_t *bitmap,
      int16_t w, int16_t h),
    drawBitmap(int16_t x, int16_t y, uint8_t *bitmap,
      int16_t w, int16_t h, uint16_t bg),
    drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
      int16_t w, int16_t h),
    drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
      int16_t w, int16_t h),
    drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap,
      int16_t w, int16_t h),
    drawGrayscaleBitmap(int16_t x, int16_t y,
      const uint8_t bitmap[], const uint8_t mask[],
      int16_t w, int16_t h),
    drawGrayscaleBitmap(int16_t x, int16_t y,
      uint8_t *bitmap, uint8_t *mask, int16_t w, int16_t h),
    drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[],
      int16_t w, int16_t h),
    drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap,
      int16_t w, int16_t h),
    drawRGBBitmap(int16_t x, int16_t y,
      const uint16_t bitmap[], const uint8_t mask[],
      int16_t w, int16_t h),
    drawRGBBitmap(int16_t x, int16_t y,
      uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h),
    drawChar(int16_t x, int16_t y, unsigned char c,
      uint16_t bg, uint8_t size),
    drawChar(int16_t x, int16_t y, unsigned char c,
	      uint16_t bg, uint8_t size_x, uint8_t size_y),
    getTextBounds(const char *string, int16_t x, int16_t y,
      int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h),
//	getTextBounds(const std::string &str, int16_t x, int16_t y,
//	          int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h),
    setTextSize(uint8_t s),
    setTextSize(uint8_t sx, uint8_t sy),
    setFont(const GFXfont *f = NULL);

  /**********************************************************************/
  /*!
    @brief  Set text cursor location
    @param  x    X coordinate in pixels
    @param  y    Y coordinate in pixels
  */
  /**********************************************************************/
  void setCursor(int16_t x, int16_t y) {
	  cursor_x = _cursor_offset_x = x;
	  cursor_y = _cursor_offset_y = y;

  }



  void setBounds(int16_t bound_w = 0, int16_t bound_h = 0) {

	  _cursor_bound_w = bound_w;
	  _cursor_bound_h = bound_h;

  }

  /**********************************************************************/
  /*!
    @brief   Set draw color.
    @param   color  16-bit 5-6-5 Color to draw graphics with.
  */
  /**********************************************************************/

  void 	setDrawColor(color_rgb16_t color) {
	  drawcolor = color;
  };

  /**********************************************************************/
  /*!
    @brief   Set text font color with transparant background
    @param   color   16-bit 5-6-5 Color to draw text with
    @note    For 'transparent' background, background and foreground
             are set to same color rather than using a separate flag.
  */
  /**********************************************************************/
  virtual void setTextColor(color_rgb16_t color) { textcolor = textbgcolor = color; }

  /**********************************************************************/
  /*!
    @brief   Set text font color with custom background color
    @param   color  16-bit 5-6-5 Color to draw text with
    @param   bg  16-bit 5-6-5 Color to draw background/fill with
  */
  /**********************************************************************/
  virtual void setTextColor(color_rgb16_t color, color_rgb16_t bg) {
    textcolor   = color;
    textbgcolor = bg;
  }

static color_rgb16_t colorHEX32To16(color_hex32_t color32) {

	color_rgb16_t color16 = COLOR_DATA_FORMAT(color32);

  	return color16;

}

  /**********************************************************************/
  /*!
    @brief   Set draw color.
    @param   color   32-bit 8-8-8-8 transperancy byte and RGB Color bytes to draw graphics with

  */
  /**********************************************************************/
  void 	setDrawColor(color_hex32_t color) {
	  drawcolor = colorHEX32To16(color);
  };

  /**********************************************************************/
  /*!
    @brief   Set text font color with transparant background
    @param   color   32-bit 8-8-8-8 transperancy byte and RGB Color bytes to draw text with
    @note    For 'transparent' background, background and foreground
             are set to same color rather than using a separate flag.
  */
  /**********************************************************************/
  virtual void setTextColor(color_hex32_t color) { textcolor = textbgcolor = colorHEX32To16(color); }

  /**********************************************************************/
  /*!
    @brief   Set text font color with custom background color
    @param   color   32-bit 8-8-8-8 transperancy byte and RGB Color bytes to draw text with
    @param   bg   32-bit 8-8-8-8 transperancy byte and RGB Color bytes to draw background/fill with
  */
  /**********************************************************************/
  virtual void setTextColor(color_hex32_t color, color_hex32_t bg) {
    textcolor   = colorHEX32To16(color);
    textbgcolor = colorHEX32To16(bg);;
  }

  /**********************************************************************/
  /*!
  @brief  Set whether text that is too long for the screen width should
          automatically wrap around to the next line (else clip right).
  @param  w  true for wrapping, false for clipping
  */
  /**********************************************************************/
  void setTextWrap(boolean w) { wrap = w; }

  /**********************************************************************/
  /*!
    @brief  Enable (or disable) Code Page 437-compatible charset.
            There was an error in glcdfont.c for the longest time -- one
            character (#176, the 'light shade' block) was missing -- this
            threw off the index of every character that followed it.
            But a TON of code has been written with the erroneous
            character indices. By default, the library uses the original
            'wrong' behavior and old sketches will still work. Pass
            'true' to this function to use correct CP437 character values
            in your code.
    @param  x  true = enable (new behavior), false = disable (old behavior)
  */
  /**********************************************************************/
  void cp437(boolean x=true) { _cp437 = x; }

  size_t write(unsigned char c);

  /************************************************************************/
  /*!
    @brief      Get width of the display, accounting for current rotation
    @returns    Width in pixels
  */
  /************************************************************************/
  int16_t width(void) const { return _width; };

  /************************************************************************/
  /*!
    @brief      Get height of the display, accounting for current rotation
    @returns    Height in pixels
  */
  /************************************************************************/
  int16_t height(void) const { return _height; }

  /************************************************************************/
  /*!
    @brief      Get rotation setting for display
    @returns    0 thru 3 corresponding to 4 cardinal rotations
  */
  /************************************************************************/
  uint8_t getRotation(void) const { return rotation; }

  // get current cursor position (get rotation safe maximum values,
  // using: width() for x, height() for y)
  /************************************************************************/
  /*!
    @brief  Get text cursor X location
    @returns    X coordinate in pixels
  */
  /************************************************************************/
  int16_t getCursorX(void) const { return cursor_x; }

  /************************************************************************/
  /*!
    @brief      Get text cursor Y location
    @returns    Y coordinate in pixels
  */
  /************************************************************************/
  int16_t getCursorY(void) const { return cursor_y; };

 protected:
  void
    charBounds(char c, int16_t *x, int16_t *y,
      int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);
  int16_t
    WIDTH,          ///< This is the 'raw' display width - never changes
    HEIGHT;         ///< This is the 'raw' display height - never changes
  int16_t
    _width,         ///< Display width as modified by current rotation
    _height,        ///< Display height as modified by current rotation
    cursor_x,       ///< x location to start print()ing text
    cursor_y,       ///< y location to start print()ing text
  	_cursor_offset_x,
	_cursor_offset_y,
  	_cursor_bound_w,
	_cursor_bound_h;
  uint16_t
  	drawcolor, 		///< 16-bit color for draw functions
    textcolor,      ///< 16-bit background color for print()
    textbgcolor;    ///< 16-bit text color for print()
  uint8_t
    textsize_x,      ///< Desired magnification in X-axis of text to print()
    textsize_y,      ///< Desired magnification in Y-axis of text to print()
    rotation;       ///< Display rotation (0 thru 3)
  boolean
    wrap,           ///< If set, 'wrap' text at right edge of display
    _cp437;         ///< If set, use correct CP437 charset (default is off)
  GFXfont
    *gfxFont;       ///< Pointer to special font
};

#endif // _ADAFRUIT_GFX_H
