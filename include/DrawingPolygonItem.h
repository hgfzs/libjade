/* DrawingPolygonItem.h
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

#ifndef DRAWINGPOLYGONITEM_H
#define DRAWINGPOLYGONITEM_H

#include <DrawingItem.h>

/*! \brief Provides a polygon item that can be added to a DrawingScene.
 *
 * To set the item's polygon, call the setPolygon() function.  The polygon() function returns the
 * current polygon.  Both functions operate in local item coordinates.
 *
 * DrawingPolygonItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the polygon using the item's pen() and brush().
 */
class DrawingPolygonItem : public DrawingItem
{
private:
	QPolygonF mPolygon;
	QPen mPen;
	QBrush mBrush;

	QRectF mBoundingRect;
	QPainterPath mShape;

public:
	/*! \brief Create a new DrawingPolygonItem with default settings.
	 *
	 * This function creates three DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the initial points of the polygon.
	 */
	DrawingPolygonItem();

	/*! \brief Create a new DrawingPolygonItem as a copy of an existing polygon item.
	 *
	 * Creates copies of all item points to the new polygon item, including the points' positions.
	 */
	DrawingPolygonItem(const DrawingPolygonItem& item);

	/*! \brief Delete an existing DrawingPolygonItem object.
	 *
	 * All of the item's points are also deleted.
	 */
	virtual ~DrawingPolygonItem();


	/*! \brief Creates a copy of the DrawingPolygonItem and return it.
	 *
	 * Simply calls the copy constructor.
	 */
	virtual DrawingItem* copy() const;


	/*! \brief Sets the item's polygon to polygon, which is given in local item coordinates.
	 *
	 * Adds a DrawingItemPoint to the polygon representing each point in the specified polygon.
	 * The position of each DrawingItemPoint is set to match the point of the polygon.
	 *
	 * \sa polygon()
	 */
	void setPolygon(const QPolygonF& polygon);

	/*! \brief Returns item's polygon in local item coordinates.
	 *
	 * \sa setPolygon()
	 */
	QPolygonF polygon() const;


	/*! \brief Sets the pen used to draw the border of the polygon.
	 *
	 * The pen's width is in local item coordinates.
	 *
	 * \sa pen()
	 */
	void setPen(const QPen& pen);

	/*! \brief Returns the pen used to draw the border of the polygon.
	 *
	 * \sa setPen()
	 */
	QPen pen() const;


	/*! \brief Sets the brush used to fill the polygon.
	 *
	 * \sa brush()
	 */
	void setBrush(const QBrush& brush);

	/*! \brief Returns the brush used to fill the polygon.
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


	/*! \brief Returns an estimate of the area painted by the polygon item.
	 *
	 * Calculates the bounding rect of the polygon based on the position of its points.
	 * The rect includes an adjustment for the width of the item's pen().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the polygon based on the position of its points as well as its
	 * pen() and brush().
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Return false if the item is invalid, true otherwise.
	 *
	 * A polygon item is considered invalid if the positions of all of its points
	 * are the same.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the polygon item into the scene.
	 *
	 * The polygon is painted in the scene using the item's pen() and brush().
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void render(QPainter* painter);


	/*! \brief Resizes the item within the scene.
	 *
	 * This function is only used for internal caching and doesn't add any new behavior.
	 */
	virtual void resize(DrawingItemPoint* point, const QPointF& pos);


	/*! \brief Creates a new DrawingItemPoint to be inserted in the item and determines the
	 * appropriate location in the item's point list to insert the new point.
	 *
	 * The position of the new point is determined by pos.  The flags of the new point are:
	 * DrawingItemPoint::Control | DrawingItemPoint::Connection.
	 *
	 * \sa pointToRemove()
	 */
	virtual DrawingItemPoint* pointToInsert(const QPointF& pos, int& index);

	/*! \brief Returns an existing DrawingItemPoint to be removed from the item at the specified
	 * position.
	 *
	 * This function removes the DrawingItemPoint nearest to pos.  Note that a polygon must
	 * always have a minimum of three points; if the item only has three points, this function
	 * returns nullptr.
	 *
	 * \sa pointToInsert()
	 */
	virtual DrawingItemPoint* pointToRemove(const QPointF& pos);

private:
	void updateGeometry();

	qreal distanceFromPointToLineSegment(const QPointF& point, const QLineF& line) const;
};

#endif
