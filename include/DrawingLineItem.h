/* DrawingLineItem.h
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

#ifndef DRAWINGLINEITEM_H
#define DRAWINGLINEITEM_H

#include <DrawingItem.h>

/*! \brief Provides a line item that can be added to a DrawingScene.
 *
 * To set the item's line, call the setLine() function.  The line() function returns the current
 * line.  Both functions operate in local item coordinates.
 *
 * Rendering options for the line can be controlled through properties of the item's style().
 * The line item supports all of the pen style properties as well as start arrow and end arrow
 * properties.
 *
 * DrawingLineItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the line using the item's associated pen.
 */
class DrawingLineItem : public DrawingItem
{
public:
	/*! \brief Create a new DrawingLineItem with default settings.
	 *
	 * This function creates three DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the start, middle, and end points of the line.  By default, the
	 * position of each point is set to the origin of the item.
	 *
	 * This function fills in the item's style() with default values for the following properties.
	 * The default values are pulled from the style's DrawingItemStyle::defaultValues() if present,
	 * otherwise DrawingLineItem attempts to use reasonable initial values for each property:
	 * \li DrawingItemStyle::PenStyle
	 * \li DrawingItemStyle::PenColor
	 * \li DrawingItemStyle::PenOpacity
	 * \li DrawingItemStyle::PenWidth
	 * \li DrawingItemStyle::PenCapStyle
	 * \li DrawingItemStyle::PenJoinStyle
	 * \li DrawingItemStyle::StartArrowStyle
	 * \li DrawingItemStyle::StartArrowSize
	 * \li DrawingItemStyle::EndArrowStyle
	 * \li DrawingItemStyle::EndArrowSize
	 */
	DrawingLineItem();

	/*! \brief Create a new DrawingLineItem as a copy of an existing line item.
	 *
	 * Creates copies of all item points to the new line item, including the point's positions.
	 * Also creates a new item style with all of the same properties as the existing item's style.
	 */
	DrawingLineItem(const DrawingLineItem& item);

	/*! \brief Delete an existing DrawingLineItem object.
	 *
	 * All of the item's points are also deleted.
	 */
	virtual ~DrawingLineItem();


	/*! \brief Creates a copy of the DrawingLineItem and return it.
	 *
	 * Simply calls the copy constructor.
	 */
	virtual DrawingItem* copy() const;


	/*! \brief Sets the item's line to line, which is given in local item coordinates.
	 *
	 * Sets the positions of the item's points so that:
	 * \li point[0] is at start point of the line (line.p1())
	 * \li point[1] is at the midpoint of the line
	 * \li point[2] is at end point of the line (line.p2())
	 *
	 * \sa line()
	 */
	void setLine(const QLineF& line);

	/*! \brief Sets the item's line, which is given in local item coordinates.
	 *
	 * This convenience function is equivalent to calling setLine(QLineF(x1, y1, x2, y2)).
	 *
	 * \sa line()
	 */
	void setLine(qreal x1, qreal y1, qreal x2, qreal y2);

	/*! \brief Returns item's line in local item coordinates.
	 *
	 * \sa setLine(const QLineF&), setLine(qreal, qreal, qreal, qreal)
	 */
	QLineF line() const;


	/*! \brief Returns an estimate of the area painted by the line item.
	 *
	 * Calculates the bounding rect of the line based on the position of its start and end points.
	 * The rect includes an adjustment for the width of the pen as set by the item's style().
	 * The rect does not include any consideration of any arrows that may be set by the style().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the line, including any arrows that may be set by the item's
	 * style().
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Return false if the item is degenerate, true otherwise.
	 *
	 * A line item is considered degenerate if the positions of its start and end points
	 * are the same.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the line item into the scene.
	 *
	 * The line is painted in the scene based on properties set by the item's style(), including
	 * any arrows set by the style.
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void render(QPainter* painter);


	/*! \brief Resizes the item within the scene.
	 *
	 * This function ensures that whenever the item is resized, the item's midpoint is also moved
	 * so that it remains at the center of the line.
	 */
	virtual void resize(DrawingItemPoint* itemPoint, const QPointF& parentPos);
};

#endif
