/* DrawingCurveItem.h
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

#ifndef DRAWINGCURVEITEM_H
#define DRAWINGCURVEITEM_H

#include <DrawingItem.h>

/*! \brief Provides a curve item that can be added to a DrawingScene.
 *
 * To set the item's curve, call the setCurve() function.  The curveStartPos(), curveEndPos(),
 * curveStartControlPos(), and curveEndControlPos() functions return the current curve points.
 * All functions operate in local item coordinates.
 *
 * Rendering options for the curve can be controlled through properties of the item's style().
 * The curve item supports all of the pen style properties as well as start arrow and end arrow
 * properties.
 *
 * DrawingCurveItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the curve using the item's associated pen.
 */
class DrawingCurveItem : public DrawingItem
{
public:
	/*! \brief Create a new DrawingCurveItem with default settings.
	 *
	 * This function creates four DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the start and end points of the curve, as well as a control point for
	 * each.
	 *
	 * This function fills in the item's style() with default values for the following properties.
	 * The default values are pulled from the style's DrawingItemStyle::defaultValues() if present,
	 * otherwise DrawingCurveItem attempts to use reasonable initial values for each property:
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
	DrawingCurveItem();

	/*! \brief Create a new DrawingCurveItem as a copy of an existing curve item.
	 *
	 * Creates copies of all item points to the new curve item, including the point's positions.
	 * Also creates a new item style with all of the same properties as the existing item's style.
	 */
	DrawingCurveItem(const DrawingCurveItem& item);

	/*! \brief Delete an existing DrawingCurveItem object.
	 *
	 * All of the item's points are also deleted.
	 */
	virtual ~DrawingCurveItem();


	/*! \brief Creates a copy of the DrawingCurveItem and return it.
	 *
	 * Simply calls the copy constructor.
	 */
	virtual DrawingItem* copy() const;


	/*! \brief Sets the item's curve, which is given in local item coordinates.
	 *
	 * Sets the positions of the item's points so that:
	 * \li point[0] is the start point of the curve (p1)
	 * \li point[1] is the end point of the curve (p2)
	 * \li point[2] is the control point for the start of the curve (controlP1)
	 * \li point[3] is the control point for the end of the curve (controlP2)
	 *
	 * \sa curveStartPos(), curveEndPos(), curveStartControlPos(), curveEndControlPos()
	 */
	void setCurve(const QPointF& p1, const QPointF& controlP1, const QPointF& controlP2, const QPointF& p2);

	/*! \brief Returns the item's start point in local item coordinates.
	 *
	 * \sa setCurve(), curveEndPos(), curveStartControlPos(), curveEndControlPos()
	 */
	QPointF curveStartPos() const;

	/*! \brief Returns the item's end point in local item coordinates.
	 *
	 * \sa setCurve(), curveStartPos(), curveStartControlPos(), curveEndControlPos()
	 */
	QPointF curveEndPos() const;

	/*! \brief Returns the item's control point for the start of the curve in local item coordinates.
	 *
	 * \sa setCurve(), curveStartPos(), curveEndPos(), curveEndControlPos()
	 */
	QPointF curveStartControlPos() const;

	/*! \brief Returns the item's control point for the end of the curve in local item coordinates.
	 *
	 * \sa setCurve(), curveStartPos(), curveEndPos(), curveStartControlPos()
	 */
	QPointF curveEndControlPos() const;


	/*! \brief Returns an estimate of the area painted by the curve item.
	 *
	 * Calculates the bounding rect of the curve based on the position of its points.
	 * The rect includes an adjustment for the width of the pen as set by the item's style().
	 * The rect does not include any consideration of any arrows that may be set by the style().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the curve, including any arrows that may be set by the item's
	 * style().
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Return false if the item is degenerate, true otherwise.
	 *
	 * A curve item is considered degenerate if its boundingRect() has zero width and height.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the curve item into the scene.
	 *
	 * The curve is painted in the scene based on properties set by the item's style(), including
	 * any arrows set by the style.
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void render(QPainter* painter);

protected:
	/*! \brief Resizes the item within the scene.
	 *
	 * This function adds behavior to the default DrawingItem::resizeEvent() implemenation.  This
	 * behavior is that if itemPoint is the start or end point, the corresponding control point is
	 * moved by the same amount when the item is resized.
	 */
	virtual void resizeEvent(DrawingItemPoint* itemPoint, const QPointF& scenePos);

private:
	QPointF pointFromRatio(qreal ratio) const;
	qreal startArrowAngle() const;
	qreal endArrowAngle() const;
};

#endif
