/* DrawingPolylineItem.h
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

#include <DrawingItem.h>

/*! \brief Provides a polyline item that can be added to a DrawingWidget.
 *
 * To set the item's polyline, call the setPolyline() function.  The polyline() function returns the
 * current polyline.  Both functions operate in local item coordinates.
 * 
 * Rendering options for the polyline can be controlled through properties of the item's style().
 * The polyline item supports all of the pen style properties as well as start arrow and end arrow
 * properties.
 * 
 * DrawingPolylineItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The paint() function draws the polyline using the item's associated pen.
 *
 * When their parent drawing() is in PlaceMode, polyline items are placed by clicking and dragging
 * within the scene.  A mouse press event sets the position of the item's start point and a mouse
 * release sets the position of the item's end point.  Mouse release events also 
 * cause the item to be placed in the drawing() if isValid() returns true. From there, a new 
 * polyline item can be placed by clicking and dragging again.
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
	 * Note that the stroke width used to determine the shape is either the actual width of the 
	 * pen set by the item's style() or a reasonable minimum width as determined by the current
	 * zoom scale of the item's drawing(), whichever is larger.  This is done to make it easier to
	 * click on polyline items when zoomed out on a large scene.
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
	virtual void paint(QPainter* painter);

	
	/*! \brief Resizes the item within the scene.
	 *
	 * Before returning, this function also remaps the position of the item and all of its points 
	 * such that the position of the polyline item's start point is at the origin of the item.
	 */
	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	
	/*! \brief Inserts a new DrawingItemPoint into the item at the specified position.
	 *
	 * The position of the new point is determined by scenePos.  The flags of the new point are:
	 * DrawingItemPoint::Control.
	 *
	 * \sa removeItemPoint()
	 */
	virtual void insertItemPoint(const QPointF& scenePos);

	/*! \brief Removes an existing DrawingItemPoint from the item at the specified position.
	 *
	 * This function removes the DrawingItemPoint nearest to scenePos, as long as it is not the first or
	 * last point of the polyline.  Note that a polyline must always have a minimum of two points; if
	 * the item only has two points, this function does nothing.
	 *
	 * \sa insertItemPoint()
	 */
	virtual void removeItemPoint(const QPointF& scenePos);

protected:
	/*! \brief Handles copy events for the polyline item when the parent drawing() is in
	 * PlaceMode.
	 *
	 * This function ensures that the position of each of the polyline item's points is
	 * at the item's origin before allowing the user to start placing it within the scene.
	 *
	 * \sa newMouseMoveEvent(), newMouseReleaseEvent()
	 */
	virtual bool newItemCopyEvent();
	
	/*! \brief Handles mouse move events for the polyline item when the parent drawing() is in
	 * PlaceMode.
	 *
	 * When the mouse button is not down, this function sets the position of the polyline item's
	 * start point within the scene.  When the user presses the mouse button, the start point 
	 * is placed and this function sets ths position of the item's end point.
	 *
	 * \sa newMouseReleaseEvent(), newItemCopyEvent()
	 */
	virtual void newMouseMoveEvent(DrawingMouseEvent* event);
	
	/*! \brief Handles mouse release events for the polyline item when the parent drawing() is in
	 * PlaceMode.
	 *
	 * This function sets the position of the polyline item's end point within the scene and, if
	 * isValid() returns true, causes the polyline item to be placed within the scene.
	 *
	 * \sa newMouseMoveEvent(), newItemCopyEvent()
	 */
	virtual bool newMouseReleaseEvent(DrawingMouseEvent* event);

private:
	qreal distanceFromPointToLineSegment(const QPointF& point, const QLineF& line) const;
};
