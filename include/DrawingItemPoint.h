/* DrawingItemPoint.h
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

#ifndef DRAWINGITEMPOINT_H
#define DRAWINGITEMPOINT_H

#include <QtCore>

class DrawingItem;

/*! \brief Represents an interaction point within a DrawingItem through which the user can
 * resize the item or connect to another item.
 *
 * Each DrawingItemPoint object is associated with a specific item().  The item point has a
 * position() that is given in the local coordinate system of its item.
 *
 * When a DrawingItem is selected in a DrawingView, its item points are shown.  If the user clicks
 * and drags on a #Control point, DrawingView will move just the item point around the scene
 * instead of the whole item.  This provides a method for resizing items whose geometry is
 * determined by the location of their item points.
 *
 * Additionally, if the user moves or resizes and item such that two #Connection points
 * overlap, DrawingView will connect the two items together.  If the user then moves one of the
 * two items, DrawingView will attempt to maintain the connection by resizing the other item.
 * The other item's connection point must have the #Free flag set.
 *
 * DrawingItemPoint objects can be both #Control and #Connection points.
 *
 * All rendering of item points is handled by DrawingView.  Item points remain hidden until their
 * parent item is selected.  #Control points (and points that are both #Control and #Connection
 * points) are rendered as green squares that the user can then click on to move.  #Connection
 * points are rendered as yellow squares; the user can drag the connecting item over the yellow
 * square to create the connection.
 */
class DrawingItemPoint
{
	friend class DrawingItem;

public:
	/*! \brief Enum used to set the type of DrawingItemPoint, which determines how it is used
	 * by its associated DrawingItem.
	 */
	enum Flag
	{
		NoFlags = 0x0000,		//!< Convenience value indicating that none of the other flags are set.
		Control = 0x0001,		//!< #Control points can be used to resize an item. If the
								//!< user clicks and drags on a #Control point, DrawingView
								//!< will move just the item point around the canvas instead of the
								//!< whole item.
		Connection = 0x0002,	//!< #Connection points can be used to connect two items
								//!< together. If the user moves one of two connected items,
								//!< DrawingView attempts to maintain the connection by resizing
								//!< the other item.
		Free = 0x0004			//!< #Free points are typically used for the ends of lines
								//!< and similar types of items. This flag indicates that
								//!< DrawingView is free to resize the associated item to try to
								//!< maintain the connection.
	};

	/*! \brief Logical OR of various #Flag values.  Valid combinations are listed in the description
	 * for the setFlags() function.
	 *
	 * \sa setFlags(), flags()
	 */
	Q_DECLARE_FLAGS(Flags, Flag)

private:
	DrawingItem* mItem;

	QPointF mPosition;
	Flags mFlags;

	QList<DrawingItemPoint*> mConnections;

public:
	/*! \brief Create a new DrawingItemPoint with the specified position and flags.
	 *
	 * The new item point is not associated with a DrawingItem and does not have any connections to
	 * any other item points.
	 *
	 * \sa setPosition(), setFlags(), item()
	 */
	DrawingItemPoint(const QPointF& pos = QPointF(), Flags flags = Control);

	/*! \brief Create a new DrawingItemPoint as a copy of an existing item point.
	 *
	 * Copies the position and flags from the specified point to the new object. The new
	 * item point is not associated with a DrawingItem and does not have any connections to
	 * any other item points.
	 *
	 * \sa setPosition(), setFlags(), item()
	 */
	DrawingItemPoint(const DrawingItemPoint& point);

	/*! \brief Delete an existing DrawingItemPoint object.
	 *
	 * Removes all connections to other item points before deletion.
	 *
	 * \sa clearConnections()
	 */
	virtual ~DrawingItemPoint();


	/*! \brief Returns the current item that this point is a member of, or nullptr if the point
	 * is not associated with an item.
	 *
	 * To add a point to an item, call DrawingItem::addPoint() or DrawingItem::insertPoint(). To
	 * remove a point from an item, call DrawingItem::removePoint().
	 */
	DrawingItem* item() const;


