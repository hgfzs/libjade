/* DrawingWidget.cpp
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

#include "DrawingWidget.h"
#include "DrawingItem.h"
#include "DrawingItemGroup.h"
#include "DrawingItemPoint.h"
#include "DrawingUndo.h"
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QStyle>
#include <QStyleHintReturnMask>
#include <QStyleOptionRubberBand>
#include <QtMath>

DrawingWidget::DrawingWidget(QWidget* parent) : QAbstractScrollArea(parent)
{
	setMouseTracking(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	mGrid = 1.0;
	mScale = 1.0;
	mMode = DefaultMode;
	mSelectedItemPoint = nullptr;

	mUndoStack.setUndoLimit(64);
	connect(&mUndoStack, SIGNAL(cleanChanged(bool)), this, SIGNAL(cleanChanged(bool)));
	connect(&mUndoStack, SIGNAL(canRedoChanged(bool)), this, SIGNAL(canRedoChanged(bool)));
	connect(&mUndoStack, SIGNAL(canUndoChanged(bool)), this, SIGNAL(canUndoChanged(bool)));

	mMouseState = MouseReady;
	mMouseDownHorizontalScrollValue = 0;
	mMouseDownVerticalScrollValue = 0;
	mMouseDownItem = nullptr;
	mFocusItem = nullptr;
	mPlaceByMousePressAndRelease = false;

	mMousePanTimer.setInterval(16);
	connect(&mMousePanTimer, SIGNAL(timeout()), this, SLOT(mousePanEvent()));
}

DrawingWidget::~DrawingWidget()
{
	setDefaultMode();
	while (!mClipboardItems.isEmpty()) delete mClipboardItems.takeFirst();
}

//==================================================================================================

void DrawingWidget::setGrid(qreal grid)
{
	mGrid = grid;
}

qreal DrawingWidget::grid() const
{
	return mGrid;
}

qreal DrawingWidget::roundToGrid(qreal value) const
{
	qreal result = value;

	if (mGrid > 0)
	{
		qreal mod = fmod(value, mGrid);
		result = value - mod;
		if (mod >= mGrid/2) result += mGrid;
		else if (mod <= -mGrid/2) result -= mGrid;
	}

	return result;
}

QPointF DrawingWidget::roundToGrid(const QPointF& pos) const
{
	return QPointF(roundToGrid(pos.x()), roundToGrid(pos.y()));
}

//==================================================================================================

void DrawingWidget::centerOn(const QPointF& pos)
{
	QPointF oldPos = mapToScene(viewport()->rect().center());

	int horizontalDelta = qRound((pos.x() - oldPos.x()) * mScale);
	int verticalDelta = qRound((pos.y() - oldPos.y()) * mScale);

	horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalDelta);
	verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);
}

void DrawingWidget::centerOnCursor(const QPointF& pos)
{
	QPointF oldPos = mapToScene(mapFromGlobal(QCursor::pos()));

	int horizontalDelta = qRound((pos.x() - oldPos.x()) * mScale);
	int verticalDelta = qRound((pos.y() - oldPos.y()) * mScale);

	horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalDelta);
	verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);
}

void DrawingWidget::fitToView(const QRectF& rect)
{
	qreal scaleX = (maximumViewportSize().width() - 5) / rect.width();
	qreal scaleY = (maximumViewportSize().height() - 5) / rect.height();

	mScale = qMin(scaleX, scaleY);

	recalculateContentSize(rect);

	centerOn(rect.center());
}

void DrawingWidget::scaleBy(qreal scale)
{
	if (scale > 0)
	{
		QPointF mousePos = mapToScene(mapFromGlobal(QCursor::pos()));
		QRectF scrollBarRect = sceneRect();

		if (horizontalScrollBar()->minimum() < horizontalScrollBar()->maximum())
		{
			scrollBarRect.setLeft(horizontalScrollBar()->minimum() / mScale + scrollBarRect.left());
			scrollBarRect.setWidth((horizontalScrollBar()->maximum() - horizontalScrollBar()->minimum() +
				maximumViewportSize().width()) / mScale);
		}

		if (verticalScrollBar()->minimum() < verticalScrollBar()->maximum())
		{
			scrollBarRect.setTop(verticalScrollBar()->minimum() / mScale + scrollBarRect.top());
			scrollBarRect.setHeight((verticalScrollBar()->maximum() - verticalScrollBar()->minimum() +
				maximumViewportSize().height()) / mScale);
		}

		mScale *= scale;

		recalculateContentSize(scrollBarRect);

		if (viewport()->rect().contains(mapFromGlobal(QCursor::pos()))) centerOnCursor(mousePos);
		else centerOn(QPointF());
	}
}

qreal DrawingWidget::scale() const
{
	return mScale;
}

QRectF DrawingWidget::visibleRect() const
{
	return QRectF(mapToScene(QPoint(0, 0)), mapToScene(QPoint(viewport()->width(), viewport()->height())));
}

QPointF DrawingWidget::mapToScene(const QPoint& pos) const
{
	QPointF p = pos;
	p.setX(p.x() + horizontalScrollBar()->value());
	p.setY(p.y() + verticalScrollBar()->value());
	return mSceneTransform.map(p);
}

QRectF DrawingWidget::mapToScene(const QRect& rect) const
{
	return QRectF(mapToScene(rect.topLeft()), mapToScene(rect.bottomRight()));
}

QPoint DrawingWidget::mapFromScene(const QPointF& pos) const
{
	QPointF p = mViewportTransform.map(pos);
	p.setX(p.x() - horizontalScrollBar()->value());
	p.setY(p.y() - verticalScrollBar()->value());
	return p.toPoint();
}

QRect DrawingWidget::mapFromScene(const QRectF& rect) const
{
	return QRect(mapFromScene(rect.topLeft()), mapFromScene(rect.bottomRight()));
}

//==================================================================================================

void DrawingWidget::setUndoLimit(int undoLimit)
{
	mUndoStack.setUndoLimit(undoLimit);
}

void DrawingWidget::pushUndoCommand(QUndoCommand* command)
{
	mUndoStack.push(command);
}

int DrawingWidget::undoLimit() const
{
	return mUndoStack.undoLimit();
}

bool DrawingWidget::isClean() const
{
	return mUndoStack.isClean();
}

bool DrawingWidget::canUndo() const
{
	return mUndoStack.canUndo();
}

bool DrawingWidget::canRedo() const
{
	return mUndoStack.canRedo();
}

QString DrawingWidget::undoText() const
{
	return mUndoStack.undoText();
}

QString DrawingWidget::redoText() const
{
	return mUndoStack.redoText();
}

//==================================================================================================

DrawingWidget::Mode DrawingWidget::mode() const
{
	return mMode;
}

QList<DrawingItem*> DrawingWidget::placeItems() const
{
	return mPlaceItems;
}

//==================================================================================================

QList<DrawingItem*> DrawingWidget::items(const QList<DrawingItem*>& itemsToCheck, const QPointF& pos) const
{
	QList<DrawingItem*> matchingItems;

	for(auto itemIter = itemsToCheck.begin(), itemEnd = itemsToCheck.end(); itemIter != itemEnd; itemIter++)
	{
		if ((*itemIter)->isVisible() && itemMatchesPoint(*itemIter, pos)) matchingItems.append(*itemIter);
	}

	return matchingItems;
}

QList<DrawingItem*> DrawingWidget::items(const QList<DrawingItem*>& itemsToCheck, const QRectF& rect) const
{
	QList<DrawingItem*> matchingItems;

	for(auto itemIter = itemsToCheck.begin(), itemEnd = itemsToCheck.end(); itemIter != itemEnd; itemIter++)
	{
		if ((*itemIter)->isVisible() && itemMatchesRect(*itemIter, rect, Qt::ContainsItemBoundingRect))
			matchingItems.append(*itemIter);
	}

	return matchingItems;
}

QList<DrawingItem*> DrawingWidget::items(const QList<DrawingItem*>& itemsToCheck, const QPainterPath& path) const
{
	QList<DrawingItem*> matchingItems;

	for(auto itemIter = itemsToCheck.begin(), itemEnd = itemsToCheck.end(); itemIter != itemEnd; itemIter++)
	{
		if ((*itemIter)->isVisible() && itemMatchesPath(*itemIter, path, Qt::ContainsItemBoundingRect))
			matchingItems.append(*itemIter);
	}

	return matchingItems;
}

DrawingItem* DrawingWidget::itemAt(const QList<DrawingItem*>& itemsToCheck, const QPointF& pos) const
{
	DrawingItem* matchingItem = nullptr;

	// Favor selected items
	auto selectedItemIter = mSelectedItems.end();
	while (matchingItem == nullptr && selectedItemIter != mSelectedItems.begin())
	{
		selectedItemIter--;
		if (itemMatchesPoint(*selectedItemIter, pos)) matchingItem = *selectedItemIter;
	}

	// Search all visible items
	auto itemIter = itemsToCheck.constEnd();
	while (matchingItem == nullptr && itemIter != itemsToCheck.constBegin())
	{
		itemIter--;
		if ((*itemIter)->isVisible() && itemMatchesPoint(*itemIter, pos)) matchingItem = *itemIter;
	}

	return matchingItem;
}

//==================================================================================================

QList<DrawingItem*> DrawingWidget::selectedItems() const
{
	return mSelectedItems;
}

DrawingItemPoint* DrawingWidget::selectedItemPoint() const
{
	return mSelectedItemPoint;
}

QPointF DrawingWidget::selectionCenter() const
{
	return mSelectionCenter;
}

//==================================================================================================

DrawingItem* DrawingWidget::mouseDownItem() const
{
	return mMouseDownItem;
}

DrawingItem* DrawingWidget::focusItem() const
{
	return mFocusItem;
}

//==================================================================================================

void DrawingWidget::render(QPainter* painter, bool paintBackground)
{
	if (paintBackground)
	{
		QPainter::RenderHints renderHints = painter->renderHints();
		painter->setRenderHints(renderHints, false);

		painter->setBrush(backgroundBrush());
		painter->setPen(Qt::NoPen);
		painter->drawRect(sceneRect());

		painter->setRenderHints(renderHints);
	}

	drawItems(painter, this->items());
}

//==================================================================================================

void DrawingWidget::zoomIn()
{
	scaleBy(qSqrt(2));
	emit scaleChanged(mScale);
	viewport()->update();
}

void DrawingWidget::zoomOut()
{
	scaleBy(qSqrt(2) / 2);
	emit scaleChanged(mScale);
	viewport()->update();
}

void DrawingWidget::zoomFit()
{
	fitToView(sceneRect());
	emit scaleChanged(mScale);
	viewport()->update();
}

//==================================================================================================

void DrawingWidget::setDefaultMode()
{
	clearPreviousMode();
	mMode = DefaultMode;
	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingWidget::setScrollMode()
{
	clearPreviousMode();
	mMode = ScrollMode;
	setCursor(Qt::OpenHandCursor);
	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingWidget::setZoomMode()
{
	clearPreviousMode();
	mMode = ZoomMode;
	setCursor(Qt::CrossCursor);
	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingWidget::setPlaceMode(const QList<DrawingItem*>& items)
{
	if (!items.isEmpty())
	{
		QPointF centerPos, deltaPos;

		clearPreviousMode();

		mMode = PlaceMode;
		setCursor(Qt::CrossCursor);

		mPlaceItems = items;

		// Move place items so that they are centered under the mouse cursor
		for(auto itemIter = mPlaceItems.begin(), itemEnd = mPlaceItems.end(); itemIter != itemEnd; itemIter++)
			centerPos += (*itemIter)->mapToScene((*itemIter)->centerPos());
		centerPos /= mPlaceItems.size();

		deltaPos = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())) - centerPos);

		for(auto itemIter = mPlaceItems.begin(), itemEnd = mPlaceItems.end(); itemIter != itemEnd; itemIter++)
			(*itemIter)->setPosition((*itemIter)->position() + deltaPos);

		emit modeChanged(mMode);
		emit placeItemsChanged(mPlaceItems);
		viewport()->update();
	}
	else setDefaultMode();
}

//==================================================================================================

void DrawingWidget::undo()
{
	if (mMode == DefaultMode && mUndoStack.canUndo())
	{
		mUndoStack.undo();
		viewport()->update();
	}
}

void DrawingWidget::redo()
{
	if (mMode == DefaultMode && mUndoStack.canRedo())
	{
		mUndoStack.redo();
		viewport()->update();
	}
}

void DrawingWidget::setClean()
{
	mUndoStack.setClean();
	viewport()->update();
}

//==================================================================================================

void DrawingWidget::cut()
{
	copy();
	deleteSelection();
}

void DrawingWidget::copy()
{
	if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		while (!mClipboardItems.isEmpty()) delete mClipboardItems.takeFirst();
		mClipboardItems = DrawingItem::copyItems(mSelectedItems);
	}
}

void DrawingWidget::paste()
{
	if (mMode == DefaultMode && !mClipboardItems.isEmpty())
	{
		selectNone();
		setPlaceMode(DrawingItem::copyItems(mClipboardItems));
	}
}

void DrawingWidget::deleteSelection()
{
	if (mMode == DefaultMode)
	{
		QList<DrawingItem*> itemsToRemove, itemsToHide, unchangedItems;

		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanDelete)
				itemsToRemove.append(*itemIter);
			else if ((*itemIter)->flags() & DrawingItem::CanHide)
				itemsToHide.append(*itemIter);
			else
				unchangedItems.append(*itemIter);
		}

		if (!itemsToRemove.isEmpty() || !itemsToHide.isEmpty())
		{
			QUndoCommand* deleteCommand = new QUndoCommand("Delete Items");

			selectItemsCommand(unchangedItems, deleteCommand);
			if (!itemsToHide.isEmpty()) hideItemsCommand(itemsToHide, deleteCommand);
			if (!itemsToRemove.isEmpty()) removeItemsCommand(itemsToRemove, deleteCommand);

			mUndoStack.push(deleteCommand);

			viewport()->update();
		}
	}
	else setDefaultMode();
}

//==================================================================================================

void DrawingWidget::selectItems(const QList<DrawingItem*>& items)
{
	if (mMode == DefaultMode)
	{
		QList<DrawingItem*> itemsToSelect;

		for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanSelect && !(*itemIter)->isSelected())
				itemsToSelect.append(*itemIter);
		}

		selectItemsCommand(itemsToSelect, true);
		viewport()->update();
	}
}

void DrawingWidget::selectArea(const QRectF& rect)
{
	if (mMode == DefaultMode) selectItems(this->items(rect));
}

void DrawingWidget::selectArea(const QPainterPath& path)
{
	if (mMode == DefaultMode) selectItems(this->items(path));
}

void DrawingWidget::selectAll()
{
	if (mMode == DefaultMode) selectItems(this->items());
}

void DrawingWidget::selectNone()
{
	if (!mSelectedItems.isEmpty())
	{
		selectItemsCommand(QList<DrawingItem*>(), true);
		viewport()->update();
	}
}

//==================================================================================================

void DrawingWidget::moveSelection(const QPointF& deltaPos)
{
	if (mMode == DefaultMode)
	{
		QList<DrawingItem*> itemsToMove;
		QHash<DrawingItem*,QPointF> newPositions;

		for(auto itemIter = mSelectedItems.begin(), itemEnd = mSelectedItems.end(); itemIter != itemEnd; itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanMove)
			{
				itemsToMove.append(*itemIter);
				newPositions[*itemIter] = (*itemIter)->position() + deltaPos;
			}
		}

		if (!itemsToMove.isEmpty())
		{
			moveItemsCommand(itemsToMove, newPositions, true);
			viewport()->update();
		}
	}
}

void DrawingWidget::resizeSelection(DrawingItemPoint* point, const QPointF& pos)
{
	if (mMode == DefaultMode && mSelectedItems.size() == 1 &&
		(mSelectedItems.first()->flags() & DrawingItem::CanResize) &&
		mSelectedItems.first()->points().contains(point))
	{
		resizeItemCommand(point, pos, true, true);
		viewport()->update();
	}
}

void DrawingWidget::rotateSelection()
{
	if (mMode == DefaultMode)
	{
		QList<DrawingItem*> itemsToRotate;

		for(auto itemIter = mSelectedItems.begin(), itemEnd = mSelectedItems.end(); itemIter != itemEnd; itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanRotate)
				itemsToRotate.append(*itemIter);
		}

		if (!itemsToRotate.isEmpty())
		{
			rotateItemsCommand(itemsToRotate, roundToGrid(mSelectionCenter));
			viewport()->update();
		}
	}
	else if (mMode == PlaceMode)
	{
		QList<DrawingItem*> itemsToRotate;

		for(auto itemIter = mPlaceItems.begin(), itemEnd = mPlaceItems.end(); itemIter != itemEnd; itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanRotate)
				itemsToRotate.append(*itemIter);
		}

		if (!itemsToRotate.isEmpty())
		{
			rotateItems(itemsToRotate, roundToGrid(mapToScene(mapFromGlobal(QCursor::pos()))));
			emit itemsTransformChanged(itemsToRotate);
			viewport()->update();
		}
	}
}

void DrawingWidget::rotateBackSelection()
{
	if (mMode == DefaultMode)
	{
		QList<DrawingItem*> itemsToRotate;

		for(auto itemIter = mSelectedItems.begin(), itemEnd = mSelectedItems.end(); itemIter != itemEnd; itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanRotate)
				itemsToRotate.append(*itemIter);
		}

		if (!itemsToRotate.isEmpty())
		{
			rotateBackItemsCommand(itemsToRotate, roundToGrid(mSelectionCenter));
			viewport()->update();
		}
	}
	else if (mMode == PlaceMode)
	{
		QList<DrawingItem*> itemsToRotate;

		for(auto itemIter = mPlaceItems.begin(), itemEnd = mPlaceItems.end(); itemIter != itemEnd; itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanRotate)
				itemsToRotate.append(*itemIter);
		}

		if (!itemsToRotate.isEmpty())
		{
			rotateBackItems(itemsToRotate, roundToGrid(mapToScene(mapFromGlobal(QCursor::pos()))));
			emit itemsTransformChanged(itemsToRotate);
			viewport()->update();
		}
	}
}

void DrawingWidget::flipSelectionHorizontal()
{
	if (mMode == DefaultMode)
	{
		QList<DrawingItem*> itemsToFlip;

		for(auto itemIter = mSelectedItems.begin(), itemEnd = mSelectedItems.end(); itemIter != itemEnd; itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanFlip)
				itemsToFlip.append(*itemIter);
		}

		if (!itemsToFlip.isEmpty())
		{
			flipItemsHorizontalCommand(itemsToFlip, roundToGrid(mSelectionCenter));
			viewport()->update();
		}
	}
	else if (mMode == PlaceMode)
	{
		QList<DrawingItem*> itemsToFlip;

		for(auto itemIter = mPlaceItems.begin(), itemEnd = mPlaceItems.end(); itemIter != itemEnd; itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanFlip)
				itemsToFlip.append(*itemIter);
		}

		if (!itemsToFlip.isEmpty())
		{
			flipItemsHorizontal(itemsToFlip, roundToGrid(mapToScene(mapFromGlobal(QCursor::pos()))));
			emit itemsTransformChanged(itemsToFlip);
			viewport()->update();
		}
	}
}

void DrawingWidget::flipSelectionVertical()
{
	if (mMode == DefaultMode)
	{
		QList<DrawingItem*> itemsToFlip;

		for(auto itemIter = mSelectedItems.begin(), itemEnd = mSelectedItems.end(); itemIter != itemEnd; itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanFlip)
				itemsToFlip.append(*itemIter);
		}

		if (!itemsToFlip.isEmpty())
		{
			flipItemsVerticalCommand(itemsToFlip, roundToGrid(mSelectionCenter));
			viewport()->update();
		}
	}
	else if (mMode == PlaceMode)
	{
		QList<DrawingItem*> itemsToFlip;

		for(auto itemIter = mPlaceItems.begin(), itemEnd = mPlaceItems.end(); itemIter != itemEnd; itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanFlip)
				itemsToFlip.append(*itemIter);
		}

		if (!itemsToFlip.isEmpty())
		{
			flipItemsVertical(itemsToFlip, roundToGrid(mapToScene(mapFromGlobal(QCursor::pos()))));
			emit itemsTransformChanged(itemsToFlip);
			viewport()->update();
		}
	}
}

//==================================================================================================

void DrawingWidget::bringForward()
{
	if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToReorder = mSelectedItems;
		QList<DrawingItem*> itemsOrdered = this->items();
		DrawingItem* item;
		int itemIndex;

		while (!itemsToReorder.empty())
		{
			item = itemsToReorder.takeLast();

			itemIndex = itemsOrdered.indexOf(item);
			if (itemIndex >= 0)
			{
				itemsOrdered.removeAll(item);
				itemsOrdered.insert(itemIndex + 1, item);
			}
		}

		reorderItemsCommand(itemsOrdered);
		viewport()->update();
	}
}

void DrawingWidget::sendBackward()
{
	if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToReorder = mSelectedItems;
		QList<DrawingItem*> itemsOrdered = this->items();
		DrawingItem* item;
		int itemIndex;

		while (!itemsToReorder.empty())
		{
			item = itemsToReorder.takeLast();

			itemIndex = itemsOrdered.indexOf(item);
			if (itemIndex >= 0)
			{
				itemsOrdered.removeAll(item);
				itemsOrdered.insert(itemIndex - 1, item);
			}
		}

		reorderItemsCommand(itemsOrdered);
		viewport()->update();
	}
}

void DrawingWidget::bringToFront()
{
	if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToReorder = mSelectedItems;
		QList<DrawingItem*> itemsOrdered = this->items();
		DrawingItem* item;
		int itemIndex;

		while (!itemsToReorder.empty())
		{
			item = itemsToReorder.takeLast();

			itemIndex = itemsOrdered.indexOf(item);
			if (itemIndex >= 0)
			{
				itemsOrdered.removeAll(item);
				itemsOrdered.append(item);
			}
		}

		reorderItemsCommand(itemsOrdered);
		viewport()->update();
	}
}

void DrawingWidget::sendToBack()
{
	if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToReorder = mSelectedItems;
		QList<DrawingItem*> itemsOrdered = this->items();
		DrawingItem* item;
		int itemIndex;

		while (!mSelectedItems.empty())
		{
			item = mSelectedItems.takeLast();

			itemIndex = itemsOrdered.indexOf(item);
			if (itemIndex >= 0)
			{
				itemsOrdered.removeAll(item);
				itemsOrdered.prepend(item);
			}
		}

		reorderItemsCommand(itemsOrdered);
		viewport()->update();
	}
}

//==================================================================================================

void DrawingWidget::insertItemPoint()
{
	if (mMode == DefaultMode)
	{
		DrawingItem* item = (mSelectedItems.size() == 1) ? mSelectedItems.first() : nullptr;

		if (item && (item->flags() & DrawingItem::CanInsertPoints))
		{
			QPointF pos = item->mapFromScene(roundToGrid(mMouseDownScenePos));
			int index = -1;
			DrawingItemPoint* point = item->pointToInsert(pos, index);

			if (point)
			{
				insertPointCommand(item, point, index);
				viewport()->update();
			}
		}
	}
}

void DrawingWidget::removeItemPoint()
{
	if (mMode == DefaultMode)
	{
		DrawingItem* item = (mSelectedItems.size() == 1) ? mSelectedItems.first() : nullptr;

		if (item && (item->flags() & DrawingItem::CanRemovePoints))
		{
			QPointF pos = item->mapFromScene(roundToGrid(mMouseDownScenePos));
			DrawingItemPoint* point = item->pointToRemove(pos);

			if (point)
			{
				removePointCommand(item, point);
				viewport()->update();
			}
		}
	}
}

//==================================================================================================

void DrawingWidget::group()
{
	if (mMode == DefaultMode && mSelectedItems.size() > 1)
	{
		QUndoCommand* command = new QUndoCommand("Group Items");

		QList<DrawingItem*> itemsToGroup = mSelectedItems;
		QList<DrawingItem*> items = DrawingItem::copyItems(itemsToGroup);
		DrawingItemGroup* itemGroup = new DrawingItemGroup();
		QList<DrawingItem*> itemsToAdd;

		itemGroup->setPosition(items.first()->position());
		for(auto iter = items.begin(); iter != items.end(); iter++)
			(*iter)->setPosition(itemGroup->mapFromScene((*iter)->position()));
		itemGroup->setItems(items);
		itemsToAdd.append(itemGroup);

		selectItemsCommand(QList<DrawingItem*>(), true, command);
		removeItemsCommand(itemsToGroup, command);
		addItemsCommand(itemsToAdd, false, command);
		selectItemsCommand(itemsToAdd, true, command);

		mUndoStack.push(command);
		viewport()->update();
	}
}

void DrawingWidget::ungroup()
{
	if (mMode == DefaultMode &&  mSelectedItems.size() == 1)
	{
		DrawingItemGroup* itemGroup = dynamic_cast<DrawingItemGroup*>(mSelectedItems.first());
		if (itemGroup)
		{
			QUndoCommand* command = new QUndoCommand("Ungroup Items");
			QList<DrawingItem*> itemsToRemove;
			itemsToRemove.append(itemGroup);

			QList<DrawingItem*> items = DrawingItem::copyItems(itemGroup->items());
			for(auto iter = items.begin(); iter != items.end(); iter++)
			{
				(*iter)->setPosition(itemGroup->mapToScene((*iter)->position()));
				(*iter)->setTransform(itemGroup->transform(), true);
			}

			selectItemsCommand(QList<DrawingItem*>(), true, command);
			removeItemsCommand(itemsToRemove, command);
			addItemsCommand(items, false, command);
			selectItemsCommand(items, true, command);

			mUndoStack.push(command);
			viewport()->update();
		}
	}
}

//==================================================================================================

void DrawingWidget::addItems(const QList<DrawingItem*>& items)
{
	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		addItem(*itemIter);

	emit numberOfItemsChanged(this->items().size());
	viewport()->update();
}

void DrawingWidget::insertItems(const QList<DrawingItem*>& items, const QHash<DrawingItem*,int>& index)
{
	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		insertItem(index.value(*itemIter), *itemIter);

	emit numberOfItemsChanged(this->items().size());
	viewport()->update();
}

void DrawingWidget::removeItems(const QList<DrawingItem*>& items)
{
	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		removeItem(*itemIter);

	emit numberOfItemsChanged(this->items().size());
	viewport()->update();
}

//==================================================================================================

void DrawingWidget::moveItems(const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& pos)
{
	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		(*itemIter)->move(pos.value(*itemIter));

	updateSelectionCenter();
	emit itemsPositionChanged(items);
	viewport()->update();
}

void DrawingWidget::resizeItem(DrawingItemPoint* point, const QPointF& pos)
{
	if (point && point->item())
	{
		DrawingItem* item = point->item();

		item->resize(point, pos);

		updateSelectionCenter();
		emit itemsGeometryChanged({item});
		viewport()->update();
	}
}

void DrawingWidget::rotateItems(const QList<DrawingItem*>& items, const QPointF& pos)
{
	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		(*itemIter)->rotate(pos);

	emit itemsTransformChanged(items);
	viewport()->update();
}

void DrawingWidget::rotateBackItems(const QList<DrawingItem*>& items, const QPointF& pos)
{
	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		(*itemIter)->rotateBack(pos);

	emit itemsTransformChanged(items);
	viewport()->update();
}

void DrawingWidget::flipItemsHorizontal(const QList<DrawingItem*>& items, const QPointF& pos)
{
	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		(*itemIter)->flipHorizontal(pos);

	emit itemsTransformChanged(items);
	viewport()->update();
}

void DrawingWidget::flipItemsVertical(const QList<DrawingItem*>& items, const QPointF& pos)
{
	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		(*itemIter)->flipVertical(pos);

	emit itemsTransformChanged(items);
	viewport()->update();
}

//==================================================================================================

void DrawingWidget::setItemsSelected(const QList<DrawingItem*>& items)
{
	for(auto itemIter = mSelectedItems.begin(), itemEnd = mSelectedItems.end(); itemIter != itemEnd; itemIter++)
		(*itemIter)->setSelected(false);

	mSelectedItems = items;

	for(auto itemIter = mSelectedItems.begin(), itemEnd = mSelectedItems.end(); itemIter != itemEnd; itemIter++)
		(*itemIter)->setSelected(true);

	updateSelectionCenter();
	emit selectionChanged(mSelectedItems);
	viewport()->update();
}

void DrawingWidget::setItemsVisibility(const QHash<DrawingItem*,bool>& visible)
{
	for(auto hashIter = visible.begin(), hashEnd = visible.end(); hashIter != hashEnd; hashIter++)
		hashIter.key()->setVisible(hashIter.value());

	emit itemsVisibilityChanged(visible.keys());
	viewport()->update();
}

//==================================================================================================

void DrawingWidget::insertItemPoint(DrawingItem* item, DrawingItemPoint* point, int index)
{
	if (item && point)
	{
		item->insertPoint(index, point);
		item->resize(point, item->mapToScene(point->position()));

		updateSelectionCenter();
		emit itemsGeometryChanged({item});
		viewport()->update();
	}
}

void DrawingWidget::removeItemPoint(DrawingItem* item, DrawingItemPoint* point)
{
	if (item && point)
	{
		item->removePoint(point);
		item->resize(point, item->mapToScene(point->position()));

		updateSelectionCenter();
		emit itemsGeometryChanged({item});
		viewport()->update();
	}
}

void DrawingWidget::connectItemPoints(DrawingItemPoint* point1, DrawingItemPoint* point2)
{
	if (point1 && point2)
	{
		point1->addConnection(point2);
		point2->addConnection(point1);
		viewport()->update();
	}
}

void DrawingWidget::disconnectItemPoints(DrawingItemPoint* point1, DrawingItemPoint* point2)
{
	if (point1 && point2)
	{
		point1->removeConnection(point2);
		point2->removeConnection(point1);
		viewport()->update();
	}
}

//==================================================================================================

void DrawingWidget::paintEvent(QPaintEvent* event)
{
	QImage image(viewport()->width(), viewport()->height(), QImage::Format_RGB32);
	image.fill(palette().brush(QPalette::Window).color());

	// Render scene
	QPainter painter(&image);

	painter.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());
	painter.setTransform(mViewportTransform, true);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	drawBackground(&painter);
	drawItems(&painter, this->items());
	drawItems(&painter, mPlaceItems);
	drawItemPoints(&painter, mSelectedItems);
	drawHotpoints(&painter, mSelectedItems + mPlaceItems);
	drawRubberBand(&painter, mRubberBandRect);
	drawForeground(&painter);

	painter.end();

	// Render scene image on to widget
	QPainter widgetPainter(viewport());
	widgetPainter.drawImage(0, 0, image);

	Q_UNUSED(event)
}

void DrawingWidget::resizeEvent(QResizeEvent* event)
{
	QAbstractScrollArea::resizeEvent(event);
	recalculateContentSize();
}

//==================================================================================================

void DrawingWidget::drawBackground(QPainter* painter)
{
	QBrush backgroundBrush = this->backgroundBrush();
	QRectF sceneRect = this->sceneRect();
	QRectF visibleRect = this->visibleRect();

	QPainter::RenderHints renderHints = painter->renderHints();
	painter->setRenderHints(renderHints, false);

	// Draw background
	painter->setBrush(backgroundBrush);
	painter->setPen(Qt::NoPen);
	painter->drawRect(visibleRect);

	// Draw border
	QPen borderPen((backgroundBrush == Qt::black) ? Qt::white : Qt::black, devicePixelRatio() * 2);
	borderPen.setCosmetic(true);

	painter->setBrush(Qt::transparent);
	painter->setPen(borderPen);
	painter->drawRect(sceneRect);

	painter->setRenderHints(renderHints);
}

void DrawingWidget::drawForeground(QPainter* painter)
{
	Q_UNUSED(painter)
}

//==================================================================================================

void DrawingWidget::drawItems(QPainter* painter, const QList<DrawingItem*>& items)
{
	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
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

void DrawingWidget::drawItemPoints(QPainter* painter, const QList<DrawingItem*>& items)
{
	QColor color = backgroundBrush().color();
	color.setRed(255 - color.red());
	color.setGreen(255 - color.green());
	color.setBlue(255 - color.blue());

	painter->save();

	painter->resetTransform();
	painter->setRenderHints(QPainter::Antialiasing, false);
	painter->setPen(QPen(color, 1));

	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			QList<DrawingItemPoint*> itemPoints = (*itemIter)->points();

			for(auto pointIter = itemPoints.begin(), pointEnd = itemPoints.end(); pointIter != pointEnd; pointIter++)
			{
				bool controlPoint = (((*pointIter)->flags() & DrawingItemPoint::Control) ||
					((*pointIter)->flags() == DrawingItemPoint::NoFlags));
				bool connectionPoint = ((*pointIter)->flags() & DrawingItemPoint::Connection);

				if (controlPoint || connectionPoint)
				{
					QRect pointRect = DrawingWidget::pointRect(*pointIter).adjusted(1, 1, -2, -2);

					if (connectionPoint && !controlPoint)
						painter->setBrush(QColor(255, 255, 0));
					else
						painter->setBrush(QColor(0, 224, 0));

					painter->drawRect(pointRect);
				}
			}
		}
	}

	painter->restore();
}

void DrawingWidget::drawHotpoints(QPainter* painter, const QList<DrawingItem*>& items)
{
	QList<DrawingItem*> mItems = this->items();

	painter->save();

	painter->resetTransform();
	painter->setRenderHints(QPainter::Antialiasing, false);
	painter->setBrush(QColor(255, 128, 0, 128));
	painter->setPen(QPen(painter->brush(), 1));

	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
	{
		QList<DrawingItemPoint*> itemPoints = (*itemIter)->points();

		for(auto pointIter = itemPoints.begin(), pointEnd = itemPoints.end(); pointIter != pointEnd; pointIter++)
		{
			for(auto otherItemIter = mItems.begin(), otherItemEnd = mItems.end();
				otherItemIter != otherItemEnd; otherItemIter++)
			{
				if ((*itemIter) != (*otherItemIter))
				{
					QList<DrawingItemPoint*> otherItemPoints = (*otherItemIter)->points();

					for(auto otherItemPointIter = otherItemPoints.begin(), otherItemPointEnd = otherItemPoints.end();
						otherItemPointIter != otherItemPointEnd; otherItemPointIter++)
					{
						if (shouldConnect(*pointIter, *otherItemPointIter))
						{
							QRect pointRect = DrawingWidget::pointRect(*pointIter);
							pointRect.adjust(-pointRect.width() / 2, -pointRect.width() / 2,
								pointRect.width() / 2, pointRect.width() / 2);
							painter->drawEllipse(pointRect);
						}
					}
				}
			}
		}
	}

	painter->restore();
}

void DrawingWidget::drawRubberBand(QPainter* painter, const QRect& rect)
{
	if (rect.isValid())
	{
		QStyleOptionRubberBand option;
		option.initFrom(viewport());
		option.rect = rect;
		option.shape = QRubberBand::Rectangle;

		painter->save();
		painter->resetTransform();

		QStyleHintReturnMask mask;
		if (viewport()->style()->styleHint(QStyle::SH_RubberBand_Mask, &option, viewport(), &mask))
			painter->setClipRegion(mask.region, Qt::IntersectClip);

		viewport()->style()->drawControl(QStyle::CE_RubberBand, &option, painter, viewport());

		painter->restore();
	}
}

//==================================================================================================

void DrawingWidget::mousePressEvent(QMouseEvent* event)
{
	updateMouseState(event);

	if (event->button() == Qt::LeftButton && mMode == ScrollMode)
	{
		setCursor(Qt::ClosedHandCursor);
	}
	else if (event->button() == Qt::MiddleButton)
	{
		setCursor(Qt::SizeAllCursor);
		mMousePanTimer.start();
	}

	viewport()->update();
}

void DrawingWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	updateMouseState(event);

	if (event->button() == Qt::LeftButton && mMode != DefaultMode) setDefaultMode();

	viewport()->update();
}

void DrawingWidget::mouseMoveEvent(QMouseEvent* event)
{
	updateMouseState(event);

	switch (mMode)
	{
	case DefaultMode:
		if (event->buttons() & Qt::LeftButton)
		{
			if (mMouseState == MouseMoveItems)
				mouseMoveItems(mSelectedItems, roundToGrid(mMouseScenePos - mMouseDownScenePos), false);
			else if (mMouseState == MouseResizeItem)
				mouseResizeItem(mSelectedItemPoint, roundToGrid(mMouseScenePos), false);
			else if (mMouseState == MouseRubberBand)
				mRubberBandRect = QRect(mMousePos, mMouseDownPos).normalized();
		}
		break;
	case ScrollMode:
		if ((event->buttons() & Qt::LeftButton) && mMouseState == MouseDragged)
		{
			horizontalScrollBar()->setValue(
				mMouseDownHorizontalScrollValue - (mMousePos.x() - mMouseDownPos.x()));
			verticalScrollBar()->setValue(
				mMouseDownVerticalScrollValue - (mMousePos.y() - mMouseDownPos.y()));
		}
		break;
	case ZoomMode:
		if ((event->buttons() & Qt::LeftButton) && mMouseState == MouseDragged)
		{
			mRubberBandRect = QRect(qMin(mMousePos.x(), mMouseDownPos.x()),
				qMin(mMousePos.y(), mMouseDownPos.y()),
				qAbs(mMouseDownPos.x() - mMousePos.x()), qAbs(mMouseDownPos.y() - mMousePos.y()));
		}
		break;
	case PlaceMode:
		mPlaceByMousePressAndRelease = (mPlaceByMousePressAndRelease ||
			(mPlaceItems.size() == 1 &&
			(mPlaceItems.first()->boundingRect().isNull()) && mPlaceItems.first()->points().size() >= 2));
		if (event->buttons() & Qt::LeftButton && mPlaceByMousePressAndRelease)
		{
			resizeItem(mPlaceItems.first()->points()[1], roundToGrid(mMouseScenePos));
		}
		else
		{
			QPointF centerPos, deltaPos;

			for(auto itemIter = mPlaceItems.begin(), itemEnd = mPlaceItems.end(); itemIter != itemEnd; itemIter++)
				centerPos += (*itemIter)->mapToScene((*itemIter)->centerPos());
			if (!mPlaceItems.isEmpty()) centerPos /= mPlaceItems.size();

			deltaPos = roundToGrid(mMouseScenePos - centerPos);

			for(auto itemIter = mPlaceItems.begin(), itemEnd = mPlaceItems.end(); itemIter != itemEnd; itemIter++)
				(*itemIter)->setPosition((*itemIter)->position() + deltaPos);

			updateSelectionCenter();
			emit itemsGeometryChanged(mPlaceItems);
		}
		break;
	case UserMode:
		// User-defined modes start here.  Do nothing.
		break;
	}

	// Assume that we only need to update the viewport when a mouse button is held down or when
	// in PlaceMode
	if (event->buttons() != Qt::NoButton || mMode == PlaceMode) viewport()->update();
}

void DrawingWidget::mouseReleaseEvent(QMouseEvent* event)
{
	updateMouseState(event);

	if (event->button() == Qt::LeftButton)
	{
		bool controlDown = ((event->modifiers() & Qt::ControlModifier) != 0);
		QList<DrawingItem*> newSelection = (controlDown) ? mSelectedItems : QList<DrawingItem*>();

		switch (mMode)
		{
		case DefaultMode:
			switch (mMouseState)
			{
			case MouseReady:
			case MouseDragged:
				// We should never reach these states, so do nothing.
				break;

			case MouseSelect:
				if (mMouseDownItem)
				{
					if (controlDown && mMouseDownItem->isSelected())
						newSelection.removeAll(mMouseDownItem);
					else if (mMouseDownItem->flags() & DrawingItem::CanSelect)
						newSelection.append(mMouseDownItem);
				}
				if (mSelectedItems != newSelection) selectItemsCommand(newSelection, true);
				break;

			case MouseMoveItems:
				mouseMoveItems(mSelectedItems, roundToGrid(mMouseScenePos - mMouseDownScenePos), true);
				break;

			case MouseResizeItem:
				mouseResizeItem(mSelectedItemPoint, roundToGrid(mMouseScenePos), true);
				break;

			case MouseRubberBand:
				if (mRubberBandRect.isValid())
				{
					QList<DrawingItem*> foundItems = items(mapToScene(mRubberBandRect));
					for(auto itemIter = foundItems.begin(), itemEnd = foundItems.end(); itemIter != itemEnd; itemIter++)
					{
						if (!newSelection.contains((*itemIter)) &&
							(*itemIter)->flags() & DrawingItem::CanSelect) newSelection.append(*itemIter);
					}
					if (mSelectedItems != newSelection) selectItemsCommand(newSelection, true);
				}
				break;
			}
			break;
		case ScrollMode:
			setCursor(Qt::OpenHandCursor);
			break;
		case ZoomMode:
			if (mRubberBandRect.isValid())
			{
				fitToView(mapToScene(mRubberBandRect));
				emit scaleChanged(mScale);
				setDefaultMode();
			}
			break;
		case PlaceMode:
			if (mPlaceItems.size() > 1 || (mPlaceItems.size() == 1 && mPlaceItems.first()->isValid()))
			{
				QList<DrawingItem*> newItems;
				DrawingItem* newItem;
				QList<DrawingItemPoint*> points;

				addItemsCommand(mPlaceItems, true);

				for(auto itemIter = mPlaceItems.begin(), itemEnd = mPlaceItems.end(); itemIter != itemEnd; itemIter++)
				{
					newItem = (*itemIter)->copy();
					if (mPlaceByMousePressAndRelease)
					{
						points = newItem->points();
						for(auto pointIter = points.begin(), pointEnd = points.end(); pointIter != pointEnd; pointIter++)
							resizeItem(*pointIter, newItem->position());
					}

					newItems.append(newItem);
				}
				mPlaceItems.clear();

				setPlaceMode(newItems);
			}
			break;
		case UserMode:
			// User-defined modes start here.  Do nothing.
			break;
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		if (mMode != DefaultMode)
			setDefaultMode();
	}

	if (mMousePanTimer.isActive())
	{
		setCursor(Qt::ArrowCursor);
		mMousePanTimer.stop();
	}

	clearMouseState();

	viewport()->update();
}

void DrawingWidget::wheelEvent(QWheelEvent* event)
{
	if (event->modifiers() & Qt::ControlModifier)
	{
		if (event->delta() > 0) zoomIn();
		else if (event->delta() < 0) zoomOut();
	}
	else QAbstractScrollArea::wheelEvent(event);
}

//==================================================================================================

void DrawingWidget::updateMouseState(QMouseEvent* event)
{
	// This code assumes the user will never press down multiple mouse buttons at the same time.
	// If they do, then the results are undefined.

	mMousePos = event->pos();
	mMouseScenePos = mapToScene(mMousePos);

	if (event->type() == QEvent::MouseButtonPress)
	{
		mMouseState = MouseSelect;

		mMouseDownPos = mMousePos;
		mMouseDownScenePos = mMouseScenePos;
		mMouseDownHorizontalScrollValue = horizontalScrollBar()->value();
		mMouseDownVerticalScrollValue = verticalScrollBar()->value();
		mMousePanStartPos = mMousePos;
		mRubberBandRect = QRect();

		if (mMode == DefaultMode)
		{
			mMouseDownItem = itemAt(mMouseScenePos);
			mSelectedItemPoint = nullptr;

			if (mMouseDownItem && mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
			{
				mSelectedItemPoint = pointAt(mMouseDownItem, mMouseDownItem->mapFromScene(mMouseScenePos));

				// Ignore the selected point if it's not one we can interact with
				if (mSelectedItemPoint && !(mSelectedItemPoint->flags() & DrawingItemPoint::Control))
					mSelectedItemPoint = nullptr;
			}

			mFocusItem = mMouseDownItem;
		}
	}
	else if (event->type() == QEvent::MouseMove && (event->buttons() & Qt::LeftButton))
	{
		if (mMouseState == MouseSelect)
		{
			bool mouseDragged = ((mMouseDownPos - event->pos()).manhattanLength() >=
				QApplication::startDragDistance());

			if (mouseDragged)
			{
				if (mMode == DefaultMode)
				{
					if (mMouseDownItem && mMouseDownItem->isSelected())
					{
						bool resizeItem = (mSelectedItems.size() == 1 &&
							(mSelectedItems.first()->flags() & DrawingItem::CanResize) &&
							mSelectedItemPoint && (mSelectedItemPoint->flags() & DrawingItemPoint::Control));
						mMouseState = (resizeItem) ? MouseResizeItem : MouseMoveItems;
					}
					else mMouseState = MouseRubberBand;
				}
				else mMouseState = MouseDragged;
			}
		}
	}
}

void DrawingWidget::clearMouseState()
{
	mMouseState = MouseReady;
	mMouseDownItem = nullptr;
	mSelectedItemPoint = nullptr;
	mRubberBandRect = QRect();
	mPlaceByMousePressAndRelease = false;
}

//==================================================================================================

void DrawingWidget::mouseMoveItems(const QList<DrawingItem*>& items, const QPointF& deltaPos, bool finalMove)
{
	QHash<DrawingItem*,QPointF> newPositions;

	if (mMouseMoveItems.isEmpty())
	{
		for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanMove)
			{
				mMouseMoveItems.append(*itemIter);
				mMouseMoveInitialPositions[*itemIter] = (*itemIter)->position();
			}
		}
	}

	for(auto hashIter = mMouseMoveInitialPositions.begin(), hashEnd = mMouseMoveInitialPositions.end();
		hashIter != hashEnd; hashIter++)
	{
		newPositions[hashIter.key()] = hashIter.value() + deltaPos;
	}

	if (!newPositions.isEmpty())
		moveItemsCommand(mMouseMoveItems, newPositions, finalMove);

	if (finalMove)
	{
		mMouseMoveInitialPositions.clear();
		mMouseMoveItems.clear();
	}
}

void DrawingWidget::mouseResizeItem(DrawingItemPoint* point, const QPointF& pos, bool finalMove)
{
	resizeItemCommand(point, pos, finalMove, true);
}

//==================================================================================================

void DrawingWidget::updateSelectionCenter()
{
	mSelectionCenter = QPointF();

	if (!mSelectedItems.isEmpty())
	{
		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
			mSelectionCenter += (*itemIter)->mapToScene((*itemIter)->centerPos());

		mSelectionCenter /= mSelectedItems.size();
	}
}

void DrawingWidget::mousePanEvent()
{
	QRectF visibleRect = this->visibleRect();
	QRectF sceneRect = this->sceneRect();

	if (horizontalScrollBar()->maximum() - horizontalScrollBar()->minimum() > 0)
	{
		if (mMousePos.x() - mMousePanStartPos.x() < 0)
		{
			int delta = (mMousePos.x() - mMousePanStartPos.x()) / 16;

			if (horizontalScrollBar()->value() + delta < horizontalScrollBar()->minimum())
			{
				if (horizontalScrollBar()->minimum() >= horizontalScrollBar()->maximum())
					horizontalScrollBar()->setMinimum(qFloor((visibleRect.left() - sceneRect.left()) * mScale) + delta);
				else
					horizontalScrollBar()->setMinimum(horizontalScrollBar()->value() + delta);

				horizontalScrollBar()->setValue(horizontalScrollBar()->minimum());
			}
			else horizontalScrollBar()->setValue(horizontalScrollBar()->value() + delta);
		}
		else if (mMousePos.x() - mMousePanStartPos.x() > 0)
		{
			int delta = (mMousePos.x() - mMousePanStartPos.x()) / 16;

			if (horizontalScrollBar()->value() + delta > horizontalScrollBar()->maximum())
			{
				if (horizontalScrollBar()->minimum() > horizontalScrollBar()->maximum())
					horizontalScrollBar()->setMaximum(qFloor((sceneRect.right() - visibleRect.right()) * mScale) + delta);
				else
					horizontalScrollBar()->setMaximum(horizontalScrollBar()->value() + delta);

				horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
			}
			else horizontalScrollBar()->setValue(horizontalScrollBar()->value() + delta);
		}
	}

	if (verticalScrollBar()->maximum() - verticalScrollBar()->minimum() > 0)
	{
		if (mMousePos.y() - mMousePanStartPos.y() < 0)
		{
			int delta = (mMousePos.y() - mMousePanStartPos.y()) / 16;

			if (verticalScrollBar()->value() + delta < verticalScrollBar()->minimum())
			{
				if (verticalScrollBar()->minimum() >= verticalScrollBar()->maximum())
					verticalScrollBar()->setMinimum(qFloor((visibleRect.top() - sceneRect.top()) * mScale) + delta);
				else
					verticalScrollBar()->setMinimum(verticalScrollBar()->value() + delta);

				verticalScrollBar()->setValue(verticalScrollBar()->minimum());
			}
			else verticalScrollBar()->setValue(verticalScrollBar()->value() + delta);
		}
		else if (mMousePos.y() - mMousePanStartPos.y() > 0)
		{
			int delta = (mMousePos.y() - mMousePanStartPos.y()) / 16;

			if (verticalScrollBar()->value() + delta > verticalScrollBar()->maximum())
			{
				if (verticalScrollBar()->minimum() >= verticalScrollBar()->maximum())
					verticalScrollBar()->setMaximum(qFloor((sceneRect.bottom() - visibleRect.bottom()) * mScale) + delta);
				else
					verticalScrollBar()->setMaximum(verticalScrollBar()->value() + delta);

				verticalScrollBar()->setValue(verticalScrollBar()->maximum());
			}
			else verticalScrollBar()->setValue(verticalScrollBar()->value() + delta);
		}
	}
}

void DrawingWidget::clearPreviousMode()
{
	while (!mPlaceItems.isEmpty()) delete mPlaceItems.takeFirst();
	emit placeItemsChanged(mPlaceItems);

	selectNone();

	mMouseDownItem = nullptr;
	mFocusItem = nullptr;
	mMouseMoveItems.clear();
	mMouseMoveInitialPositions.clear();

	setCursor(Qt::ArrowCursor);
}

//==================================================================================================

void DrawingWidget::addItemsCommand(const QList<DrawingItem*>& items, bool place,
	QUndoCommand* command)
{
	DrawingAddItemsCommand* addCommand = new DrawingAddItemsCommand(this, items, command);

	addCommand->redo();
	if (place) placeItems(items, addCommand);
	addCommand->undo();

	if (!command) mUndoStack.push(addCommand);
}

void DrawingWidget::removeItemsCommand(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	DrawingRemoveItemsCommand* removeCommand = new DrawingRemoveItemsCommand(this, items, command);

	removeCommand->redo();
	unplaceItems(items, removeCommand);
	removeCommand->undo();

	if (!command) mUndoStack.push(removeCommand);
}

//==================================================================================================

void DrawingWidget::moveItemsCommand(const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& newPos,
	bool place, QUndoCommand* command)
{
	DrawingMoveItemsCommand* moveCommand =
		new DrawingMoveItemsCommand(this, items, newPos, place, command);

	moveCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, moveCommand);
	if (place) placeItems(items, moveCommand);
	moveCommand->undo();

	if (!command) mUndoStack.push(moveCommand);
}

void DrawingWidget::resizeItemCommand(DrawingItemPoint* itemPoint, const QPointF& pos,
	bool place, bool disconnect, QUndoCommand* command)
{
	if (itemPoint && itemPoint->item())
	{
		DrawingResizeItemCommand* resizeCommand =
			new DrawingResizeItemCommand(this, itemPoint, pos, place, command);
		QList<DrawingItem*> resizeItems;
		resizeItems.append(itemPoint->item());

		resizeCommand->redo();
		if (disconnect) disconnectAll(itemPoint, resizeCommand);
		tryToMaintainConnections(resizeItems, true,
			!(itemPoint->flags() & DrawingItemPoint::Free), itemPoint, resizeCommand);
		if (place) placeItems(resizeItems, resizeCommand);
		resizeCommand->undo();

		if (!command) mUndoStack.push(resizeCommand);
	}
}

void DrawingWidget::rotateItemsCommand(const QList<DrawingItem*>& items, const QPointF& pos, QUndoCommand* command)
{
	DrawingRotateItemsCommand* rotateCommand =
		new DrawingRotateItemsCommand(this, items, pos, command);

	rotateCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, rotateCommand);
	rotateCommand->undo();

	if (!command) mUndoStack.push(rotateCommand);
}

void DrawingWidget::rotateBackItemsCommand(const QList<DrawingItem*>& items, const QPointF& pos, QUndoCommand* command)
{
	DrawingRotateBackItemsCommand* rotateCommand =
		new DrawingRotateBackItemsCommand(this, items, pos, command);

	rotateCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, rotateCommand);
	rotateCommand->undo();

	if (!command) mUndoStack.push(rotateCommand);
}

void DrawingWidget::flipItemsHorizontalCommand(const QList<DrawingItem*>& items, const QPointF& pos, QUndoCommand* command)
{
	DrawingFlipItemsHorizontalCommand* flipCommand =
		new DrawingFlipItemsHorizontalCommand(this, items, pos, command);

	flipCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, flipCommand);
	flipCommand->undo();

	if (!command) mUndoStack.push(flipCommand);
}

void DrawingWidget::flipItemsVerticalCommand(const QList<DrawingItem*>& items, const QPointF& pos, QUndoCommand* command)
{
	DrawingFlipItemsVerticalCommand* flipCommand =
		new DrawingFlipItemsVerticalCommand(this, items, pos, command);

	flipCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, flipCommand);
	flipCommand->undo();

	if (!command) mUndoStack.push(flipCommand);
}

//==================================================================================================

void DrawingWidget::reorderItemsCommand(const QList<DrawingItem*>& itemsOrdered, QUndoCommand* command)
{
	DrawingReorderItemsCommand* selectCommand =
		new DrawingReorderItemsCommand(this, itemsOrdered, command);

	if (!command) mUndoStack.push(selectCommand);
}

void DrawingWidget::selectItemsCommand(const QList<DrawingItem*>& items, bool finalSelect,
	QUndoCommand* command)
{
	DrawingSelectItemsCommand* selectCommand =
		new DrawingSelectItemsCommand(this, items, finalSelect, command);

	if (!command) mUndoStack.push(selectCommand);
}

void DrawingWidget::hideItemsCommand(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	DrawingItemSetVisibilityCommand* visibilityCommand =
		new DrawingItemSetVisibilityCommand(this, items, false, command);

	if (!command) mUndoStack.push(visibilityCommand);
}

//==================================================================================================

void DrawingWidget::insertPointCommand(DrawingItem* item, DrawingItemPoint* point, int index,
	QUndoCommand* command)
{
	DrawingItemInsertPointCommand* insertPointCommand =
		new DrawingItemInsertPointCommand(this, item, point, index, command);

	if (!command) mUndoStack.push(insertPointCommand);
}

void DrawingWidget::removePointCommand(DrawingItem* item, DrawingItemPoint* point,
	QUndoCommand* command)
{
	DrawingItemRemovePointCommand* removePointCommand =
		new DrawingItemRemovePointCommand(this, item, point, command);

	disconnectAll(point, removePointCommand);

	if (!command) mUndoStack.push(removePointCommand);
}

void DrawingWidget::connectItemPointsCommand(DrawingItemPoint* point1, DrawingItemPoint* point2,
	QUndoCommand* command)
{
	DrawingItemPointConnectCommand* connectCommand =
		new DrawingItemPointConnectCommand(this, point1, point2, command);

	QPointF point0Pos = point1->item()->mapToScene(point1->position());
	QPointF point1Pos = point2->item()->mapToScene(point2->position());

	if (point0Pos != point1Pos)
	{
		if ((point2->flags() & DrawingItemPoint::Control) && (point2->item()->flags() & DrawingItem::CanResize))
			resizeItemCommand(point2, point0Pos, false, true, connectCommand);
		else if ((point1->flags() & DrawingItemPoint::Control) && (point1->item()->flags() & DrawingItem::CanResize))
			resizeItemCommand(point1, point1Pos, false, true, connectCommand);
	}

	if (!command) mUndoStack.push(connectCommand);
}

void DrawingWidget::disconnectItemPointsCommand(DrawingItemPoint* point1, DrawingItemPoint* point2,
	QUndoCommand* command)
{
	DrawingItemPointDisconnectCommand* disconnectCommand =
		new DrawingItemPointDisconnectCommand(this, point1, point2, command);

	if (!command) mUndoStack.push(disconnectCommand);
}

//==================================================================================================

void DrawingWidget::placeItems(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	QList<DrawingItemPoint*> itemPoints, otherItemPoints;
	QList<DrawingItem*> mItems = this->items();

	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
	{
		for(auto otherItemIter = mItems.begin(), otherItemEnd = mItems.end();
			otherItemIter != otherItemEnd; otherItemIter++)
		{
			if (!items.contains(*otherItemIter) && !mPlaceItems.contains(*otherItemIter))
			{
				itemPoints = (*itemIter)->points();
				otherItemPoints = (*otherItemIter)->points();

				for(auto itemPointIter = itemPoints.begin(), itemPointEnd = itemPoints.end();
					itemPointIter != itemPointEnd; itemPointIter++)
				{
					for(auto otherItemPointIter = otherItemPoints.begin(), otherItemPointEnd = otherItemPoints.end();
						otherItemPointIter != otherItemPointEnd; otherItemPointIter++)
					{
						if (shouldConnect(*itemPointIter, *otherItemPointIter))
							connectItemPointsCommand(*itemPointIter, *otherItemPointIter, command);
					}
				}
			}
		}
	}
}

void DrawingWidget::unplaceItems(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	DrawingItem* item;
	DrawingItemPoint* itemPoint;
	QList<DrawingItemPoint*> itemPoints, targetPoints;

	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
	{
		item = *itemIter;
		itemPoints = item->points();

		for(auto itemPointIter = itemPoints.begin(), itemPointEnd = itemPoints.end();
			itemPointIter != itemPointEnd; itemPointIter++)
		{
			itemPoint = *itemPointIter;
			targetPoints = itemPoint->connections();
			for(auto targetPointIter = targetPoints.begin(), targetPointEnd = targetPoints.end();
				targetPointIter != targetPointEnd; targetPointIter++)
			{
				if (!items.contains((*targetPointIter)->item()))
					disconnectItemPointsCommand(itemPoint, *targetPointIter, command);
			}
		}
	}
}

void DrawingWidget::tryToMaintainConnections(const QList<DrawingItem*>& items, bool allowResize,
	bool checkControlPoints, DrawingItemPoint* pointToSkip, QUndoCommand* command)
{
	QList<DrawingItemPoint*> itemPoints, targetPoints;
	DrawingItem* item;
	DrawingItem* targetItem;
	DrawingItemPoint* itemPoint;
	DrawingItemPoint* targetItemPoint;

	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
	{
		item = *itemIter;
		itemPoints = item->points();

		for(auto itemPointIter = itemPoints.begin(), itemPointEnd = itemPoints.end();
			itemPointIter != itemPointEnd; itemPointIter++)
		{
			itemPoint = *itemPointIter;
			if (itemPoint != pointToSkip && (checkControlPoints || !(itemPoint->flags() & DrawingItemPoint::Control)))
			{
				targetPoints = itemPoint->connections();
				for(auto targetPointIter = targetPoints.begin(), targetPointEnd = targetPoints.end();
					targetPointIter != targetPointEnd; targetPointIter++)
				{
					targetItemPoint = *targetPointIter;
					targetItem = targetItemPoint->item();

					if (item->mapToScene(itemPoint->position()) != targetItem->mapToScene(targetItemPoint->position()))
					{
						// Try to maintain the connection by resizing targetPoint if possible
						if (allowResize && (targetItem->flags() & DrawingItem::CanResize) &&
							(targetItemPoint->flags() & DrawingItemPoint::Free) &&
							!shouldDisconnect(itemPoint, targetItemPoint))
						{
							resizeItemCommand(targetItemPoint, item->mapToScene(itemPoint->position()),
								false, false, command);
						}
						else
							disconnectItemPointsCommand(itemPoint, targetItemPoint, command);
					}
				}
			}
		}
	}
}

void DrawingWidget::disconnectAll(DrawingItemPoint* point, QUndoCommand* command)
{
	if (point)
	{
		QList<DrawingItemPoint*> targetPoints = point->connections();

		for(auto targetPointIter = targetPoints.begin(), targetPointEnd = targetPoints.end();
			targetPointIter != targetPointEnd; targetPointIter++)
		{
			disconnectItemPointsCommand(point, *targetPointIter, command);
		}
	}
}

//==================================================================================================

void DrawingWidget::reorderItems(const QList<DrawingItem*>& items)
{
	QList<DrawingItem*> oldItems = this->items();

	for(auto itemIter = oldItems.begin(), itemEnd = oldItems.end(); itemIter != itemEnd; itemIter++)
		removeItem(*itemIter);
	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		addItem(*itemIter);

	viewport()->update();
}

//==================================================================================================

bool DrawingWidget::itemMatchesPoint(DrawingItem* item, const QPointF& pos) const
{
	bool match = false;

	if (item)
	{
		// Check item shape
		match = itemAdjustedShape(item).contains(item->mapFromScene(pos));

		// Check item points
		if (!match && item->isSelected())
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			QRectF pointSceneRect;

			for(auto pointIter = itemPoints.begin(), pointEnd = itemPoints.end();
				!match && pointIter != pointEnd; pointIter++)
			{
				pointSceneRect = mapToScene(pointRect(*pointIter));
				match = pointSceneRect.contains(pos);
			}
		}
	}

	return match;
}

bool DrawingWidget::itemMatchesRect(DrawingItem* item, const QRectF& rect, Qt::ItemSelectionMode mode) const
{
	bool match = false;

	if (item)
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
		case Qt::ContainsItemBoundingRect:
			match = rect.contains(item->mapToScene(item->boundingRect()).boundingRect());
			break;
		}

		// Check item points
		if (!match && item->isSelected())
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			QRectF pointSceneRect;

			for(auto pointIter = itemPoints.begin(), pointEnd = itemPoints.end();
				!match && pointIter != pointEnd; pointIter++)
			{
				pointSceneRect = mapToScene(pointRect(*pointIter));

				if (mode == Qt::IntersectsItemBoundingRect || mode == Qt::IntersectsItemShape)
					match = rect.intersects(pointSceneRect);
				else
					match = rect.contains(pointSceneRect);
			}
		}
	}

	return match;
}

bool DrawingWidget::itemMatchesPath(DrawingItem* item, const QPainterPath& path, Qt::ItemSelectionMode mode) const
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
		case Qt::ContainsItemBoundingRect:
			match = path.contains(item->mapToScene(item->boundingRect()).boundingRect());
			break;
		}

		// Check item points
		if (!match && item->isSelected())
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			QRectF pointSceneRect;

			for(auto pointIter = itemPoints.begin(), pointEnd = itemPoints.end();
				!match && pointIter != pointEnd; pointIter++)
			{
				pointSceneRect = mapToScene(pointRect(*pointIter));

				if (mode == Qt::IntersectsItemBoundingRect || mode == Qt::IntersectsItemShape)
					match = path.intersects(pointSceneRect);
				else
					match = path.contains(pointSceneRect);
			}
		}
	}

	return match;
}

QPainterPath DrawingWidget::itemAdjustedShape(DrawingItem* item) const
{
	QPainterPath adjustedShape;

	if (item)
	{
		QHash<QString,QVariant> properties = item->properties();

		if (properties.contains("pen-width"))
		{
			bool ok = false;
			qreal penWidth = properties.value("pen-width").toDouble(&ok);

			if (ok)
			{
				// Determine minimum pen width
				qreal minimumPenWidth = this->minimumPenWidth(item);

				if (0 < penWidth && penWidth < minimumPenWidth)
				{
					properties["pen-width"] = QVariant(minimumPenWidth);
					item->setProperties(properties);

					adjustedShape = item->shape();

					properties["pen-width"] = QVariant(penWidth);
					item->setProperties(properties);
				}
				else adjustedShape = item->shape();
			}
			else adjustedShape = item->shape();
		}
		else adjustedShape = item->shape();
	}

	return adjustedShape;
}

qreal DrawingWidget::minimumPenWidth(DrawingItem* item) const
{
	const int penWidthHint = 8;

	qreal minimumPenWidth = 0;

	if (item)
	{
		QPointF mappedPenSize = item->mapFromScene(item->position() +
			mapToScene(QPoint(penWidthHint, penWidthHint)) - mapToScene(QPoint(0, 0)));

		minimumPenWidth = qMax(qAbs(mappedPenSize.x()), qAbs(mappedPenSize.y()));
	}

	return minimumPenWidth;
}

//==================================================================================================

bool DrawingWidget::shouldConnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const
{
	bool shouldConnect = false;

	if (point1 && point1->item() && point2 && point2->item() && point1->item() != point2->item())
	{
		qreal threshold = grid() / 4000;
		QPointF vec = point1->item()->mapToScene(point1->position()) - point2->item()->mapToScene(point2->position());
		qreal distance = qSqrt(vec.x() * vec.x() + vec.y() * vec.y());

		shouldConnect = ((point1->flags() & DrawingItemPoint::Connection) && (point2->flags() & DrawingItemPoint::Connection) &&
			((point1->flags() & DrawingItemPoint::Free) || (point2->flags() & DrawingItemPoint::Free)) &&
			!point1->isConnected(point2) && !point1->isConnected(point2->item()) && distance <= threshold);
	}

	return shouldConnect;
}

bool DrawingWidget::shouldDisconnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const
{
	bool shouldDisconnect = true;

	if (point1 && point1->item() && point2 && point2->item())
	{
		shouldDisconnect = (
			point1->item()->mapToScene(point1->position()) != point2->item()->mapToScene(point2->position()) &&
			!(point2->flags() & DrawingItemPoint::Control));
	}

	return shouldDisconnect;
}

DrawingItemPoint* DrawingWidget::pointAt(DrawingItem* item, const QPointF& pos) const
{
	DrawingItemPoint* point = nullptr;

	if (item)
	{
		QList<DrawingItemPoint*> points = item->points();
		QRectF rect;

		for(auto pointIter = points.begin(), pointEnd = points.end();
			point == nullptr && pointIter != pointEnd; pointIter++)
		{
			rect = item->mapFromScene(mapToScene(pointRect(*pointIter))).boundingRect();
			if (rect.contains(pos)) point = *pointIter;
		}
	}

	return point;
}

QRect DrawingWidget::pointRect(DrawingItemPoint* point) const
{
	const QSize pointSizeHint(8 * devicePixelRatio(), 8 * devicePixelRatio());

	QRect viewRect;

	if (point && point->item())
	{
		QPoint centerPoint = mapFromScene(point->item()->mapToScene(point->position()));

		int hDelta = pointSizeHint.width() / 2 * devicePixelRatio();
		int vDelta = pointSizeHint.height() / 2 * devicePixelRatio();
		QPoint deltaPoint(hDelta, vDelta);

		viewRect = QRect(centerPoint - deltaPoint, centerPoint + deltaPoint);
	}

	return viewRect;
}

//==================================================================================================

void DrawingWidget::recalculateContentSize(const QRectF& rect)
{
	QRectF sceneRect = this->sceneRect();
	QRectF targetRect = sceneRect;
	qreal dx = 0, dy = 0;

	if (rect.isValid())
	{
		targetRect.setLeft(qMin(rect.left(), sceneRect.left()));
		targetRect.setTop(qMin(rect.top(), sceneRect.top()));
		targetRect.setRight(qMax(rect.right(), sceneRect.right()));
		targetRect.setBottom(qMax(rect.bottom(), sceneRect.bottom()));
	}

	int contentWidth = qRound(targetRect.width() * mScale);
	int contentHeight = qRound(targetRect.height() * mScale);
	int viewportWidth = maximumViewportSize().width();
	int viewportHeight = maximumViewportSize().height();
	int scrollBarExtent = style()->pixelMetric(QStyle::PM_ScrollBarExtent, nullptr, this);

	// Set scroll bar range
	if (contentWidth > viewportWidth)
	{
		int contentLeft = qFloor((targetRect.left() - sceneRect.left()) * mScale);

		if (verticalScrollBarPolicy() == Qt::ScrollBarAsNeeded) viewportWidth -= scrollBarExtent;

		horizontalScrollBar()->setRange(contentLeft - 1, contentLeft + contentWidth - viewportWidth + 1);
		horizontalScrollBar()->setSingleStep(viewportWidth / 80);
		horizontalScrollBar()->setPageStep(viewportWidth);
	}
	else horizontalScrollBar()->setRange(0, 0);

	if (contentHeight > viewportHeight)
	{
		int contentTop = qFloor((targetRect.top() - sceneRect.top()) * mScale);

		if (horizontalScrollBarPolicy() == Qt::ScrollBarAsNeeded) viewportHeight -= scrollBarExtent;

		verticalScrollBar()->setRange(contentTop - 1, contentTop + contentHeight - viewportHeight + 1);
		verticalScrollBar()->setSingleStep(viewportHeight / 80);
		verticalScrollBar()->setPageStep(viewportHeight);
	}
	else verticalScrollBar()->setRange(0, 0);

	// Recalculate transforms
	dx = -targetRect.left() * mScale;
	dy = -targetRect.top() * mScale;

	if (horizontalScrollBar()->maximum() <= horizontalScrollBar()->minimum())
		dx += -(targetRect.width() * mScale - viewportWidth) / 2;
	if (verticalScrollBar()->maximum() <= verticalScrollBar()->minimum())
		dy += -(targetRect.height() * mScale - viewportHeight) / 2;

	mViewportTransform = QTransform();
	mViewportTransform.translate(qRound(dx), qRound(dy));
	mViewportTransform.scale(mScale, mScale);

	mSceneTransform = mViewportTransform.inverted();
}
