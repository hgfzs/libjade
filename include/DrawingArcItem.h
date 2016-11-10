/* DrawingArcItem.h
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

#ifndef DRAWINGARCITEM_H
#define DRAWINGARCITEM_H

#include <DrawingItem.h>

/*! \brief Provides an arc item that can be added to a DrawingWidget.
 *
 * To set the item's arc, call the setArc() function.  The arc() function returns the current
 * arc.  Both functions operate in local item coordinates.
 * 
 * Rendering options for the arc can be controlled through properties of the item's style().
 * The arc item supports all of the pen style properties as well as start arrow and end arrow
 * properties.
 * 
 * DrawingArcItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The paint() function draws the arc using the item's associated pen.
 *
 * When their parent drawing() is in PlaceMode, arc items are placed by clicking and dragging
 * within the scene.  A mouse press event sets the position of the item's start point and a mouse
 * release sets the position of the item's end point.  Mouse release events also 
 * cause the item to be placed in the drawing() if isValid() returns true. From there, a new 
 * arc item can be placed by clicking and dragging again.
 */
class DrawingArcItem : public DrawingItem
{
public:
	/*! \brief Create a new DrawingArcItem with default settings.
	 *
	 * This function creates two DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the start and end points of the arc.  By default, the
	 * position of each point is set to the origin of the item.
	 *
	 * This function fills in the item's style() with default values for the following properties.
	 * The default values are pulled from the style's DrawingItemStyle::defaultValues() if present,
	 * otherwise DrawingArcItem attempts to use reasonable initial values for each property:
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
	DrawingArcItem();
	
	/*! \brief Create a new DrawingArcItem as a copy of an existing arc item.
	 *
	 * Creates copies of all item points to the new arc item, including the point's positions.
	 * Also creates a new item style with all of the same properties as the existing item's style.
	 */
	DrawingArcItem(const DrawingArcItem& item);
	
	/*! \brief Delete an existing DrawingArcItem object.
	 *
	 * All of the item's points are also deleted.
	 */
	virtual ~DrawingArcItem();

	/*! \brief Creates a copy of the DrawingArcItem and return it.
	 *
	 * Simply calls the copy constructor.
	 */
	virtual DrawingItem* copy() const;

	
	/*! \brief Sets the item's arc to line, which is given in local item coordinates.
	 *
	 * Sets the positions of the item's points so that:
	 * \li point[0] is at start point of the arc (line.p1())
	 * \li point[1] is at end point of the arc (line.p2())
	 *
	 * \sa arc()
	 */
	void setArc(const QLineF& line);
	
	/*! \brief Sets the item's arc, which is given in local item coordinates.
	 *
	 * This convenience function is equivalent to calling setArc(QLineF(x1, y1, x2, y2)).
	 *
	 * \sa arc()
	 */
	void setArc(qreal x1, qreal y1, qreal x2, qreal y2);
	
	/*! \brief Returns item's arc in local item coordinates.
	 *
	 * \sa setArc(const QLineF&), setArc(qreal, qreal, qreal, qreal)
	 */
	QLineF arc() const;
	
	
	/*! \brief Returns an estimate of the area painted by the arc item.
	 *
	 * Calculates the bounding rect of the arc based on the position of its start and end points.
	 * The rect includes an adjustment for the width of the pen as set by the item's style().
	 * The rect does not include any consideration of any arrows that may be set by the style().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;
	
	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the arc, including any arrows that may be set by the item's
	 * style().
	 *
	 * Note that the stroke width used to determine the shape is either the actual width of the 
	 * pen set by the item's style() or a reasonable minimum width as determined by the current
	 * zoom scale of the item's drawing(), whichever is larger.  This is done to make it easier to
	 * click on arc items when zoomed out on a large scene.
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;
	
	/*! \brief Return false if the item is degenerate, true otherwise.
	 *
	 * An arc item is considered degenerate if the positions of its start and end points
	 * are the same.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;

	
	/*! \brief Paints the contents of the arc item into the scene.
	 *
	 * The arc is painted in the scene based on properties set by the item's style(), including
	 * any arrows set by the style.
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void paint(QPainter* painter);

	
	/*! \brief Resizes the item within the scene.
	 *
	 * This function also remaps the position of the item and all of its points
	 * such that the position of the arc item's start point is at the origin of the item.
	 */
	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	
protected:
	/*! \brief Handles copy events for the arc item when the parent drawing() is in
	 * PlaceMode.
	 *
	 * This function ensures that the position of the arc item's start and end points is
	 * at the item's origin before allowing the user to start placing it within the scene.
	 *
	 * \sa newMouseMoveEvent(), newMouseReleaseEvent()
	 */
	virtual bool newItemCopyEvent();
	
	/*! \brief Handles mouse move events for the arc item when the parent drawing() is in
	 * PlaceMode.
	 *
	 * When the mouse button is not down, this function sets the position of the arc item's
	 * start point within the scene.  When the user presses the mouse button, the start point 
	 * is placed and this function sets ths position of the item's end point.
	 *
	 * \sa newMouseReleaseEvent(), newItemCopyEvent()
	 */
	virtual void newMouseMoveEvent(DrawingMouseEvent* event);
	
	/*! \brief Handles mouse release events for the arc item when the parent drawing() is in
	 * PlaceMode.
	 *
	 * This function sets the position of the arc item's end point within the scene and, if
	 * isValid() returns true, causes the arc item to be placed within the scene.
	 *
	 * \sa newMouseMoveEvent(), newItemCopyEvent()
	 */
	virtual bool newMouseReleaseEvent(DrawingMouseEvent* event);

private:
	QRectF arcRect() const;
	qreal arcStartAngle() const;

	QPointF pointFromAngle(qreal angle) const;
	qreal startArrowAngle() const;
	qreal endArrowAngle() const;
};

#endif
