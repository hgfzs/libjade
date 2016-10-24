/* DrawingItemGroup.h
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

#ifndef DRAWINGITEMGROUP_H
#define DRAWINGITEMGROUP_H

#include <DrawingItem.h>

class DrawingItemGroup : public DrawingItem
{
private:
	QList<DrawingItem*> mItems;
	QRectF mItemsRect;

public:
	DrawingItemGroup();
	DrawingItemGroup(const DrawingItemGroup& item);
	~DrawingItemGroup();

	DrawingItem* copy() const;

	// Selectors
	void setItems(const QList<DrawingItem*>& items);
	QList<DrawingItem*> items() const;

	// Description
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	// Slots
	virtual void rotateItem(const QPointF& scenePos);
	virtual void rotateBackItem(const QPointF& scenePos);
	virtual void flipItem(const QPointF& scenePos);

private:
	void recalculateContentsRect();
};

#endif
