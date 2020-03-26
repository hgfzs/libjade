/* DrawingScene.cpp
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

#include "DrawingScene.h"
#include "DrawingView.h"
#include "DrawingItem.h"
#include "DrawingItemStyle.h"
#include "DrawingItemPoint.h"

DrawingScene::DrawingScene() : QObject()
{
	mSceneRect = QRectF(0, 0, 11000, 8500);
	mBackgroundBrush = Qt::white;
}

DrawingScene::~DrawingScene()
{
	clearItems();
}

//==================================================================================================

void DrawingScene::setSceneRect(const QRectF& rect)
{
	mSceneRect = rect;
}

void DrawingScene::setSceneRect(qreal left, qreal top, qreal width, qreal height)
{
	mSceneRect = QRectF(left, top, width, height);
}

QRectF DrawingScene::sceneRect() const
{
	return mSceneRect;
}

//==================================================================================================

void DrawingScene::setBackgroundBrush(const QBrush& brush)
{
	mBackgroundBrush = brush;
}

QBrush DrawingScene::backgroundBrush() const
{
	return mBackgroundBrush;
}

//==================================================================================================

void DrawingScene::addItem(DrawingItem* item)
{
	if (item && item->mScene == nullptr)
	{
		mItems.append(item);
		item->mScene = this;
	}
}

void DrawingScene::insertItem(int index, DrawingItem* item)
{
	if (item && item->mScene == nullptr)
	{
		mItems.insert(index, item);
		item->mScene = this;
	}
}

void DrawingScene::removeItem(DrawingItem* item)
{
	if (item && item->mScene == this)
	{
		mItems.removeAll(item);
		item->mScene = nullptr;
	}
}

void DrawingScene::clearItems()
{
	DrawingItem* item = nullptr;

	while (!mItems.empty())
	{
		item = mItems.first();
		removeItem(item);
		delete item;
		item = nullptr;
	}
}

void DrawingScene::setItems(const QList<DrawingItem*>& items)
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		(*itemIter)->mScene = nullptr;
		if (!items.contains(*itemIter)) delete *itemIter;
	}

	mItems = items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->mScene = this;
}

QList<DrawingItem*> DrawingScene::items() const
{
	return mItems;
}

//==================================================================================================

QList<DrawingItem*> DrawingScene::visibleItems() const
{
	QList<DrawingItem*> foundItems;
	findItems(mItems, foundItems);
	return foundItems;
}

QList<DrawingItem*> DrawingScene::visibleItems(const DrawingView* view, const QPointF& pos) const
{
	QList<DrawingItem*> items;
	QList<DrawingItem*> visibleItems = DrawingScene::visibleItems();

	for(auto itemIter = visibleItems.begin(); itemIter != visibleItems.end(); itemIter++)
	{
		if (itemMatchesPoint(view, *itemIter, pos)) items.append(*itemIter);
	}

	return items;
}

QList<DrawingItem*> DrawingScene::visibleItems(const DrawingView* view, const QRectF& rect, Qt::ItemSelectionMode selectMode) const
{
	QList<DrawingItem*> items;
	QList<DrawingItem*> visibleItems = DrawingScene::visibleItems();

	for(auto itemIter = visibleItems.begin(); itemIter != visibleItems.end(); itemIter++)
	{
		if (itemMatchesRect(view, *itemIter, rect, selectMode))
			items.append(*itemIter);
	}

	return items;
}

QList<DrawingItem*> DrawingScene::visibleItems(const DrawingView* view, const QPainterPath& path, Qt::ItemSelectionMode selectMode) const
{
	QList<DrawingItem*> items;
	QList<DrawingItem*> visibleItems = DrawingScene::visibleItems();

	for(auto itemIter = visibleItems.begin(); itemIter != visibleItems.end(); itemIter++)
	{
		if (itemMatchesPath(view, *itemIter, path, selectMode))
			items.append(*itemIter);
	}

	return items;
}

DrawingItem* DrawingScene::visibleItemAt(const DrawingView* view, const QPointF& pos) const
{
	DrawingItem* item = nullptr;
	QList<DrawingItem*> visibleItems = DrawingScene::visibleItems();

	auto itemIter = visibleItems.end();
	while (item == nullptr && itemIter != visibleItems.begin())
	{
		itemIter--;
		if (itemMatchesPoint(view, *itemIter, pos)) item = *itemIter;
	}

	return item;
}

//==================================================================================================

void DrawingScene::render(QPainter* painter)
{
	drawBackground(painter);
	drawItems(painter);
	drawForeground(painter);
}

//==================================================================================================

void DrawingScene::addItems(const QList<DrawingItem*>& items)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		addItem(*itemIter);

	emit numberOfItemsChanged(mItems.size());
}

void DrawingScene::insertItems(const QList<DrawingItem*>& items, const QHash<DrawingItem*,int>& index)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		insertItem(index[*itemIter], *itemIter);

	emit numberOfItemsChanged(mItems.size());
}

void DrawingScene::removeItems(const QList<DrawingItem*>& items)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		removeItem(*itemIter);

	emit numberOfItemsChanged(mItems.size());
}

//==================================================================================================

void DrawingScene::setItemsVisibility(const QList<DrawingItem*>& items, const QHash<DrawingItem*,bool>& visibility)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->setVisible(visibility[*itemIter]);

	emit itemsVisibilityChanged(items);
}

//==================================================================================================

void DrawingScene::moveItems(const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& parentPos)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->moveEvent(parentPos[*itemIter]);

	emit itemsPositionChanged(items);
}

void DrawingScene::resizeItem(DrawingItemPoint* itemPoint, const QPointF& parentPos)
{
	if (itemPoint && itemPoint->item())
	{
		QList<DrawingItem*> items;
		items.append(itemPoint->item());

		itemPoint->item()->resizeEvent(itemPoint, parentPos);

		emit itemsGeometryChanged(items);
	}
}

//==================================================================================================

void DrawingScene::rotateItems(const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& parentPos)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->rotateEvent(parentPos[*itemIter]);

	emit itemsTransformChanged(items);
}

void DrawingScene::rotateBackItems(const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& parentPos)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->rotateBackEvent(parentPos[*itemIter]);

	emit itemsTransformChanged(items);
}

void DrawingScene::flipItemsHorizontal(const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& parentPos)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->flipHorizontalEvent(parentPos[*itemIter]);

	emit itemsTransformChanged(items);
}

void DrawingScene::flipItemsVertical(const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& parentPos)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->flipVerticalEvent(parentPos[*itemIter]);

	emit itemsTransformChanged(items);
}

//==================================================================================================

void DrawingScene::insertItemPoint(DrawingItem* item, DrawingItemPoint* itemPoint, int pointIndex)
{
	if (item && itemPoint)
	{
		QList<DrawingItem*> items;
		items.append(item);

		item->insertPoint(pointIndex, itemPoint);

		emit itemsGeometryChanged(items);
	}
}

void DrawingScene::removeItemPoint(DrawingItem* item, DrawingItemPoint* itemPoint)
{
	if (item && itemPoint)
	{
		QList<DrawingItem*> items;
		items.append(item);

		item->removePoint(itemPoint);

		emit itemsGeometryChanged(items);
	}
}

//==================================================================================================

void DrawingScene::connectItemPoints(DrawingItemPoint* point1, DrawingItemPoint* point2)
{
	if (point1 && point2)
	{
		point1->addConnection(point2);
		point2->addConnection(point1);
	}
}

void DrawingScene::disconnectItemPoints(DrawingItemPoint* point1, DrawingItemPoint* point2)
{
	if (point1 && point2)
	{
		point1->removeConnection(point2);
		point2->removeConnection(point1);
	}
}

//==================================================================================================

void DrawingScene::drawBackground(QPainter* painter)
{
	QPainter::RenderHints hints = painter->renderHints();

	QColor backgroundColor = mBackgroundBrush.color();
	QColor borderColor(255 - backgroundColor.red(), 255 - backgroundColor.green(),
		255 - backgroundColor.blue());
	QPen borderPen(borderColor, 1);
	borderPen.setCosmetic(true);

	painter->setRenderHints(hints, false);
	painter->setBrush(mBackgroundBrush);
	painter->setPen(borderPen);
	painter->drawRect(mSceneRect);
	painter->setRenderHints(hints, true);
}

void DrawingScene::drawItems(QPainter* painter)
{
	drawItems(painter, mItems);
}

void DrawingScene::drawForeground(QPainter* painter)
{
	Q_UNUSED(painter);
}

//==================================================================================================

void DrawingScene::findItems(const QList<DrawingItem*>& items, QList<DrawingItem*>& foundItems) const
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			foundItems.append(*itemIter);
		}
	}
}

void DrawingScene::drawItems(QPainter* painter, const QList<DrawingItem*>& items)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			painter->translate((*itemIter)->position());
			painter->setTransform((*itemIter)->transformInverted(), true);

			(*itemIter)->render(painter);

			//painter->save();
			//painter->setBrush(QColor(255, 0, 255, 128));
			//painter->setPen(QPen(QColor(255, 0, 255, 128), 1));
			//painter->drawPath(itemAdjustedShape(*itemIter));
			//painter->restore();

			painter->setTransform((*itemIter)->transform(), true);
			painter->translate(-(*itemIter)->position());
		}
	}
}

//==================================================================================================

bool DrawingScene::itemMatchesPoint(const DrawingView* view, DrawingItem* item, const QPointF& scenePos) const
{
	bool match = false;

	if (view && item && item->isVisible())
	{
		// Check item shape
		match = itemAdjustedShape(view, item).contains(item->mapFromScene(scenePos));

		// Check item points
		if (!match && item->isSelected())
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			QRectF pointSceneRect;

			for(auto pointIter = itemPoints.begin(); !match && pointIter != itemPoints.end(); pointIter++)
			{
				pointSceneRect = view->mapToScene(view->pointRect(*pointIter));
				match = pointSceneRect.contains(scenePos);
			}
		}
	}

	return match;
}

bool DrawingScene::itemMatchesRect(const DrawingView* view, DrawingItem* item, const QRectF& rect, Qt::ItemSelectionMode mode) const
{
	bool match = false;

	if (item && item->isVisible())
	{
		// Check item boundingRect or shape
		switch (mode)
		{
		case Qt::IntersectsItemShape:
			match = item->shape().intersects(item->mapFromScene(rect).boundingRect());
			break;
		case Qt::ContainsItemShape:
			match = rect.contains(item->mapToScene(item->shape().boundingRect()).boundingRect());
			break;
		case Qt::IntersectsItemBoundingRect:
			match = rect.intersects(item->mapToScene(item->boundingRect()).boundingRect());
			break;
		default:	// Qt::ContainsItemBoundingRect
			match = rect.contains(item->mapToScene(item->boundingRect()).boundingRect());
			break;
		}

		// Check item points
		if (!match && item->isSelected())
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			QRectF pointSceneRect;

			for(auto pointIter = itemPoints.begin(); !match && pointIter != itemPoints.end(); pointIter++)
			{
				pointSceneRect = view->mapToScene(view->pointRect(*pointIter));

				if (mode == Qt::IntersectsItemBoundingRect || mode == Qt::IntersectsItemShape)
					match = rect.intersects(pointSceneRect);
				else
					match = rect.contains(pointSceneRect);
			}
		}
	}

	return match;
}

bool DrawingScene::itemMatchesPath(const DrawingView* view, DrawingItem* item, const QPainterPath& path, Qt::ItemSelectionMode mode) const
{
	bool match = false;

	if (item && item->isVisible())
	{
		// Check item boundingRect or shape
		switch (mode)
		{
		case Qt::IntersectsItemShape:
			match = item->shape().intersects(item->mapFromScene(path));
			break;
		case Qt::ContainsItemShape:
			match = path.contains(item->mapToScene(item->shape().boundingRect()).boundingRect());
			break;
		case Qt::IntersectsItemBoundingRect:
			match = path.intersects(item->mapToScene(item->boundingRect()).boundingRect());
			break;
		default:	// Qt::ContainsItemBoundingRect
			match = path.contains(item->mapToScene(item->boundingRect()).boundingRect());
			break;
		}

		// Check item points
		if (!match && item->isSelected())
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			QRectF pointSceneRect;

			for(auto pointIter = itemPoints.begin(); !match && pointIter != itemPoints.end(); pointIter++)
			{
				pointSceneRect = view->mapToScene(view->pointRect(*pointIter));

				if (mode == Qt::IntersectsItemBoundingRect || mode == Qt::IntersectsItemShape)
					match = path.intersects(pointSceneRect);
				else
					match = path.contains(pointSceneRect);
			}
		}
	}

	return match;
}

QPainterPath DrawingScene::itemAdjustedShape(const DrawingView* view, DrawingItem* item) const
{
	QPainterPath adjustedShape;

	if (view && item)
	{
		DrawingItemStyle* style = item->style();
		if (style)
		{
			qreal penWidth = 0;

			// Determine existing item's pen width
			QVariant value = style->valueLookup(DrawingItemStyle::PenWidth);
			if (value.isValid()) penWidth = value.toDouble();

			// Determine minimum pen width
			qreal minimumPenWidth = view->minimumPenWidth(item);

			if (0 < penWidth && penWidth < minimumPenWidth)
			{
				// Make it easier to select items when zoomed out by increasing pen width
				bool styleHadPenWidth = style->hasValue(DrawingItemStyle::PenWidth);
				if (styleHadPenWidth) penWidth = style->value(DrawingItemStyle::PenWidth).toDouble();

				style->setValue(DrawingItemStyle::PenWidth, QVariant(minimumPenWidth));
				adjustedShape = item->shape();

				if (styleHadPenWidth) style->setValue(DrawingItemStyle::PenWidth, QVariant(penWidth));
				else style->unsetValue(DrawingItemStyle::PenWidth);
			}
			else adjustedShape = item->shape();
		}
		else adjustedShape = item->shape();
	}

	return adjustedShape;
}
