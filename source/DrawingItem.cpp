/* DrawingItem.cpp
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

#include "DrawingItem.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"

DrawingItem::DrawingItem()
{
	mScene = nullptr;

	mFlags = (CanMove | CanResize | CanRotate | CanFlip | CanSelect);
	mStyle = new DrawingItemStyle();

	mParent = nullptr;

	mSelected = false;
	mVisible = true;
}

DrawingItem::DrawingItem(const DrawingItem& item)
{
	mScene = nullptr;

	mPosition = item.mPosition;
	mTransform = item.mTransform;
	mTransformInverse = item.mTransformInverse;

	mFlags = item.mFlags;
	mStyle = new DrawingItemStyle(*item.mStyle);

	for(auto pointIter = item.mPoints.begin(); pointIter != item.mPoints.end(); pointIter++)
		addPoint(new DrawingItemPoint(**pointIter));

	for(auto itemIter = item.mChildren.begin(); itemIter != item.mChildren.end(); itemIter++)
		addChild((*itemIter)->copy());
	mParent = nullptr;

	mSelected = false;
	mVisible = true;
}

DrawingItem::~DrawingItem()
{
	clearPoints();
	clearChildren();
	delete mStyle;
	mParent = nullptr;
	mScene = nullptr;
}

//==================================================================================================

DrawingScene* DrawingItem::scene() const
{
	return mScene;
}

//==================================================================================================

void DrawingItem::setPosition(const QPointF& pos)
{
	mPosition = pos;
}

void DrawingItem::setPosition(qreal x, qreal y)
{
	mPosition.setX(x);
	mPosition.setY(y);
}

void DrawingItem::setX(qreal x)
{
	mPosition.setX(x);
}

void DrawingItem::setY(qreal y)
{
	mPosition.setY(y);
}

QPointF DrawingItem::position() const
{
	return mPosition;
}

qreal DrawingItem::x() const
{
	return mPosition.x();
}

qreal DrawingItem::y() const
{
	return mPosition.y();
}

//==================================================================================================

void DrawingItem::setTransform(const QTransform& transform, bool combine)
{
	if (combine) mTransform = mTransform * transform;
	else mTransform = transform;

	mTransformInverse = mTransform.inverted();
}

QTransform DrawingItem::transform() const
{
	return mTransform;
}

QTransform DrawingItem::transformInverted() const
{
	return mTransformInverse;
}

//==================================================================================================

void DrawingItem::setFlags(Flags flags)
{
	mFlags = flags;
}

DrawingItem::Flags DrawingItem::flags() const
{
	return mFlags;
}

//==================================================================================================

void DrawingItem::setStyle(DrawingItemStyle* style)
{
	if (style)
	{
		delete mStyle;
		mStyle = style;
	}
}

DrawingItemStyle* DrawingItem::style() const
{
	return mStyle;
}

//==================================================================================================

void DrawingItem::addPoint(DrawingItemPoint* itemPoint)
{
	if (itemPoint && itemPoint->mItem == nullptr)
	{
		mPoints.append(itemPoint);
		itemPoint->mItem = this;
	}
}

void DrawingItem::insertPoint(int index, DrawingItemPoint* itemPoint)
{
	if (itemPoint && itemPoint->mItem == nullptr)
	{
		mPoints.insert(index, itemPoint);
		itemPoint->mItem = this;
	}
}

void DrawingItem::removePoint(DrawingItemPoint* itemPoint)
{
	if (itemPoint && itemPoint->mItem == this)
	{
		mPoints.removeAll(itemPoint);
		itemPoint->mItem = nullptr;
	}
}

void DrawingItem::clearPoints()
{
	DrawingItemPoint* itemPoint = nullptr;

	while (!mPoints.empty())
	{
		itemPoint = mPoints.first();
		removePoint(itemPoint);
		delete itemPoint;
		itemPoint = nullptr;
	}
}

QList<DrawingItemPoint*> DrawingItem::points() const
{
	return mPoints;
}

//==================================================================================================

DrawingItemPoint* DrawingItem::pointAt(const QPointF& itemPos) const
{
	DrawingItemPoint* itemPoint = nullptr;

	QList<DrawingItemPoint*> itemPoints = points();
	for(auto pointIter = itemPoints.begin(); itemPoint == nullptr && pointIter != itemPoints.end(); pointIter++)
	{
		if (itemPos == (*pointIter)->position()) itemPoint = *pointIter;
	}

	return itemPoint;
}

DrawingItemPoint* DrawingItem::pointNearest(const QPointF& itemPos) const
{
	DrawingItemPoint *itemPoint = nullptr;

	QList<DrawingItemPoint*> itemPoints = points();
	if (!itemPoints.isEmpty())
	{
		itemPoint = itemPoints.first();

		QPointF vec = (itemPoint->position() - itemPos);
		qreal minimumDistanceSq = vec.x() * vec.x() + vec.y() * vec.y();
		qreal distanceSq;

		for(auto pointIter = itemPoints.begin() + 1; pointIter != itemPoints.end(); pointIter++)
		{
			vec = ((*pointIter)->position() - itemPos);
			distanceSq = vec.x() * vec.x() + vec.y() * vec.y();

			if (distanceSq < minimumDistanceSq)
			{
				itemPoint = *pointIter;
				minimumDistanceSq = distanceSq;
			}
		}
	}

	return itemPoint;
}

DrawingItemPoint* DrawingItem::itemPointToInsert(const QPointF& itemPos, int& index)
{
	Q_UNUSED(itemPos);
	Q_UNUSED(index);
	return nullptr;
}

DrawingItemPoint* DrawingItem::itemPointToRemove(const QPointF& itemPos)
{
	Q_UNUSED(itemPos);
	return nullptr;
}

//==================================================================================================

void DrawingItem::addChild(DrawingItem* item)
{
	if (item && item->mParent == nullptr)
	{
		mChildren.append(item);
		item->mParent = this;
	}
}

void DrawingItem::insertChild(int index, DrawingItem* item)
{
	if (item && item->mParent == nullptr)
	{
		mChildren.insert(index, item);
		item->mParent = this;
	}
}

void DrawingItem::removeChild(DrawingItem* item)
{
	if (item && item->mParent == this)
	{
		mChildren.removeAll(item);
		item->mParent = nullptr;
	}
}

void DrawingItem::clearChildren()
{
	DrawingItem* item = nullptr;

	while (!mPoints.empty())
	{
		item = mChildren.first();
		removeChild(item);
		delete item;
		item = nullptr;
	}
}

QList<DrawingItem*> DrawingItem::children() const
{
	return mChildren;
}

DrawingItem* DrawingItem::parent() const
{
	return mParent;
}

//==================================================================================================

void DrawingItem::setVisible(bool visible)
{
	mVisible = visible;
}

void DrawingItem::setSelected(bool selected)
{
	mSelected = selected;
}

bool DrawingItem::isVisible() const
{
	return mVisible;
}

bool DrawingItem::isSelected() const
{
	return mSelected;
}

//==================================================================================================

QPointF DrawingItem::mapFromParent(const QPointF& point) const
{
	return mTransform.map(point - mPosition);
}

QPolygonF DrawingItem::mapFromParent(const QRectF& rect) const
{
	return mapFromParent(QPolygonF(rect));
}

QPolygonF DrawingItem::mapFromParent(const QPolygonF& polygon) const
{
	QPolygonF poly = polygon;
	poly.translate(-mPosition);
	return mTransform.map(poly);
}

QPainterPath DrawingItem::mapFromParent(const QPainterPath& path) const
{
	QPainterPath painterPath = path;
	painterPath.translate(-mPosition);
	return mTransform.map(painterPath);
}

QPointF DrawingItem::mapToParent(const QPointF& point) const
{
	return mTransformInverse.map(point) + mPosition;
}

QPolygonF DrawingItem::mapToParent(const QRectF& rect) const
{
	return mapToParent(QPolygonF(rect));
}

QPolygonF DrawingItem::mapToParent(const QPolygonF& polygon) const
{
	QPolygonF poly = mTransformInverse.map(polygon);
	poly.translate(mPosition);
	return poly;
}

QPainterPath DrawingItem::mapToParent(const QPainterPath& path) const
{
	QPainterPath painterPath = mTransformInverse.map(path);
	painterPath.translate(mPosition);
	return painterPath;
}

//==================================================================================================

QPointF DrawingItem::mapFromScene(const QPointF& point) const
{
	return (mParent) ? mapFromParent(mParent->mapFromScene(point)) : mapFromParent(point);
}

QPolygonF DrawingItem::mapFromScene(const QRectF& rect) const
{
	return (mParent) ? mapFromParent(mParent->mapFromScene(rect)) : mapFromParent(rect);
}

QPolygonF DrawingItem::mapFromScene(const QPolygonF& polygon) const
{
	return (mParent) ? mapFromParent(mParent->mapFromScene(polygon)) : mapFromParent(polygon);
}

QPainterPath DrawingItem::mapFromScene(const QPainterPath& path) const
{
	return (mParent) ? mapFromParent(mParent->mapFromScene(path)) : mapFromParent(path);
}

QPointF DrawingItem::mapToScene(const QPointF& point) const
{
	return (mParent) ? mParent->mapToScene(mapToParent(point)) : mapToParent(point);
}

QPolygonF DrawingItem::mapToScene(const QRectF& rect) const
{
	return (mParent) ? mParent->mapToScene(mapToParent(rect)) : mapToParent(rect);
}

QPolygonF DrawingItem::mapToScene(const QPolygonF& polygon) const
{
	return (mParent) ? mParent->mapToScene(mapToParent(polygon)) : mapToParent(polygon);
}

QPainterPath DrawingItem::mapToScene(const QPainterPath& path) const
{
	return (mParent) ? mParent->mapToScene(mapToParent(path)) : mapToParent(path);
}

//==================================================================================================

QPainterPath DrawingItem::shape() const
{
	QPainterPath path;
	path.addRect(boundingRect());
	return path;
}

QPointF DrawingItem::centerPos() const
{
	return boundingRect().center();
}

bool DrawingItem::isValid() const
{
	return boundingRect().isValid();
}

//==================================================================================================

void DrawingItem::moveEvent(const QPointF& parentPos)
{
	mPosition = parentPos;
}

void DrawingItem::resizeEvent(DrawingItemPoint* itemPoint, const QPointF& parentPos)
{
	if (itemPoint)
	{
		itemPoint->setPosition(mapFromParent(parentPos));

		if (mFlags & AdjustPositionOnResize)
		{
			// Adjust position of item and item points so that point(0)->position() == QPointF(0, 0)
			QPointF deltaPos = -mPoints.first()->position();
			QPointF pointParentPos = mapFromParent(mPoints.first()->position());

			for(auto pointIter = mPoints.begin(); pointIter != mPoints.end(); pointIter++)
				(*pointIter)->setPosition((*pointIter)->position() + deltaPos);

			for(auto childIter = mChildren.begin(); childIter != mChildren.end(); childIter++)
				(*childIter)->setPosition((*childIter)->position() + deltaPos);

			setPosition(pointParentPos);
		}
	}
}

void DrawingItem::rotateEvent(const QPointF& parentPos)
{
	QPointF difference(mPosition - parentPos);

	// Calculate new position of reference point
	mPosition = QPointF(parentPos.x() + difference.y(), parentPos.y() - difference.x());

	// Update orientation
	mTransform.rotate(90);
	mTransformInverse = mTransform.inverted();
}

void DrawingItem::rotateBackEvent(const QPointF& parentPos)
{
	QPointF difference(mPosition - parentPos);

	// Calculate new position of reference point
	mPosition = QPointF(parentPos.x() - difference.y(), parentPos.y() + difference.x());

	// Update orientation
	mTransform.rotate(-90);
	mTransformInverse = mTransform.inverted();
}

void DrawingItem::flipHorizontalEvent(const QPointF& parentPos)
{
	// Calculate new position of reference point
	mPosition.setX(2 * parentPos.x() - mPosition.x());

	// Update orientation
	mTransform.scale(-1, 1);
	mTransformInverse = mTransform.inverted();
}

void DrawingItem::flipVerticalEvent(const QPointF& parentPos)
{
	// Calculate new position of reference point
	mPosition.setY(2 * parentPos.y() - mPosition.y());

	// Update orientation
	mTransform.scale(1, -1);
	mTransformInverse = mTransform.inverted();
}

void DrawingItem::keyPressEvent(QKeyEvent* event)
{
	Q_UNUSED(event);
}

void DrawingItem::keyReleaseEvent(QKeyEvent* event)
{
	Q_UNUSED(event);
}

//==================================================================================================

QPainterPath DrawingItem::strokePath(const QPainterPath& path, const QPen& pen) const
{
	if (path == QPainterPath()) return path;

	QPainterPathStroker ps;
	const qreal penWidthZero = qreal(0.00000001);

	if (pen.widthF() <= 0.0)
		ps.setWidth(penWidthZero);
	else
		ps.setWidth(pen.widthF());

	//ps.setCapStyle(pen.capStyle());
	//ps.setJoinStyle(pen.joinStyle());
	//ps.setMiterLimit(pen.miterLimit());

	ps.setCapStyle(Qt::SquareCap);
	ps.setJoinStyle(Qt::BevelJoin);

	return ps.createStroke(path);
}

//==================================================================================================

QList<DrawingItem*> DrawingItem::copyItems(const QList<DrawingItem*>& items)
{
	QList<DrawingItem*> copiedItems;
	QList<DrawingItemPoint*> itemPoints;
	QList<DrawingItemPoint*> targetPoints;
	DrawingItem* targetItem;
	DrawingItem* copiedTargetItem;
	DrawingItemPoint* copiedTargetPoint;
	DrawingItemPoint* copiedPoint;

	// Copy items
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		copiedItems.append((*itemIter)->copy());

	// Maintain connections to other items in this list
	for(int itemIndex = 0; itemIndex < items.size(); itemIndex++)
	{
		itemPoints = items[itemIndex]->points();
		for(int pointIndex = 0; pointIndex < itemPoints.size(); pointIndex++)
		{
			targetPoints = itemPoints[pointIndex]->connections();
			for(auto targetIter = targetPoints.begin(); targetIter != targetPoints.end(); targetIter++)
			{
				targetItem = (*targetIter)->item();
				if (items.contains(targetItem))
				{
					// There is a connection here that must be maintained in the copied items
					copiedPoint = copiedItems[itemIndex]->points().at(pointIndex);

					copiedTargetItem = copiedItems[items.indexOf(targetItem)];
					copiedTargetPoint =
						copiedTargetItem->points().at(targetItem->points().indexOf(*targetIter));

					if (copiedPoint && copiedTargetPoint)
					{
						copiedPoint->addConnection(copiedTargetPoint);
						copiedTargetPoint->addConnection(copiedPoint);
					}
				}
			}
		}
	}

	return copiedItems;
}
