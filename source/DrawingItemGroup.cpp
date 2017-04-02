/* DrawingItemGroup.cpp
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

#include "DrawingItemGroup.h"
#include "DrawingItemPoint.h"

DrawingItemGroup::DrawingItemGroup() : DrawingItem()
{
	setFlags(CanMove | CanRotate | CanFlip | CanSelect);

	for(int i = 0; i < 8; i++)
		addPoint(new DrawingItemPoint(QPointF(0.0, 0.0), DrawingItemPoint::NoFlags));
}

DrawingItemGroup::DrawingItemGroup(const DrawingItemGroup& item) : DrawingItem(item)
{
	mItems = copyItems(item.mItems);
	mItemsRect = item.mItemsRect;
}

DrawingItemGroup::~DrawingItemGroup()
{
	while (!mItems.isEmpty()) delete mItems.takeFirst();
}

//==================================================================================================

DrawingItem* DrawingItemGroup::copy() const
{
	return new DrawingItemGroup(*this);
}

//==================================================================================================

void DrawingItemGroup::setItems(const QList<DrawingItem*>& items)
{
	while (!mItems.isEmpty()) delete mItems.takeFirst();
	mItems = items;
	recalculateContentsRect();
}

QList<DrawingItem*> DrawingItemGroup::items() const
{
	return mItems;
}

//==================================================================================================

QRectF DrawingItemGroup::boundingRect() const
{
	return mItemsRect;
}

QPainterPath DrawingItemGroup::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		{
			if ((*itemIter)->isVisible())
				shape = shape.united((*itemIter)->mapToParent((*itemIter)->shape()));
		}
	}

	return shape;
}

bool DrawingItemGroup::isValid() const
{
	return !mItems.isEmpty();
}

//==================================================================================================

void DrawingItemGroup::render(QPainter* painter)
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			painter->translate((*itemIter)->position());
			painter->setTransform((*itemIter)->transformInverted(), true);
			(*itemIter)->render(painter);
			painter->setTransform((*itemIter)->transform(), true);
			painter->translate(-(*itemIter)->position());
		}
	}
}

//==================================================================================================

void DrawingItemGroup::recalculateContentsRect()
{
	// Update items rect
	QRectF itemRect;
	mItemsRect = QRectF();

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			itemRect = (*itemIter)->mapToParent((*itemIter)->boundingRect()).boundingRect();

			if (!mItemsRect.isValid()) mItemsRect = itemRect;
			else mItemsRect = mItemsRect.united(itemRect);
		}
	}

	// Update points
	QList<DrawingItemPoint*> points = DrawingItemGroup::points();
	if (points.size() >= 8)
	{
		points[0]->setPosition(mItemsRect.left(), mItemsRect.top());
		points[1]->setPosition(mItemsRect.center().x(), mItemsRect.top());
		points[2]->setPosition(mItemsRect.right(), mItemsRect.top());
		points[3]->setPosition(mItemsRect.right(), mItemsRect.center().y());
		points[4]->setPosition(mItemsRect.right(), mItemsRect.bottom());
		points[5]->setPosition(mItemsRect.center().x(), mItemsRect.bottom());
		points[6]->setPosition(mItemsRect.left(), mItemsRect.bottom());
		points[7]->setPosition(mItemsRect.left(), mItemsRect.center().y());
	}
}
