/* DrawingView.cpp
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

#include "DrawingView.h"
#include "DrawingScene.h"
#include "DrawingItem.h"
#include "DrawingItemGroup.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"
#include "DrawingUndo.h"

DrawingView::DrawingView() : QAbstractScrollArea()
{
	setMouseTracking(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	mScene = nullptr;
	setScene(new DrawingScene());

	mFlags = (ViewOwnsScene | UndoableSelectCommands | SendsMouseMoveInfo);
	mItemSelectionMode = Qt::ContainsItemBoundingRect;
	mGrid = 50;

	mUndoStack.setUndoLimit(64);
	connect(&mUndoStack, SIGNAL(cleanChanged(bool)), this, SIGNAL(cleanChanged(bool)));
	connect(&mUndoStack, SIGNAL(canRedoChanged(bool)), this, SIGNAL(canRedoChanged(bool)));
	connect(&mUndoStack, SIGNAL(canUndoChanged(bool)), this, SIGNAL(canUndoChanged(bool)));

	mMode = DefaultMode;
	mScale = 1.0;

	mSelectedItemPoint = nullptr;
	connect(this, SIGNAL(selectionChanged(const QList<DrawingItem*>&)), this, SLOT(updateSelectionCenter()));

	mMouseDownItem = nullptr;
	mFocusItem = nullptr;

	mDragged = false;

	mDefaultMouseState = MouseReady;

	mScrollButtonDownHorizontalScrollValue = 0;
	mScrollButtonDownVerticalScrollValue = 0;

	mPlaceByMousePressAndRelease = false;

	mPanTimer.setInterval(16);
	connect(&mPanTimer, SIGNAL(timeout()), this, SLOT(mousePanEvent()));
}

DrawingView::~DrawingView()
{
	mSelectedItems.clear();
	mSelectedItemPoint = nullptr;

	mMouseDownItem = nullptr;
	mFocusItem = nullptr;
	while (!mClipboardItems.isEmpty()) delete mClipboardItems.takeFirst();

	mDefaultInitialPositions.clear();

	while (!mNewItems.isEmpty()) delete mNewItems.takeFirst();

	if (mFlags & ViewOwnsScene) delete mScene;
}

//==================================================================================================

void DrawingView::setScene(DrawingScene* scene)
{
	if (mScene)
	{
		disconnect(mScene);

		if (mFlags & ViewOwnsScene) delete mScene;
	}

	mScene = scene;

	if (mScene)
	{
		connect(mScene, SIGNAL(numberOfItemsChanged(int)), this, SIGNAL(numberOfItemsChanged(int)));
		connect(mScene, SIGNAL(itemsPositionChanged(const QList<DrawingItem*>&)), this, SIGNAL(itemsPositionChanged(const QList<DrawingItem*>&)));
		connect(mScene, SIGNAL(itemsTransformChanged(const QList<DrawingItem*>&)), this, SIGNAL(itemsTransformChanged(const QList<DrawingItem*>&)));
		connect(mScene, SIGNAL(itemsGeometryChanged(const QList<DrawingItem*>&)), this, SIGNAL(itemsGeometryChanged(const QList<DrawingItem*>&)));
		connect(mScene, SIGNAL(itemsVisibilityChanged(const QList<DrawingItem*>&)), this, SIGNAL(itemsVisibilityChanged(const QList<DrawingItem*>&)));

		connect(mScene, SIGNAL(itemsPositionChanged(const QList<DrawingItem*>&)), this, SLOT(updateSelectionCenter()));
		connect(mScene, SIGNAL(itemsGeometryChanged(const QList<DrawingItem*>&)), this, SLOT(updateSelectionCenter()));

		void numberOfItemsChanged(int itemCount);
	}
}

DrawingScene* DrawingView::scene() const
{
	return mScene;
}

//==================================================================================================

void DrawingView::setFlags(Flags flags)
{
	mFlags = flags;
}

DrawingView::Flags DrawingView::flags() const
{
	return mFlags;
}

//==================================================================================================

void DrawingView::setItemSelectionMode(Qt::ItemSelectionMode mode)
{
	mItemSelectionMode = mode;
}

Qt::ItemSelectionMode DrawingView::itemSelectionMode() const
{
	return mItemSelectionMode;
}

//==================================================================================================

void DrawingView::setGrid(qreal grid)
{
	mGrid = grid;
}

qreal DrawingView::grid() const
{
	return mGrid;
}

qreal DrawingView::roundToGrid(qreal value) const
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

QPointF DrawingView::roundToGrid(const QPointF& scenePos) const
{
	return QPointF(roundToGrid(scenePos.x()), roundToGrid(scenePos.y()));
}

//==================================================================================================

void DrawingView::setUndoLimit(int undoLimit)
{
	mUndoStack.setUndoLimit(undoLimit);
}

void DrawingView::pushUndoCommand(QUndoCommand* command)
{
	mUndoStack.push(command);
}

int DrawingView::undoLimit() const
{
	return mUndoStack.undoLimit();
}

bool DrawingView::isClean() const
{
	return mUndoStack.isClean();
}

bool DrawingView::canUndo() const
{
	return mUndoStack.canUndo();
}

bool DrawingView::canRedo() const
{
	return mUndoStack.canRedo();
}

QString DrawingView::undoText() const
{
	return mUndoStack.undoText();
}

QString DrawingView::redoText() const
{
	return mUndoStack.redoText();
}

//==================================================================================================

DrawingView::Mode DrawingView::mode() const
{
	return mMode;
}

qreal DrawingView::scale() const
{
	return mScale;
}

//==================================================================================================

void DrawingView::selectItem(DrawingItem* item)
{
	if (item && !mSelectedItems.contains(item))
	{
		item->setSelected(true);
		mSelectedItems.append(item);
	}
}

void DrawingView::deselectItem(DrawingItem* item)
{
	if (item && mSelectedItems.contains(item))
	{
		mSelectedItems.removeAll(item);
		item->setSelected(false);
	}
}

void DrawingView::clearSelection()
{
	for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		(*itemIter)->setSelected(false);

	mSelectedItems.clear();
}

QList<DrawingItem*> DrawingView::selectedItems() const
{
	return mSelectedItems;
}

//==================================================================================================

QList<DrawingItem*> DrawingView::newItems() const
{
	return mNewItems;
}

DrawingItem* DrawingView::mouseDownItem() const
{
	return mMouseDownItem;
}

DrawingItem* DrawingView::focusItem() const
{
	return mFocusItem;
}

//==================================================================================================

void DrawingView::centerOn(const QPointF& scenePos)
{
	QPointF oldScenePos = mapToScene(viewport()->rect().center());

	int horizontalDelta = qRound((scenePos.x() - oldScenePos.x()) * mScale);
	int verticalDelta = qRound((scenePos.y() - oldScenePos.y()) * mScale);

	horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalDelta);
	verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);
}

void DrawingView::centerOnCursor(const QPointF& scenePos)
{
	QPointF oldScenePos = mapToScene(mapFromGlobal(QCursor::pos()));

	int horizontalDelta = qRound((scenePos.x() - oldScenePos.x()) * mScale);
	int verticalDelta = qRound((scenePos.y() - oldScenePos.y()) * mScale);

	horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalDelta);
	verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);
}

void DrawingView::fitToView(const QRectF& sceneRect)
{
	qreal scaleX = (maximumViewportSize().width() - 5) / sceneRect.width();
	qreal scaleY = (maximumViewportSize().height() - 5) / sceneRect.height();

	mScale = qMin(scaleX, scaleY);

	recalculateContentSize(sceneRect);

	centerOn(sceneRect.center());
}

void DrawingView::scaleBy(qreal scale)
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

QPointF DrawingView::mapToScene(const QPoint& screenPos) const
{
	QPointF p = screenPos;
	p.setX(p.x() + horizontalScrollBar()->value());
	p.setY(p.y() + verticalScrollBar()->value());
	return mSceneTransform.map(p);
}

QRectF DrawingView::mapToScene(const QRect& screenRect) const
{
	return QRectF(mapToScene(screenRect.topLeft()), mapToScene(screenRect.bottomRight()));
}

QPoint DrawingView::mapFromScene(const QPointF& scenePos) const
{
	QPointF p = mViewportTransform.map(scenePos);
	p.setX(p.x() - horizontalScrollBar()->value());
	p.setY(p.y() - verticalScrollBar()->value());
	return p.toPoint();
}

QRect DrawingView::mapFromScene(const QRectF& sceneRect) const
{
	return QRect(mapFromScene(sceneRect.topLeft()), mapFromScene(sceneRect.bottomRight()));
}

QRectF DrawingView::visibleRect() const
{
	return QRectF(mapToScene(QPoint(0, 0)), mapToScene(QPoint(viewport()->width(), viewport()->height())));
}

QRectF DrawingView::scrollBarDefinedRect() const
{
	QRectF scrollBarRect;

	if (mScene)
	{
		scrollBarRect = mScene->sceneRect();

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
	}

	return scrollBarRect;
}

//==================================================================================================

QList<DrawingItem*> DrawingView::visibleItems(const QPointF& scenePos) const
{
	return (mScene) ? mScene->visibleItems(this, scenePos) : QList<DrawingItem*>();
}

QList<DrawingItem*> DrawingView::visibleItems(const QRectF& sceneRect) const
{
	return (mScene) ? mScene->visibleItems(this, sceneRect, mItemSelectionMode) : QList<DrawingItem*>();
}

QList<DrawingItem*> DrawingView::visibleItems(const QPainterPath& scenePath) const
{
	return (mScene) ? mScene->visibleItems(this, scenePath, mItemSelectionMode) : QList<DrawingItem*>();
}

DrawingItem* DrawingView::visibleItemAt(const QPointF& scenePos) const
{
	DrawingItem* item = nullptr;

	if (mScene)
	{
		// Favor selected items
		auto itemIter = mSelectedItems.end();
		while (item == nullptr && itemIter != mSelectedItems.begin())
		{
			itemIter--;
			if (mScene->itemMatchesPoint(this, *itemIter, scenePos)) item = *itemIter;
		}

		// Search all items
		if (item == nullptr) item = mScene->visibleItemAt(this, scenePos);
	}

	return item;
}

//==================================================================================================

void DrawingView::render(QPainter* painter)
{
	if (mScene) mScene->render(painter);
}

//==================================================================================================

void DrawingView::zoomIn()
{
	if (mScene)
	{
		scaleBy(qSqrt(2));

		emit scaleChanged(mScale);
		viewport()->update();
	}
}

void DrawingView::zoomOut()
{
	if (mScene)
	{
		scaleBy(qSqrt(2) / 2);

		emit scaleChanged(mScale);
		viewport()->update();
	}
}

void DrawingView::zoomFit()
{
	if (mScene)
	{
		fitToView(mScene->sceneRect());

		emit scaleChanged(mScale);
		viewport()->update();
	}
}

//==================================================================================================

void DrawingView::setDefaultMode()
{
	mMode = DefaultMode;
	setCursor(Qt::ArrowCursor);

	while (!mNewItems.isEmpty()) delete mNewItems.takeFirst();
	emit newItemsChanged(mNewItems);

	clearSelection();
	emit selectionChanged(mSelectedItems);

	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingView::setScrollMode()
{
	mMode = ScrollMode;
	setCursor(Qt::OpenHandCursor);

	while (!mNewItems.isEmpty()) delete mNewItems.takeFirst();
	emit newItemsChanged(mNewItems);

	clearSelection();
	emit selectionChanged(mSelectedItems);

	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingView::setZoomMode()
{
	mMode = ZoomMode;
	setCursor(Qt::CrossCursor);

	while (!mNewItems.isEmpty()) delete mNewItems.takeFirst();
	emit newItemsChanged(mNewItems);

	clearSelection();
	emit selectionChanged(mSelectedItems);

	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingView::setPlaceMode(const QList<DrawingItem*>& items)
{
	if (!items.isEmpty())
	{
		QPointF centerPos, deltaPos;

		mMode = PlaceMode;
		setCursor(Qt::CrossCursor);

		clearSelection();
		emit selectionChanged(mSelectedItems);

		while (!mNewItems.isEmpty()) delete mNewItems.takeFirst();
		mNewItems = items;

		mPlaceByMousePressAndRelease = (mNewItems.size() == 1 && !mNewItems.first()->isValid());

		for(auto itemIter = mNewItems.begin(); itemIter != mNewItems.end(); itemIter++)
			centerPos += (*itemIter)->mapToScene((*itemIter)->centerPos());
		centerPos /= mNewItems.size();

		deltaPos = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())) - centerPos);

		for(auto itemIter = mNewItems.begin(); itemIter != mNewItems.end(); itemIter++)
			(*itemIter)->setPosition((*itemIter)->position() + deltaPos);

		emit newItemsChanged(mNewItems);

		emit modeChanged(mMode);
		viewport()->update();
	}
	else setDefaultMode();
}

//==================================================================================================

void DrawingView::undo()
{
	if (mMode == DefaultMode && mUndoStack.canUndo())
	{
		if ((mFlags & UndoableSelectCommands) == 0)
		{
			clearSelection();
			emit selectionChanged(mSelectedItems);
		}

		mUndoStack.undo();
		viewport()->update();
	}
}

void DrawingView::redo()
{
	if (mMode == DefaultMode && mUndoStack.canRedo())
	{
		if ((mFlags & UndoableSelectCommands) == 0)
		{
			clearSelection();
			emit selectionChanged(mSelectedItems);
		}

		mUndoStack.redo();
		viewport()->update();
	}
}

void DrawingView::setClean()
{
	mUndoStack.setClean();
	viewport()->update();
}

//==================================================================================================

void DrawingView::cut()
{
	copy();
	deleteSelection();
}

void DrawingView::copy()
{
	if (mMode == DefaultMode && mScene)
	{
		while (!mClipboardItems.isEmpty()) delete mClipboardItems.takeFirst();
		mClipboardItems = DrawingItem::copyItems(mSelectedItems);
	}
}

void DrawingView::paste()
{
	if (mMode == DefaultMode && mScene)
	{
		QList<DrawingItem*> newItems = DrawingItem::copyItems(mClipboardItems);

		if (!newItems.isEmpty())
		{
			selectNone();
			setPlaceMode(newItems);
		}
	}
}

void DrawingView::deleteSelection()
{
	if (mMode == DefaultMode && mScene)
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

void DrawingView::selectItems(const QList<DrawingItem*>& items)
{
	for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		(*itemIter)->setSelected(false);

	mSelectedItems = items;

	for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		(*itemIter)->setSelected(true);

	emit selectionChanged(mSelectedItems);
}

void DrawingView::selectAll()
{
	if (mMode == DefaultMode && mScene)
	{
		QList<DrawingItem*> foundItems = mScene->visibleItems();
		QList<DrawingItem*> itemsToSelect;

		for(auto itemIter = foundItems.begin(); itemIter != foundItems.end(); itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanSelect) itemsToSelect.append(*itemIter);
		}
		if (mSelectedItems != itemsToSelect)
		{
			selectItemsCommand(itemsToSelect, true);
			viewport()->update();
		}
	}
}

void DrawingView::selectArea(const QRectF& rect)
{
	if (mMode == DefaultMode)
	{
		QList<DrawingItem*> foundItems = visibleItems(rect);
		QList<DrawingItem*> itemsToSelect;

		for(auto itemIter = foundItems.begin(); itemIter != foundItems.end(); itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanSelect) itemsToSelect.append(*itemIter);
		}

		if (mSelectedItems != itemsToSelect)
		{
			selectItemsCommand(itemsToSelect, true);
			viewport()->update();
		}
	}
}

void DrawingView::selectArea(const QPainterPath& path)
{
	if (mMode == DefaultMode)
	{
		QList<DrawingItem*> foundItems = visibleItems(path);
		QList<DrawingItem*> itemsToSelect;

		for(auto itemIter = foundItems.begin(); itemIter != foundItems.end(); itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanSelect) itemsToSelect.append(*itemIter);
		}

		if (mSelectedItems != itemsToSelect)
		{
			selectItemsCommand(itemsToSelect, true);
			viewport()->update();
		}
	}
}

void DrawingView::selectNone()
{
	if (mMode == DefaultMode && mScene && !mSelectedItems.isEmpty())
	{
		selectItemsCommand(QList<DrawingItem*>(), true);
		viewport()->update();
	}
}

//==================================================================================================

void DrawingView::moveSelection(const QPointF& deltaScenePos)
{
	if (mMode == DefaultMode && mScene && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToMove;
		QHash<DrawingItem*,QPointF> originalPositions, newPositions;

		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
			originalPositions[*itemIter] = (*itemIter)->position();

		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanMove)
			{
				itemsToMove.append(*itemIter);
				newPositions[*itemIter] = (*itemIter)->position() + deltaScenePos;
				(*itemIter)->setPosition(newPositions[*itemIter]);
			}
		}

		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
			(*itemIter)->setPosition(originalPositions[*itemIter]);

		if (!itemsToMove.isEmpty())
		{
			moveItemsCommand(itemsToMove, newPositions, true);
			viewport()->update();
		}
	}
}

void DrawingView::resizeSelection(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	if (mMode == DefaultMode && mScene && mSelectedItems.size() == 1 &&
		(mSelectedItems.first()->flags() & DrawingItem::CanResize) &&
		mSelectedItems.first()->points().contains(itemPoint))
	{
		resizeItemCommand(itemPoint, scenePos, true, true);
		viewport()->update();
	}
}

void DrawingView::rotateSelection()
{
	if (mMode == DefaultMode && mScene && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToRotate;
		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanRotate) itemsToRotate.append(*itemIter);
		}

		if (!itemsToRotate.isEmpty())
		{
			rotateItemsCommand(itemsToRotate, roundToGrid(mSelectionCenter));
			viewport()->update();
		}
	}
	else if (mMode == PlaceMode && mScene && !mNewItems.isEmpty())
	{
		QList<DrawingItem*> itemsToRotate;
		for(auto itemIter = mNewItems.begin(); itemIter != mNewItems.end(); itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanRotate) itemsToRotate.append(*itemIter);
		}

		if (!itemsToRotate.isEmpty())
		{
			QPointF scenePos = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())));
			QHash<DrawingItem*,QPointF> parentPos;

			for(auto itemIter = itemsToRotate.begin(); itemIter != itemsToRotate.end(); itemIter++)
				parentPos[*itemIter] = scenePos;

			mScene->rotateItems(itemsToRotate, parentPos);
			viewport()->update();
		}
	}
}

void DrawingView::rotateBackSelection()
{
	if (mMode == DefaultMode && mScene && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToRotate;
		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanRotate) itemsToRotate.append(*itemIter);
		}

		if (!itemsToRotate.isEmpty())
		{
			rotateBackItemsCommand(itemsToRotate, roundToGrid(mSelectionCenter));
			viewport()->update();
		}
	}
	else if (mMode == PlaceMode && mScene && !mNewItems.isEmpty())
	{
		QList<DrawingItem*> itemsToRotate;
		for(auto itemIter = mNewItems.begin(); itemIter != mNewItems.end(); itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanRotate) itemsToRotate.append(*itemIter);
		}

		if (!itemsToRotate.isEmpty())
		{
			QPointF scenePos = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())));
			QHash<DrawingItem*,QPointF> parentPos;

			for(auto itemIter = itemsToRotate.begin(); itemIter != itemsToRotate.end(); itemIter++)
				parentPos[*itemIter] = scenePos;

			mScene->rotateBackItems(itemsToRotate, parentPos);
			viewport()->update();
		}
	}
}

void DrawingView::flipSelectionHorizontal()
{
	if (mMode == DefaultMode && mScene && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToFlip;
		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanFlip) itemsToFlip.append(*itemIter);
		}

		if (!itemsToFlip.isEmpty())
		{
			flipItemsHorizontalCommand(itemsToFlip, roundToGrid(mSelectionCenter));
			viewport()->update();
		}
	}
	else if (mMode == PlaceMode && mScene && !mNewItems.isEmpty())
	{
		QList<DrawingItem*> itemsToFlip;
		for(auto itemIter = mNewItems.begin(); itemIter != mNewItems.end(); itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanFlip) itemsToFlip.append(*itemIter);
		}

		if (!itemsToFlip.isEmpty())
		{
			QPointF scenePos = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())));
			QHash<DrawingItem*,QPointF> parentPos;

			for(auto itemIter = itemsToFlip.begin(); itemIter != itemsToFlip.end(); itemIter++)
				parentPos[*itemIter] = scenePos;

			mScene->flipItemsHorizontal(itemsToFlip, parentPos);
			viewport()->update();
		}
	}
}

void DrawingView::flipSelectionVertical()
{
	if (mMode == DefaultMode && mScene && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToFlip;
		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanFlip) itemsToFlip.append(*itemIter);
		}

		if (!itemsToFlip.isEmpty())
		{
			flipItemsVerticalCommand(itemsToFlip, roundToGrid(mSelectionCenter));
			viewport()->update();
		}
	}
	else if (mMode == PlaceMode && mScene && !mNewItems.isEmpty())
	{
		QList<DrawingItem*> itemsToFlip;
		for(auto itemIter = mNewItems.begin(); itemIter != mNewItems.end(); itemIter++)
		{
			if ((*itemIter)->flags() & DrawingItem::CanFlip) itemsToFlip.append(*itemIter);
		}

		if (!itemsToFlip.isEmpty())
		{
			QPointF scenePos = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())));
			QHash<DrawingItem*,QPointF> parentPos;

			for(auto itemIter = itemsToFlip.begin(); itemIter != itemsToFlip.end(); itemIter++)
				parentPos[*itemIter] = scenePos;

			mScene->flipItemsVertical(itemsToFlip, parentPos);
			viewport()->update();
		}
	}
}

//==================================================================================================

void DrawingView::bringForward()
{
	if (mMode == DefaultMode && mScene && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToReorder = mSelectedItems;

		QList<DrawingItem*> itemsOrdered = mScene->mItems;
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

void DrawingView::sendBackward()
{
	if (mMode == DefaultMode && mScene && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToReorder = mSelectedItems;

		QList<DrawingItem*> itemsOrdered = mScene->mItems;
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

void DrawingView::bringToFront()
{
	if (mMode == DefaultMode && mScene && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToReorder = mSelectedItems;

		QList<DrawingItem*> itemsOrdered = mScene->mItems;
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

void DrawingView::sendToBack()
{
	if (mMode == DefaultMode && mScene && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToReorder = mSelectedItems;

		QList<DrawingItem*> itemsOrdered = mScene->mItems;
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

void DrawingView::insertItemPoint()
{
	if (mMode == DefaultMode && mScene)
	{
		DrawingItem* item = nullptr;
		if (mSelectedItems.size() == 1) item = mSelectedItems.first();

		if (item && (item->flags() & DrawingItem::CanInsertPoints))
		{
			int index = 0;
			DrawingItemPoint* pointToInsert = item->itemPointToInsert(item->mapFromScene(roundToGrid(mButtonDownScenePos)), index);
			if (pointToInsert)
			{
				mUndoStack.push(new DrawingItemInsertPointCommand(mScene, item, pointToInsert, index));
				viewport()->update();
			}
		}
	}
}

void DrawingView::removeItemPoint()
{
	if (mMode == DefaultMode && mScene)
	{
		DrawingItem* item = nullptr;
		if (mSelectedItems.size() == 1) item = mSelectedItems.first();

		if (item && (item->flags() & DrawingItem::CanRemovePoints))
		{
			DrawingItemPoint* pointToRemove = item->itemPointToRemove(item->mapFromScene(roundToGrid(mButtonDownScenePos)));
			if (pointToRemove)
			{
				mUndoStack.push(new DrawingItemRemovePointCommand(mScene, item, pointToRemove));
				viewport()->update();
			}
		}
	}
}

//==================================================================================================

void DrawingView::group()
{
	if (mMode == DefaultMode && mScene && mSelectedItems.size() > 1)
	{
		QUndoCommand* command = new QUndoCommand("Group Items");

		QList<DrawingItem*> items = DrawingItem::copyItems(mSelectedItems);
		DrawingItemGroup* itemGroup = new DrawingItemGroup();
		QList<DrawingItem*> itemsToAdd;

		itemGroup->setPosition(items.first()->position());
		for(auto iter = items.begin(); iter != items.end(); iter++)
			(*iter)->setPosition(itemGroup->mapFromScene((*iter)->position()));
		itemGroup->setItems(items);
		itemsToAdd.append(itemGroup);

		selectItemsCommand(QList<DrawingItem*>(), true, command);
		removeItemsCommand(mSelectedItems, command);
		addItemsCommand(itemsToAdd, false, command);
		selectItemsCommand(itemsToAdd, true, command);

		mUndoStack.push(command);
		viewport()->update();
	}
}

void DrawingView::ungroup()
{
	if (mMode == DefaultMode && mScene && mSelectedItems.size() == 1)
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

void DrawingView::paintEvent(QPaintEvent* event)
{
	QImage image(viewport()->width(), viewport()->height(), QImage::Format_RGB32);
	image.fill(palette().brush(QPalette::Window).color());

	// Render scene
	QPainter painter(&image);

	painter.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());
	painter.setTransform(mViewportTransform, true);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	render(&painter);

	painter.end();

	// Render scene image on to widget
	QPainter widgetPainter(viewport());
	widgetPainter.drawImage(0, 0, image);

	Q_UNUSED(event);
}

void DrawingView::resizeEvent(QResizeEvent* event)
{
	QAbstractScrollArea::resizeEvent(event);
	recalculateContentSize();
}

//==================================================================================================

void DrawingView::mousePressEvent(QMouseEvent* event)
{
	if (mScene)
	{
		if (event->button() == Qt::LeftButton)
		{
			mButtonDownPos = event->pos();
			mButtonDownScenePos = mapToScene(mButtonDownPos);
			mDragged = false;

			if (mMode == ScrollMode)
			{
				setCursor(Qt::ClosedHandCursor);
				mScrollButtonDownHorizontalScrollValue = horizontalScrollBar()->value();
				mScrollButtonDownVerticalScrollValue = verticalScrollBar()->value();
			}
			else if (mMode == ZoomMode) { }
			else if (mMode == PlaceMode) { }
			else
			{
				mDefaultMouseState = MouseSelect;

				mMouseDownItem = visibleItemAt(mButtonDownScenePos);
				if (mMouseDownItem)
				{
					mDefaultInitialPositions.clear();
					for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
						mDefaultInitialPositions[*itemIter] = (*itemIter)->position();

					if (mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
					{
						mSelectedItemPoint = pointAt(mMouseDownItem, mMouseDownItem->mapFromScene(mButtonDownScenePos));
						if (mSelectedItemPoint)
						{
							if (mSelectedItemPoint->flags() & DrawingItemPoint::Control)
								mDefaultSelectedItemPointOriginalPos = mMouseDownItem->mapToScene(mSelectedItemPoint->position());
							else
								mSelectedItemPoint = nullptr;
						}
					}
				}

				mFocusItem = mMouseDownItem;
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
			mButtonDownScenePos = mapToScene(event->pos());
		}

		viewport()->update();
		emit mouseInfoChanged("");
	}
}

void DrawingView::mouseMoveEvent(QMouseEvent* event)
{
	if (mScene)
	{
		mScenePos = mapToScene(event->pos());
		mDragged = (mDragged |
			((mButtonDownPos - event->pos()).manhattanLength() >= QApplication::startDragDistance()));

		if (mMode == ScrollMode)
		{
			if ((event->buttons() & Qt::LeftButton) && mDragged)
			{
				horizontalScrollBar()->setValue(
					mScrollButtonDownHorizontalScrollValue - (event->pos().x() - mButtonDownPos.x()));
				verticalScrollBar()->setValue(
					mScrollButtonDownVerticalScrollValue - (event->pos().y() - mButtonDownPos.y()));
				sendMouseInfoText(mButtonDownScenePos);
			}
			else sendMouseInfoText(mScenePos);
		}
		else if (mMode == ZoomMode)
		{
			if ((event->buttons() & Qt::LeftButton) && mDragged)
			{
				QPoint p1 = event->pos();
				QPoint p2 = mButtonDownPos;
				mRubberBandRect = QRect(qMin(p1.x(), p2.x()), qMin(p1.y(), p2.y()), qAbs(p2.x() - p1.x()), qAbs(p2.y() - p1.y()));

				sendMouseInfoText(mButtonDownScenePos, mScenePos);
			}
			else sendMouseInfoText(mScenePos);
		}
		else if (mMode == PlaceMode)
		{
			if (event->buttons() & Qt::LeftButton && mPlaceByMousePressAndRelease &&
				mNewItems.size() == 1 && mNewItems.first()->points().size() >= 2)
			{
				mScene->resizeItem(mNewItems.first()->points()[1], roundToGrid(mScenePos));
			}
			else
			{
				QPointF centerPos, deltaPos;

				for(auto itemIter = mNewItems.begin(); itemIter != mNewItems.end(); itemIter++)
					centerPos += (*itemIter)->mapToScene((*itemIter)->centerPos());
				if (!mNewItems.isEmpty()) centerPos /= mNewItems.size();

				deltaPos = roundToGrid(mScenePos - centerPos);

				for(auto itemIter = mNewItems.begin(); itemIter != mNewItems.end(); itemIter++)
					(*itemIter)->setPosition((*itemIter)->position() + deltaPos);

				emit itemsGeometryChanged(mNewItems);
			}

			if (event->buttons() & Qt::LeftButton)
				sendMouseInfoText(roundToGrid(mButtonDownScenePos), roundToGrid(mScenePos));
			else
				sendMouseInfoText(roundToGrid(mScenePos));
		}
		else
		{
			if (event->buttons() & Qt::LeftButton)
			{
				QPointF deltaScenePos;
				QList<DrawingItem*> itemsToMove;
				QHash<DrawingItem*,QPointF> originalPositions, newPositions;

				switch (mDefaultMouseState)
				{
				case MouseSelect:
					if (mDragged)
					{
						if (mMouseDownItem && mMouseDownItem->isSelected())
						{
							bool resizeItem = (mSelectedItems.size() == 1 &&
								(mSelectedItems.first()->flags() & DrawingItem::CanResize) &&
								mSelectedItemPoint && (mSelectedItemPoint->flags() & DrawingItemPoint::Control));
							mDefaultMouseState = (resizeItem) ? MouseResizeItem : MouseMoveItems;
						}
						else mDefaultMouseState = MouseRubberBand;
					}
					sendMouseInfoText(mScenePos);
					break;

				case MouseMoveItems:
					for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
						originalPositions[*itemIter] = (*itemIter)->position();

					deltaScenePos = roundToGrid(mScenePos - mButtonDownScenePos);
					for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
					{
						if ((*itemIter)->flags() & DrawingItem::CanMove)
						{
							itemsToMove.append(*itemIter);
							newPositions[*itemIter] = mDefaultInitialPositions[*itemIter] + deltaScenePos;
							(*itemIter)->setPosition(newPositions[*itemIter]);
						}
					}

					for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
						(*itemIter)->setPosition(originalPositions[*itemIter]);

					if (!itemsToMove.isEmpty())
					{
						moveItemsCommand(itemsToMove, newPositions, false);
						viewport()->update();
					}

					sendMouseInfoText(mDefaultInitialPositions[mMouseDownItem],
						mDefaultInitialPositions[mMouseDownItem] + roundToGrid(mScenePos - mButtonDownScenePos));
					break;

				case MouseResizeItem:
					resizeItemCommand(mSelectedItemPoint, roundToGrid(mScenePos), false, true);
					sendMouseInfoText(mDefaultSelectedItemPointOriginalPos, roundToGrid(mScenePos));
					break;

				case MouseRubberBand:
					mRubberBandRect = QRect(event->pos(), mButtonDownPos).normalized();
					sendMouseInfoText(mButtonDownScenePos, mScenePos);
					break;

				default:
					sendMouseInfoText(mButtonDownScenePos, mScenePos);
					break;
				}
			}
			else sendMouseInfoText(mScenePos);
		}
	}

	if (mPanTimer.isActive()) mPanCurrentPos = event->pos();

	if (event->buttons() != Qt::NoButton || mMode == PlaceMode) viewport()->update();
}

void DrawingView::mouseReleaseEvent(QMouseEvent* event)
{
	if (mScene)
	{
		if (event->button() == Qt::LeftButton)
		{
			mScenePos = mapToScene(event->pos());

			if (mMode == ScrollMode)
			{
				setCursor(Qt::OpenHandCursor);
			}
			else if (mMode == ZoomMode)
			{
				if (mRubberBandRect.isValid())
				{
					fitToView(mapToScene(mRubberBandRect));
					mRubberBandRect = QRect();

					emit scaleChanged(mScale);
					setDefaultMode();
				}
			}
			else if (mMode == PlaceMode)
			{
				if (mNewItems.size() > 1 ||
					(mNewItems.size() == 1 && mNewItems.first()->isValid()))
				{
					QList<DrawingItem*> newItems;
					DrawingItem* newItem;
					QList<DrawingItemPoint*> points;

					addItemsCommand(mNewItems, true);

					for(auto itemIter = mNewItems.begin(); itemIter != mNewItems.end(); itemIter++)
					{
						newItem = (*itemIter)->copy();
						if (mPlaceByMousePressAndRelease)
						{
							points = newItem->points();
							for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
								(*pointIter)->setPosition(0, 0);
						}

						newItems.append(newItem);
					}
					mNewItems.clear();

					setPlaceMode(newItems);
				}
			}
			else
			{
				bool controlDown = ((event->modifiers() & Qt::ControlModifier) != 0);
				QList<DrawingItem*> newSelection = (controlDown) ? mSelectedItems : QList<DrawingItem*>();
				QPointF deltaScenePos;
				QList<DrawingItem*> itemsToMove;
				QList<DrawingItem*> foundItems;
				QHash<DrawingItem*,QPointF> originalPositions, newPositions;

				switch (mDefaultMouseState)
				{
				case MouseSelect:
					if (mMouseDownItem)
					{
						if (controlDown && mMouseDownItem->isSelected())
						{
							newSelection.removeAll(mMouseDownItem);
						}
						else if (mMouseDownItem->flags() & DrawingItem::CanSelect)
						{
							newSelection.append(mMouseDownItem);
						}
					}
					if (mSelectedItems != newSelection) selectItemsCommand(newSelection, true);
					break;

				case MouseMoveItems:
					for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
						originalPositions[*itemIter] = (*itemIter)->position();

					deltaScenePos = roundToGrid(mScenePos - mButtonDownScenePos);
					for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
					{
						if ((*itemIter)->flags() & DrawingItem::CanMove)
						{
							itemsToMove.append(*itemIter);
							newPositions[*itemIter] = mDefaultInitialPositions[*itemIter] + deltaScenePos;
							(*itemIter)->setPosition(newPositions[*itemIter]);
						}
					}

					for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
						(*itemIter)->setPosition(originalPositions[*itemIter]);

					if (!itemsToMove.isEmpty())
					{
						moveItemsCommand(itemsToMove, newPositions, true);
						viewport()->update();
					}
					break;

				case MouseResizeItem:
					resizeItemCommand(mSelectedItemPoint, roundToGrid(mScenePos), true, true);
					break;

				case MouseRubberBand:
					//selectArea(mapToScene(mRubberBandRect));
					foundItems = visibleItems(mapToScene(mRubberBandRect));

					for(auto itemIter = foundItems.begin(); itemIter != foundItems.end(); itemIter++)
					{
						if (!newSelection.contains((*itemIter)) &&
							(*itemIter)->flags() & DrawingItem::CanSelect) newSelection.append(*itemIter);
					}

					if (mSelectedItems != newSelection) selectItemsCommand(newSelection, true);

					mRubberBandRect = QRect();
					break;

				default:
					break;
				}

				mSelectedItemPoint = nullptr;
				mDefaultSelectedItemPointOriginalPos = QPointF();
				mDefaultMouseState = MouseReady;

				updateSelectionCenter();
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
		emit mouseInfoChanged("");
	}
}

void DrawingView::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (mScene)
	{
		if (event->button() == Qt::LeftButton)
		{
			mButtonDownPos = event->pos();
			mButtonDownScenePos = mapToScene(mButtonDownPos);
			mDragged = false;

			if (mMode != DefaultMode) setDefaultMode();
			else
			{
				mDefaultMouseState = MouseSelect;

				mMouseDownItem = visibleItemAt(mButtonDownScenePos);
				if (mMouseDownItem)
				{
					mDefaultInitialPositions.clear();
					for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
						mDefaultInitialPositions[*itemIter] = (*itemIter)->position();

					if (mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
					{
						mSelectedItemPoint = pointAt(mMouseDownItem, mMouseDownItem->mapFromScene(mButtonDownScenePos));
						if (mSelectedItemPoint)
						{
							if (mSelectedItemPoint->flags() & DrawingItemPoint::Control)
								mDefaultSelectedItemPointOriginalPos = mMouseDownItem->mapToScene(mSelectedItemPoint->position());
							else
								mSelectedItemPoint = nullptr;
						}
					}
				}

				mFocusItem = mMouseDownItem;
			}
		}

		viewport()->update();
		emit mouseInfoChanged("");
	}
}

void DrawingView::wheelEvent(QWheelEvent* event)
{
	if (event->modifiers() & Qt::ControlModifier)
	{
		if (event->delta() > 0) zoomIn();
		else if (event->delta() < 0) zoomOut();
	}
	else QAbstractScrollArea::wheelEvent(event);
}

//==================================================================================================

void DrawingView::keyPressEvent(QKeyEvent* event)
{
	if (mFocusItem) mFocusItem->keyPressEvent(event);
}

void DrawingView::keyReleaseEvent(QKeyEvent* event)
{
	if (mFocusItem) mFocusItem->keyReleaseEvent(event);
}

//==================================================================================================

void DrawingView::drawBackground(QPainter* painter)
{
	if (mScene) mScene->drawBackground(painter);
}

void DrawingView::drawItems(QPainter* painter)
{
	if (mScene) mScene->drawItems(painter);
}

void DrawingView::drawForeground(QPainter* painter)
{
	if (mScene)
	{
		mScene->drawForeground(painter);

		// Draw new items
		mScene->drawItems(painter, mNewItems);

		// Draw item points
		QColor color = mScene->backgroundBrush().color();
		color.setRed(255 - color.red());
		color.setGreen(255 - color.green());
		color.setBlue(255 - color.blue());

		painter->save();

		painter->resetTransform();
		painter->setRenderHints(QPainter::Antialiasing, false);
		painter->setPen(QPen(color, 1));

		for(auto itemIter = mSelectedItems.begin(), itemEnd = mSelectedItems.end(); itemIter != itemEnd; itemIter++)
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
						QRect pointRect = DrawingView::pointRect(*pointIter).adjusted(1, 1, -2, -2);

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

		// Draw hotpoints
		QList<DrawingItem*> items = mNewItems + mSelectedItems;

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
				for(auto otherItemIter = mScene->mItems.begin(); otherItemIter != mScene->mItems.end(); otherItemIter++)
				{
					if ((*itemIter) != (*otherItemIter))
					{
						QList<DrawingItemPoint*> otherItemPoints = (*otherItemIter)->points();

						for(auto otherItemPointIter = otherItemPoints.begin();
							otherItemPointIter != otherItemPoints.end(); otherItemPointIter++)
						{
							if (shouldConnect(*pointIter, *otherItemPointIter))
							{
								QRect pointRect = DrawingView::pointRect(*pointIter);
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
}

//==================================================================================================

void DrawingView::updateSelectionCenter()
{
	mSelectionCenter = QPointF();

	if (!mSelectedItems.isEmpty())
	{
		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
			mSelectionCenter += (*itemIter)->mapToScene((*itemIter)->centerPos());

		mSelectionCenter /= mSelectedItems.size();
	}
}

void DrawingView::mousePanEvent()
{
	if (mScene)
	{
		QRectF visibleRect = DrawingView::visibleRect();
		QRectF sceneRect = mScene->sceneRect();

		if (mPanCurrentPos.x() - mPanStartPos.x() < 0)
		{
			int delta = (mPanCurrentPos.x() - mPanStartPos.x()) / 16;

			if (horizontalScrollBar()->value() + delta < horizontalScrollBar()->minimum())
			{
				if (horizontalScrollBar()->minimum() >= horizontalScrollBar()->maximum())
					horizontalScrollBar()->setMinimum((visibleRect.left() - sceneRect.left()) * mScale + delta);
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
					horizontalScrollBar()->setMaximum((sceneRect.right() - visibleRect.right()) * mScale + delta);
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
					verticalScrollBar()->setMinimum((visibleRect.top() - sceneRect.top()) * mScale + delta);
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
					verticalScrollBar()->setMaximum((sceneRect.bottom() - visibleRect.bottom()) * mScale + delta);
				else
					verticalScrollBar()->setMaximum(verticalScrollBar()->value() + delta);

				verticalScrollBar()->setValue(verticalScrollBar()->maximum());
			}
			else verticalScrollBar()->setValue(verticalScrollBar()->value() + delta);
		}
	}
}

//==================================================================================================

void DrawingView::addItemsCommand(const QList<DrawingItem*>& items, bool place, QUndoCommand* command)
{
	DrawingAddItemsCommand* addCommand = new DrawingAddItemsCommand(mScene, items, command);

	addCommand->redo();
	if (place) placeItems(items, addCommand);
	addCommand->undo();

	if (!command) mUndoStack.push(addCommand);
}

void DrawingView::removeItemsCommand(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	DrawingRemoveItemsCommand* removeCommand = new DrawingRemoveItemsCommand(mScene, items, command);

	removeCommand->redo();
	unplaceItems(items, removeCommand);
	removeCommand->undo();

	if (!command) mUndoStack.push(removeCommand);
}

void DrawingView::moveItemsCommand(const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& newPos,
	bool place, QUndoCommand* command)
{
	DrawingMoveItemsCommand* moveCommand =
		new DrawingMoveItemsCommand(mScene, items, newPos, place, command);

	moveCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, moveCommand);
	if (place) placeItems(items, moveCommand);
	moveCommand->undo();

	if (!command) mUndoStack.push(moveCommand);
}

void DrawingView::resizeItemCommand(DrawingItemPoint* itemPoint, const QPointF& scenePos,
	bool place, bool disconnect, QUndoCommand* command)
{
	if (itemPoint && itemPoint->item())
	{
		DrawingResizeItemCommand* resizeCommand =
			new DrawingResizeItemCommand(mScene, itemPoint, scenePos, place, command);
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

void DrawingView::rotateItemsCommand(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command)
{
	DrawingRotateItemsCommand* rotateCommand =
		new DrawingRotateItemsCommand(mScene, items, scenePos, command);

	rotateCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, rotateCommand);
	rotateCommand->undo();

	if (!command) mUndoStack.push(rotateCommand);
}

void DrawingView::rotateBackItemsCommand(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command)
{
	DrawingRotateBackItemsCommand* rotateCommand =
		new DrawingRotateBackItemsCommand(mScene, items, scenePos, command);

	rotateCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, rotateCommand);
	rotateCommand->undo();

	if (!command) mUndoStack.push(rotateCommand);
}

void DrawingView::flipItemsHorizontalCommand(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command)
{
	DrawingFlipItemsHorizontalCommand* flipCommand =
		new DrawingFlipItemsHorizontalCommand(mScene, items, scenePos, command);

	flipCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, flipCommand);
	flipCommand->undo();

	if (!command) mUndoStack.push(flipCommand);
}

void DrawingView::flipItemsVerticalCommand(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command)
{
	DrawingFlipItemsVerticalCommand* flipCommand =
		new DrawingFlipItemsVerticalCommand(mScene, items, scenePos, command);

	flipCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, flipCommand);
	flipCommand->undo();

	if (!command) mUndoStack.push(flipCommand);
}

void DrawingView::reorderItemsCommand(const QList<DrawingItem*>& itemsOrdered, QUndoCommand* command)
{
	DrawingReorderItemsCommand* selectCommand =
		new DrawingReorderItemsCommand(mScene, itemsOrdered, command);

	if (!command) mUndoStack.push(selectCommand);
}

void DrawingView::selectItemsCommand(const QList<DrawingItem*>& items, bool finalSelect,
	QUndoCommand* command)
{
	if (mFlags & UndoableSelectCommands)
	{
		DrawingSelectItemsCommand* selectCommand =
			new DrawingSelectItemsCommand(this, items, finalSelect, command);

		if (!command) mUndoStack.push(selectCommand);
	}
	else selectItems(items);
}

void DrawingView::connectItemPointsCommand(DrawingItemPoint* point1, DrawingItemPoint* point2,
	QUndoCommand* command)
{
	DrawingItemPointConnectCommand* connectCommand =
		new DrawingItemPointConnectCommand(mScene, point1, point2, command);

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

void DrawingView::disconnectItemPointsCommand(DrawingItemPoint* point1, DrawingItemPoint* point2,
	QUndoCommand* command)
{
	DrawingItemPointDisconnectCommand* disconnectCommand =
		new DrawingItemPointDisconnectCommand(mScene, point1, point2, command);

	if (!command) mUndoStack.push(disconnectCommand);
}

void DrawingView::hideItemsCommand(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	DrawingItemSetVisibilityCommand* visibilityCommand =
		new DrawingItemSetVisibilityCommand(mScene, items, false, command);

	if (!command) mUndoStack.push(visibilityCommand);
}

//==================================================================================================

void DrawingView::placeItems(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	QList<DrawingItemPoint*> itemPoints, otherItemPoints;

	if (mScene)
	{
		for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		{
			for(auto otherItemIter = mScene->mItems.begin(); otherItemIter != mScene->mItems.end(); otherItemIter++)
			{
				if (!items.contains(*otherItemIter) && !mNewItems.contains(*otherItemIter))
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
}

void DrawingView::unplaceItems(const QList<DrawingItem*>& items, QUndoCommand* command)
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

void DrawingView::tryToMaintainConnections(const QList<DrawingItem*>& items, bool allowResize,
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

void DrawingView::disconnectAll(DrawingItemPoint* itemPoint, QUndoCommand* command)
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

void DrawingView::recalculateContentSize(const QRectF& targetSceneRect)
{
	qreal dx = 0, dy = 0;

	if (mScene)
	{
		QRectF sceneRect = mScene->sceneRect();
		QRectF targetRect;

		if (targetSceneRect.isValid())
		{
			targetRect.setLeft(qMin(targetSceneRect.left(), sceneRect.left()));
			targetRect.setTop(qMin(targetSceneRect.top(), sceneRect.top()));
			targetRect.setRight(qMax(targetSceneRect.right(), sceneRect.right()));
			targetRect.setBottom(qMax(targetSceneRect.bottom(), sceneRect.bottom()));
		}
		else targetRect = sceneRect;

		int contentWidth = qRound(targetRect.width() * mScale);
		int contentHeight = qRound(targetRect.height() * mScale);
		int viewportWidth = maximumViewportSize().width();
		int viewportHeight = maximumViewportSize().height();
		int scrollBarExtent = style()->pixelMetric(QStyle::PM_ScrollBarExtent, 0, this);

		// Set scroll bar range
		if (contentWidth > viewportWidth)
		{
			int contentLeft = (targetRect.left() - sceneRect.left()) * mScale;

			if (verticalScrollBarPolicy() == Qt::ScrollBarAsNeeded) viewportWidth -= scrollBarExtent;

			horizontalScrollBar()->setRange(contentLeft - 1, contentLeft + contentWidth - viewportWidth + 1);
			horizontalScrollBar()->setSingleStep(viewportWidth / 80);
			horizontalScrollBar()->setPageStep(viewportWidth);
		}
		else horizontalScrollBar()->setRange(0, 0);

		if (contentHeight > viewportHeight)
		{
			int contentTop = (targetRect.top() - sceneRect.top()) * mScale;

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
	}

	mViewportTransform = QTransform();
	mViewportTransform.translate(qRound(dx), qRound(dy));
	mViewportTransform.scale(mScale, mScale);

	mSceneTransform = mViewportTransform.inverted();
}

//==================================================================================================

qreal DrawingView::minimumPenWidth(DrawingItem* item) const
{
	const qreal penWidthHint = 8;

	qreal minimumPenWidth = 0;

	if (item)
	{
		QPointF mappedPenSize = item->mapFromScene(item->position() +
			mapToScene(QPoint(penWidthHint, penWidthHint)) - mapToScene(QPoint(0, 0)));

		minimumPenWidth = qMax(qAbs(mappedPenSize.x()), qAbs(mappedPenSize.y()));
	}

	return minimumPenWidth;
}

QRect DrawingView::pointRect(DrawingItemPoint* point) const
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

DrawingItemPoint* DrawingView::pointAt(DrawingItem* item, const QPointF& itemPos) const
{
	DrawingItemPoint* itemPoint = nullptr;

	if (item)
	{
		QList<DrawingItemPoint*> itemPoints = item->points();
		QRectF pointItemRect;

		for(auto pointIter = itemPoints.begin(); itemPoint == nullptr && pointIter != itemPoints.end(); pointIter++)
		{
			pointItemRect = item->mapFromScene(mapToScene(pointRect(*pointIter))).boundingRect();
			if (pointItemRect.contains(itemPos)) itemPoint = *pointIter;
		}
	}

	return itemPoint;
}

//==================================================================================================

bool DrawingView::shouldConnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const
{
	bool shouldConnect = false;

	if (point1 && point1->item() && point2 && point2->item() && point1->item() != point2->item())
	{
		qreal threshold = mGrid / 4000;
		QPointF vec = point1->item()->mapToScene(point1->position()) - point2->item()->mapToScene(point2->position());
		qreal distance = qSqrt(vec.x() * vec.x() + vec.y() * vec.y());

		shouldConnect = ((point1->flags() & DrawingItemPoint::Connection) && (point2->flags() & DrawingItemPoint::Connection) &&
			((point1->flags() & DrawingItemPoint::Free) || (point2->flags() & DrawingItemPoint::Free)) &&
			!point1->isConnected(point2) && !point1->isConnected(point2->item()) && distance <= threshold);
	}

	return shouldConnect;
}

bool DrawingView::shouldDisconnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const
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

//==================================================================================================

void DrawingView::sendMouseInfoText(const QPointF& pos)
{
	if (mFlags & SendsMouseMoveInfo)
		emit mouseInfoChanged("(" + QString::number(pos.x()) + "," + QString::number(pos.y()) + ")");
}

void DrawingView::sendMouseInfoText(const QPointF& p1, const QPointF& p2)
{
	if (mFlags & SendsMouseMoveInfo)
	{
		QString mouseInfoText;
		QPointF delta = p2 - p1;

		mouseInfoText += "(" + QString::number(p1.x()) + "," + QString::number(p1.y()) + ")";
		mouseInfoText += " - (" + QString::number(p2.x()) + "," + QString::number(p2.y()) + ")";
		mouseInfoText += "  " + QString(QChar(0x394)) + " = (" +
			QString::number(delta.x()) + "," + QString::number(delta.y()) + ")";

		emit mouseInfoChanged(mouseInfoText);
	}
}
