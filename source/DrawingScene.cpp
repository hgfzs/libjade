/* DrawingScene.cpp
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
