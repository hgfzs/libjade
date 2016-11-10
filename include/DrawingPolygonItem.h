/* DrawingPolygonItem.h
 *
 * Copyright (C) 2013-2016 Jason Allen
 *
 * This file is part of the jade library.
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

#ifndef DRAWINGPOLYGONITEM_H
#define DRAWINGPOLYGONITEM_H

#include <DrawingItem.h>

/*! \brief Provides a polygon item that can be added to a DrawingWidget.
 *
 * To set the item's polygon, call the setPolygon() function.  The polygon() function returns the
 * current polygon.  Both functions operate in local item coordinates.
 * 
 * Rendering options for the polygon can be controlled through properties of the item's style().
 * The polygon item supports all of the pen and brush style properties.
 * 
 * DrawingPolygonItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The paint() function draws the polygon using the item's associated pen and brush.
 */
class DrawingPolygonItem : public DrawingItem
{
public:
	/*! \brief Create a new DrawingPolygonItem with default settings.
	 *
	 * This function creates three DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the initial points of the polygon.
	 *
	 * This function fills in the item's style() with default values for the following properties.
	 * The default values are pulled from the style's DrawingItemStyle::defaultValues() if present,
	 * otherwise DrawingPolygonItem attempts to use reasonable initial values for each property:
	 * \li DrawingItemStyle::PenStyle
	 * \li DrawingItemStyle::PenColor
	 * \li DrawingItemStyle::PenOpacity
	 * \li DrawingItemStyle::PenWidth
	 * \li DrawingItemStyle::PenCapStyle
	 * \li DrawingItemStyle::PenJoinStyle
	 * \li DrawingItemStyle::BrushStyle
	 * \li DrawingItemStyle::BrushColor
	 * \li DrawingItemStyle::BrushOpacity
	 */
	DrawingPolygonItem();
	
	/*! \brief Create a new DrawingPolygonItem as a copy of an existing polygon item.
	 *
	 * Creates copies of all item points to the new polygon item, including the point's positions.
	 * Also creates a new item style with all of the same properties as the existing item's style.
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

	
	/*! \brief Returns an estimate of the area painted by the polygon item.
	 *
	 * Calculates the bounding rect of the polygon based on the position of its points.
	 * The rect includes an adjustment for the width of the pen as set by the item's style().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;
	
	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the polygon based on the position of its points.
	 *
	 * Note that the stroke width used to determine the shape is either the actual width of the 
	 * pen set by the item's style() or a reasonable minimum width as determined by the current
	 * zoom scale of the item's drawing(), whichever is larger.  This is done to make it easier to
	 * click on polygon items when zoomed out on a large scene.
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;
	
	/*! \brief Return false if the item is degenerate, true otherwise.
	 *
	 * A polygon item is considered degenerate if the positions of all of its points
	 * are the same.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;

	
	/*! \brief Paints the contents of the polygon item into the scene.
	 *
	 * The polygon is painted in the scene based on properties set by the item's style().
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void paint(QPainter* painter);

	
	/*! \brief Resizes the item within the scene.
	 *
	 * Before returning, this function also remaps the position of the item and all of its points 
	 * such that the position of the polygon item's start point is at the origin of the item.
	 */
	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	
	/*! \brief Inserts a new DrawingItemPoint into the item at the specified position.
	 *
	 * The position of the new point is determined by scenePos.  The flags of the new point are:
	 * DrawingItemPoint::Control | DrawingItemPoint::Connection.
	 *
	 * \sa removeItemPoint()
	 */
	virtual void insertItemPoint(const QPointF& scenePos);

	/*! \brief Removes an existing DrawingItemPoint from the item at the specified position.
	 *
	 * This function removes the DrawingItemPoint nearest to scenePos.  Note that a polygon must
	 * always have a minimum of three points; if the item only has three points, this function
	 * does nothing.
	 *
	 * \sa insertItemPoint()
	 */
	virtual void removeItemPoint(const QPointF& scenePos);

private:
	qreal distanceFromPointToLineSegment(const QPointF& point, const QLineF& line) const;
};

#endif
