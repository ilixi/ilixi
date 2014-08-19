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

#ifndef ILIXI_CAIROPAINTER_H_
#define ILIXI_CAIROPAINTER_H_

#include <graphics/Painter.h>

namespace ilixi
{
class EGLWidget;

//! Draws shapes using Cairo.
/*!
 * CairoPainter paints on a widget's surface using its pen and brush.
 *
 * For example, following code will draw an ellipse on widget's surface using default pen:
 * \code
 * void
 * MyWidget::compose(const PaintEvent& event)
 * {
 *    CairoPainter painter(this);           // Painter should use this widget's surface.
 *    painter.begin(event);                 // Painter clips event rectangle and lock surface.
 *    painter.drawEllipse(0, 0, 100, 100);
 *    painter.end();                        // Painter resets clip and unlocks surface.
 * }
 * \endcode
 */
class CairoPainter
{
public:
    /*!
     * This enum is provided for convenience. It simply maps from cairo's implementation.
     */
    enum AntiAliasMode
    {
        AliasDefault,   //!< Use the default antialiasing for the subsystem and target device
        AliasNone,      //!< Use a bilevel alpha mask
        AliasGray,      //!< Perform single-color antialiasing
        AliasSubPixel   //!< Perform antialiasing by taking advantage of the order of subpixel elements on devices such as LCD panels
    };

    /*!
     * This enum is used to control behavior of drawing methods, e.g. drawRectangle.
     */
    enum DrawingMode
    {
        StrokePath,     //!< Outline shape using pen.
        StrokeAndFill,  //!< Outline shape using pen and fill inside using brush.
        FillPath,       //!< Fill inside shape using brush.
        ClipPath,       //!< Create a clipping path.
        AddPath         //!< Add path to context.
    };

    /*!
     * Constructor.
     */
    CairoPainter(Widget* widget);

    /*!
     * Destructor.
     */
    virtual
    ~CairoPainter();

    /*!
     * Locks current widget's surface for drawing operations and blocks
     * consecutive calls to begin() until end() is called. You should
     * call this method in order to serialise access to the widget's surface.
     */
    void
    begin(const PaintEvent& event);

    /*!
     * Unlocks current widget's surface. This method is automatically
     * called upon destruction of the painter.
     */
    void
    end();

#ifdef ILIXI_HAVE_CAIROGLES
    void
    beginGL(EGLWidget* widget, const PaintEvent& event);

    void
    endGL();
#endif

    /*!
     * Sets painter's brush.
     *
     * Brush is used to fill inside shapes and text.
     *
     * @param brush Brush.
     */
    void
    setBrush(const Brush& brush);

    /*!
     * Sets painter's pen.
     *
     * Pen is used to draw outline of shapes.
     *
     * @param pen Pen.
     */
    void
    setPen(const Pen& pen);

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
     * Returns widget's cairo context.
     */
    cairo_t*
    cairoContext() const;

    /*!
     * Returns current anti-aliasing setting.
     */
    AntiAliasMode
    getAntiAliasMode() const;

    /*!
     * Draws an arc inside the bounding rectangle defined by the top left point (x, y)
     * and dimensions (width, height) between start and end angles using current pen.
     *
     * Angles are measured in degrees. An angle of 0 is in the direction of the positive X axis (right).
     * An angle of 90 degrees is in the direction of the positive Y axis (down). Angles increase in
     * the direction from the positive X axis toward the positive Y axis (Clockwise).
     *
     * The following code draws a semi-circle.
     * \code
     * painter.drawArc(0, 0, 100, 100, 180, 360);
     * \endcode
     *
     * @param x X coordinate of bounding rectangle.
     * @param y X coordinate of bounding rectangle.
     * @param width Width of bounding rectangle.
     * @param height Height of bounding rectangle.
     * @param angleStart Start angle in degrees.
     * @param angleEnd End angle in degrees.
     */
    void
    drawArc(double x, double y, double width, double height, double angleStart, double angleEnd);

    /*!
     * Draws an arc inside the bounding rectangle between start and end angles using current pen.
     *
     * \sa drawArc(double x, double y, double width, double height, double angle1, double angle2);
     *
     * @param rect Bounding rectangle.
     * @param angleStart Start angle in degrees.
     * @param angleEnd End angle in degrees.
     */
    void
    drawArc(const Rectangle& rect, double angleStart, double angleEnd);

