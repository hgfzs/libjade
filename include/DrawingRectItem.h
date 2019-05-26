/* DrawingRectItem.h
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

#ifndef DRAWINGRECTITEM_H
#define DRAWINGRECTITEM_H

#include <DrawingItem.h>

/*! \brief Provides a rectangle item that can be added to a DrawingScene.
 *
 * To set the item's rect, call the setRect() function.  The rect() function returns the
 * current rect.  Both functions operate in local item coordinates.
 *
 * Rendering options for the rect can be controlled through properties of the item's style().
 * The rect item supports all of the pen and brush style properties.
 *
 * DrawingRectItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the rect using the item's associated pen and brush.
 */
class DrawingRectItem : public DrawingItem
{
private:
	enum PointIndex { TopLeft, BottomRight, TopRight, BottomLeft, TopMiddle, MiddleRight, BottomMiddle, MiddleLeft };

	qreal mCornerRadiusX, mCornerRadiusY;

public:
	/*! \brief Create a new DrawingRectItem with default settings.
	 *
	 * This function creates eight DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the bounding points of the rect.
	 *
	 * This function fills in the item's style() with default values for the following properties.
	 * The default values are pulled from the style's DrawingItemStyle::defaultValues() if present,
	 * otherwise DrawingRectItem attempts to use reasonable initial values for each property:
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
	DrawingRectItem();

	/*! \brief Create a new DrawingRectItem as a copy of an existing rect item.
	 *
	 * Creates copies of all item points to the new rect item, including the point's positions.
	 * Also creates a new item style with all of the same properties as the existing item's style.
	 */
	DrawingRectItem(const DrawingRectItem& item);

	/*! \brief Delete an existing DrawingRectItem object.
	 *
	 * All of the item's points are also deleted.
	 */
	virtual ~DrawingRectItem();


	/*! \brief Creates a copy of the DrawingRectItem and return it.
	 *
	 * Simply calls the copy constructor.
	 */
	virtual DrawingItem* copy() const;


	/*! \brief Sets the item's rect to rect, which is given in local item coordinates.
	 *
	 * \sa rect()
	 */
	void setRect(const QRectF& rect);

	/*! \brief Sets the item's rect, which is given in local item coordinates.
	 *
	 * This convenience function is equivalent to calling setRect(QRectF(left, top, width, height)).
	 *
	 * \sa rect()
	 */
	void setRect(qreal left, qreal top, qreal width, qreal height);

	/*! \brief Returns the item's rect in local item coordinates.
	 *
	 * \sa setRect(const QRectF&), setRect(qreal, qreal, qreal, qreal)
	 */
	QRectF rect() const;


	/*! \brief Sets the item's corner radius for the x-axis and y-axis as given in local item
	 * coordinates.
	 *
	 * \sa cornerRadiusX(), cornerRadiusY()
	 */
	void setCornerRadii(qreal radiusX, qreal radiusY);

	/*! \brief Returns the item's corner radius on the x-axis.
	 *
	 * \sa setCornerRadii(), cornerRadiusY()
	 */
	qreal cornerRadiusX() const;

	/*! \brief Returns the item's corner radius on the y-axis.
	 *
	 * \sa setCornerRadii(), cornerRadiusX()
	 */
	qreal cornerRadiusY() const;


	/*! \brief Returns an estimate of the area painted by the rect item.
	 *
	 * Calculates the bounding rect of the rect based on the position of its points.
	 * The rect includes an adjustment for the width of the pen as set by the item's style().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the rect based on the position of its points.
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Return false if the item is degenerate, true otherwise.
	 *
	 * A rect item is considered degenerate if the positions of all of its points
	 * are the same.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the rect item into the scene.
	 *
	 * The rect is painted in the scene based on properties set by the item's style().
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void render(QPainter* painter);

protected:
	/*! \brief Resizes the item within the scene.
	 *
	 * This function ensures that whenever the item is resized, all of the item's points are resized
	 * to maintain position on the item's perimeter.
	 */
	virtual void resizeEvent(DrawingItemPoint* itemPoint, const QPointF& parentPos);
};

#endif
