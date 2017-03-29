/* DrawingPolylineItem.h
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

#ifndef DRAWINGPOLYLINEITEM_H
#define DRAWINGPOLYLINEITEM_H

#include <DrawingItem.h>

/*! \brief Provides a polyline item that can be added to a DrawingScene.
 *
 * To set the item's polyline, call the setPolyline() function.  The polyline() function returns the
 * current polyline.  Both functions operate in local item coordinates.
 *
 * Rendering options for the polyline can be controlled through properties of the item's style().
 * The polyline item supports all of the pen style properties as well as start arrow and end arrow
 * properties.
 *
 * DrawingPolylineItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the polyline using the item's associated pen.
 */
class DrawingPolylineItem : public DrawingItem
{
public:
	/*! \brief Create a new DrawingPolylineItem with default settings.
	 *
	 * This function creates two DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the start and end points of the polyline.  By default, the
	 * position of each point is set to the origin of the item.
	 *
	 * This function fills in the item's style() with default values for the following properties.
	 * The default values are pulled from the style's DrawingItemStyle::defaultValues() if present,
	 * otherwise DrawingPolylineItem attempts to use reasonable initial values for each property:
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
	DrawingPolylineItem();

	/*! \brief Create a new DrawingPolylineItem as a copy of an existing polyline item.
	 *
	 * Creates copies of all item points to the new polyline item, including the point's positions.
	 * Also creates a new item style with all of the same properties as the existing item's style.
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

	/*! \brief Returns item's polyline in local item coordinates.
	 *
	 * \sa setPolyline()
	 */
	QPolygonF polyline() const;


	/*! \brief Returns an estimate of the area painted by the polyline item.
	 *
	 * Calculates the bounding rect of the polyline based on the position of its points.
	 * The rect includes an adjustment for the width of the pen as set by the item's style().
	 * The rect does not include any consideration of any arrows that may be set by the style().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the polyline, including any arrows that may be set by the item's
	 * style().
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Return false if the item is degenerate, true otherwise.
	 *
	 * A polyline item is considered degenerate if the positions of all of its points
	 * are the same.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the polyline item into the scene.
	 *
	 * The polyline is painted in the scene based on properties set by the item's style(), including
	 * any arrows set by the style.
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void render(QPainter* painter);


	/*! \brief Creates a new DrawingItemPoint to be inserted in the item and determines the
	 * appropriate location in the item's point list to insert the new point.
	 *
	 * The position of the new point is determined by itemPos.  The flags of the new point are:
	 * DrawingItemPoint::Control.
	 *
	 * \sa itemPointToRemove()
	 */
	virtual DrawingItemPoint* itemPointToInsert(const QPointF& itemPos, int& index);

	/*! \brief Returns an existing DrawingItemPoint to be removed from the item at the specified
	 * position.
	 *
	 * This function removes the DrawingItemPoint nearest to itemPos, as long as it is not the first or
	 * last point of the polyline.  Note that a polyline must always have a minimum of two points; if
	 * the item only has two points, this function returns nullptr.
	 *
	 * \sa itemPointToInsert()
	 */
	virtual DrawingItemPoint* itemPointToRemove(const QPointF& itemPos);

private:
	qreal distanceFromPointToLineSegment(const QPointF& point, const QLineF& line) const;
};

#endif