    /*!
     * Draws a chord inside the bounding rectangle defined by top left point (x, y)
     * and dimensions (width, height) between start and end angles using current pen.
     *
     * The following code draws a closed semi-circle with current pen
     * and fills it with current brush.
     * \code
     * painter.getPen()->setColor(Color(0, 0, 0));
     * painter.getBrush()->setColor(Color(0, 1, 0));
     * painter.getBrush()->setBrushMode(Brush::SolidColorMode);
     * painter.drawChord(0, 0, 100, 100, 180, 360, Painter::StrokeAndFill);
     * \endcode
     *
     * @param x X coordinate of bounding rectangle.
     * @param y X coordinate of bounding rectangle.
     * @param width Width of bounding rectangle.
     * @param height Height of bounding rectangle.
     * @param angleStart Start angle in degrees.
     * @param angleEnd End angle in degrees.
     * @param mode Set painter's drawing mode.
     */
    void
    drawChord(double x, double y, double width, double height, double angleStart, double angleEnd, DrawingMode mode = StrokePath);

    /*!
     * Draws a chord inside the bounding rectangle between start and end angles using current pen.
     *
     * @param rect Bounding rectangle.
     * @param angleStart Start angle in degrees.
     * @param angleEnd End angle in degrees.
     * @param mode Set painter's drawing mode.
     */
    void
    drawChord(const Rectangle& rect, double angleStart, double angleEnd, DrawingMode mode = StrokePath);

    /*!
     * Draws an ellipse inside the bounding rectangle defined by top left point (x, y)
     * and dimensions (width, height).
     *
     * The following code will draw an ellipse.
     * \code
     * painter.getPen()->setColor(Color(0, 0, 0));
     * painter.getBrush()->setColor(Color(0, 1, 0));
     * painter.getBrush()->setBrushMode(Brush::SolidColorMode);
     * painter.drawEllipse(0, 0, 100, 100, Painter::StrokeAndFill);
     * \endcode
     *
     * @param x X coordinate of bounding rectangle.
     * @param y X coordinate of bounding rectangle.
     * @param width Width of bounding rectangle.
     * @param height Height of bounding rectangle.
     * @param mode Set painter's drawing mode.
     */
    void
    drawEllipse(double x, double y, double width, double height, DrawingMode mode = StrokePath);

    /*!
     * Draws an ellipse inside the bounding rectangle.
     *
     * @param rect Bounding rectangle.
     * @param mode Set painter's drawing mode.
     */
    void
    drawEllipse(const Rectangle& rect, DrawingMode mode = StrokePath);

    /*!
     * Draws a line between two points defined by (x1, y1) and (x2, y2).
     * By default AntiAliasing is On and this will effect the appearance of lines.
     *
     * The following code segment will draw a single pixel line.
     * \code
     * painter.getPen()->setLineWidth(1);
     * painter.drawLine(0, 0, 100, 100);
     * \endcode
     *
     * @param x1 X coordinate of start point.
     * @param y1 Y coordinate of start point.
     * @param x2 X coordinate of end point.
     * @param y2 Y coordinate of end point.
     * @param mode Set painter's drawing mode.
     */
    void
    drawLine(double x1, double y1, double x2, double y2, DrawingMode mode = StrokePath);

    /*!
     * Draws a line between two given points p1 and p2.
     *
     * @param p1 Start point.
     * @param p2 End point.
     * @param mode Set painter's drawing mode.
     */
    void
    drawLine(const Point& p1, const Point& p2, DrawingMode mode = StrokePath);

    /*!
     * Draws a line inside the bounding rectangle.
     *
     * @param rect Bounding rectangle.
     * @param mode Set painter's drawing mode.
     */
    void
    drawLine(const Rectangle& rect, DrawingMode mode = StrokePath);

    /*!
     * Draws a closed polygon using given points.
     *
     * The following code draws a triangle.
     * \code
     * Point points[3] = { Point(100, 10), Point(200, 200), Point(10, 200) };
     * painter.drawPolygon(points, 3);
     * \endcode
     *
     * @param points Array of points.
     * @param pointCount Number of points in the array.
     * @param mode Set painter's drawing mode.
     */
    void
    drawPolygon(const Point* points, int pointCount, DrawingMode mode = StrokePath);

