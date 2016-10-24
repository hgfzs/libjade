/* DrawingItemPoint.h
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

#ifndef DRAWINGITEMPOINT_H
#define DRAWINGITEMPOINT_H

#include <DrawingTypes.h>

/*! \brief Represents an interaction point within a DrawingItem through which the user can
 * resize the item or connect the item to another item.
 *
 * Each DrawingItemPoint object is associated with a specific item().  The item point has a
 * position (pos()) that is given in the local coordinate system of that item.
 *
 * When a DrawingItem is selected, its item points are shown.  If the user clicks and drags on a
 * #Control point, DrawingWidget will move just the item point around the canvas instead of
 * the whole item.  This provides a method for resizing items whose geometry is determined by the
 * location of their item points.
 *
 * Additionally, if the user moves or resizes and item such that two #Connection points
 * overlap, DrawingWidget will connect the two items together.  If the user then moves one of the
 * two items, DrawingWidget will attempt to maintain the connection by resizing the other item.
 *
 * DrawingItemPoint objects can be both #Control and #Connection points.
 *
 * All rendering of item points is handled by DrawingWidget.  Item points remain hidden until their
 * parent item is selected.  #Control points are rendered as green squares that the user can
 * then click on to move.  #Connection points are rendered as X's; the user can drag the
 * connecting item over the X to create the connection.  Item points that are both #Control
 * and #Connection points are rendered as green squares with X's inside them.
 */
class DrawingItemPoint
{
	friend class DrawingItem;

public:
	/*! \brief Enum used to set the type of DrawingItemPoint, which determines how	it is used
	 * by its associated DrawingItem.
	 */
	enum Flag
	{
		NoFlags = 0x0000,		//!< Convenience value indicating that none of the other flags are set.
		Control = 0x0001,		//!< #Control points can be used to resize an item. If the
								//!< user clicks and drags on a #Control point, DrawingWidget
								//!< will move just the item point around the canvas instead of the
								//!< whole item.
		Connection = 0x0002,	//!< #Connection points can be used to connect two items
								//!< together. If the user moves one of two connected items,
								//!< DrawingWidget attempts to maintain the connection by resizing
								//!< the other item.
		Free = 0x0004			//!< #Free points are typically used for the ends of lines
								//!< and similar types of items. This flag indicates that
								//!< DrawingWidget is free to resize the associateditem to try to
								//!< maintain the connection.
	};
	Q_DECLARE_FLAGS(Flags, Flag)

private:
	DrawingItem* mItem;

	QPointF mPosition;
	Flags mFlags;

	QList<DrawingItemPoint*> mConnections;

public:
	/*! \brief Create a new DrawingItemPoint with the specified settings.
	 *
	 * Initializes the new DrawingItemPoint object with the specified position and flags. The new
	 * item point is not associated with a DrawingItem and does not have any connections to
	 * any other item points.
	 *
	 * \sa setPos(), setFlags(), item()
	 */
	DrawingItemPoint(const QPointF& pos = QPointF(), Flags flags = Control);
	
	/*! \brief Create a new DrawingItemPoint as a copy of an existing item point.
	 *
	 * Copies the position and flags from the specified point to the new object. The new
	 * item point is not associated with a DrawingItem and does not have any connections to
	 * any other item points.
	 *
	 * \sa setPos(), setFlags(), item()
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
	 * DrawingWidget::resizeSelection().
	 *
	 * \sa pos()
	 */
	void setPos(const QPointF& pos);
	
	/*! \brief Sets the position of the item point.
	 *
	 * This convenience function is equivalent to calling setPos(QPointF(x,y)).
	 *
	 * \sa pos(), x(), y()
	 */
	void setPos(qreal x, qreal y);
	
	/*! \brief Sets the x-coordinate of the item point's position.
	 *
	 * This convenience function is equivalent to calling setPos(QPointF(x,y())).
	 *
	 * \sa setPos(), x()
	 */
	void setX(qreal x);
	
	/*! \brief Sets the y-coordinate of the item point's position.
	 *
	 * This convenience function is equivalent to calling setPos(QPointF(x(),y)).
	 *
	 * \sa setPos(), y()
	 */
	void setY(qreal y);
	
	/*! \brief Returns the position of the item point.
	 *
	 * \sa setPos(), x(), y()
	 */
	QPointF pos() const;
	
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
	 * connected item is moved, DrawingWidget breaks the connection.
	 * \li #Control|#Connection|#Free - point is both a #Control and #Connection point. When a
	 * connected item is moved, DrawingWidget resizes the item to maintain the connection.
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
	 * \sa setFlags(), isControlPoint(), isConnectionPoint(), isFree()
	 */
	Flags flags() const;
	
	/*! \brief Returns true if the item point is a #Control point, false otherwise
	 *
	 * \sa flags(), isConnectionPoint(), isFree()
	 */
	bool isControlPoint() const;
	
	/*! \brief Returns true if the item point is a #Connection point, false otherwise
	 *
	 * \sa flags(), isControlPoint(), isFree()
	 */
	bool isConnectionPoint() const;
	
	/*! \brief Returns true if the item point is a #Free point, false otherwise
	 *
	 * \sa flags(), isControlPoint(), isConnectionPoint()
	 */
	bool isFree() const;

	
	/*! \brief Creates a connection between two item points.
	 *
	 * The connection is only created if it does not already exist.
	 * 
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * does nothing.
	 *
	 * \sa removeConnection(), connections()
	 */
	void addConnection(DrawingItemPoint* itemPoint);
	
	/*! \brief Breaks a connection between two item points.
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * does nothing.
	 *
	 * \sa addConnection(), clearConnections()
	 */
	void removeConnection(DrawingItemPoint* itemPoint);
	
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
	 * This function does not delete any DrawingItemPoint objects from memory.
	 *
	 * \sa removeConnection(), connections()
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
	bool isConnected(DrawingItemPoint* itemPoint) const;
	
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
