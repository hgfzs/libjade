/* DrawingPolylineItem.h
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

#ifndef DRAWINGPOLYLINEITEM_H
#define DRAWINGPOLYLINEITEM_H

#include <DrawingItem.h>
#include <DrawingArrow.h>

/*! \brief Provides a polyline item that can be added to a DrawingScene.
 *
 * To set the item's polyline, call the setPolyline() function.  The polyline() function returns the
 * current polyline.  Both functions operate in local item coordinates.
 *
 * DrawingPolylineItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the polyline using the item's pen().
 */
class DrawingPolylineItem : public DrawingItem
{
private:
	QPolygonF mPolyline;
	QPen mPen;
	DrawingArrow mStartArrow;
	DrawingArrow mEndArrow;

	QRectF mBoundingRect;
	QPainterPath mShape;
	QPainterPath mStrokePath;

public:
	/*! \brief Create a new DrawingPolylineItem with default settings.
	 *
	 * This function creates two DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the start and end points of the polyline.  By default, the
	 * position of each point is set to the origin of the item.
	 */
	DrawingPolylineItem();

	/*! \brief Create a new DrawingPolylineItem as a copy of an existing polyline item.
	 *
	 * Creates copies of all item points to the new polyline item, including the points' positions.
	 */
	DrawingPolylineItem(const DrawingPolylineItem& item);

	/*! \brief Delete an existing DrawingPolylineItem object.
	 *
	 * All of the item's points are also deleted.
	 */
	virtual ~DrawingPolylineItem();


	/*! \brief Creates a copy of the DrawingPolylineItem and return it.
	 *
	 * Simply calls the copy constructor.
	 */
	virtual DrawingItem* copy() const;


	/*! \brief Sets the item's polyline to polygon, which is given in local item coordinates.
	 *
	 * Adds a DrawingItemPoint to the polyline representing each point in the specified polygon.
	 * The position of each DrawingItemPoint is set to match the point of the polygon.
	 *
	 * \sa polyline()
	 */
	void setPolyline(const QPolygonF& polygon);

	/*! \brief Returns the item's polyline in local item coordinates.
	 *
	 * \sa setPolyline()
	 */
	QPolygonF polyline() const;


	/*! \brief Sets the pen used to draw the polyline.
	 *
	 * The pen's width is in local item coordinates.
	 *
	 * \sa pen()
	 */
	void setPen(const QPen& pen);

	/*! \brief Returns the pen used to draw the polyline.
	 *
	 * \sa setPen()
	 */
	QPen pen() const;


	/*! \brief Sets the arrow drawn at the start point of the polyline.
	 *
	 * The arrow's size is in local item coordinates.
	 *
	 * \sa setEndArrow(), startArrow()
	 */
	void setStartArrow(const DrawingArrow& arrow);

	/*! \brief Sets the arrow drawn at the end point of the polyline.
	 *
	 * The arrow's size is in local item coordinates.
	 *
	 * \sa setStartArrow(), endArrow()
	 */
	void setEndArrow(const DrawingArrow& arrow);

	/*! \brief Returns the arrow drawn at the start point of the polyline.
	 *
	 * \sa setStartArrow(), endArrow()
	 */
	DrawingArrow startArrow() const;

	/*! \brief Returns the arrow drawn at the end point of the polyline.
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


	/*! \brief Returns an estimate of the area painted by the polyline item.
	 *
	 * Calculates the bounding rect of the polyline based on the position of its start and end points.
	 * The rect includes an adjustment for the width of the item's pen(). The rect does not
	 * include any consideration of the item's startArrow() or endArrow().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the polyline based on the position of its points as well as its
	 * pen(), startArrow(), and endArrow().
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Return false if the item is invalid, true otherwise.
	 *
	 * A polyline item is considered invalid if the positions of all of its points
	 * are the same.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the polyline item into the scene.
	 *
	 * The polyline is painted in the scene using the item's pen().  The item's startArrow() and
	 * endArrow() are drawn as well.
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
	 * DrawingItemPoint::Control.
	 *
	 * \sa pointToRemove()
	 */
	virtual DrawingItemPoint* pointToInsert(const QPointF& pos, int& index);

	/*! \brief Returns an existing DrawingItemPoint to be removed from the item at the specified
	 * position.
	 *
	 * This function removes the DrawingItemPoint nearest to pos, as long as it is not the first or
	 * last point of the polyline.  Note that a polyline must always have a minimum of two points; if
	 * the item only has two points, this function returns nullptr.
	 *
	 * \sa pointToInsert()
	 */
	virtual DrawingItemPoint* pointToRemove(const QPointF& pos);

private:
	void updateGeometry();

	qreal distanceFromPointToLineSegment(const QPointF& point, const QLineF& line) const;
};

#endif
