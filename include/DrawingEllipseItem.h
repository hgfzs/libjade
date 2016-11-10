/* DrawingEllipseItem.h
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

#ifndef DRAWINGELLIPSEITEM_H
#define DRAWINGELLIPSEITEM_H

#include <DrawingItem.h>

/*! \brief Provides an ellipse item that can be added to a DrawingWidget.
 *
 * To set the item's ellipse, call the setEllipse() function.  The ellipse() function returns the
 * current ellipse.  Both functions operate in local item coordinates.
 * 
 * Rendering options for the ellipse can be controlled through properties of the item's style().
 * The ellipse item supports all of the pen and brush style properties.
 * 
 * DrawingEllipseItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The paint() function draws the ellipse using the item's associated pen and brush.
 */
class DrawingEllipseItem : public DrawingItem
{
public:
	/*! \brief Create a new DrawingEllipseItem with default settings.
	 *
	 * This function creates eight DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the bounding points of the ellipse.
	 *
	 * This function fills in the item's style() with default values for the following properties.
	 * The default values are pulled from the style's DrawingItemStyle::defaultValues() if present,
	 * otherwise DrawingEllipseItem attempts to use reasonable initial values for each property:
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
	DrawingEllipseItem();
	
	/*! \brief Create a new DrawingEllipseItem as a copy of an existing ellipse item.
	 *
	 * Creates copies of all item points to the new ellipse item, including the point's positions.
	 * Also creates a new item style with all of the same properties as the existing item's style.
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


	/*! \brief Returns an estimate of the area painted by the ellipse item.
	 *
	 * Calculates the bounding rect of the ellipse based on the position of its points.
	 * The ellipse includes an adjustment for the width of the pen as set by the item's style().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;
	
	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the ellipse based on the position of its points.
	 *
	 * Note that the stroke width used to determine the shape is either the actual width of the 
	 * pen set by the item's style() or a reasonable minimum width as determined by the current
	 * zoom scale of the item's drawing(), whichever is larger.  This is done to make it easier to
	 * click on ellipse items when zoomed out on a large scene.
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;
	
	/*! \brief Return false if the item is degenerate, true otherwise.
	 *
	 * A ellipse item is considered degenerate if the positions of all of its points
	 * are the same.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;

	
	/*! \brief Paints the contents of the ellipse item into the scene.
	 *
	 * The ellipse is painted in the scene based on properties set by the item's style().
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void paint(QPainter* painter);

	
	/*! \brief Resizes the item within the scene.
	 *
	 * Before returning, this function also remaps the position of the item and all of its points 
	 * such that the position of the ellipse item's start point is at the origin of the item.
	 */
	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);

protected:
	/*! \brief Handles copy events for the ellipse item when the parent drawing() is in
	 * PlaceMode.
	 *
	 * This function ensures that the position of the ellipse item's start and end points is
	 * at the item's origin before allowing the user to start placing it within the scene.
	 *
	 * \sa newMouseMoveEvent(), newMouseReleaseEvent()
	 */
	virtual bool newItemCopyEvent();

	/*! \brief Handles mouse move events for the ellipse item when the parent drawing() is in
	 * PlaceMode.
	 *
	 * When the mouse button is not down, this function sets the position of the ellipse item's
	 * start point within the scene.  When the user presses the mouse button, the start point
	 * is placed and this function sets ths position of the item's end point.
	 *
	 * \sa newMouseReleaseEvent(), newItemCopyEvent()
	 */
	virtual void newMouseMoveEvent(DrawingMouseEvent* event);

	/*! \brief Handles mouse release events for the ellipse item when the parent drawing() is in
	 * PlaceMode.
	 *
	 * This function sets the position of the ellipse item's end point within the scene and, if
	 * isValid() returns true, causes the ellipse item to be placed within the scene.
	 *
	 * \sa newMouseMoveEvent(), newItemCopyEvent()
	 */
	virtual bool newMouseReleaseEvent(DrawingMouseEvent* event);
};

#endif
