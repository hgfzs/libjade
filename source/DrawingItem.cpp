/* DrawingItem.cpp
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

#include "DrawingItem.h"
#include "DrawingWidget.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"

DrawingItem::DrawingItem()
{
	mDrawing = nullptr;

	mFlags = (CanMove | CanResize | CanRotate | CanFlip | CanSelect);

	mRotation = 0;
	mFlipped = false;
	recalculateTransform();

	mSelected = false;
	mVisible = true;

	mStyle = new DrawingItemStyle();
}

DrawingItem::DrawingItem(const DrawingItem& item)
{
	mDrawing = nullptr;

	mPos = item.mPos;
	mFlags = item.mFlags;

	mRotation = item.mRotation;
	mFlipped = item.mFlipped;
	recalculateTransform();

	mSelected = false;
	mVisible = true;

	for(auto pointIter = item.mPoints.begin(); pointIter != item.mPoints.end(); pointIter++)
		addPoint(new DrawingItemPoint(**pointIter));

	mStyle = new DrawingItemStyle(*item.mStyle);
}

DrawingItem::~DrawingItem()
{
	delete mStyle;

	clearPoints();

	if (mDrawing) mDrawing->removeItem(this);
	mDrawing = nullptr;
}

//==================================================================================================

DrawingWidget* DrawingItem::drawing() const
{
	return mDrawing;
}

//==================================================================================================

void DrawingItem::setPos(const QPointF& pos)
{
	mPos = pos;
}

void DrawingItem::setPos(qreal x, qreal y)
{
	setPos(QPointF(x, y));
}

void DrawingItem::setX(qreal x)
{
	setPos(QPointF(x, y()));
}

void DrawingItem::setY(qreal y)
{
	setPos(QPointF(x(), y));
}

QPointF DrawingItem::pos() const
{
	return mPos;
}

qreal DrawingItem::x() const
{
	return mPos.x();
}

qreal DrawingItem::y() const
{
	return mPos.y();
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

void DrawingItem::setRotation(qreal angle)
{
	mRotation = angle;
	while (mRotation >= 360.0) mRotation -= 360.0;
	while (mRotation < 0.0) mRotation += 360.0;
	recalculateTransform();
}

void DrawingItem::setFlipped(bool flipped)
{
	mFlipped = flipped;
	recalculateTransform();
}

qreal DrawingItem::rotation() const
{
	return mRotation;
}

bool DrawingItem::isFlipped() const
{
	return mFlipped;
}

QTransform DrawingItem::transform() const
{
	return mTransform;
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

void DrawingItem::addPoint(DrawingItemPoint* itemPoint)
{
	if (itemPoint && !mPoints.contains(itemPoint))
	{
		mPoints.append(itemPoint);
		itemPoint->mItem = this;
	}
}

void DrawingItem::insertPoint(int index, DrawingItemPoint* itemPoint)
{
	if (itemPoint && !mPoints.contains(itemPoint))
	{
		mPoints.insert(index, itemPoint);
		itemPoint->mItem = this;
	}
}

void DrawingItem::removePoint(DrawingItemPoint* itemPoint)
{
	if (itemPoint && mPoints.contains(itemPoint))
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

	if (mDrawing)
	{
		QRectF pointItemRect;

		for(auto pointIter = mPoints.begin(); itemPoint == nullptr && pointIter != mPoints.end(); pointIter++)
		{
			pointItemRect = mapFromScene(mDrawing->mapToScene(mDrawing->pointRect(*pointIter)));
			if (pointItemRect.contains(itemPos)) itemPoint = *pointIter;
		}
	}

	return itemPoint;
}

DrawingItemPoint* DrawingItem::pointNearest(const QPointF& itemPos) const
{
	DrawingItemPoint *itemPoint = nullptr;

	if (!mPoints.isEmpty())
	{
		itemPoint = mPoints.first();

		QPointF vec = (itemPoint->pos() - itemPos);
		qreal minimumDistanceSq = vec.x() * vec.x() + vec.y() * vec.y();
		qreal distanceSq;

		for(auto pointIter = mPoints.begin() + 1; pointIter != mPoints.end(); pointIter++)
		{
			vec = ((*pointIter)->pos() - itemPos);
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

QPointF DrawingItem::mapFromScene(const QPointF& point) const
{
	return mTransform.map(point - mPos);
}

QRectF DrawingItem::mapFromScene(const QRectF& rect) const
{
	return mapFromScene(QPolygonF(rect)).boundingRect();
}

QPolygonF DrawingItem::mapFromScene(const QPolygonF& polygon) const
{
	QPolygonF poly = polygon;
	poly.translate(-mPos);
	return mTransform.map(poly);
}

QPainterPath DrawingItem::mapFromScene(const QPainterPath& path) const
{
	QPainterPath painterPath = path;
	painterPath.translate(-mPos);
	return mTransform.map(painterPath);
}

QPointF DrawingItem::mapToScene(const QPointF& point) const
{
	return mTransformInverse.map(point) + mPos;
}

QRectF DrawingItem::mapToScene(const QRectF& rect) const
{
	return mapToScene(QPolygonF(rect)).boundingRect();
}

QPolygonF DrawingItem::mapToScene(const QPolygonF& polygon) const
{
	QPolygonF poly = mTransformInverse.map(polygon);
	poly.translate(mPos);
	return poly;
}

QPainterPath DrawingItem::mapToScene(const QPainterPath& path) const
{
	QPainterPath painterPath = mTransformInverse.map(path);
	painterPath.translate(mPos);
	return painterPath;
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

void DrawingItem::moveItem(const QPointF& scenePos)
{
	setPos(scenePos);
}

void DrawingItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	if (itemPoint) itemPoint->setPos(mapFromScene(scenePos));
}

void DrawingItem::rotateItem(const QPointF& scenePos)
{
	QPointF difference(mPos - scenePos);

	// Calculate new position of reference point
	setPos(scenePos.x() - difference.y(), scenePos.y() + difference.x());

	// Update orientation
	if (isFlipped()) setRotation(rotation() - 90.0);
	else setRotation(rotation() + 90.0);
}

void DrawingItem::rotateBackItem(const QPointF& scenePos)
{
	QPointF difference(mPos - scenePos);

	// Calculate new position of reference point
	setPos(scenePos.x() + difference.y(), scenePos.y() - difference.x());

	// Update orientation
	if (isFlipped()) setRotation(rotation() + 90.0);
	else setRotation(rotation() - 90.0);
}

void DrawingItem::flipItem(const QPointF& scenePos)
{
	// Calculate new position of reference point
	setPos(2 * scenePos.x() - x(), y());

	// Update orientation
	setFlipped(!isFlipped());
}

void DrawingItem::insertItemPoint(const QPointF& scenePos)
{
	Q_UNUSED(scenePos);
}

void DrawingItem::removeItemPoint(const QPointF& scenePos)
{
	Q_UNUSED(scenePos);
}

//==================================================================================================

void DrawingItem::mousePressEvent(DrawingMouseEvent* event)
{
	Q_UNUSED(event);
}

void DrawingItem::mouseMoveEvent(DrawingMouseEvent* event)
{
	Q_UNUSED(event);
}

void DrawingItem::mouseReleaseEvent(DrawingMouseEvent* event)
{
	Q_UNUSED(event);
}

void DrawingItem::mouseDoubleClickEvent(DrawingMouseEvent* event)
{
	Q_UNUSED(event);
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

void DrawingItem::newMousePressEvent(DrawingMouseEvent* event)
{
	if (mDrawing && mDrawing->mode() == DrawingWidget::PlaceMode)
		setPos(mDrawing->roundToGrid(event->scenePos()));
}

void DrawingItem::newMouseMoveEvent(DrawingMouseEvent* event)
{
	if (mDrawing && mDrawing->mode() == DrawingWidget::PlaceMode)
		setPos(mDrawing->roundToGrid(event->scenePos()));
}

bool DrawingItem::newMouseReleaseEvent(DrawingMouseEvent* event)
{
	if (mDrawing && mDrawing->mode() == DrawingWidget::PlaceMode)
		setPos(mDrawing->roundToGrid(event->scenePos()));
	
	return true;
}

bool DrawingItem::newMouseDoubleClickEvent(DrawingMouseEvent* event)
{
	if (mDrawing && mDrawing->mode() == DrawingWidget::PlaceMode)
		setPos(mDrawing->roundToGrid(event->scenePos()));
	
	return true;
}

bool DrawingItem::newItemCopyEvent()
{
	return true;
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

qreal DrawingItem::minimumPenWidth() const
{
	qreal width = 0;

	if (mDrawing)
	{
		QSize sizeHint = mDrawing->pointSizeHint();
		QPointF mappedSize = mDrawing->mapToScene(
			QPoint(sizeHint.width(), sizeHint.height())) - mDrawing->mapToScene(QPoint(0, 0));
		width = qMax(qAbs(mappedSize.x()), qAbs(mappedSize.y()));
	}

	return width;
}

//==================================================================================================

void DrawingItem::recalculateTransform()
{
	mTransform = QTransform();
	mTransform = mTransform.rotate(-mRotation);
	if (mFlipped) mTransform = mTransform.scale(-1.0, 1.0);

	mTransformInverse = mTransform.inverted();
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
