/* DrawingCurveItem.h
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

#ifndef DRAWINGCURVEITEM_H
#define DRAWINGCURVEITEM_H

#include <DrawingItem.h>
#include <DrawingArrow.h>

/*! \brief Provides a curve item that can be added to a DrawingScene.
 *
 * To set the item's curve, call the setCurve() function.  The curveStartPos(), curveEndPos(),
 * curveStartControlPos(), and curveEndControlPos() functions return the current curve points.
 * All functions operate in local item coordinates.
 *
 * DrawingCurveItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the curve using the item's pen().
 */
class DrawingCurveItem : public DrawingItem
{
private:
	QPointF mStartPos, mStartControlPos;
	QPointF mEndPos, mEndControlPos;

	QPen mPen;
	DrawingArrow mStartArrow;
	DrawingArrow mEndArrow;

	QRectF mBoundingRect;
	QPainterPath mShape;
	QRectF mStrokeRect;
	QPainterPath mStrokeShape;

public:
	/*! \brief Create a new DrawingCurveItem with default settings.
	 *
	 * This function creates four DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the start and end points of the curve, as well as a control point for
	 * each.
	 */
	DrawingCurveItem();

	/*! \brief Create a new DrawingCurveItem as a copy of an existing curve item.
	 *
	 * Creates copies of all item points to the new curve item, including the point's positions.
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


	/*! \brief Sets the pen used to draw the curve.
	 *
	 * The pen's width is in local item coordinates.
	 *
	 * \sa pen()
	 */
	void setPen(const QPen& pen);

	/*! \brief Returns the pen used to draw the curve.
	 *
	 * \sa setPen()
	 */
	QPen pen() const;


	/*! \brief Sets the arrow drawn at the start point of the curve.
	 *
	 * The arrow's size is in local item coordinates.
	 *
	 * \sa setEndArrow(), startArrow()
	 */
	void setStartArrow(const DrawingArrow& arrow);

	/*! \brief Sets the arrow drawn at the end point of the curve.
	 *
	 * The arrow's size is in local item coordinates.
	 *
	 * \sa setStartArrow(), endArrow()
	 */
	void setEndArrow(const DrawingArrow& arrow);

	/*! \brief Returns the arrow drawn at the start point of the curve.
	 *
	 * \sa setStartArrow(), endArrow()
	 */
	DrawingArrow startArrow() const;

	/*! \brief Returns the arrow drawn at the end point of the curve.
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


	/*! \brief Returns an estimate of the area painted by the curve item.
	 *
	 * Calculates the bounding rect of the curve based on the position of its start and end points.
	 * The rect includes an adjustment for the width of the item's pen(). The rect does not
	 * include any consideration of the item's startArrow() or endArrow().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the curve based on the position of its points as well as its
	 * pen(), startArrow(), and endArrow().
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Return false if the item is invalid, true otherwise.
	 *
	 * A curve item is considered invalid if its boundingRect() has zero width and height.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the curve item into the scene.
	 *
	 * The curve is painted in the scene using the item's pen().  The item's startArrow() and
	 * endArrow() are drawn as well.
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void render(QPainter* painter);


	/*! \brief Resizes the item within the scene.
	 *
	 * This function ensures that if point is the start or end point, the corresponding control
	 * point is moved by the same amount as the selected point.
	 */
	virtual void resize(DrawingItemPoint* point, const QPointF& pos);

private:
	void updateGeometry();

	QPointF pointFromRatio(qreal ratio) const;
	qreal startArrowAngle() const;
	qreal endArrowAngle() const;
};

#endif
