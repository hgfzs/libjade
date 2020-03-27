/* DrawingItem.cpp
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

#include "DrawingItem.h"
#include "DrawingItemPoint.h"

DrawingItem::DrawingItem()
{
	mScene = nullptr;

	mFlags = (CanMove | CanResize | CanRotate | CanFlip | CanSelect | CanDelete);

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

	for(auto pointIter = item.mPoints.begin(), pointEnd = item.mPoints.end();
		pointIter != pointEnd; pointIter++)
	{
		addPoint(new DrawingItemPoint(**pointIter));
	}

	mSelected = false;
	mVisible = true;
}

DrawingItem::~DrawingItem()
{
	clearPoints();
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

void DrawingItem::addPoint(DrawingItemPoint* point)
{
	if (point && point->mItem == nullptr)
	{
		mPoints.append(point);
		point->mItem = this;
	}
}

void DrawingItem::insertPoint(int index, DrawingItemPoint* point)
{
	if (point && point->mItem == nullptr)
	{
		mPoints.insert(index, point);
		point->mItem = this;
	}
}

void DrawingItem::removePoint(DrawingItemPoint* point)
{
	if (point && point->mItem == this)
	{
		mPoints.removeAll(point);
		point->mItem = nullptr;
	}
}

void DrawingItem::clearPoints()
{
	DrawingItemPoint* point = nullptr;

	while (!mPoints.empty())
	{
		point = mPoints.first();
		removePoint(point);
		delete point;
	}
}

QList<DrawingItemPoint*> DrawingItem::points() const
{
	return mPoints;
}

//==================================================================================================

DrawingItemPoint* DrawingItem::pointAt(const QPointF& pos) const
{
	DrawingItemPoint* point = nullptr;

	QList<DrawingItemPoint*> itemPoints = points();
	for(auto pointIter = itemPoints.begin();
		point == nullptr && pointIter != itemPoints.end(); pointIter++)
	{
		if (pos == (*pointIter)->position()) point = *pointIter;
	}

	return point;
}

DrawingItemPoint* DrawingItem::pointNearest(const QPointF& pos) const
{
	DrawingItemPoint *point = nullptr;

	QList<DrawingItemPoint*> itemPoints = points();
	if (!itemPoints.isEmpty())
	{
		point = itemPoints.first();

		QPointF vec = (point->position() - pos);
		qreal minimumDistanceSq = vec.x() * vec.x() + vec.y() * vec.y();
		qreal distanceSq;

		for(auto pointIter = itemPoints.begin() + 1; pointIter != itemPoints.end(); pointIter++)
		{
			vec = ((*pointIter)->position() - pos);
			distanceSq = vec.x() * vec.x() + vec.y() * vec.y();

			if (distanceSq < minimumDistanceSq)
			{
				point = *pointIter;
				minimumDistanceSq = distanceSq;
			}
		}
	}

	return point;
}

DrawingItemPoint* DrawingItem::itemPointToInsert(const QPointF& pos, int& index)
{
	Q_UNUSED(pos)
	Q_UNUSED(index)
	return nullptr;
}

DrawingItemPoint* DrawingItem::itemPointToRemove(const QPointF& pos)
{
	Q_UNUSED(pos)
	return nullptr;
}

//==================================================================================================

void DrawingItem::setSelected(bool selected)
{
	mSelected = selected;
}

void DrawingItem::setVisible(bool visible)
{
	mVisible = visible;
}

bool DrawingItem::isSelected() const
{
	return mSelected;
}

bool DrawingItem::isVisible() const
{
	return mVisible;
}

//==================================================================================================

QPointF DrawingItem::mapFromScene(const QPointF& point) const
{
	return mTransform.map(point - mPosition);
}

QPolygonF DrawingItem::mapFromScene(const QRectF& rect) const
{
	return mapFromScene(QPolygonF(rect));
}

QPolygonF DrawingItem::mapFromScene(const QPolygonF& polygon) const
{
	QPolygonF poly = polygon;
	poly.translate(-mPosition);
	return mTransform.map(poly);
}

QPainterPath DrawingItem::mapFromScene(const QPainterPath& path) const
{
	QPainterPath painterPath = path;
	painterPath.translate(-mPosition);
	return mTransform.map(painterPath);
}

QPointF DrawingItem::mapToScene(const QPointF& point) const
{
	return mTransformInverse.map(point) + mPosition;
}

QPolygonF DrawingItem::mapToScene(const QRectF& rect) const
{
	return mapToScene(QPolygonF(rect));
}

QPolygonF DrawingItem::mapToScene(const QPolygonF& polygon) const
{
	QPolygonF poly = mTransformInverse.map(polygon);
	poly.translate(mPosition);
	return poly;
}

QPainterPath DrawingItem::mapToScene(const QPainterPath& path) const
{
	QPainterPath painterPath = mTransformInverse.map(path);
	painterPath.translate(mPosition);
	return painterPath;
}

//==================================================================================================

void DrawingItem::setProperties(const QHash<QString,QVariant>& properties)
{
	Q_UNUSED(properties)
}

QHash<QString,QVariant> DrawingItem::properties() const
{
	return QHash<QString,QVariant>();
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

void DrawingItem::move(const QPointF& pos)
{
	mPosition = pos;
}

void DrawingItem::resize(DrawingItemPoint* point, const QPointF& pos)
{
	if (point)
	{
		point->setPosition(mapFromScene(pos));

		// Adjust position of item and item points so that point(0)->position() == QPointF(0, 0)
		QPointF deltaPos = -mPoints.first()->position();
		QPointF pointParentPos = mapToScene(mPoints.first()->position());

		for(auto pointIter = mPoints.begin(); pointIter != mPoints.end(); pointIter++)
			(*pointIter)->setPosition((*pointIter)->position() + deltaPos);

		setPosition(pointParentPos);
	}
}

void DrawingItem::rotate(const QPointF& pos)
{
	QPointF difference(mPosition - pos);

	// Calculate new position of reference point
	mPosition = QPointF(pos.x() + difference.y(), pos.y() - difference.x());

	// Update orientation
	mTransform.rotate(90);
	mTransformInverse = mTransform.inverted();
}

void DrawingItem::rotateBack(const QPointF& pos)
{
	QPointF difference(mPosition - pos);

	// Calculate new position of reference point
	mPosition = QPointF(pos.x() - difference.y(), pos.y() + difference.x());

	// Update orientation
	mTransform.rotate(-90);
	mTransformInverse = mTransform.inverted();
}

void DrawingItem::flipHorizontal(const QPointF& pos)
{
	// Calculate new position of reference point
	mPosition.setX(2 * pos.x() - mPosition.x());

	// Update orientation
	mTransform.scale(-1, 1);
	mTransformInverse = mTransform.inverted();
}

void DrawingItem::flipVertical(const QPointF& pos)
{
	// Calculate new position of reference point
	mPosition.setY(2 * pos.y() - mPosition.y());

	// Update orientation
	mTransform.scale(1, -1);
	mTransformInverse = mTransform.inverted();
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
			for(auto targetIter = targetPoints.begin();
				targetIter != targetPoints.end(); targetIter++)
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