    /*!
     * Draws a rectangle inside the bounding rectangle defined by top left point (x, y)
     * and (width, height).
     *
     * The following code will draw a filled rectangle.
     * \code
     * painter.getPen()->setColor(Color(0, 0, 0));
     * painter.getBrush()->setColor(Color(0, 1, 0));
     * painter.getBrush()->setBrushMode(Brush::SolidColorMode);
     * painter.drawRectangle(0, 0, 100, 100, Painter::StrokeAndFill);
     * \endcode
     *
     * @param x X coordinate of bounding rectangle.
     * @param y X coordinate of bounding rectangle.
     * @param width Width of bounding rectangle.
     * @param height Height of bounding rectangle.
     * @param mode Set painter's drawing mode.
     */
    void
    drawRectangle(double x, double y, double width, double height, DrawingMode mode = StrokePath);

    /*!
     * Draws a rectangle inside the bounding rectangle.
     *
     * @param rect Bounding rectangle.
     * @param mode Set painter's drawing mode.
     */
    void
    drawRectangle(const Rectangle& rect, DrawingMode mode = StrokePath);

    /*!
     * Draws a rectangle inside the bounding rectangle defined by top left point (x, y)
     * and dimensions (width, height).
     *
     * The following code will draw a filled rounded rectangle with a radius of 5px.
     * \code
     * painter.getPen()->setColor(Color(0, 0, 0));
     * painter.getBrush()->setColor(Color(0, 1, 0));
     * painter.getBrush()->setBrushMode(Brush::SolidColorMode);
     * painter.drawRoundRectangle(0, 0, 100, 100, 5, Painter::StrokeAndFill);
     * \endcode
     *
     * @param x X coordinate of bounding rectangle.
     * @param y X coordinate of bounding rectangle.
     * @param width Width of bounding rectangle.
     * @param height Height of bounding rectangle.
     * @param radius Corner radius.
     * @param mode Set painter's drawing mode.
     * @param Corners specifies which corners should be rounded.
     */
    void
    drawRoundRectangle(double x, double y, double width, double height, int radius, DrawingMode mode = StrokePath, Corners corners = AllCorners);

    /*!
     * Draws a rounded rectangle inside the bounding rectangle.
     *
     * @param rect Bounding rectangle.
     * @param radius Corner radius.
     * @param mode Set painter's drawing mode.
     * @param Corners specifies which corners should be rounded.
     */
    void
    drawRoundRectangle(const Rectangle& rect, int radius, DrawingMode mode = StrokePath, Corners corners = AllCorners);

    /*!
     * Draws a single point at given coordinates.
     */
    void
    drawPoint(double x, double y);

    /*!
     * Returns text extents as calculated by cairo.
     */
    Rectangle
    textExtents(const std::string& text);

    /*!
     * Draws text using current brush and font at given coordinates.
     */
    void
    drawText(const std::string& text, int x = 0, int y = 0);

    /*!
     * Draws a layout using its bounding rectangle using Brush and Font.
     */
    void
    drawLayout(const TextLayout& layout, int x = 0, int y = 0);

    /*!
     * Draws current path using drawing mode.
     */
    void
    drawCurrentPath(DrawingMode mode = StrokePath);

    /*!
     * Sets antialiasing mode of the cairo rasterizer.
     *
     * @param mode
     */
    void
    setAntiAliasMode(AntiAliasMode mode);

private:
    //! This property holds Painter's current widget.
    Widget* _myWidget;
    //! This property holds current anti aliasing mode.
    AntiAliasMode _antiAliasMode;
    //! Pointer to _surface's cairo context.
    cairo_t* _context;

    //! This is painter's current brush.
    Brush _brush;
    //! This is painter's current pen.
    Pen _pen;
    //! This is painter's current font.
    Font _font;

    enum PainterFlags
    {
        PFNone = 0x000,           //!< Initial state
        PFActive = 0x001,         //!< Painter is activated by begin()
        PFBrushActive = 0x002,
        PFFontModified = 0x004
    };

    //! Set using painter flags
    PainterFlags _state;

    //! Apply brush to context if it is modified.
    void
    applyCairoBrush();

    //! Apply pen to context if it is modified.
    void
    applyCairoPen();

    //! Set font for context.
    void
    applyCairoFont();

    //! Applies brush and/or pen accordingly.
    void
    applyDrawingMode(DrawingMode mode);

    //! Transform coordinates so that cairo's stroke operation fills only opaque pixels.
    void
    getUserCoordinates(double &x, double &y, double &width, double &height);
};

} /* namespace ilixi */
#endif /* ILIXI_CAIROPAINTER_H_ */
