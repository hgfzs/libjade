/* DrawingPathItem.h
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

#ifndef DRAWINGPATHITEM_H
#define DRAWINGPATHITEM_H

#include <DrawingItem.h>

/*! \brief Provides a path item that can be added to a DrawingScene.
 *
 * To set the item's rect, call the setRect() function.  The rect() function returns the
 * current rect.  Both functions operate in local item coordinates.
 *
 * A DrawingPathItem can draw an arbitrary QPainterPath, which can be set using setPath().  The path
 * is given in local path coordinates as specified by the pathRect(); path coordinates are mapped to
 * item coordinates using mapToPath() and mapFromPath().  These functions scale coordinates between
 * the rect() and the pathRect().
 *
 * Rendering options for the path can be controlled through properties of the item's style().
 * The path item supports all of the pen and brush style properties.
 *
 * DrawingPathItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the path using the item's associated pen and brush.
 */
class DrawingPathItem : public DrawingItem
{
private:
	enum PointIndex { TopLeft, BottomRight, TopRight, BottomLeft, TopMiddle, MiddleRight, BottomMiddle, MiddleLeft };

	QString mName;
	QPainterPath mPath;
	QRectF mPathRect;
	QHash<DrawingItemPoint*,QPointF> mPathConnectionPoints;

public:
	/*! \brief Create a new DrawingPathItem with default settings.
	 *
	 * This function creates eight DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the bounding points of the item's rect.
	 *
	 * This function fills in the item's style() with default values for the following properties.
	 * The default values are pulled from the style's DrawingItemStyle::defaultValues() if present,
	 * otherwise DrawingPathItem attempts to use reasonable initial values for each property:
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
	DrawingPathItem();

	/*! \brief Create a new DrawingPathItem as a copy of an existing path item.
	 *
	 * Creates copies of all item points to the new path item, including the point's positions.
	 * Also creates a new item style with all of the same properties as the existing item's style.
	 */
	DrawingPathItem(const DrawingPathItem& item);

	/*! \brief Delete an existing DrawingPathItem object.
	 *
	 * All of the item's points are also deleted.
	 */
	virtual ~DrawingPathItem();


	/*! \brief Creates a copy of the DrawingPathItem and return it.
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


	/*! \brief Sets the name of the path item to name.
	 *
	 * The name may be useful for distinguishing between path items with different path() functions.
	 *
	 * \sa name()
	 */
	void setName(const QString& name);

	/*! \brief Returns the path item's name.
	 *
	 * \sa setName()
	 */
	QString name() const;


	/*! \brief Sets the path of the item to path.
	 *
	 * The path is specified in its own local coordinate system; the bounds of this coordinate
	 * system are specified in pathRect.  The mapToPath() and mapFromPath() functions may be used
	 * to map coordinates from the item's local coordinate system to/from path coordinates.
	 *
	 * \sa path(), pathRect()
	 */
	void setPath(const QPainterPath& path, const QRectF& pathRect);

	/*! \brief Returns the item's path.
	 *
	 * \sa setPath()
	 */
	QPainterPath path() const;

	/*! \brief Returns the rect that sets the bounds of the path() coordinate system.
	 *
	 * \sa setPath()
	 */
	QRectF pathRect() const;


	/*! \brief Adds	a connection point at the specified position, given in path coordinates.
	 *
	 * This function adds a DrawingItemPoint at the specified location with flags set to
	 * DrawingItemPoint::Connection.
	 *
	 * \sa connectionPoints()
	 */
	void addConnectionPoint(const QPointF& pathPos);

	/*! \brief Adds	connection points at the specified positions, given in path coordinates.
	 *
	 * This convenience function calls addConnectionPoint() on each point in the specified polygon.
	 *
	 * \sa connectionPoints()
	 */
	void addConnectionPoints(const QPolygonF& pathPos);

	/*! \brief Returns all of the path item's connection points in path coordinates.
	 *
	 * \sa addConnectionPoint(), addConnectionPoints()
	 */
	QPolygonF connectionPoints() const;


	/*! \brief Maps the point from the item's coordinate system to the coordinate system of the
	 * path.
	 *
	 * \sa mapFromPath(const QPointF&) const
	 */
	QPointF mapToPath(const QPointF& itemPos) const;

	/*! \brief Maps the rect from the item's coordinate system to the coordinate system of the
	 * path.
	 *
	 * \sa mapFromPath(const QRectF&) const
	 */
	QRectF mapToPath(const QRectF& itemRect) const;

	/*! \brief Maps the point from the coordinate system of the path to the item's
	 * coordinate system.
	 *
	 * \sa mapToPath(const QPointF&) const
	 */
	QPointF mapFromPath(const QPointF& pathPos) const;

	/*! \brief Maps the rect from the coordinate system of the path to the item's
	 * coordinate system.
	 *
	 * \sa mapToPath(const QRectF&) const
	 */
	QRectF mapFromPath(const QRectF& pathRect) const;


	/*! \brief Returns an estimate of the area painted by the path item.
	 *
	 * Calculates the bounding rect of the item based on the position of its points.
	 * The rect includes an adjustment for the width of the pen as set by the item's style().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the item based on the position of its points.  For path items, the
	 * shape of the item is the same as its boundingRect().
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Return false if the item is degenerate, true otherwise.
	 *
	 * A path item is considered degenerate if the positions of all of its points
	 * are the same.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the path item into the scene.
	 *
	 * The path is painted in the scene based on properties set by the item's style().
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void render(QPainter* painter);


	/*! \brief Resizes the item within the scene.
	 *
	 * This function ensures that whenever the item is resized, all of the item's points are resized
	 * to maintain position on the item's perimeter.
	 */
	virtual void resize(DrawingItemPoint* itemPoint, const QPointF& parentPos);

private:
	QPainterPath transformedPath() const;
};

#endif
