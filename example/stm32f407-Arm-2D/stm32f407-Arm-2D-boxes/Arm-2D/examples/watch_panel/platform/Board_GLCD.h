/*-----------------------------------------------------------------------------
 * Copyright (c) 2013 - 2019 Arm Limited (or its affiliates). All
 * rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   1.Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   2.Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   3.Neither the name of Arm nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *-----------------------------------------------------------------------------
 * Name:    Board_GLCD.h
 * Purpose: Graphic LCD interface header file
 * Rev.:    1.0.0
 *----------------------------------------------------------------------------*/

#ifndef __BOARD_GLCD_H
#define __BOARD_GLCD_H

#include <stdint.h>
#include <stdbool.h>
//#include "arm_2d/#include "arm_2d.h""

/* Font definitions */
typedef struct _GLCD_FONT {
        uint16_t width;         ///< Character width
        uint16_t height;        ///< Character height
        uint32_t offset;        ///< Character offset
        uint32_t count;         ///< Character count
  const uint8_t *bitmap;        ///< Characters bitmaps
} const GLCD_FONT;

#define GUI_COLOR_RGB(__R, __G, __B)                                            \
            (gui_color_t) { {(__B)>>3, (__G)>>2, (__R)>>3} }

typedef union {
    struct {
        uint16_t    R : 5;
        uint16_t    G : 6;
        uint16_t    B : 5;
    }Channel;
    uint16_t tValue;
} gui_color_t;

typedef gui_color_t color_spectrum_t[64] ;

/**
  \fn          int32_t GLCD_Initialize (void)
  \brief       Initialize Graphic LCD
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_Uninitialize (void)
  \brief       De-initialize Graphic LCD
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_SetForegroundColor (uint32_t color)
  \brief       Set foreground color
  \param[in]   color  Color value
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_SetBackgroundColor (uint32_t color)
  \brief       Set background color
  \param[in]   color  Color value
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_ClearScreen (void)
  \brief       Clear screen (with active background color)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_SetFont (GLCD_FONT *font)
  \brief       Set active font
  \param[in]   font   Pointer to font structure
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_DrawPixel (uint32_t x, uint32_t y)
  \brief       Draw pixel (in active foreground color)
  \param[in]   x  x position in pixels (0 = left corner)
  \param[in]   y  y position in pixels (0 = upper corner)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_DrawHLine (uint32_t x, uint32_t y, uint32_t length)
  \brief       Draw horizontal line (in active foreground color)
  \param[in]   x      Start x position in pixels (0 = left corner)
  \param[in]   y      Start y position in pixels (0 = upper corner)
  \param[in]   length Line length
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_DrawVLine (uint32_t x, uint32_t y, uint32_t length)
  \brief       Draw vertical line (in active foreground color)
  \param[in]   x      Start x position in pixels (0 = left corner)
  \param[in]   y      Start y position in pixels (0 = upper corner)
  \param[in]   length Line length in pixels
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_DrawRectangle (uint32_t x, uint32_t y, uint32_t width, uint32_t height)
  \brief       Draw rectangle (in active foreground color)
  \param[in]   x      Start x position in pixels (0 = left corner)
  \param[in]   y      Start y position in pixels (0 = upper corner)
  \param[in]   width  Rectangle width in pixels
  \param[in]   height Rectangle height in pixels
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_DrawChar (uint32_t x, uint32_t y, int32_t ch)
  \brief       Draw character (in active foreground color)
  \param[in]   x      x position in pixels (0 = left corner)
  \param[in]   y      y position in pixels (0 = upper corner)
  \param[in]   ch     Character
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_DrawString (uint32_t x, uint32_t y, const char *str)
  \brief       Draw string (in active foreground color)
  \param[in]   x      x position in pixels (0 = left corner)
  \param[in]   y      y position in pixels (0 = upper corner)
  \param[in]   str    Null-terminated String
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_DrawBargraph (uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t val)
  \brief       Draw bargraph (in active foreground color)
  \param[in]   x      Start x position in pixels (0 = left corner)
  \param[in]   y      Start y position in pixels (0 = upper corner)
  \param[in]   width  Full bargraph width in pixels
  \param[in]   height Full bargraph height in pixels
  \param[in]   val    Active bargraph value (maximum value is 100)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_DrawBitmap (uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t *bitmap)
  \brief       Draw bitmap
  \param[in]   x      Start x position in pixels (0 = left corner)
  \param[in]   y      Start y position in pixels (0 = upper corner)
  \param[in]   width  Bitmap width in pixels
  \param[in]   height Bitmap height in pixels
  \param[in]   bitmap Bitmap data
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_VScroll (uint32_t dy)
  \brief       Scroll picture on display vertically (newly scrolled in area
               should be filled with background color)
  \param[in]   dy  Scroll size in pixels
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t GLCD_FrameBufferAccess (bool enable)
  \brief       Enable or disable direct access to FrameBuffer
  \param[in]   enable Enable or disable request
                - true (1): enable
                - false(0): disable
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          uint32_t GLCD_FrameBufferAddress (void)
  \brief       Retrieve FrameBuffer address
  \returns     FrameBuffer address
*/

extern int32_t  GLCD_Initialize          (void);
extern int32_t  GLCD_Uninitialize        (void);
extern int32_t  GLCD_SetForegroundColor  (uint32_t color);
extern int32_t  GLCD_SetBackgroundColor  (uint32_t color);
extern int32_t  GLCD_ClearScreen         (void);
extern int32_t  GLCD_SetFont             (GLCD_FONT *font);
extern int32_t  GLCD_DrawPixel           (uint32_t x, uint32_t y);
extern int32_t  GLCD_DrawHLine           (uint32_t x, uint32_t y, uint32_t length);
extern int32_t  GLCD_DrawVLine           (uint32_t x, uint32_t y, uint32_t length);
extern int32_t  GLCD_DrawRectangle       (uint32_t x, uint32_t y, uint32_t width, uint32_t height);
extern int32_t  GLCD_DrawChar            (uint32_t x, uint32_t y, int32_t  ch);
extern int32_t  GLCD_DrawString          (uint32_t x, uint32_t y, const char *str);
extern int32_t  GLCD_DrawBargraph        (uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t val);
extern int32_t  GLCD_DrawBitmap          (uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t *bitmap);
extern int32_t  GLCD_VScroll             (uint32_t dy);
extern int32_t  GLCD_FrameBufferAccess   (bool enable);
extern uint32_t GLCD_FrameBufferAddress  (void);

extern 
void GLCD_Print(uint32_t x, uint32_t y, const char *str);

/*
extern
arm_fsm_rt_t GLCD_Draw_Tile(const arm_2d_tile_t *ptTile,
                            const arm_2d_region_t *ptRegion);
extern 
int32_t GLCD_Draw_Vertical_Bargraph (   arm_2d_region_t *ptRegion, 
                                        uint_fast8_t chPattern, 
                                        const color_spectrum_t *ptSpectrum,
                                        uint_fast16_t hwVisibleHeight);
*/
#endif /* __BOARD_GLCD_H */
