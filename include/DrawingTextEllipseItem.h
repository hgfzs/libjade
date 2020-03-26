/* DrawingTextEllipseItem.h
 *
 * Copyright (C) 2013-2017 Jason Allen
 *
 * This file is part of the jade application.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef DRAWINGTEXTELLIPSEITEM_H
#define DRAWINGTEXTELLIPSEITEM_H

#include <DrawingItem.h>

/*! \brief Provides a text ellipse item that can be added to a DrawingScene.
 *
 * To set the item's ellipse, call the setEllipse() function.  The ellipse() function returns the
 * current ellipse.  Both functions operate in local item coordinates.
 *
 * To set the item's text, call the setCaption() function.  The caption() function returns the
 * current text.

 * Rendering options for the text ellipse can be controlled through properties of the item's style().
 * The text ellipse item supports all of the pen, brush, font, and text brush style properties.
 *
 * DrawingTextEllipseItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the text ellipse using the item's style properties.
 */
class DrawingTextEllipseItem : public DrawingItem
{
private:
	enum PointIndex { TopLeft, BottomRight, TopRight, BottomLeft, TopMiddle, MiddleRight, BottomMiddle, MiddleLeft };

	QString mCaption;

public:
	/*! \brief Create a new DrawingTextEllipseItem with default settings.
	 *
	 * This function creates eight DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the bounding points of the text ellipse.
	 *
	 * This function fills in the item's style() with default values for the following properties.
	 * The default values are pulled from the style's DrawingItemStyle::defaultValues() if present,
	 * otherwise DrawingTextEllipseItem attempts to use reasonable initial values for each property:
	 * \li DrawingItemStyle::PenStyle
	 * \li DrawingItemStyle::PenColor
	 * \li DrawingItemStyle::PenOpacity
	 * \li DrawingItemStyle::PenWidth
	 * \li DrawingItemStyle::PenCapStyle
	 * \li DrawingItemStyle::PenJoinStyle
	 * \li DrawingItemStyle::BrushStyle
	 * \li DrawingItemStyle::BrushColor
	 * \li DrawingItemStyle::BrushOpacity
	 * \li DrawingItemStyle::TextColor
	 * \li DrawingItemStyle::TextOpacity
	 * \li DrawingItemStyle::FontName
	 * \li DrawingItemStyle::FontSize
	 * \li DrawingItemStyle::FontBold
	 * \li DrawingItemStyle::FontItalic
	 * \li DrawingItemStyle::FontUnderline
	 * \li DrawingItemStyle::FontOverline
	 * \li DrawingItemStyle::FontStrikeThrough
	 */
	DrawingTextEllipseItem();

	/*! \brief Create a new DrawingTextEllipseItem as a copy of an existing text ellipse item.
	 *
	 * Creates copies of all item points to the new text ellipse item, including the point's positions.
	 * Also creates a new item style with all of the same properties as the existing item's style.
	 */
	DrawingTextEllipseItem(const DrawingTextEllipseItem& item);

	/*! \brief Delete an existing DrawingTextEllipseItem object.
	 *
	 * All of the item's points are also deleted.
	 */
	virtual ~DrawingTextEllipseItem();


	/*! \brief Creates a copy of the DrawingTextEllipseItem and return it.
	 *
	 * Simply calls the copy constructor.
	 */
	virtual DrawingItem* copy() const;


	/*! \brief Sets the item's ellipse to rect, which is given in local item coordinates.
	 *
	 * \sa ellipse()
	 */
	void setEllipse(const QRectF& rect);

	/*! \brief Sets the item's ellipse, which is given in local item coordinates.
	 *
	 * This convenience function is equivalent to calling setRect(QRectF(left, top, width, height)).
	 *
	 * \sa ellipse()
	 */
	void setEllipse(qreal left, qreal top, qreal width, qreal height);

	/*! \brief Returns the item's ellipse in local item coordinates.
	 *
	 * \sa setEllipse(const QRectF&), setEllipse(qreal, qreal, qreal, qreal)
	 */
	QRectF ellipse() const;


	/*! \brief Sets the item's text to caption.
	 *
	 * \sa caption()
	 */
	void setCaption(const QString& caption);

	/*! \brief Returns the item's text.
	 *
	 * \sa setCaption()
	 */
	QString caption() const;


	/*! \brief Returns an estimate of the area painted by the text ellipse item.
	 *
	 * Calculates the bounding rect of the text ellipse based on the position of its points and the
	 * size of the item's text.  The rect includes an adjustment for the width of the pen as set
	 * by the item's style().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the text ellipse based on the position of its points and the size of the
	 * item's text.
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Return false if the item is degenerate, true otherwise.
	 *
	 * A text ellipse item is considered degenerate if the positions of all of its points
	 * are the same and the item's caption is empty.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the text ellipse item into the scene.
	 *
	 * The text ellipse is painted in the scene based on properties set by the item's style().
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void render(QPainter* painter);


	/*! \brief Resizes the item within the scene.
	 *
	 * This function ensures that whenever the item is resized, all of the item's points are resized
	 * to maintain position on the item's perimeter.
	 */
	virtual void resize(DrawingItemPoint* itemPoint, const QPointF& parentPos);

private:
	QRectF calculateTextRect(const QString& caption, const QFont& font) const;
};

#endif
