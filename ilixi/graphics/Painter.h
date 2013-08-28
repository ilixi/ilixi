/*
 Copyright 2010-2013 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>.

 This file is part of ilixi.

 ilixi is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ilixi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with ilixi.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ILIXI_PAINTER_H_
#define ILIXI_PAINTER_H_

#include <ui/Widget.h>
#include <types/Brush.h>
#include <types/Pen.h>
#include <types/Font.h>
#include <types/Image.h>
#include <types/Affine2D.h>

namespace ilixi
{
class TextLayout;

//! Draws primitive shapes and renders text.
/*!
 * Painter paints on a widget's surface using its pen and brush. It is also used
 * for drawing text.
 *
 * For example, following code will draw a rectangle on widget's surface using default pen:
 * \code
 * void
 * MyWidget::compose(const PaintEvent& event)
 * {
 *    Painter painter(this);                // Painter should use this widget's surface.
 *    painter.begin(event);                 // Painter clips event rectangle and lock surface.
 *    painter.drawRectangle(0, 0, 10, 10);
 *    painter.end();                        // Painter resets clip and unlocks surface.
 * }
 * \endcode
 */
class Painter
{
public:
    /*!
     * Constructor creates a new painter instance for the given widget.
     * Upon creation default pen and font are applied to cairo and pango layouts
     * respectively.
     *
     * @param widget Painter will work on this widget's surface.
     * @param clipRect Clipping is applied to this rectangle.
     */
    Painter(Widget* widget);

    /*!
     * Destructor. Calls end() automatically.
     */
    ~Painter();

    /*!
     * Locks current widget's surface for drawing operations and blocks
     * consecutive calls to begin() until end() is called.
     *
     * You should call this method in order to serialise access to the widget's surface.
     */
    void
    begin(const PaintEvent& event);

    /*!
     * Unlocks current widget's surface.
     *
     * This method is automatically called upon destruction of the painter.
     */
    void
    end();

    /*!
     * Returns painter's current brush.
     */
    const Brush&
    brush() const;

    /*!
     * Returns painter's current font.
     */
    const Font&
    font() const;

    /*!
     * Returns painter's current pen.
     */
    const Pen&
    pen();

    /*!
     * Draws a line between two points defined by (x1, y1) and (x2, y2) using Pen.
     *
     * @param x1 X coordinate of start point.
     * @param y1 Y coordinate of start point.
     * @param x2 X coordinate of end point.
     * @param y2 Y coordinate of end point.
     */
    void
    drawLine(double x1, double y1, double x2, double y2, const DFBSurfaceDrawingFlags& flags = DSDRAW_NOFX);

    /*!
     * Draws a line between two given points p1 and p2 using Pen.
     *
     * @param p1 Start point.
     * @param p2 End point.
     */
    void
    drawLine(const Point& p1, const Point& p2, const DFBSurfaceDrawingFlags& flags = DSDRAW_NOFX);

    /*!
     * Draws a line inside the bounding rectangle using Pen.
     *
     * @param rect Bounding rectangle.
     */
    void
    drawLine(const Rectangle& rect, const DFBSurfaceDrawingFlags& flags = DSDRAW_NOFX);

    /*!
     * Draws a rectangle inside the bounding rectangle defined by top left point (x, y)
     * and (width, height) using Pen.
     *
     * @param x X coordinate of bounding rectangle.
     * @param y X coordinate of bounding rectangle.
     * @param width Width of bounding rectangle.
     * @param height Height of bounding rectangle.
     */
    void
    drawRectangle(double x, double y, double width, double height, const DFBSurfaceDrawingFlags& flags = DSDRAW_NOFX);

    /*!
     * Draws a rectangle inside the bounding rectangle using Pen.
     *
     * @param rect Bounding rectangle.
     */
    void
    drawRectangle(const Rectangle& rect, const DFBSurfaceDrawingFlags& flags = DSDRAW_NOFX);

    /*!
     * Fills inside the given rectangle using Brush.
     *
     * @param x
     * @param y
     * @param width
     * @param height
     */
    void
    fillRectangle(double x, double y, double width, double height, const DFBSurfaceDrawingFlags& flags = DSDRAW_NOFX);

    /*!
     * Fills inside the given rectangle using Brush.
     *
     * @param rect
     */
    void
    fillRectangle(const Rectangle& rect, const DFBSurfaceDrawingFlags& flags = DSDRAW_NOFX);

    /*!
     * Draws the text with its topleft positioned at given point (x, y) using Brush and Font.
     * By default all text will be rendered.
     * If you only wish to limit the number of characters to display, set the
     * singleLine parameter to false and use a positive number for length.
     *
     * Text is rendered using current pen color and font. \sa getFont(), getPen()
     *
     * For example the following code will render a simple message.
     * \code
     * painter.drawText("Hello World!", 0, 0);
     * \endcode
     *
     * @param text Text to draw.
     * @param x X coordinate of topleft point.
     * @param y X coordinate of topleft point.
     * @param length Maximum length of text.
     */
    void
    drawText(const std::string& text, int x, int y, const DFBSurfaceDrawingFlags& flags = DSDRAW_NOFX);

    /*!
     * Draws a layout using its bounding rectangle using Brush and Font.
     */
    void
    drawLayout(const TextLayout& layout, const DFBSurfaceDrawingFlags& flags = DSDRAW_NOFX);

