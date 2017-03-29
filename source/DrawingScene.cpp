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
#include "DrawingItem.h"

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

qreal DrawingScene::sceneWidth() const
{
	return mSceneRect.width();
}

qreal DrawingScene::sceneHeight() const
{
	return mSceneRect.height();
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

QList<DrawingItem*> DrawingScene::items(const QPointF& pos) const
{
	QList<DrawingItem*> items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if (itemMatchesPoint(*itemIter, pos)) items.append(*itemIter);
	}

	return items;
}

QList<DrawingItem*> DrawingScene::items(const QRectF& rect, Qt::ItemSelectionMode selectMode) const
{
	QList<DrawingItem*> items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if (itemMatchesRect(*itemIter, rect, selectMode))
			items.append(*itemIter);
	}

	return items;
}

QList<DrawingItem*> DrawingScene::items(const QPainterPath& path, Qt::ItemSelectionMode selectMode) const
{
	QList<DrawingItem*> items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if (itemMatchesPath(*itemIter, path, selectMode))
			items.append(*itemIter);
	}

	return items;
}

DrawingItem* DrawingScene::itemAt(const QPointF& pos) const
{
	DrawingItem* item = nullptr;

	auto itemIter = mItems.end();
	while (item == nullptr && itemIter != mItems.begin())
	{
		itemIter--;
		if (itemMatchesPoint(*itemIter, pos)) item = *itemIter;
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

bool DrawingScene::itemMatchesPoint(DrawingItem* item, const QPointF& scenePos) const
{
	bool match = false;

	if (item && item->isVisible())
		match = item->shape().contains(item->mapFromScene(scenePos));

	return match;
}

bool DrawingScene::itemMatchesRect(DrawingItem* item, const QRectF& rect, Qt::ItemSelectionMode mode) const
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
	}

	return match;
}

bool DrawingScene::itemMatchesPath(DrawingItem* item, const QPainterPath& path, Qt::ItemSelectionMode mode) const
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
	}

	return match;
}
