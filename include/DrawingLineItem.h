/* DrawingLineItem.h
 *
 * Copyright (C) 2013-2020 Jason Allen
 *
 * This file is part of the libjade library.
 *
 * libjade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libjade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libjade.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef DRAWINGLINEITEM_H
#define DRAWINGLINEITEM_H

#include <DrawingItem.h>
#include <DrawingArrow.h>
#include <QLineF>

/*! \brief Provides a line item that can be added to a DrawingScene.
 *
 * To set the item's line, call the setLine() function.  The line() function returns the current
 * line.  Both functions operate in local item coordinates.
 *
 * DrawingLineItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the line using the item's pen().
 */
class DrawingLineItem : public DrawingItem
{
private:
	QLineF mLine;
	QPen mPen;
	DrawingArrow mStartArrow;
	DrawingArrow mEndArrow;

	QRectF mBoundingRect;
	QPainterPath mShape;

public:
	/*! \brief Create a new DrawingLineItem with default settings.
	 *
	 * This function creates three DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the start, middle, and end points of the line.  By default, the
	 * position of each point is set to the origin of the item.
	 */
	DrawingLineItem();

	/*! \brief Create a new DrawingLineItem as a copy of an existing line item.
	 *
	 * Creates copies of all item points to the new line item, including the points' positions.
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
	 * This convenience function is equivalent to calling
	 * \link setLine(const QLineF& line) setLine(QLineF(x1, y1, x2, y2)) \endlink.
	 *
	 * \sa line()
	 */
	void setLine(qreal x1, qreal y1, qreal x2, qreal y2);

	/*! \brief Returns item's line in local item coordinates.
	 *
	 * \sa setLine(const QLineF&), setLine(qreal, qreal, qreal, qreal)
	 */
	QLineF line() const;


	/*! \brief Sets the pen used to draw the line.
	 *
	 * The pen's width is in local item coordinates.
	 *
	 * \sa pen()
	 */
	void setPen(const QPen& pen);

	/*! \brief Returns the pen used to draw the line.
	 *
	 * \sa setPen()
	 */
	QPen pen() const;


	/*! \brief Sets the arrow drawn at the start point of the line.
	 *
	 * The arrow's size is in local item coordinates.
	 *
	 * \sa setEndArrow(), startArrow()
	 */
	void setStartArrow(const DrawingArrow& arrow);

	/*! \brief Sets the arrow drawn at the end point of the line.
	 *
	 * The arrow's size is in local item coordinates.
	 *
	 * \sa setStartArrow(), endArrow()
	 */
	void setEndArrow(const DrawingArrow& arrow);

	/*! \brief Returns the arrow drawn at the start point of the line.
	 *
	 * \sa setStartArrow(), endArrow()
	 */
	DrawingArrow startArrow() const;

	/*! \brief Returns the arrow drawn at the end point of the line.
	 *
	 * \sa setEndArrow(), startArrow()
	 */
	DrawingArrow endArrow() const;


	/*! \brief Sets the values of all item properties.
	 *
	 * The supported properties are listed below:
	 *
	 * <table>
	 *   <tr>
	 *     <th>Name</th>
	 *     <th>Type</th>
	 *     <th>Description</th>
	 *   </tr>
	 *   <tr>
	 *     <td>pen-color</td>
	 *     <td>QColor</td>
	 *     <td>Color of the item's pen(), including alpha channel</td>
	 *   </tr>
	 *   <tr>
	 *     <td>pen-width</td>
	 *     <td>qreal</td>
	 *     <td>Width of the item's pen()</td>
	 *   </tr>
	 *   <tr>
	 *     <td>pen-style</td>
	 *     <td>unsigned int</td>
	 *     <td>Style of the item's pen(), casted from Qt::PenStyle</td>
	 *   </tr>
	 *   <tr>
	 *     <td>pen-cap-style</td>
	 *     <td>unsigned int</td>
	 *     <td>Cap style of the item's pen(), casted from Qt::PenCapStyle</td>
	 *   </tr>
	 *   <tr>
	 *     <td>pen-join-style</td>
	 *     <td>unsigned int</td>
	 *     <td>Join style of the item's pen(), casted from Qt::PenJoinStyle</td>
	 *   </tr>
	 *   <tr>
	 *     <td>start-arrow-style</td>
	 *     <td>unsigned int</td>
	 *     <td>Style of the item's startArrow(), casted from DrawingArrow::Style</td>
	 *   </tr>
	 *   <tr>
	 *     <td>start-arrow-size</td>
	 *     <td>qreal</td>
	 *     <td>Size of the item's startArrow()</td>
	 *   </tr>
	 *   <tr>
	 *     <td>end-arrow-style</td>
	 *     <td>unsigned int</td>
	 *     <td>Style of the item's endArrow(), casted from DrawingArrow::Style</td>
	 *   </tr>
	 *   <tr>
	 *     <td>end-arrow-size</td>
	 *     <td>qreal</td>
	 *     <td>Size of the item's endArrow()</td>
	 *   </tr>
	 * </table>
	 *
	 * \sa properties()
	 */
	void setProperties(const QHash<QString,QVariant>& properties);

	/*! \brief Returns the values of all item properties.
	 *
	 * See the setProperties() function for the list of properties supported.
	 */
	QHash<QString,QVariant> properties() const;


	/*! \brief Returns an estimate of the area painted by the line item.
	 *
	 * Calculates the bounding rect of the line based on the position of its start and end points.
	 * The rect includes an adjustment for the width of the item's pen(). The rect does not
	 * include any consideration of the item's startArrow() or endArrow().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the line based on the position of its points as well as its
	 * pen(), startArrow(), and endArrow().
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Return false if the item is invalid, true otherwise.
	 *
	 * A line item is considered invalid if the positions of its start and end points
	 * are the same.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the line item into the scene.
	 *
	 * The line is painted in the scene using the item's pen().  The item's startArrow() and
	 * endArrow() are drawn as well.
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
	virtual void resize(DrawingItemPoint* point, const QPointF& pos);

private:
	void updateGeometry();
};

#endif
