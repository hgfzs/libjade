/* DrawingEllipseItem.h
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

#ifndef DRAWINGELLIPSEITEM_H
#define DRAWINGELLIPSEITEM_H

#include <DrawingItem.h>

/*! \brief Provides an ellipse item that can be added to a DrawingScene.
 *
 * To set the item's ellipse, call the setEllipse() function.  The ellipse() function returns the
 * current ellipse.  Both functions operate in local item coordinates.
 *
 * DrawingEllipseItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the ellipse using the item's pen() and brush().
 */
class DrawingEllipseItem : public DrawingItem
{
private:
	enum PointIndex { TopLeft, BottomRight, TopRight, BottomLeft, TopMiddle, MiddleRight,
					  BottomMiddle, MiddleLeft };
private:
	QRectF mRect;
	QPen mPen;
	QBrush mBrush;

	QRectF mBoundingRect;
	QPainterPath mShape;

public:
	/*! \brief Create a new DrawingEllipseItem with default settings.
	 *
	 * This function creates eight DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the bounding points of the ellipse.
	 */
	DrawingEllipseItem();

	/*! \brief Create a new DrawingEllipseItem as a copy of an existing ellipse item.
	 *
	 * Creates copies of all item points to the new ellipse item, including the point's positions.
	 */
	DrawingEllipseItem(const DrawingEllipseItem& item);

	/*! \brief Delete an existing DrawingEllipseItem object.
	 *
	 * All of the item's points are also deleted.
	 */
	virtual ~DrawingEllipseItem();


	/*! \brief Creates a copy of the DrawingEllipseItem and return it.
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
	 * This convenience function is equivalent to calling
	 * \link setEllipse(const QRectF& rect) setEllipse(QRectF(left, top, width, height)) \endlink .
	 *
	 * \sa ellipse()
	 */
	void setEllipse(qreal left, qreal top, qreal width, qreal height);

	/*! \brief Returns the item's ellipse in local item coordinates.
	 *
	 * \sa setEllipse(const QRectF&), setEllipse(qreal, qreal, qreal, qreal)
	 */
	QRectF ellipse() const;


	/*! \brief Sets the pen used to draw the border of the ellipse.
	 *
	 * The pen's width is in local item coordinates.
	 *
	 * \sa pen()
	 */
	void setPen(const QPen& pen);

	/*! \brief Returns the pen used to draw the border of the ellipse.
	 *
	 * \sa setPen()
	 */
	QPen pen() const;


	/*! \brief Sets the brush used to fill the ellipse.
	 *
	 * \sa brush()
	 */
	void setBrush(const QBrush& brush);

	/*! \brief Returns the brush used to fill the ellipse.
	 *
	 * \sa setBrush()
	 */
	QBrush brush() const;


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
	 *     <td>brush-color</td>
	 *     <td>QColor</td>
	 *     <td>Color of the item's brush(), including alpha channel</td>
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


	/*! \brief Returns an estimate of the area painted by the ellipse item.
	 *
	 * Calculates the bounding rect of the ellipse based on the position of its points.
	 * The ellipse includes an adjustment for the width of the item's pen().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the ellipse based on the position of its points as well as its
	 * pen() and brush().
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Return false if the item is invalid, true otherwise.
	 *
	 * A ellipse item is invalid degenerate if the positions of all of its points
	 * are the same.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the ellipse item into the scene.
	 *
	 * The ellipse is painted in the scene using the item's pen() and brush().
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
	virtual void resize(DrawingItemPoint* point, const QPointF& pos);

private:
	void updateGeometry();
};

#endif