    /*!
     * Draws a layout on given x and y coordinates using Brush and Font.
     */
    void
    drawLayout(const TextLayout& layout, int x, int y, const DFBSurfaceDrawingFlags& flags = DSDRAW_NOFX);

    /*!
     * Streches image inside given rectangle.
     * Brush color is used for colorizing.
     */
    void
    stretchImage(Image* image, int x, int y, int w, int h, const DFBSurfaceBlittingFlags& flags = DSBLIT_BLEND_ALPHACHANNEL);

    /*!
     * Streches image inside given rectangle.
     * Brush color is used for colorizing.
     */
    void
    stretchImage(Image* image, const Rectangle& destRect, const DFBSurfaceBlittingFlags& flags = DSBLIT_BLEND_ALPHACHANNEL);

    void
    stretchImage(Image* image, const Rectangle& destRect, const Rectangle& sourceRect, const DFBSurfaceBlittingFlags& flags = DSBLIT_BLEND_ALPHACHANNEL);

    /*!
     * Draws image with top-left corner at given x and y.
     * Brush color is used for colorizing.
     */
    void
    drawImage(Image* image, int x, int y, const DFBSurfaceBlittingFlags& flags = DSBLIT_BLEND_ALPHACHANNEL);

    /*!
     * Draws image with top-left corner at given x and y.
     * Brush color is used for colorizing.
     */
    void
    drawImage(Image* image, const Point& point, const DFBSurfaceBlittingFlags& flags = DSBLIT_BLEND_ALPHACHANNEL);

    void
    tileImage(Image* image, int x, int y, const DFBSurfaceBlittingFlags& flags = DSBLIT_BLEND_ALPHACHANNEL);

    void
    tileImage(Image* image, int x, int y, const Rectangle& source, const DFBSurfaceBlittingFlags& flags = DSBLIT_BLEND_ALPHACHANNEL);

    void
    blitImage(Image* image, const Rectangle& source, int x, int y, const DFBSurfaceBlittingFlags& flags = DSBLIT_BLEND_ALPHACHANNEL);

    void
    batchBlitImage(Image* image, const DFBRectangle* sourceRects, const DFBPoint* points, int num, const DFBSurfaceBlittingFlags& flags = DSBLIT_BLEND_ALPHACHANNEL);

    void
    batchBlitImage(Image* image, const Rectangle* sourceRects, const Point* points, int num, const DFBSurfaceBlittingFlags& flags = DSBLIT_BLEND_ALPHACHANNEL);
#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
    void
    batchStretchBlitImage(Image* image, const DFBRectangle* sourceRects, const DFBRectangle* destRects, int num, const DFBSurfaceBlittingFlags& flags = DSBLIT_BLEND_ALPHACHANNEL);

    void
    batchStretchBlitImage(Image* image, const Rectangle* sourceRects, const Rectangle* destRects, int num, const DFBSurfaceBlittingFlags& flags = DSBLIT_BLEND_ALPHACHANNEL);
#endif

    /*!
     * Sets the clip rectangle using given values.
     */
    void
    setClip(int x, int y, int w, int h);

    /*!
     * Sets the clip rectangle to given rectangle.
     */
    void
    setClip(const Rectangle& rect);

    /*!
     * Resets clip rectangle to NULL.
     */
    void
    resetClip();

    /*!
     * Sets painter's brush.
     *
     * Brush is used to fill inside the primitive shapes and text.
     *
     * @param brush Brush.
     */
    void
    setBrush(const Brush& brush);

    /*!
     * Sets the painter's brush to given color.
     */
    void
    setBrush(const Color& color);

    /*!
     * Sets painter's font.
     *
     * Font is used for rendering text.
     *
     * @param font
     */
    void
    setFont(const Font& font);

    /*!
     * Sets painter's pen.
     *
     * Pen is used to draw outline of primitive shapes.
     *
     * @param pen Pen.
     */
    void
    setPen(const Pen& pen);

    /*!
     * Sets the painter's pen to given color.
     */
    void
    setPen(const Color& color);

    /*!
     * Sets to transformation matrix.
     * @param affine2D
     */
    void
    setAffine2D(const Affine2D& affine2D = Affine2D());

    /*!
     * Returns dimension of text in pixels.
     *
     * An invalid size is returned if font can not be loaded.
     *
     * @param bytes number of bytes to use, -1 for whole string.
     */
    Size
    textExtents(const std::string& text, int bytes = -1);

private:
    enum PainterFlags
    {
        None = 0x000, //!< Initial state
        Active = 0x001, //!< Painter is activated by begin()
        BrushActive = 0x002,
        FontModified = 0x004,
        Clipped = 0x008,
        Transformed = 0x010
    };

    //! This property holds Painter's current widget.
    Widget* _myWidget;
    //! Underlying surface.
    IDirectFBSurface* dfbSurface;

    //! This is painter's current brush.
    Brush _brush;
    //! This is painter's current pen.
    Pen _pen;
    //! This is painter's current font.
    Font _font;
    //! Set using painter flags
    PainterFlags _state;

    Affine2D* _affine;

    //! Apply brush to context if it is modified.
    void
    applyBrush();

    //! Apply font to context if it is modified.
    void
    applyFont();

    //! Apply pen to content if it is modified.
    void
    applyPen();
};
}

#endif /* ILIXI_PAINTER_H_ */