	/*! \brief Sets the position of the item point.
	 *
	 * The position is given in item coordinates, i.e. the local coordinate system of the
	 * parent item.
	 *
	 * To move an item point around the scene as if the user clicked on it, call
	 * DrawingView::resizeSelection().
	 *
	 * \sa position()
	 */
	void setPosition(const QPointF& pos);

	/*! \brief Sets the position of the item point.
	 *
	 * This convenience function is equivalent to calling \link setPosition(const QPointF& pos) setPosition(QPointF(x,y)) \endlink .
	 *
	 * \sa position(), x(), y()
	 */
	void setPosition(qreal x, qreal y);

	/*! \brief Sets the x-coordinate of the item point's position.
	 *
	 * This convenience function is equivalent to calling \link setPosition(const QPointF& pos) setPosition(QPointF(x,y())) \endlink .
	 *
	 * \sa setPosition(), x()
	 */
	void setX(qreal x);

	/*! \brief Sets the y-coordinate of the item point's position.
	 *
	 * This convenience function is equivalent to calling \link setPosition(const QPointF& pos) setPosition(QPointF(x(),y)) \endlink .
	 *
	 * \sa setPosition(), y()
	 */
	void setY(qreal y);

	/*! \brief Returns the position of the item point.
	 *
	 * \sa setPosition(), x(), y()
	 */
	QPointF position() const;

	/*! \brief Returns the x-coordinate of the item point's position.
	 *
	 * \sa setX(), y()
	 */
	qreal x() const;

	/*! \brief Returns the y-coordinate of the item point's position.
	 *
	 * \sa setX(), y()
	 */
	qreal y() const;


	/*! \brief Sets the type of item point.
	 *
	 * The type of item point is represented by a series of flags.  Valid combinations of flags
	 * include:
	 * \li #Control - point is only a #Control point
	 * \li #Connection - point is only a #Connection point
	 * \li #Control|#Connection - point is both a #Control and #Connection point.  When a
	 * connected item is moved, DrawingView breaks the connection.
	 * \li #Control|#Connection|#Free - point is both a #Control and #Connection point. When a
	 * connected item is moved, DrawingView resizes the item to maintain the connection.
	 *
	 * For any other combination of flags, the behavior is undefined.
	 *
	 * By default, only the #Control flag is set.
	 *
	 * \sa flags()
	 */
	void setFlags(Flags flags);

	/*! \brief Returns the type of the item point.
	 *
	 * \sa setFlags()
	 */
	Flags flags() const;


	/*! \brief Creates a connection between two item points.
	 *
	 * The connection is only created if it does not already exist.
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * does nothing.
	 *
	 * \sa removeConnection(), connections()
	 */
	void addConnection(DrawingItemPoint* point);

	/*! \brief Breaks a connection between two item points.
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * does nothing.
	 *
	 * \sa addConnection(), clearConnections()
	 */
	void removeConnection(DrawingItemPoint* point);

	/*! \brief Breaks all connections with any other item points.
	 *
	 * This function will remove the connection from both items.  This function does not delete
	 * any of the connected item points from memory.
	 *
	 * \sa addConnection(), removeConnection()
	 */
	void clearConnections();

	/*! \brief Returns a list of all item points connected to this point.
	 *
	 * \sa addConnection(), removeConnection()
	 */
	QList<DrawingItemPoint*> connections() const;


	/*! \brief Returns true if a connection exists between this point and the specified point,
	 * false otherwise.
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * simply returns false.
	 *
	 * \sa connections()
	 */
	bool isConnected(DrawingItemPoint* point) const;

	/*! \brief Returns true if a connection exists between this point and the specified item
	 * through any of its item points, false otherwise.
	 *
	 * This convenience function iterates through each connection point and determines if any of
	 * their associated items match the specified item.
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * simply returns false.
	 *
	 * \sa connections()
	 */
	bool isConnected(DrawingItem* item) const;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DrawingItemPoint::Flags)

#endif
