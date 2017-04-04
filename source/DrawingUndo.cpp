/* DrawingUndo.cpp
 *
 * Copyright (C) 2013-2017 Jason Allen
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

#include "DrawingUndo.h"
#include "DrawingView.h"
#include "DrawingScene.h"
#include "DrawingItem.h"
#include "DrawingItemPoint.h"

DrawingUndoCommand::DrawingUndoCommand(const QString& title, QUndoCommand* parent) :
	QUndoCommand(title, parent) { }

DrawingUndoCommand::DrawingUndoCommand(const DrawingUndoCommand& command, QUndoCommand* parent) :
	QUndoCommand(command.text(), parent)
{
	QList<QUndoCommand*> otherChildren;

	for(int i = 0; i < command.childCount(); i++)
		otherChildren.append(const_cast<QUndoCommand*>(command.child(i)));

	for(auto otherChildIter = otherChildren.begin(); 
		otherChildIter != otherChildren.end(); otherChildIter++)
	{
		switch ((*otherChildIter)->id())
		{
		case ItemResizeType:
			new DrawingResizeItemCommand(
				*static_cast<DrawingResizeItemCommand*>(*otherChildIter), this);
			break;
		case PointConnectType:
			new DrawingItemPointConnectCommand(
				*static_cast<DrawingItemPointConnectCommand*>(*otherChildIter), this);
			break;
		case PointDisconnectType:
			new DrawingItemPointDisconnectCommand(
				*static_cast<DrawingItemPointDisconnectCommand*>(*otherChildIter), this);
			break;
		default:
			break;
		}
	}
}

DrawingUndoCommand::~DrawingUndoCommand() { }

void DrawingUndoCommand::mergeChildren(const QUndoCommand* command)
{
	bool mergeSuccess;
	QList<QUndoCommand*> children, otherChildren;

	for(int i = 0; i < childCount(); i++)
		children.append(const_cast<QUndoCommand*>(child(i)));
	for(int i = 0; i < command->childCount(); i++)
		otherChildren.append(const_cast<QUndoCommand*>(command->child(i)));

	for(auto otherChildIter = otherChildren.begin(); 
		otherChildIter != otherChildren.end(); otherChildIter++)
	{
		mergeSuccess = false;
		for(auto childIter = children.begin(); childIter != children.end(); childIter++)
			mergeSuccess = ((*childIter)->mergeWith(*otherChildIter) && mergeSuccess);

		if (!mergeSuccess)
		{
			switch ((*otherChildIter)->id())
			{
			case ItemResizeType:
				new DrawingResizeItemCommand(
					*static_cast<DrawingResizeItemCommand*>(*otherChildIter), this);
				break;
			case PointConnectType:
				new DrawingItemPointConnectCommand(
					*static_cast<DrawingItemPointConnectCommand*>(*otherChildIter), this);
				break;
			case PointDisconnectType:
				new DrawingItemPointDisconnectCommand(
					*static_cast<DrawingItemPointDisconnectCommand*>(*otherChildIter), this);
				break;
			default:
				break;
			}
		}
	}
}

//==================================================================================================

DrawingAddItemsCommand::DrawingAddItemsCommand(DrawingView* view,
	const QList<DrawingItem*>& items, QUndoCommand* parent)
	: DrawingUndoCommand("Add Items", parent)
{
	mView = view;
	mItems = items;
	mUndone = true;
}

DrawingAddItemsCommand::~DrawingAddItemsCommand()
{
	if (mUndone)
	{
		while (!mItems.isEmpty()) delete mItems.takeFirst();
	}
}

int DrawingAddItemsCommand::id() const
{
	return AddItemsType;
}

void DrawingAddItemsCommand::redo()
{
	mUndone = false;
	if (mView) mView->addItems(mItems);
	DrawingUndoCommand::redo();
}

void DrawingAddItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->removeItems(mItems);
	mUndone = true;
}

//==================================================================================================

DrawingRemoveItemsCommand::DrawingRemoveItemsCommand(DrawingView* view,
	const QList<DrawingItem*>& items, QUndoCommand* parent)
	: DrawingUndoCommand("Remove Items", parent)
{
	mView = view;
	mItems = items;
	mUndone = true;
	
	if (mView && mView->scene())
	{
		QList<DrawingItem*> drawingItems = mView->scene()->items();
		for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
			mItemIndex[*itemIter] = drawingItems.indexOf(*itemIter);
	}
}

DrawingRemoveItemsCommand::~DrawingRemoveItemsCommand()
{
	if (!mUndone)
	{
		while (!mItems.isEmpty()) delete mItems.takeFirst();
	}
}

int DrawingRemoveItemsCommand::id() const
{
	return RemoveItemsType;
}

void DrawingRemoveItemsCommand::redo()
{
	mUndone = false;
	if (mView) mView->removeItems(mItems);
	DrawingUndoCommand::redo();
}

void DrawingRemoveItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->insertItems(mItems, mItemIndex);
	mUndone = true;
}

//==================================================================================================

DrawingMoveItemsCommand::DrawingMoveItemsCommand(DrawingView* view,
	const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& newPos, bool finalMove,
	QUndoCommand* parent) : DrawingUndoCommand("Move Items", parent)
{
	mView = view;
	mItems = items;
	mScenePos = newPos;
	mFinalMove = finalMove;
	
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		mOriginalScenePos[*itemIter] = (*itemIter)->position();
}

DrawingMoveItemsCommand::~DrawingMoveItemsCommand() { }

int DrawingMoveItemsCommand::id() const
{
	return MoveItemsType;
}

bool DrawingMoveItemsCommand::mergeWith(const QUndoCommand* command)
{
	bool mergeSuccess = false;

	if (command && command->id() == MoveItemsType)
	{
		const DrawingMoveItemsCommand* moveCommand =
			static_cast<const DrawingMoveItemsCommand*>(command);

		if (moveCommand && mView == moveCommand->mView && mItems == moveCommand->mItems && !mFinalMove)
		{
			mScenePos = moveCommand->mScenePos;
			mFinalMove = moveCommand->mFinalMove;
			mergeChildren(moveCommand);
			mergeSuccess = true;
		}
	}

	return mergeSuccess;
}

void DrawingMoveItemsCommand::redo()
{
	if (mView) mView->moveItems(mItems, mScenePos);
	DrawingUndoCommand::redo();
}

void DrawingMoveItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->moveItems(mItems, mOriginalScenePos);
}

//==================================================================================================

DrawingResizeItemCommand::DrawingResizeItemCommand(DrawingView* view, DrawingItemPoint* point,
	const QPointF& scenePos, bool finalResize, QUndoCommand* parent)
	: DrawingUndoCommand("Resize Item", parent)
{
	mView = view;
	mPoint = point;
	mFinalResize = finalResize;
	
	if (mPoint && mPoint->item())
	{
		mNewPos = mPoint->item()->mapToParent(mPoint->item()->mapFromScene(scenePos));
		mOriginalPos = mPoint->item()->mapToParent(mPoint->position());
	}
}

DrawingResizeItemCommand::DrawingResizeItemCommand(const DrawingResizeItemCommand& command,
	QUndoCommand* parent) : DrawingUndoCommand(command, parent)
{
	mView = command.mView;
	mPoint = command.mPoint;
	mNewPos = command.mNewPos;
	mOriginalPos = command.mOriginalPos;
	mFinalResize = command.mFinalResize;
}

DrawingResizeItemCommand::~DrawingResizeItemCommand() { }

int DrawingResizeItemCommand::id() const
{
	return ItemResizeType;
}

bool DrawingResizeItemCommand::mergeWith(const QUndoCommand* command)
{
	bool mergeSuccess = false;

	if (command && command->id() == ItemResizeType)
	{
		const DrawingResizeItemCommand* resizeCommand =
			static_cast<const DrawingResizeItemCommand*>(command);

		if (resizeCommand && mView == resizeCommand->mView &&
			mPoint == resizeCommand->mPoint && !mFinalResize)
		{
			mNewPos = resizeCommand->mNewPos;
			mFinalResize = resizeCommand->mFinalResize;
			mergeChildren(resizeCommand);
			mergeSuccess = true;
		}
	}

	return mergeSuccess;
}

void DrawingResizeItemCommand::redo()
{
	if (mView) mView->resizeItem(mPoint, mNewPos);
	DrawingUndoCommand::redo();
}

void DrawingResizeItemCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->resizeItem(mPoint, mOriginalPos);
}

//==================================================================================================

DrawingRotateItemsCommand::DrawingRotateItemsCommand(DrawingView* view,
	const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* parent)
	: DrawingUndoCommand("Rotate Items", parent)
{
	mView = view;
	mItems = items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		mParentPos[*itemIter] = (*itemIter)->mapToParent((*itemIter)->mapFromScene(scenePos));
}

DrawingRotateItemsCommand::~DrawingRotateItemsCommand() { }

int DrawingRotateItemsCommand::id() const
{
	return RotateItemsType;
}

void DrawingRotateItemsCommand::redo()
{
	if (mView) mView->rotateItems(mItems, mParentPos);
	DrawingUndoCommand::redo();
}

void DrawingRotateItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->rotateBackItems(mItems, mParentPos);
}

//==================================================================================================

DrawingRotateBackItemsCommand::DrawingRotateBackItemsCommand(DrawingView* view,
	const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* parent)
	: DrawingUndoCommand("Rotate Back Items", parent)
{
	mView = view;
	mItems = items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		mParentPos[*itemIter] = (*itemIter)->mapToParent((*itemIter)->mapFromScene(scenePos));
}

DrawingRotateBackItemsCommand::~DrawingRotateBackItemsCommand() { }

int DrawingRotateBackItemsCommand::id() const
{
	return RotateBackItemsType;
}

void DrawingRotateBackItemsCommand::redo()
{
	if (mView) mView->rotateBackItems(mItems, mParentPos);
	DrawingUndoCommand::redo();
}

void DrawingRotateBackItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->rotateItems(mItems, mParentPos);
}

//==================================================================================================

DrawingFlipItemsHorizontalCommand::DrawingFlipItemsHorizontalCommand(DrawingView* view,
	const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* parent)
	: DrawingUndoCommand("Flip Items Horizontal", parent)
{
	mView = view;
	mItems = items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		mParentPos[*itemIter] = (*itemIter)->mapToParent((*itemIter)->mapFromScene(scenePos));
}

DrawingFlipItemsHorizontalCommand::~DrawingFlipItemsHorizontalCommand() { }

int DrawingFlipItemsHorizontalCommand::id() const
{
	return FlipItemsHorizontalType;
}

void DrawingFlipItemsHorizontalCommand::redo()
{
	if (mView) mView->flipItemsHorizontal(mItems, mParentPos);
	DrawingUndoCommand::redo();
}

void DrawingFlipItemsHorizontalCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->flipItemsHorizontal(mItems, mParentPos);
}

//==================================================================================================

DrawingFlipItemsVerticalCommand::DrawingFlipItemsVerticalCommand(DrawingView* view,
	const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* parent)
	: DrawingUndoCommand("Flip Items Vertical", parent)
{
	mView = view;
	mItems = items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		mParentPos[*itemIter] = (*itemIter)->mapToParent((*itemIter)->mapFromScene(scenePos));
}

DrawingFlipItemsVerticalCommand::~DrawingFlipItemsVerticalCommand() { }

int DrawingFlipItemsVerticalCommand::id() const
{
	return FlipItemsVerticalType;
}

void DrawingFlipItemsVerticalCommand::redo()
{
	if (mView) mView->flipItemsVertical(mItems, mParentPos);
	DrawingUndoCommand::redo();
}

void DrawingFlipItemsVerticalCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->flipItemsVertical(mItems, mParentPos);
}

//==================================================================================================

DrawingReorderItemsCommand::DrawingReorderItemsCommand(DrawingView* view,
	const QList<DrawingItem*>& newItemOrder, QUndoCommand* parent)
	: DrawingUndoCommand("Reorder Items", parent)
{
	mView = view;
	mNewItemOrder = newItemOrder;
	if (mView && mView->scene()) mOriginalItemOrder = mView->scene()->items();
}

DrawingReorderItemsCommand::~DrawingReorderItemsCommand() { }

int DrawingReorderItemsCommand::id() const
{
	return ReorderItemsType;
}

void DrawingReorderItemsCommand::redo()
{
	if (mView) mView->reorderItems(mNewItemOrder);
	DrawingUndoCommand::redo();
}

void DrawingReorderItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->reorderItems(mOriginalItemOrder);
}

//==================================================================================================

DrawingSelectItemsCommand::DrawingSelectItemsCommand(DrawingView* view,
	const QList<DrawingItem*>& newSelectedItems, bool finalSelect, QUndoCommand* parent)
	: DrawingUndoCommand("Select Items", parent)
{
	mView = view;
	mSelectedItems = newSelectedItems;
	mFinalSelect = finalSelect;
	
	if (mView) mOriginalSelectedItems = mView->selectedItems();
}

DrawingSelectItemsCommand::~DrawingSelectItemsCommand() { }

int DrawingSelectItemsCommand::id() const
{
	return SelectItemsType;
}

bool DrawingSelectItemsCommand::mergeWith(const QUndoCommand* command)
{
	bool mergeSuccess = false;

	if (command && command->id() == SelectItemsType)
	{
		const DrawingSelectItemsCommand* selectCommand =
			static_cast<const DrawingSelectItemsCommand*>(command);

		if (selectCommand && mView == selectCommand->mView && !mFinalSelect)
		{
			for(auto itemIter = selectCommand->mSelectedItems.begin();
				itemIter != selectCommand->mSelectedItems.end(); itemIter++)
			{
				if (!mSelectedItems.contains(*itemIter)) mSelectedItems.append(*itemIter);
			}

			mFinalSelect = selectCommand->mFinalSelect;
			mergeChildren(selectCommand);
			mergeSuccess = true;
		}
	}

	return mergeSuccess;
}

void DrawingSelectItemsCommand::redo()
{
	if (mView) mView->selectItems(mSelectedItems);
	DrawingUndoCommand::redo();
}

void DrawingSelectItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->selectItems(mOriginalSelectedItems);
}

//==================================================================================================

DrawingItemInsertPointCommand::DrawingItemInsertPointCommand(DrawingView* view,
	DrawingItem* item, DrawingItemPoint* point, int pointIndex, QUndoCommand* parent) 
	: DrawingUndoCommand("Insert Point", parent)
{
	mView = view;
	mItem = item;
	mPoint = point;
	mPointIndex = pointIndex;
	mUndone = true;
}

DrawingItemInsertPointCommand::~DrawingItemInsertPointCommand()
{
	if (mUndone) delete mPoint;
}

int DrawingItemInsertPointCommand::id() const
{
	return InsertItemPointType;
}

void DrawingItemInsertPointCommand::redo()
{
	mUndone = false;
	if (mView) mView->insertItemPoint(mItem, mPoint, mPointIndex);
	DrawingUndoCommand::redo();
}

void DrawingItemInsertPointCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->removeItemPoint(mItem, mPoint);
	mUndone = true;
}

//==================================================================================================

DrawingItemRemovePointCommand::DrawingItemRemovePointCommand(DrawingView* view,
	DrawingItem* item, DrawingItemPoint* point, QUndoCommand* parent) 
	: DrawingUndoCommand("Remove Point", parent)
{
	mView = view;
	mItem = item;
	mPoint = point;
	mUndone = true;
	
	mPointIndex = (mItem) ? mItem->points().indexOf(mPoint) : -1;
}

DrawingItemRemovePointCommand::~DrawingItemRemovePointCommand()
{
	if (!mUndone) delete mPoint;
}

int DrawingItemRemovePointCommand::id() const
{
	return RemoveItemPointType;
}

void DrawingItemRemovePointCommand::redo()
{
	mUndone = false;
	if (mView) mView->removeItemPoint(mItem, mPoint);
	DrawingUndoCommand::redo();
}

void DrawingItemRemovePointCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->insertItemPoint(mItem, mPoint, mPointIndex);
	mUndone = true;
}

//==================================================================================================

DrawingItemPointConnectCommand::DrawingItemPointConnectCommand(DrawingView* view,
	DrawingItemPoint* point1, DrawingItemPoint* point2, QUndoCommand* parent)
	: DrawingUndoCommand("Connect Points", parent)
{
	mView = view;
	mPoint1 = point1;
	mPoint2 = point2;
}

DrawingItemPointConnectCommand::DrawingItemPointConnectCommand(
	const DrawingItemPointConnectCommand& command, QUndoCommand* parent)
	: DrawingUndoCommand(command, parent)
{
	mView = command.mView;
	mPoint1 = command.mPoint1;
	mPoint2 = command.mPoint2;
}

DrawingItemPointConnectCommand::~DrawingItemPointConnectCommand() { }

int DrawingItemPointConnectCommand::id() const
{
	return PointConnectType;
}

void DrawingItemPointConnectCommand::redo()
{
	if (mView) mView->connectItemPoints(mPoint1, mPoint2);
	DrawingUndoCommand::redo();
}

void DrawingItemPointConnectCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->disconnectItemPoints(mPoint1, mPoint2);
}

//==================================================================================================

DrawingItemPointDisconnectCommand::DrawingItemPointDisconnectCommand(DrawingView* view,
	DrawingItemPoint* point1, DrawingItemPoint* point2, QUndoCommand* parent)
	: DrawingUndoCommand("Disconnect Points", parent)
{
	mView = view;
	mPoint1 = point1;
	mPoint2 = point2;
}

DrawingItemPointDisconnectCommand::DrawingItemPointDisconnectCommand(
	const DrawingItemPointDisconnectCommand& command, QUndoCommand* parent)
	: DrawingUndoCommand(command, parent)
{
	mView = command.mView;
	mPoint1 = command.mPoint1;
	mPoint2 = command.mPoint2;
}

DrawingItemPointDisconnectCommand::~DrawingItemPointDisconnectCommand() { }

int DrawingItemPointDisconnectCommand::id() const
{
	return PointDisconnectType;
}

void DrawingItemPointDisconnectCommand::redo()
{
	if (mView) mView->disconnectItemPoints(mPoint1, mPoint2);
	DrawingUndoCommand::redo();
}

void DrawingItemPointDisconnectCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->connectItemPoints(mPoint1, mPoint2);
}

//==================================================================================================

DrawingItemSetVisibilityCommand::DrawingItemSetVisibilityCommand(DrawingView* view,
	const QList<DrawingItem*>& items, bool visible, QUndoCommand* parent) :
	DrawingUndoCommand("Set Items' Visibility", parent)
{
	mView = view;
	mItems = items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		mVisibility[*itemIter] = visible;
		mOriginalVisibility[*itemIter] = (*itemIter)->isVisible();
	}
}

DrawingItemSetVisibilityCommand::~DrawingItemSetVisibilityCommand() { }

int DrawingItemSetVisibilityCommand::id() const
{
	return SetItemsVisibilityType;
}

void DrawingItemSetVisibilityCommand::redo()
{
	if (mView) mView->setItemsVisibility(mItems, mVisibility);
	DrawingUndoCommand::redo();
}

void DrawingItemSetVisibilityCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mView) mView->setItemsVisibility(mItems, mOriginalVisibility);
}
