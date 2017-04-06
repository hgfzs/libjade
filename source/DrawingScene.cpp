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
	QColor backgroundColor = mBackgroundBrush.color();
	QColor borderColor(255 - backgroundColor.red(), 255 - backgroundColor.green(),
		255 - backgroundColor.blue());
	QPen borderPen(borderColor, 1);
	borderPen.setCosmetic(true);

	// Draw background
	QPainter::RenderHints hints = painter->renderHints();

	painter->setRenderHints(hints, false);
	painter->setBrush(mBackgroundBrush);
	painter->setPen(borderPen);
	painter->drawRect(mSceneRect);
	painter->setRenderHints(hints, true);

	// Draw items
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

void DrawingScene::findItems(const QList<DrawingItem*>& items, QList<DrawingItem*>& foundItems) const
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			foundItems.append(*itemIter);

			if (!(*itemIter)->mChildren.isEmpty())
				findItems((*itemIter)->mChildren, foundItems);
		}
	}
}

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
