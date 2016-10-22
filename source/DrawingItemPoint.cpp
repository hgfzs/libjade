/* DrawingItemPoint.cpp
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

#include "DrawingItemPoint.h"
#include "DrawingItem.h"
#include "DrawingWidget.h"

DrawingItemPoint::DrawingItemPoint(const QPointF& position, Flags flags)
{
	mItem = nullptr;
	mPosition = position;
	mFlags = flags;
}

DrawingItemPoint::DrawingItemPoint(const DrawingItemPoint& point)
{
	mItem = nullptr;
	mPosition = point.mPosition;
	mFlags = point.mFlags;
}

DrawingItemPoint::~DrawingItemPoint()
{
	clearConnections();
}

//==================================================================================================

DrawingItem* DrawingItemPoint::item() const
{
	return mItem;
}

//==================================================================================================

void DrawingItemPoint::setPos(const QPointF& pos)
{
	mPosition = pos;
}

void DrawingItemPoint::setPos(qreal x, qreal y)
{
	mPosition.setX(x);
	mPosition.setY(y);
}

void DrawingItemPoint::setX(qreal x)
{
	mPosition.setX(x);
}

void DrawingItemPoint::setY(qreal y)
{
	mPosition.setY(y);
}

QPointF DrawingItemPoint::pos() const
{
	return mPosition;
}

qreal DrawingItemPoint::x() const
{
	return mPosition.x();
}

qreal DrawingItemPoint::y() const
{
	return mPosition.y();
}

//==================================================================================================

void DrawingItemPoint::setFlags(Flags flags)
{
	mFlags = flags;
}

DrawingItemPoint::Flags DrawingItemPoint::flags() const
{
	return mFlags;
}

bool DrawingItemPoint::isControlPoint() const
{
	return (mFlags & Control);
}

bool DrawingItemPoint::isConnectionPoint() const
{
	return (mFlags & Connection);
}

bool DrawingItemPoint::isFree() const
{
	return (mFlags & Free);
}

//==================================================================================================

void DrawingItemPoint::addConnection(DrawingItemPoint* point)
{
	if (point && !isConnected(point)) mConnections.append(point);
}

void DrawingItemPoint::removeConnection(DrawingItemPoint* point)
{
	if (point) mConnections.removeAll(point);
}

void DrawingItemPoint::clearConnections()
{
	DrawingItemPoint* point;

	while (!mConnections.isEmpty())
	{
		point = mConnections.first();

		removeConnection(point);
		point->removeConnection(this);
	}
}

QList<DrawingItemPoint*> DrawingItemPoint::connections() const
{
	return mConnections;
}

//==================================================================================================

bool DrawingItemPoint::isConnected(DrawingItemPoint* point) const
{
	return (point) ? mConnections.contains(point) : false;
}

bool DrawingItemPoint::isConnected(DrawingItem* item) const
{
	bool connected = false;

	if (item)
	{
		for(auto pointIter = mConnections.begin(); !connected && pointIter != mConnections.end(); pointIter++)
			connected = ((*pointIter)->item() == item);
	}

	return connected;
}
