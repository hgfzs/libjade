/* DrawingItemGroup.h
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

#ifndef DRAWINGITEMGROUP_H
#define DRAWINGITEMGROUP_H

#include <DrawingItem.h>

/*! \brief Provides a group item that can be added to a DrawingScene.
 *
 * To set the items of a group, call setItems().  Items in a group are treated as a single item
 * within the widget.
 *
 * DrawingItemGroup provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the group by calling the paint() function of each item.
 */
class DrawingItemGroup : public DrawingItem
{
private:
	QList<DrawingItem*> mItems;
	QRectF mItemsRect;

public:
	/*! \brief Create a new DrawingItemGroup with default settings.
	 *
	 * The new group item does not contain any items.
	 *
	 * This function creates eight DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the corners and edge midpoints used to mark the boundary of the item.
	 */
	DrawingItemGroup();

	/*! \brief Create a new DrawingItemGroup as a copy of an existing group item.
	 *
	 * Creates copies of all of the existing group's items in the new group item.
	 */
	DrawingItemGroup(const DrawingItemGroup& item);

	/*! \brief Delete an existing DrawingItem object.
	 *
	 * All of the group's items are also deleted.
	 */
	~DrawingItemGroup();


	/*! \brief Creates a copy of the DrawingItemGroup and return it.
	 *
	 * Simply calls the copy constructor.
	 */
	DrawingItem* copy() const;


	/*! \brief Sets the items that make up the group.
	 *
	 * DrawingItemGroup will delete any items that were already part of the group.
	 * DrawingItemGroup takes ownership of the specified items and will delete them as necessary.
	 *
	 * \sa items()
	 */
	void setItems(const QList<DrawingItem*>& items);

	/*! \brief Returns a list of all items added to the group.
	 *
	 * \sa setItems()
	 */
	QList<DrawingItem*> items() const;


	/*! \brief Returns an estimate of the area painted by the group item.
	 *
	 * Calculates the bounding rect of each of the group's items().  The union of all these bounding
	 * rects is returned as the bounding rect for the group.
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the line, including any arrows that may be set by the item's
	 * style().
	 *
	 * Calculates the shape of each of the group's items().  The union of all these shapes is
	 * returned as the shape for the group.
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Return false if the item is degenerate, true otherwise.
	 *
	 * A group item is considered degenerate if it contains no items().
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the group item into the scene.
	 *
	 * The group is painted by calling the paint() function for each of the group's items().
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void render(QPainter* painter);


private:
	void recalculateContentsRect();
};

#endif
