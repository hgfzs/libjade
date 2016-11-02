/* DrawingWidget.cpp
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

#include "DrawingWidget.h"
#include "DrawingItem.h"
#include "DrawingItemGroup.h"
#include "DrawingItemPoint.h"
#include "DrawingUndo.h"

DrawingWidget::DrawingWidget() : QAbstractScrollArea()
{
	setMouseTracking(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	mSceneRect = QRectF(-5000, -3750, 10000, 7500);
	mGrid = 50;
	mBackgroundBrush = Qt::white;

	mSelectedItemPoint = nullptr;
	mNewItem = nullptr;
	mMouseDownItem = nullptr;
	mFocusItem = nullptr;

	mFlags = 0;
	mItemSelectionMode = Qt::ContainsItemBoundingRect;

	mMode = DefaultMode;
	mScale = 1.0;

	mUndoStack.setUndoLimit(64);
	connect(&mUndoStack, SIGNAL(cleanChanged(bool)), this, SIGNAL(cleanChanged(bool)));
	connect(&mUndoStack, SIGNAL(canRedoChanged(bool)), this, SIGNAL(canRedoChanged(bool)));
	connect(&mUndoStack, SIGNAL(canUndoChanged(bool)), this, SIGNAL(canUndoChanged(bool)));
	connect(this, SIGNAL(selectionChanged(const QList<DrawingItem*>&)), this, SLOT(updateSelectionCenter()));

	mMouseState = MouseReady;
	mScrollButtonDownHorizontalScrollValue = 0;
	mScrollButtonDownVerticalScrollValue = 0;
	mPanTimer.setInterval(16);
	connect(&mPanTimer, SIGNAL(timeout()), this, SLOT(mousePanEvent()));
	mConsecutivePastes = 0;
}

DrawingWidget::~DrawingWidget()
{
	while (!mClipboardItems.isEmpty()) delete mClipboardItems.takeFirst();
	
	delete mNewItem;
	clearItems();
}

//==================================================================================================

void DrawingWidget::setSceneRect(const QRectF& rect)
{
	mSceneRect = rect;
}

void DrawingWidget::setSceneRect(qreal left, qreal top, qreal width, qreal height)
{
	setSceneRect(QRectF(left, top, width, height));
}

QRectF DrawingWidget::sceneRect() const
{
	return mSceneRect;
}

qreal DrawingWidget::width() const
{
	return sceneRect().width();
}

qreal DrawingWidget::height() const
{
	return sceneRect().height();
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

QPointF DrawingWidget::roundToGrid(const QPointF& scenePos) const
{
	return QPointF(roundToGrid(scenePos.x()), roundToGrid(scenePos.y()));
}

//==================================================================================================

void DrawingWidget::setBackgroundBrush(const QBrush& brush)
{
	mBackgroundBrush = brush;
}

QBrush DrawingWidget::backgroundBrush() const
{
	return mBackgroundBrush;
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

void DrawingWidget::setFlags(Flags flags)
{
	mFlags = flags;
}

void DrawingWidget::setItemSelectionMode(Qt::ItemSelectionMode mode)
{
	mItemSelectionMode = mode;
}

DrawingWidget::Flags DrawingWidget::flags() const
{
	return mFlags;
}

Qt::ItemSelectionMode DrawingWidget::itemSelectionMode() const
{
	return mItemSelectionMode;
}

//==================================================================================================

DrawingWidget::Mode DrawingWidget::mode() const
{
	return mMode;
}

qreal DrawingWidget::scale() const
{
	return mScale;
}

//==================================================================================================

void DrawingWidget::addItem(DrawingItem* item)
{
	if (item && !mItems.contains(item))
	{
		mItems.append(item);
		item->mDrawing = this;
	}
}

void DrawingWidget::insertItem(int index, DrawingItem* item)
{
	if (item && !mItems.contains(item))
	{
		mItems.insert(index, item);
		item->mDrawing = this;
	}
}

void DrawingWidget::removeItem(DrawingItem* item)
{
	if (item && mItems.contains(item))
	{
		if (item->isSelected()) deselectItem(item);
		mItems.removeAll(item);
		item->mDrawing = nullptr;
	}
}

void DrawingWidget::clearItems()
{
	DrawingItem* item;

	while (!mItems.isEmpty())
	{
		item = mItems.first();
		removeItem(item);
		delete item;
	}

	mUndoStack.clear();
	mSelectedItems.clear();
	mMouseDownItem = nullptr;
	mFocusItem = nullptr;
	mInitialPositions.clear();
}

QList<DrawingItem*> DrawingWidget::items() const
{
	return mItems;
}

QList<DrawingItem*> DrawingWidget::items(const QPointF& scenePos) const
{
	QList<DrawingItem*> items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if (itemMatchesPoint(*itemIter, scenePos)) items.append(*itemIter);
	}

	return items;
}

QList<DrawingItem*> DrawingWidget::items(const QRectF& sceneRect) const
{
	QList<DrawingItem*> items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if (itemMatchesRect(*itemIter, sceneRect, mItemSelectionMode))
			items.append(*itemIter);
	}

	return items;
}

DrawingItem* DrawingWidget::itemAt(const QPointF& scenePos) const
{
	DrawingItem* item = nullptr;

	if (mFlags & SelectedItemsOnTop)
	{
		// Favor selected items
		auto itemIter = mSelectedItems.end();
		while (item == nullptr && itemIter != mSelectedItems.begin())
		{
			itemIter--;
			if (itemMatchesPoint(*itemIter, scenePos)) item = *itemIter;
		}
	}

	// Search all items
	auto itemIter = mItems.end();
	while (item == nullptr && itemIter != mItems.begin())
	{
		itemIter--;
		if (itemMatchesPoint(*itemIter, scenePos)) item = *itemIter;
	}

	return item;
}

//==================================================================================================

void DrawingWidget::selectItem(DrawingItem* item)
{
	if (item && !mSelectedItems.contains(item))
	{
		item->setSelected(true);
		mSelectedItems.append(item);
	}
}

void DrawingWidget::deselectItem(DrawingItem* item)
{
	if (item && mSelectedItems.contains(item))
	{
		mSelectedItems.removeAll(item);
		item->setSelected(false);
	}
}

void DrawingWidget::clearSelection()
{
	for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		(*itemIter)->setSelected(false);
	
	mSelectedItems.clear();
}

QList<DrawingItem*> DrawingWidget::selectedItems() const
{
	return mSelectedItems;
}

//==================================================================================================

DrawingItem* DrawingWidget::newItem() const
{
	return mNewItem;
}

DrawingItem* DrawingWidget::mouseDownItem() const
{
	return mMouseDownItem;
}

DrawingItem* DrawingWidget::focusItem() const
{
	return mFocusItem;
}

//==================================================================================================

QSize DrawingWidget::pointSizeHint() const
{
	return QSize(8 * devicePixelRatio(), 8 * devicePixelRatio());
}

QRect DrawingWidget::pointRect(DrawingItemPoint* point) const
{
	QRect viewRect;

	if (point && point->item())
	{
		QPoint centerPoint = mapFromScene(point->item()->mapToScene(point->pos()));
		QSize pointSize = pointSizeHint();

		int hDelta = pointSize.width() / 2 * devicePixelRatio();
		int vDelta = pointSize.height() / 2 * devicePixelRatio();
		QPoint deltaPoint(hDelta, vDelta);

		viewRect = QRect(centerPoint - deltaPoint, centerPoint + deltaPoint);
	}

	return viewRect;
}

//==================================================================================================

void DrawingWidget::centerOn(const QPointF& scenePos)
{
	QPointF oldScenePos = mapToScene(viewport()->rect().center());

	int horizontalDelta = qRound((scenePos.x() - oldScenePos.x()) * mScale);
	int verticalDelta = qRound((scenePos.y() - oldScenePos.y()) * mScale);

	horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalDelta);
	verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);
}

void DrawingWidget::centerOnCursor(const QPointF& scenePos)
{
	QPointF oldScenePos = mapToScene(mapFromGlobal(QCursor::pos()));

	int horizontalDelta = qRound((scenePos.x() - oldScenePos.x()) * mScale);
	int verticalDelta = qRound((scenePos.y() - oldScenePos.y()) * mScale);

	horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalDelta);
	verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);
}

void DrawingWidget::fitToView(const QRectF& sceneRect)
{
	qreal scaleX = (maximumViewportSize().width() - 5) / sceneRect.width();
	qreal scaleY = (maximumViewportSize().height() - 5) / sceneRect.height();

	mScale = qMin(scaleX, scaleY);

	recalculateContentSize(sceneRect);

	centerOn(sceneRect.center());
}

void DrawingWidget::scaleBy(qreal scale)
{
	if (scale > 0)
	{
		QPointF mousePos = mapToScene(mapFromGlobal(QCursor::pos()));
		QRectF scrollBarRect = scrollBarDefinedRect();

		mScale *= scale;

		recalculateContentSize(scrollBarRect);

		if (viewport()->rect().contains(mapFromGlobal(QCursor::pos()))) centerOnCursor(mousePos);
		else centerOn(QPointF());
	}
}

//==================================================================================================

QPointF DrawingWidget::mapToScene(const QPoint& screenPos) const
{
	QPointF p = screenPos;
	p.setX(p.x() + horizontalScrollBar()->value());
	p.setY(p.y() + verticalScrollBar()->value());
	return mSceneTransform.map(p);
}

QRectF DrawingWidget::mapToScene(const QRect& screenRect) const
{
	return QRectF(mapToScene(screenRect.topLeft()), mapToScene(screenRect.bottomRight()));
}

QPoint DrawingWidget::mapFromScene(const QPointF& scenePos) const
{
	QPointF p = mViewportTransform.map(scenePos);
	p.setX(p.x() - horizontalScrollBar()->value());
	p.setY(p.y() - verticalScrollBar()->value());
	return p.toPoint();
}

QRect DrawingWidget::mapFromScene(const QRectF& sceneRect) const
{
	return QRect(mapFromScene(sceneRect.topLeft()), mapFromScene(sceneRect.bottomRight()));
}

QRectF DrawingWidget::visibleRect() const
{
	return QRectF(mapToScene(QPoint(0, 0)), mapToScene(QPoint(viewport()->width(), viewport()->height())));
}

QRectF DrawingWidget::scrollBarDefinedRect() const
{
	QRectF scrollBarRect = mSceneRect;

	if (horizontalScrollBar()->minimum() < horizontalScrollBar()->maximum())
	{
		scrollBarRect.setLeft(horizontalScrollBar()->minimum() / mScale + mSceneRect.left());
		scrollBarRect.setWidth((horizontalScrollBar()->maximum() - horizontalScrollBar()->minimum() +
			maximumViewportSize().width()) / mScale);
	}

	if (verticalScrollBar()->minimum() < verticalScrollBar()->maximum())
	{
		scrollBarRect.setTop(verticalScrollBar()->minimum() / mScale + mSceneRect.top());
		scrollBarRect.setHeight((verticalScrollBar()->maximum() - verticalScrollBar()->minimum() +
			maximumViewportSize().height()) / mScale);
	}

	return scrollBarRect;
}

//==================================================================================================

void DrawingWidget::render(QPainter* painter)
{
	drawBackground(painter);
	drawItems(painter);
	drawForeground(painter);
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
	fitToView(mSceneRect);

	emit scaleChanged(mScale);
	viewport()->update();
}

//==================================================================================================

void DrawingWidget::setDefaultMode()
{
	mMode = DefaultMode;
	setCursor(Qt::ArrowCursor);

	delete mNewItem;
	mNewItem = nullptr;
	emit newItemChanged(mNewItem);

	clearSelection();
	emit selectionChanged(mSelectedItems);

	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingWidget::setScrollMode()
{
	mMode = ScrollMode;
	setCursor(Qt::OpenHandCursor);

	delete mNewItem;
	mNewItem = nullptr;
	emit newItemChanged(mNewItem);

	clearSelection();
	emit selectionChanged(mSelectedItems);

	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingWidget::setZoomMode()
{
	mMode = ZoomMode;
	setCursor(Qt::CrossCursor);

	delete mNewItem;
	mNewItem = nullptr;
	emit newItemChanged(mNewItem);

	clearSelection();
	emit selectionChanged(mSelectedItems);

	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingWidget::setPlaceMode(DrawingItem* item)
{
	if (item)
	{
		mMode = PlaceMode;
		setCursor(Qt::CrossCursor);

		clearSelection();
		emit selectionChanged(mSelectedItems);

		mNewItem = item;
		mNewItem->mDrawing = this;
		mNewItem->setVisible(true);
		mNewItem->setPos(mapToScene(mapFromGlobal(QCursor::pos())));

		emit newItemChanged(mNewItem);
		emit modeChanged(mMode);
		viewport()->update();
	}
	else setDefaultMode();
}

//==================================================================================================

void DrawingWidget::undo()
{
	if (mMode == DefaultMode && mUndoStack.canUndo())
	{
		if ((mFlags & UndoableSelectCommands) == 0) clearSelection();
		mUndoStack.undo();
		viewport()->update();
	}
}

void DrawingWidget::redo()
{
	if (mMode == DefaultMode && mUndoStack.canRedo())
	{
		if ((mFlags & UndoableSelectCommands) == 0) clearSelection();
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
	if (mMode == DefaultMode)
	{
		while (!mClipboardItems.isEmpty()) delete mClipboardItems.takeFirst();
		mClipboardItems = DrawingItem::copyItems(mSelectedItems);
	}
}

void DrawingWidget::paste()
{
	if (mMode == DefaultMode)
	{
		QList<DrawingItem*> newItems = DrawingItem::copyItems(mClipboardItems);

		if (!newItems.isEmpty())
		{
			QUndoCommand* pasteCommand = new QUndoCommand("Paste Items");
			
			// Offset items based on mConsecutivePastes
			qreal offset = 2 * mConsecutivePastes * mGrid;
			QPointF deltaPosition = QPointF(offset, offset) +
				roundToGrid(mMouseEvent.buttonDownScenePos() - newItems.first()->pos());

			for(auto itemIter = newItems.begin(); itemIter != newItems.end(); itemIter++)
			{
				(*itemIter)->setPos((*itemIter)->pos() + deltaPosition);
				(*itemIter)->setSelected(false);
			}

			// Add new items to scene and select them
			addItems(newItems, false, pasteCommand);
			selectItemsCommand(newItems, true, pasteCommand);
			
			pushUndoCommand(pasteCommand);
			
			mConsecutivePastes++;
			viewport()->update();
		}
	}
}

void DrawingWidget::deleteSelection()
{
	if (mMode == DefaultMode)
	{
		QList<DrawingItem*> itemsToRemove = mSelectedItems;
		
		if (!itemsToRemove.isEmpty())
		{
			QUndoCommand* deleteCommand = new QUndoCommand("Delete Items");
			
			selectItemsCommand(QList<DrawingItem*>(), deleteCommand);
			removeItemsCommand(itemsToRemove, deleteCommand);

			pushUndoCommand(deleteCommand);

			viewport()->update();
		}
	}
	else setDefaultMode();
}

//==================================================================================================

void DrawingWidget::selectAll()
{
	if (mMode == DefaultMode)
	{
		QList<DrawingItem*> itemsToSelect;
		
		for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		{
			if ((*itemIter)->canSelect()) itemsToSelect.append(*itemIter);
		}
		
		selectItemsCommand(itemsToSelect, true);
		viewport()->update();
	}
}

void DrawingWidget::selectArea(const QRectF& rect)
{
	if (mMode == DefaultMode && rect.isValid())
	{
		QList<DrawingItem*> itemsToSelect;
		
		for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		{
			if (itemMatchesRect(*itemIter, rect, mItemSelectionMode) && (*itemIter)->canSelect())
				itemsToSelect.append(*itemIter);
		}
		
		selectItemsCommand(itemsToSelect, true);
		viewport()->update();
	}
}

void DrawingWidget::selectArea(const QPainterPath& path)
{
	if (mMode == DefaultMode && !path.isEmpty())
	{
		QList<DrawingItem*> itemsToSelect;
		
		for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		{
			if (itemMatchesPath(*itemIter, path, mItemSelectionMode) && (*itemIter)->canSelect())
				itemsToSelect.append(*itemIter);
		}
		
		selectItemsCommand(itemsToSelect, true);
		viewport()->update();
	}
}

void DrawingWidget::selectNone()
{
	if (mMode == DefaultMode)
	{
		selectItemsCommand(QList<DrawingItem*>(), true);
		viewport()->update();
	}
}

//==================================================================================================

void DrawingWidget::moveSelection(const QPointF& newPos)
{
	if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		QHash<DrawingItem*,QPointF> newPositions;
		QPointF deltaPos = newPos - mSelectedItems.first()->pos();

		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		{
			if ((*itemIter)->canMove())
				newPositions[*itemIter] = (*itemIter)->pos() + deltaPos;
		}

		if (!newPositions.isEmpty())
		{
			moveItemsCommand(newPositions.keys(), newPositions, true);
			viewport()->update();
		}
	}
}

void DrawingWidget::resizeSelection(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	if (mMode == DefaultMode && mSelectedItems.size() == 1 && mSelectedItems.first()->canResize() &&
		mSelectedItems.first()->points().contains(itemPoint))
	{
		resizeItemCommand(itemPoint, scenePos, true, true);
		viewport()->update();
	}
}

void DrawingWidget::rotateSelection()
{
	if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToRotate;
		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		{
			if ((*itemIter)->canRotate()) itemsToRotate.append(*itemIter);
		}

		if (!itemsToRotate.isEmpty())
		{
			rotateItemsCommand(itemsToRotate, roundToGrid(mSelectionCenter));
			viewport()->update();
		}
	}
}

void DrawingWidget::rotateBackSelection()
{
	if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToRotate;
		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		{
			if ((*itemIter)->canRotate()) itemsToRotate.append(*itemIter);
		}

		if (!itemsToRotate.isEmpty())
		{
			rotateBackItemsCommand(itemsToRotate, roundToGrid(mSelectionCenter));
			viewport()->update();
		}
	}
}

void DrawingWidget::flipSelection()
{
	if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToFlip;
		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		{
			if ((*itemIter)->canFlip()) itemsToFlip.append(*itemIter);
		}

		if (!itemsToFlip.isEmpty())
		{
			flipItemsCommand(itemsToFlip, roundToGrid(mSelectionCenter));
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
		QList<DrawingItem*> itemsOrdered = mItems;
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
		QList<DrawingItem*> itemsOrdered = mItems;
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
		QList<DrawingItem*> itemsOrdered = mItems;
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
		QList<DrawingItem*> itemsOrdered = mItems;
		DrawingItem* item;
		int itemIndex;

		while (!itemsToReorder.empty())
		{
			item = itemsToReorder.takeLast();

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
		DrawingItem* item = nullptr;
		if (mSelectedItems.size() == 1) item = mSelectedItems.first();

		if (item && item->canInsertPoints()) 
		{
			item->insertItemPoint(roundToGrid(mMouseEvent.buttonDownScenePos()));
			viewport()->update();
		}
	}
}

void DrawingWidget::removeItemPoint()
{
	if (mMode == DefaultMode)
	{
		DrawingItem* item = nullptr;
		if (mSelectedItems.size() == 1) item = mSelectedItems.first();

		if (item && item->canRemovePoints()) 
		{
			item->removeItemPoint(roundToGrid(mMouseEvent.buttonDownScenePos()));
			viewport()->update();
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

		itemGroup->setPos(items.first()->pos());
		for(auto iter = items.begin(); iter != items.end(); iter++)
			(*iter)->setPos(itemGroup->mapFromScene((*iter)->pos()));
		itemGroup->setItems(items);
		itemsToAdd.append(itemGroup);

		selectItemsCommand(QList<DrawingItem*>(), true, command);
		removeItemsCommand(itemsToGroup, command);
		addItemsCommand(itemsToAdd, false, command);
		selectItemsCommand(itemsToAdd, true, command);
		
		pushUndoCommand(command);
		viewport()->update();
	}
}

void DrawingWidget::ungroup()
{
	if (mMode == DefaultMode && mSelectedItems.size() == 1)
	{
		DrawingItemGroup* itemGroup = dynamic_cast<DrawingItemGroup*>(mSelectedItems.first());
		if (itemGroup)
		{
			QUndoCommand* command = new QUndoCommand("Ungroup Items");
			QList<DrawingItem*> itemsToRemove;
			itemsToRemove.append(itemGroup);

			QList<DrawingItem*> items = DrawingItem::copyItems(itemGroup->items());
			for(auto iter = items.begin(); iter != items.end(); iter++)
				(*iter)->setPos(itemGroup->mapToScene((*iter)->pos()));

			selectItemsCommand(QList<DrawingItem*>(), true, command);
			removeItemsCommand(itemsToRemove, command);
			addItemsCommand(items, false, command);
			selectItemsCommand(items, true, command);
			
			pushUndoCommand(command);
			viewport()->update();
		}
	}
}

//==================================================================================================

void DrawingWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		mMouseEvent.setFromEvent(event, this);

		switch (mMode)
		{
		case ScrollMode:
			setCursor(Qt::ClosedHandCursor);
			mScrollButtonDownHorizontalScrollValue = horizontalScrollBar()->value();
			mScrollButtonDownVerticalScrollValue = verticalScrollBar()->value();
			break;
		case ZoomMode:
			break;
		case PlaceMode:
			placeModeMousePressEvent(&mMouseEvent);
			break;
		default:
			defaultMousePressEvent(&mMouseEvent);
			break;
		}

	}
	else if (event->button() == Qt::MiddleButton)
	{
		setCursor(Qt::SizeAllCursor);

		mPanStartPos = event->pos();
		mPanCurrentPos = event->pos();
		mPanTimer.start();
	}
	else if (event->button() == Qt::RightButton)
	{
		mMouseEvent.setFromEvent(event, this);
	}

	viewport()->update();
}

void DrawingWidget::mouseMoveEvent(QMouseEvent* event)
{
	mMouseEvent.setFromEvent(event, this);

	switch (mMode)
	{
	case ScrollMode:
		if ((mMouseEvent.buttons() & Qt::LeftButton) && mMouseEvent.isDragged())
		{
			horizontalScrollBar()->setValue(
				mScrollButtonDownHorizontalScrollValue - (mMouseEvent.pos().x() - mMouseEvent.buttonDownPos().x()));
			verticalScrollBar()->setValue(
				mScrollButtonDownVerticalScrollValue - (mMouseEvent.pos().y() - mMouseEvent.buttonDownPos().y()));
		}
		break;
	case ZoomMode:
		if ((mMouseEvent.buttons() & Qt::LeftButton) && mMouseEvent.isDragged())
		{
			QPoint p1 = mMouseEvent.pos();
			QPoint p2 = mMouseEvent.buttonDownPos();
			mRubberBandRect = QRect(qMin(p1.x(), p2.x()), qMin(p1.y(), p2.y()), qAbs(p2.x() - p1.x()), qAbs(p2.y() - p1.y()));
		}
	case PlaceMode:
		placeModeMouseMoveEvent(&mMouseEvent);
		break;
	default:
		defaultMouseMoveEvent(&mMouseEvent);
		break;
	}

	if (mPanTimer.isActive()) mPanCurrentPos = event->pos();

	viewport()->update();
}

void DrawingWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		mMouseEvent.setFromEvent(event, this);

		switch (mMode)
		{
		case ScrollMode:
			setCursor(Qt::OpenHandCursor);
			break;
		case ZoomMode:
			if (mRubberBandRect.isValid())
			{
				fitToView(mapToScene(mRubberBandRect));
				mRubberBandRect = QRect();

				emit scaleChanged(mScale);
				setDefaultMode();
			}
			break;
		case PlaceMode:
			placeModeMouseReleaseEvent(&mMouseEvent);
			break;
		default:
			defaultMouseReleaseEvent(&mMouseEvent);
			break;
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		if (mMode != DefaultMode) setDefaultMode();
	}

	if (mPanTimer.isActive())
	{
		setCursor(Qt::ArrowCursor);
		mPanTimer.stop();
	}

	viewport()->update();
}

void DrawingWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	mMouseEvent.setFromEvent(event, this);

	if (mMode == PlaceMode) placeModeMouseDoubleClickEvent(&mMouseEvent);
	else if (mMode != DefaultMode) setDefaultMode();
	else defaultMouseDoubleClickEvent(&mMouseEvent);

	viewport()->update();
}

void DrawingWidget::wheelEvent(QWheelEvent* event)
{
	if (event->modifiers() && Qt::ControlModifier)
	{
		if (event->delta() > 0) zoomIn();
		else if (event->delta() < 0) zoomOut();
	}
	else QAbstractScrollArea::wheelEvent(event);
}

//==================================================================================================

void DrawingWidget::keyPressEvent(QKeyEvent* event)
{
	if (mFocusItem) mFocusItem->keyPressEvent(event);
}

void DrawingWidget::keyReleaseEvent(QKeyEvent* event)
{
	if (mFocusItem) mFocusItem->keyReleaseEvent(event);
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
	drawItems(&painter);
	drawForeground(&painter);

	painter.end();

	// Render scene image on to widget
	QPainter widgetPainter(viewport());
	widgetPainter.drawImage(0, 0, image);

	Q_UNUSED(event);
}

void DrawingWidget::drawBackground(QPainter* painter)
{
	painter->setBrush(mBackgroundBrush);
	painter->setPen(QPen(mBackgroundBrush, 1));
	painter->drawRect(viewport()->rect());
}

void DrawingWidget::drawItems(QPainter* painter)
{
	// Draw items
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			painter->translate((*itemIter)->pos());
			painter->setTransform((*itemIter)->transform().inverted(), true);
			(*itemIter)->paint(painter);
			painter->setTransform((*itemIter)->transform(), true);
			painter->translate(-(*itemIter)->pos());
		}
	}

	// Draw new item
	if (mNewItem)
	{
		painter->translate(mNewItem->pos());
		painter->setTransform(mNewItem->transform().inverted(), true);
		mNewItem->paint(painter);
		painter->setTransform(mNewItem->transform(), true);
		painter->translate(-mNewItem->pos());
	}

	// Draw item points
	QColor color = mBackgroundBrush.color();
	color.setRed(255 - color.red());
	color.setGreen(255 - color.green());
	color.setBlue(255 - color.blue());

	painter->save();

	painter->resetTransform();
	painter->setRenderHints(QPainter::Antialiasing, false);
	painter->setPen(QPen(color, 1));
	painter->setBrush(QColor(0, 224, 0));

	for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			QList<DrawingItemPoint*> itemPoints = (*itemIter)->points();

			for(auto pointIter = itemPoints.begin(); pointIter != itemPoints.end(); pointIter++)
			{
				QRect pointRect = DrawingWidget::pointRect(*pointIter).adjusted(0, 0, -1, -1);

				if (((*pointIter)->flags() & DrawingItemPoint::Control) ||
					((*pointIter)->flags() == DrawingItemPoint::NoFlags))
				{
					pointRect.adjust(1, 1, -1, -1);
					painter->drawRect(pointRect);
				}

				if ((*pointIter)->flags() & DrawingItemPoint::Connection)
				{
					for(int x = 0; x <= pointRect.width(); x++)
					{
						painter->drawPoint(pointRect.left() + x, pointRect.bottom() + 1 - x);
						painter->drawPoint(pointRect.left() + x, pointRect.top() + x);
					}
				}
			}
		}
	}

	painter->restore();

	// Draw hotpoints
	QList<DrawingItem*> items = mSelectedItems;
	if (mNewItem) items.prepend(mNewItem);

	painter->save();

	painter->resetTransform();
	painter->setRenderHints(QPainter::Antialiasing, false);
	painter->setBrush(QColor(255, 128, 0, 128));
	painter->setPen(QPen(painter->brush(), 1));

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		QList<DrawingItemPoint*> itemPoints = (*itemIter)->points();

		for(auto pointIter = itemPoints.begin(); pointIter != itemPoints.end(); pointIter++)
		{
			for(auto otherItemIter = mItems.begin(); otherItemIter != mItems.end(); otherItemIter++)
			{
				if ((*itemIter) != (*otherItemIter))
				{
					QList<DrawingItemPoint*> otherItemPoints = (*otherItemIter)->points();

					for(auto otherItemPointIter = otherItemPoints.begin();
						otherItemPointIter != otherItemPoints.end(); otherItemPointIter++)
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

	// Draw rubber band
	QStyleOptionRubberBand option;
	option.initFrom(viewport());
	option.rect = mRubberBandRect;
	option.shape = QRubberBand::Rectangle;

	painter->save();
	painter->resetTransform();

	QStyleHintReturnMask mask;
	if (viewport()->style()->styleHint(QStyle::SH_RubberBand_Mask, &option, viewport(), &mask))
		painter->setClipRegion(mask.region, Qt::IntersectClip);

	viewport()->style()->drawControl(QStyle::CE_RubberBand, &option, painter, viewport());

	painter->restore();
}

void DrawingWidget::drawForeground(QPainter* painter)
{
	Q_UNUSED(painter);
}

//==================================================================================================

void DrawingWidget::resizeEvent(QResizeEvent* event)
{
	QAbstractScrollArea::resizeEvent(event);
	recalculateContentSize();
}

//==================================================================================================

void DrawingWidget::defaultMousePressEvent(DrawingMouseEvent* event)
{
	mMouseState = MouseSelect;

	if (event->button() == Qt::LeftButton)
	{
		mMouseDownItem = itemAt(event->scenePos());

		if (mMouseDownItem)
		{
			mInitialPositions.clear();
			for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
				mInitialPositions[*itemIter] = (*itemIter)->pos();

			if (mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
			{
				mSelectedItemPoint = mMouseDownItem->pointAt(mMouseDownItem->mapFromScene(event->scenePos()));
				if (mSelectedItemPoint && !mSelectedItemPoint->isControlPoint()) mSelectedItemPoint = nullptr;
				
				mMouseDownItem->mousePressEvent(event);
			}
		}

		mFocusItem = mMouseDownItem;

		event->accept();
	}
}

void DrawingWidget::defaultMouseMoveEvent(DrawingMouseEvent* event)
{
	QHash<DrawingItem*,QPointF> newPositions;

	if (event->buttons() & Qt::LeftButton)
	{
		switch (mMouseState)
		{
		case MouseSelect:
			if (event->isDragged())
			{
				if (mMouseDownItem && mMouseDownItem->isSelected())
				{
					bool resizeItem = (mSelectedItems.size() == 1 &&
									   mSelectedItems.first()->canResize() &&
									   mSelectedItemPoint && mSelectedItemPoint->isControlPoint());
					mMouseState = (resizeItem) ? MouseResizeItem : MouseMoveItems;
				}
				else mMouseState = MouseRubberBand;
			}
			break;

		case MouseMoveItems:
			for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
			{
				if ((*itemIter)->canMove())
				{
					newPositions[*itemIter] = mInitialPositions[*itemIter] +
						roundToGrid(event->scenePos() - event->buttonDownScenePos());
				}
			}

			if (!newPositions.isEmpty())
				moveItemsCommand(newPositions.keys(), newPositions, false);
			break;

		case MouseResizeItem:
			resizeItemCommand(mSelectedItemPoint, roundToGrid(event->scenePos()), false, true);
			break;

		case MouseRubberBand:
			mRubberBandRect = QRect(event->pos(), event->buttonDownPos()).normalized();
			break;

		default:
			break;
		}

		if (mMouseDownItem && mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
			mMouseDownItem->mouseMoveEvent(event);
	}
}

void DrawingWidget::defaultMouseReleaseEvent(DrawingMouseEvent* event)
{
	bool controlDown = ((event->modifiers() & Qt::ControlModifier) != 0);
	QList<DrawingItem*> newSelection = (controlDown) ? mSelectedItems : QList<DrawingItem*>();
	QHash<DrawingItem*,QPointF> newPositions;
	QRectF rubberBandSceneRect;
	
	switch (mMouseState)
	{
	case MouseSelect:
		if (mMouseDownItem)
		{
			if (controlDown && mMouseDownItem->isSelected()) newSelection.removeAll(mMouseDownItem);
			else if (mMouseDownItem->canSelect()) newSelection.append(mMouseDownItem);
		}
		selectItemsCommand(newSelection, !controlDown);
		break;

	case MouseMoveItems:
		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		{
			if ((*itemIter)->canMove())
			{
				newPositions[*itemIter] = mInitialPositions[*itemIter] +
					roundToGrid(event->scenePos() - event->buttonDownScenePos());
			}
		}

		if (!newPositions.isEmpty())
			moveItemsCommand(newPositions.keys(), newPositions, true);
		break;

	case MouseResizeItem:
		resizeItemCommand(mSelectedItemPoint, roundToGrid(event->scenePos()), true, true);
		break;

	case MouseRubberBand:
		rubberBandSceneRect = mapToScene(mRubberBandRect);
		for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		{
			if (itemMatchesRect(*itemIter, rubberBandSceneRect, mItemSelectionMode) &&
				(*itemIter)->canSelect() && !newSelection.contains(*itemIter))
			{
				newSelection.append(*itemIter);
			}
		}	
		selectItemsCommand(newSelection, true);
		mRubberBandRect = QRect();
		break;

	default:
		break;
	}

	if (mMouseDownItem && mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
		mMouseDownItem->mouseReleaseEvent(event);

	mSelectedItemPoint = nullptr;
	mMouseState = MouseReady;
	mConsecutivePastes = 0;

	updateSelectionCenter();
}

void DrawingWidget::defaultMouseDoubleClickEvent(DrawingMouseEvent* event)
{
	mMouseState = MouseSelect;

	if (event->button() == Qt::LeftButton)
	{
		mMouseDownItem = itemAt(event->scenePos());

		if (mMouseDownItem)
		{
			mInitialPositions.clear();
			for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
				mInitialPositions[*itemIter] = (*itemIter)->pos();

			if (mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
			{
				mSelectedItemPoint = mMouseDownItem->pointAt(mMouseDownItem->mapFromScene(event->scenePos()));
				if (mSelectedItemPoint && !mSelectedItemPoint->isControlPoint()) mSelectedItemPoint = nullptr;
				
				mMouseDownItem->mouseDoubleClickEvent(event);
			}
		}

		mFocusItem = mMouseDownItem;

		event->accept();
	}
}

//==================================================================================================

void DrawingWidget::placeModeMousePressEvent(DrawingMouseEvent* event)
{
	if (mNewItem)
	{
		QList<DrawingItem*> newItems;
		newItems.append(mNewItem);

		mNewItem->newMousePressEvent(event);
		emit itemsGeometryChanged(newItems);
	}
}

void DrawingWidget::placeModeMouseMoveEvent(DrawingMouseEvent* event)
{
	if (mNewItem)
	{
		QList<DrawingItem*> newItems;
		newItems.append(mNewItem);

		mNewItem->newMouseMoveEvent(event);
		emit itemsGeometryChanged(newItems);
	}
}

void DrawingWidget::placeModeMouseReleaseEvent(DrawingMouseEvent* event)
{
	if (mNewItem && mNewItem->newMouseReleaseEvent(event) && mNewItem->isValid())
	{
		QList<DrawingItem*> newItems;
		newItems.append(mNewItem);

		mNewItem->mDrawing = nullptr;
		addItemsCommand(newItems, true);

		mNewItem = mNewItem->copy();
		mNewItem->mDrawing = this;
		mNewItem->setPos(roundToGrid(event->scenePos()));
		emit newItemChanged(mNewItem);

		if (!mNewItem->newItemCopyEvent()) setDefaultMode();
	}
}

void DrawingWidget::placeModeMouseDoubleClickEvent(DrawingMouseEvent* event)
{
	if (mNewItem && mNewItem->newMouseDoubleClickEvent(event) && mNewItem->isValid())
	{
		QList<DrawingItem*> newItems;
		newItems.append(mNewItem);

		mNewItem->mDrawing = nullptr;
		addItemsCommand(newItems, true);

		mNewItem = mNewItem->copy();
		mNewItem->mDrawing = this;
		mNewItem->setPos(roundToGrid(event->scenePos()));
		emit newItemChanged(mNewItem);

		if (!mNewItem->newItemCopyEvent()) setDefaultMode();
	}
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
	QRectF visibleRect = DrawingWidget::visibleRect();

	if (mPanCurrentPos.x() - mPanStartPos.x() < 0)
	{
		int delta = (mPanCurrentPos.x() - mPanStartPos.x()) / 16;

		if (horizontalScrollBar()->value() + delta < horizontalScrollBar()->minimum())
		{
			if (horizontalScrollBar()->minimum() >= horizontalScrollBar()->maximum())
				horizontalScrollBar()->setMinimum((visibleRect.left() - mSceneRect.left()) * mScale + delta);
			else
				horizontalScrollBar()->setMinimum(horizontalScrollBar()->value() + delta);

			horizontalScrollBar()->setValue(horizontalScrollBar()->minimum());
		}
		else horizontalScrollBar()->setValue(horizontalScrollBar()->value() + delta);
	}
	else if (mPanCurrentPos.x() - mPanStartPos.x() > 0)
	{
		int delta = (mPanCurrentPos.x() - mPanStartPos.x()) / 16;

		if (horizontalScrollBar()->value() + delta > horizontalScrollBar()->maximum())
		{
			if (horizontalScrollBar()->minimum() > horizontalScrollBar()->maximum())
				horizontalScrollBar()->setMaximum((mSceneRect.right() - visibleRect.right()) * mScale + delta);
			else
				horizontalScrollBar()->setMaximum(horizontalScrollBar()->value() + delta);

			horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
		}
		else horizontalScrollBar()->setValue(horizontalScrollBar()->value() + delta);
	}

	if (mPanCurrentPos.y() - mPanStartPos.y() < 0)
	{
		int delta = (mPanCurrentPos.y() - mPanStartPos.y()) / 16;

		if (verticalScrollBar()->value() + delta < verticalScrollBar()->minimum())
		{
			if (verticalScrollBar()->minimum() >= verticalScrollBar()->maximum())
				verticalScrollBar()->setMinimum((visibleRect.top() - mSceneRect.top()) * mScale + delta);
			else
				verticalScrollBar()->setMinimum(verticalScrollBar()->value() + delta);

			verticalScrollBar()->setValue(verticalScrollBar()->minimum());
		}
		else verticalScrollBar()->setValue(verticalScrollBar()->value() + delta);
	}
	else if (mPanCurrentPos.y() - mPanStartPos.y() > 0)
	{
		int delta = (mPanCurrentPos.y() - mPanStartPos.y()) / 16;

		if (verticalScrollBar()->value() + delta > verticalScrollBar()->maximum())
		{
			if (verticalScrollBar()->minimum() >= verticalScrollBar()->maximum())
				verticalScrollBar()->setMaximum((mSceneRect.bottom() - visibleRect.bottom()) * mScale + delta);
			else
				verticalScrollBar()->setMaximum(verticalScrollBar()->value() + delta);

			verticalScrollBar()->setValue(verticalScrollBar()->maximum());
		}
		else verticalScrollBar()->setValue(verticalScrollBar()->value() + delta);
	}
}

//==================================================================================================

void DrawingWidget::addItemsCommand(const QList<DrawingItem*>& items, bool place, QUndoCommand* command)
{
	DrawingAddItemsCommand* addCommand = new DrawingAddItemsCommand(this, items, command);

	addCommand->redo();
	if (place) placeItems(items, addCommand);
	addCommand->undo();

	if (!command) pushUndoCommand(addCommand);
}

void DrawingWidget::removeItemsCommand(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	DrawingRemoveItemsCommand* removeCommand = new DrawingRemoveItemsCommand(this, items, command);

	removeCommand->redo();
	unplaceItems(items, removeCommand);
	removeCommand->undo();

	if (!command) pushUndoCommand(removeCommand);
}

void DrawingWidget::moveItemsCommand(const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& newPos,
	bool place, QUndoCommand* command)
{
	DrawingMoveItemsCommand* moveCommand =
		new DrawingMoveItemsCommand(this, items, newPos, place, command);

	moveCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, moveCommand);
	if (place) placeItems(items, moveCommand);
	moveCommand->undo();

	if (!command) pushUndoCommand(moveCommand);
}

void DrawingWidget::resizeItemCommand(DrawingItemPoint* itemPoint, const QPointF& scenePos,
	bool place, bool disconnect, QUndoCommand* command)
{
	if (itemPoint && itemPoint->item())
	{
		DrawingResizeItemCommand* resizeCommand =
			new DrawingResizeItemCommand(this, itemPoint, scenePos, place, command);
		QList<DrawingItem*> resizeItems;
		resizeItems.append(itemPoint->item());

		resizeCommand->redo();
		if (disconnect) disconnectAll(itemPoint, resizeCommand);
		tryToMaintainConnections(resizeItems, true,
			!(itemPoint->flags() & DrawingItemPoint::Free), itemPoint, resizeCommand);
		if (place) placeItems(resizeItems, resizeCommand);
		resizeCommand->undo();

		if (!command) pushUndoCommand(resizeCommand);
	}
}

void DrawingWidget::rotateItemsCommand(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command)
{
	DrawingRotateItemsCommand* rotateCommand =
		new DrawingRotateItemsCommand(this, items, scenePos, command);

	rotateCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, rotateCommand);
	rotateCommand->undo();

	if (!command) pushUndoCommand(rotateCommand);
}

void DrawingWidget::rotateBackItemsCommand(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command)
{
	DrawingRotateBackItemsCommand* rotateCommand =
		new DrawingRotateBackItemsCommand(this, items, scenePos, command);

	rotateCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, rotateCommand);
	rotateCommand->undo();

	if (!command) pushUndoCommand(rotateCommand);
}

void DrawingWidget::flipItemsCommand(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command)
{
	DrawingFlipItemsCommand* flipCommand =
		new DrawingFlipItemsCommand(this, items, scenePos, command);

	flipCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, flipCommand);
	flipCommand->undo();

	if (!command) pushUndoCommand(flipCommand);
}

void DrawingWidget::reorderItemsCommand(const QList<DrawingItem*>& itemsOrdered, QUndoCommand* command)
{
	DrawingReorderItemsCommand* selectCommand = 
		new DrawingReorderItemsCommand(this, itemsOrdered, command);
			
	if (!command) pushUndoCommand(selectCommand);
}

void DrawingWidget::selectItemsCommand(const QList<DrawingItem*>& items, bool finalSelect, 
	QUndoCommand* command)
{
	if (mFlags & UndoableSelectCommands)
	{
		DrawingSelectItemsCommand* selectCommand = 
			new DrawingSelectItemsCommand(this, items, finalSelect, command);
			
		if (!command) pushUndoCommand(selectCommand);
	}
	else selectItems(items);
}
	
void DrawingWidget::connectItemPointsCommand(DrawingItemPoint* point1, DrawingItemPoint* point2,
	QUndoCommand* command)
{
	DrawingItemPointConnectCommand* connectCommand = 
		new DrawingItemPointConnectCommand(this, point1, point2, command);
	
	QPointF point0Pos = point1->item()->mapToScene(point1->pos());
	QPointF point1Pos = point2->item()->mapToScene(point2->pos());

	if (point0Pos != point1Pos)
	{
		if (point2->isControlPoint() && point2->item()->canResize())
			resizeItemCommand(point2, point0Pos, false, true, connectCommand);
		else if (point1->isControlPoint() && point1->item()->canResize())
			resizeItemCommand(point1, point1Pos, false, true, connectCommand);
	}

	if (!command) pushUndoCommand(connectCommand);
}

void DrawingWidget::disconnectItemPointsCommand(DrawingItemPoint* point1, DrawingItemPoint* point2,
	QUndoCommand* command)
{
	DrawingItemPointDisconnectCommand* disconnectCommand =
		new DrawingItemPointDisconnectCommand(this, point1, point2, command);

	if (!command) pushUndoCommand(disconnectCommand);
}

//==================================================================================================

void DrawingWidget::placeItems(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	QList<DrawingItemPoint*> itemPoints, otherItemPoints;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		for(auto otherItemIter = mItems.begin(); otherItemIter != mItems.end(); otherItemIter++)
		{
			if (!items.contains(*otherItemIter) && (mNewItem == nullptr || (*otherItemIter) != mNewItem))
			{
				itemPoints = (*itemIter)->points();
				otherItemPoints = (*otherItemIter)->points();

				for(auto itemPointIter = itemPoints.begin(); itemPointIter != itemPoints.end(); itemPointIter++)
				{
					for(auto otherItemPointIter = otherItemPoints.begin(); otherItemPointIter != otherItemPoints.end(); otherItemPointIter++)
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

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		item = *itemIter;
		itemPoints = item->points();

		for(auto itemPointIter = itemPoints.begin(); itemPointIter != itemPoints.end(); itemPointIter++)
		{
			itemPoint = *itemPointIter;
			targetPoints = itemPoint->connections();
			for(auto targetPointIter = targetPoints.begin(); targetPointIter != targetPoints.end(); targetPointIter++)
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

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		item = *itemIter;
		itemPoints = item->points();

		for(auto itemPointIter = itemPoints.begin(); itemPointIter != itemPoints.end(); itemPointIter++)
		{
			itemPoint = *itemPointIter;
			if (itemPoint != pointToSkip && (checkControlPoints || !(itemPoint->flags() & DrawingItemPoint::Control)))
			{
				targetPoints = itemPoint->connections();
				for(auto targetPointIter = targetPoints.begin(); targetPointIter != targetPoints.end(); targetPointIter++)
				{
					targetItemPoint = *targetPointIter;
					targetItem = targetItemPoint->item();

					if (item->mapToScene(itemPoint->pos()) != targetItem->mapToScene(targetItemPoint->pos()))
					{
						// Try to maintain the connection by resizing targetPoint if possible
						if (allowResize && targetItem->canResize() &&
							(targetItemPoint->flags() & DrawingItemPoint::Free) &&
							!shouldDisconnect(itemPoint, targetItemPoint))
						{
							resizeItemCommand(targetItemPoint, item->mapToScene(itemPoint->pos()),
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

void DrawingWidget::disconnectAll(DrawingItemPoint* itemPoint, QUndoCommand* command)
{
	QList<DrawingItemPoint*> targetPoints;

	if (itemPoint)
	{
		targetPoints = itemPoint->connections();
		for(auto targetPointIter = targetPoints.begin(); targetPointIter != targetPoints.end(); targetPointIter++)
			disconnectItemPointsCommand(itemPoint, *targetPointIter, command);
	}
}

//==================================================================================================

void DrawingWidget::addItems(const QList<DrawingItem*>& items)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		addItem(*itemIter);

	emit numberOfItemsChanged(mItems.size());
}

void DrawingWidget::insertItems(const QList<DrawingItem*>& items, const QHash<DrawingItem*,int>& index)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		insertItem(index[*itemIter], *itemIter);

	emit numberOfItemsChanged(mItems.size());
}

void DrawingWidget::removeItems(const QList<DrawingItem*>& items)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		removeItem(*itemIter);

	emit numberOfItemsChanged(mItems.size());
}

void DrawingWidget::moveItems(const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& scenePos)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->moveItem(scenePos[*itemIter]);

	emit itemsGeometryChanged(items);
}

void DrawingWidget::resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	if (itemPoint && itemPoint->item()) 
	{
		QList<DrawingItem*> items;
		items.append(itemPoint->item());

		itemPoint->item()->resizeItem(itemPoint, scenePos);

		emit itemsGeometryChanged(items);
	}
}

void DrawingWidget::rotateItems(const QList<DrawingItem*>& items, const QPointF& scenePos)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->rotateItem(scenePos);
	
	emit itemsGeometryChanged(items);
}

void DrawingWidget::rotateBackItems(const QList<DrawingItem*>& items, const QPointF& scenePos)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->rotateBackItem(scenePos);
	
	emit itemsGeometryChanged(items);
}

void DrawingWidget::flipItems(const QList<DrawingItem*>& items, const QPointF& scenePos)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->flipItem(scenePos);
	
	emit itemsGeometryChanged(items);
}

void DrawingWidget::reorderItems(const QList<DrawingItem*>& items)
{
	mItems = items;
}

void DrawingWidget::selectItems(const QList<DrawingItem*>& items)
{
	for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++) 
		(*itemIter)->setSelected(false);
	
	mSelectedItems = items;
	
	for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++) 
		(*itemIter)->setSelected(true);
	
	emit selectionChanged(mSelectedItems);
}

void DrawingWidget::insertItemPoint(DrawingItem* item, DrawingItemPoint* itemPoint, int pointIndex)
{
	if (item && itemPoint)
	{
		QList<DrawingItem*> items;
		items.append(item);

		item->insertPoint(pointIndex, itemPoint);

		emit itemsGeometryChanged(items);
	}
}

void DrawingWidget::removeItemPoint(DrawingItem* item, DrawingItemPoint* itemPoint)
{
	if (item && itemPoint)
	{
		QList<DrawingItem*> items;
		items.append(item);

		item->removePoint(itemPoint);

		emit itemsGeometryChanged(items);
	}
}

void DrawingWidget::connectItemPoints(DrawingItemPoint* point1, DrawingItemPoint* point2)
{
	if (point1 && point2) 
	{
		point1->addConnection(point2);
		point2->addConnection(point1);
	}
}

void DrawingWidget::disconnectItemPoints(DrawingItemPoint* point1, DrawingItemPoint* point2)
{
	if (point1 && point2) 
	{
		point1->removeConnection(point2);
		point2->removeConnection(point1);
	}
}

//==================================================================================================

bool DrawingWidget::itemMatchesPoint(DrawingItem* item, const QPointF& scenePos) const
{
	bool match = false;

	if (item && item->isVisible())
	{
		// Check item shape
		match = item->shape().contains(item->mapFromScene(scenePos));

		// Check item points
		if (!match && item->isSelected())
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			QRectF pointSceneRect;

			for(auto pointIter = itemPoints.begin(); !match && pointIter != itemPoints.end(); pointIter++)
			{
				pointSceneRect = mapToScene(pointRect(*pointIter));
				match = pointSceneRect.contains(scenePos);
			}
		}
	}

	return match;
}

bool DrawingWidget::itemMatchesRect(DrawingItem* item, const QRectF& rect, Qt::ItemSelectionMode mode) const
{
	bool match = false;

	if (item && item->isVisible())
	{
		// Check item boundingRect or shape
		switch (mode)
		{
		case Qt::IntersectsItemShape:
			match = item->shape().intersects(item->mapFromScene(rect));
			break;
		case Qt::ContainsItemShape:
			match = rect.contains(item->mapToScene(item->shape().boundingRect()));
			break;
		case Qt::IntersectsItemBoundingRect:
			match = rect.intersects(item->mapToScene(item->boundingRect()));
			break;
		default:	// Qt::ContainsItemBoundingRect
			match = rect.contains(item->mapToScene(item->boundingRect()));
			break;
		}

		// Check item points
		if (!match && item->isSelected())
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			QRectF pointSceneRect;

			for(auto pointIter = itemPoints.begin(); !match && pointIter != itemPoints.end(); pointIter++)
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
			match = path.contains(item->mapToScene(item->shape().boundingRect()));
			break;
		case Qt::IntersectsItemBoundingRect:
			match = path.intersects(item->mapToScene(item->boundingRect()));
			break;
		default:	// Qt::ContainsItemBoundingRect
			match = path.contains(item->mapToScene(item->boundingRect()));
			break;
		}

		// Check item points
		if (!match && item->isSelected())
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			QRectF pointSceneRect;

			for(auto pointIter = itemPoints.begin(); !match && pointIter != itemPoints.end(); pointIter++)
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

//==================================================================================================

bool DrawingWidget::shouldConnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const
{
	bool shouldConnect = false;

	if (point1 && point1->item() && point2 && point2->item() && point1->item() != point2->item())
	{
		qreal threshold = mGrid / 4000;
		QPointF vec = point1->item()->mapToScene(point1->pos()) - point2->item()->mapToScene(point2->pos());
		qreal distance = qSqrt(vec.x() * vec.x() + vec.y() * vec.y());

		shouldConnect = (point1->isConnectionPoint() && point2->isConnectionPoint() &&
			(point1->isFree() || point2->isFree()) &&
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
			point1->item()->mapToScene(point1->pos()) != point2->item()->mapToScene(point2->pos()) &&
			!point2->isControlPoint());
	}

	return shouldDisconnect;
}

//==================================================================================================

void DrawingWidget::recalculateContentSize(const QRectF& targetSceneRect)
{
	QRectF targetRect;
	if (targetSceneRect.isValid())
	{
		targetRect.setLeft(qMin(targetSceneRect.left(), mSceneRect.left()));
		targetRect.setTop(qMin(targetSceneRect.top(), mSceneRect.top()));
		targetRect.setRight(qMax(targetSceneRect.right(), mSceneRect.right()));
		targetRect.setBottom(qMax(targetSceneRect.bottom(), mSceneRect.bottom()));
	}
	else targetRect = mSceneRect;

	int contentWidth = qRound(targetRect.width() * mScale);
	int contentHeight = qRound(targetRect.height() * mScale);
	int viewportWidth = maximumViewportSize().width();
	int viewportHeight = maximumViewportSize().height();
	int scrollBarExtent = style()->pixelMetric(QStyle::PM_ScrollBarExtent, 0, this);

	// Set scroll bar range
	if (contentWidth > viewportWidth)
	{
		int contentLeft = (targetRect.left() - mSceneRect.left()) * mScale;

		if (verticalScrollBarPolicy() == Qt::ScrollBarAsNeeded) viewportWidth -= scrollBarExtent;

		horizontalScrollBar()->setRange(contentLeft - 1, contentLeft + contentWidth - viewportWidth + 1);
		horizontalScrollBar()->setSingleStep(viewportWidth / 80);
		horizontalScrollBar()->setPageStep(viewportWidth);
	}
	else horizontalScrollBar()->setRange(0, 0);

	if (contentHeight > viewportHeight)
	{
		int contentTop = (targetRect.top() - mSceneRect.top()) * mScale;

		if (horizontalScrollBarPolicy() == Qt::ScrollBarAsNeeded) viewportHeight -= scrollBarExtent;

		verticalScrollBar()->setRange(contentTop - 1, contentTop + contentHeight - viewportHeight + 1);
		verticalScrollBar()->setSingleStep(viewportHeight / 80);
		verticalScrollBar()->setPageStep(viewportHeight);
	}
	else verticalScrollBar()->setRange(0, 0);

	// Recalculate transforms
	qreal dx = -targetRect.left() * mScale;
	qreal dy = -targetRect.top() * mScale;

	if (horizontalScrollBar()->maximum() <= horizontalScrollBar()->minimum())
		dx += -(targetRect.width() * mScale - viewportWidth) / 2;
	if (verticalScrollBar()->maximum() <= verticalScrollBar()->minimum())
		dy += -(targetRect.height() * mScale - viewportHeight) / 2;

	mViewportTransform = QTransform();
	mViewportTransform.translate(qRound(dx), qRound(dy));
	mViewportTransform.scale(mScale, mScale);

	mSceneTransform = mViewportTransform.inverted();
}
