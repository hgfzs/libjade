/* DrawingUndo.cpp
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

#include "DrawingUndo.h"
#include "DrawingWidget.h"
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

DrawingAddItemsCommand::DrawingAddItemsCommand(DrawingWidget* drawing, 
	const QList<DrawingItem*>& items, QUndoCommand* parent)
	: DrawingUndoCommand("Add Items", parent)
{
	mDrawing = drawing;
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
	if (mDrawing) mDrawing->addItems(mItems);
	DrawingUndoCommand::redo();
}

void DrawingAddItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mDrawing) mDrawing->removeItems(mItems);
	mUndone = true;
}

//==================================================================================================

DrawingRemoveItemsCommand::DrawingRemoveItemsCommand(DrawingWidget* drawing, 
	const QList<DrawingItem*>& items, QUndoCommand* parent)
	: DrawingUndoCommand("Remove Items", parent)
{
	mDrawing = drawing;
	mItems = items;
	mUndone = true;
	
	if (mDrawing)
	{
		QList<DrawingItem*> drawingItems = mDrawing->items();
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
	if (mDrawing) mDrawing->removeItems(mItems);
	DrawingUndoCommand::redo();
}

void DrawingRemoveItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mDrawing) mDrawing->insertItems(mItems, mItemIndex);
	mUndone = true;
}

//==================================================================================================

DrawingMoveItemsCommand::DrawingMoveItemsCommand(DrawingWidget* drawing, 
	const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& newPos, bool finalMove,
	QUndoCommand* parent) : DrawingUndoCommand("Move Items", parent)
{
	mDrawing = drawing;
	mItems = items;
	mScenePos = newPos;
	mFinalMove = finalMove;
	
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		mOriginalScenePos[*itemIter] = (*itemIter)->pos();
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

		if (moveCommand && mDrawing == moveCommand->mDrawing && mItems == moveCommand->mItems && !mFinalMove)
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
	if (mDrawing) mDrawing->moveItems(mItems, mScenePos);
	DrawingUndoCommand::redo();
}

void DrawingMoveItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mDrawing) mDrawing->moveItems(mItems, mOriginalScenePos);
}

//==================================================================================================

DrawingResizeItemCommand::DrawingResizeItemCommand(DrawingWidget* drawing, DrawingItemPoint* point, 
	const QPointF& scenePos, bool finalResize, QUndoCommand* parent)
	: DrawingUndoCommand("Resize Item", parent)
{
	mDrawing = drawing;
	mPoint = point;
	mScenePos = scenePos;
	mFinalResize = finalResize;
	
	if (mPoint && mPoint->item()) mOriginalScenePos = mPoint->item()->mapToScene(mPoint->pos());
}

DrawingResizeItemCommand::DrawingResizeItemCommand(const DrawingResizeItemCommand& command,
	QUndoCommand* parent) : DrawingUndoCommand(command, parent)
{
	mDrawing = command.mDrawing;
	mPoint = command.mPoint;
	mScenePos = command.mScenePos;
	mOriginalScenePos = command.mOriginalScenePos;
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

		if (resizeCommand && mDrawing == resizeCommand->mDrawing && 
			mPoint == resizeCommand->mPoint && !mFinalResize)
		{
			mScenePos = resizeCommand->mScenePos;
			mFinalResize = resizeCommand->mFinalResize;
			mergeChildren(resizeCommand);
			mergeSuccess = true;
		}
	}

	return mergeSuccess;
}

void DrawingResizeItemCommand::redo()
{
	if (mDrawing) mDrawing->resizeItem(mPoint, mScenePos);
	DrawingUndoCommand::redo();
}

void DrawingResizeItemCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mDrawing) mDrawing->resizeItem(mPoint, mOriginalScenePos);
}

//==================================================================================================

DrawingRotateItemsCommand::DrawingRotateItemsCommand(DrawingWidget* drawing, 
	const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* parent)
	: DrawingUndoCommand("Rotate Items", parent)
{
	mDrawing = drawing;
	mItems = items;
	mScenePos = scenePos;
}

DrawingRotateItemsCommand::~DrawingRotateItemsCommand() { }

int DrawingRotateItemsCommand::id() const
{
	return RotateItemsType;
}

void DrawingRotateItemsCommand::redo()
{
	if (mDrawing) mDrawing->rotateItems(mItems, mScenePos);
	DrawingUndoCommand::redo();
}

void DrawingRotateItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mDrawing) mDrawing->rotateBackItems(mItems, mScenePos);
}

//==================================================================================================

DrawingRotateBackItemsCommand::DrawingRotateBackItemsCommand(DrawingWidget* drawing, 
	const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* parent)
	: DrawingUndoCommand("Rotate Back Items", parent)
{
	mDrawing = drawing;
	mItems = items;
	mScenePos = scenePos;
}

DrawingRotateBackItemsCommand::~DrawingRotateBackItemsCommand() { }

int DrawingRotateBackItemsCommand::id() const
{
	return RotateBackItemsType;
}

void DrawingRotateBackItemsCommand::redo()
{
	if (mDrawing) mDrawing->rotateBackItems(mItems, mScenePos);
	DrawingUndoCommand::redo();
}

void DrawingRotateBackItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mDrawing) mDrawing->rotateItems(mItems, mScenePos);
}

//==================================================================================================

DrawingFlipItemsCommand::DrawingFlipItemsCommand(DrawingWidget* drawing, 
	const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* parent)
	: DrawingUndoCommand("Flip Items", parent)
{
	mDrawing = drawing;
	mItems = items;
	mScenePos = scenePos;
}

DrawingFlipItemsCommand::~DrawingFlipItemsCommand() { }

int DrawingFlipItemsCommand::id() const
{
	return FlipItemsType;
}

void DrawingFlipItemsCommand::redo()
{
	if (mDrawing) mDrawing->flipItems(mItems, mScenePos);
	DrawingUndoCommand::redo();
}

void DrawingFlipItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mDrawing) mDrawing->flipItems(mItems, mScenePos);
}

//==================================================================================================

DrawingReorderItemsCommand::DrawingReorderItemsCommand(DrawingWidget* drawing,
	const QList<DrawingItem*>& newItemOrder, QUndoCommand* parent)
	: DrawingUndoCommand("Reorder Items", parent)
{
	mDrawing = drawing;
	mNewItemOrder = newItemOrder;
	if (mDrawing) mOriginalItemOrder = mDrawing->items();
}

DrawingReorderItemsCommand::~DrawingReorderItemsCommand() { }

int DrawingReorderItemsCommand::id() const
{
	return ReorderItemsType;
}

void DrawingReorderItemsCommand::redo()
{
	if (mDrawing) mDrawing->reorderItems(mNewItemOrder);
	DrawingUndoCommand::redo();
}

void DrawingReorderItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mDrawing) mDrawing->reorderItems(mOriginalItemOrder);
}

//==================================================================================================

DrawingSelectItemsCommand::DrawingSelectItemsCommand(DrawingWidget* drawing, 
	const QList<DrawingItem*>& newSelectedItems, bool finalSelect, QUndoCommand* parent)
	: DrawingUndoCommand("Select Items", parent)
{
	mDrawing = drawing;
	mSelectedItems = newSelectedItems;
	mFinalSelect = finalSelect;
	
	if (mDrawing) mOriginalSelectedItems = mDrawing->selectedItems();
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

		if (selectCommand && mDrawing == selectCommand->mDrawing && !mFinalSelect)
		{
			mSelectedItems = selectCommand->mSelectedItems;
			mFinalSelect = selectCommand->mFinalSelect;
			mergeChildren(selectCommand);
			mergeSuccess = true;
		}
	}

	return mergeSuccess;
}

void DrawingSelectItemsCommand::redo()
{
	if (mDrawing) mDrawing->selectItems(mSelectedItems);
	DrawingUndoCommand::redo();
}

void DrawingSelectItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mDrawing) mDrawing->selectItems(mOriginalSelectedItems);
}

//==================================================================================================

DrawingItemInsertPointCommand::DrawingItemInsertPointCommand(DrawingWidget* drawing, 
	DrawingItem* item, DrawingItemPoint* point, int pointIndex, QUndoCommand* parent) 
	: DrawingUndoCommand("Insert Point", parent)
{
	mDrawing = drawing;
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
	if (mDrawing) mDrawing->insertItemPoint(mItem, mPoint, mPointIndex);
	DrawingUndoCommand::redo();
}

void DrawingItemInsertPointCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mDrawing) mDrawing->removeItemPoint(mItem, mPoint);
	mUndone = true;
}

//==================================================================================================

DrawingItemRemovePointCommand::DrawingItemRemovePointCommand(DrawingWidget* drawing, 
	DrawingItem* item, DrawingItemPoint* point, QUndoCommand* parent) 
	: DrawingUndoCommand("Remove Point", parent)
{
	mDrawing = drawing;
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
	if (mDrawing) mDrawing->removeItemPoint(mItem, mPoint);
	DrawingUndoCommand::redo();
}

void DrawingItemRemovePointCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mDrawing) mDrawing->insertItemPoint(mItem, mPoint, mPointIndex);
	mUndone = true;
}

//==================================================================================================

DrawingItemPointConnectCommand::DrawingItemPointConnectCommand(DrawingWidget* drawing, 
	DrawingItemPoint* point1, DrawingItemPoint* point2, QUndoCommand* parent)
	: DrawingUndoCommand("Connect Points", parent)
{
	mDrawing = drawing;
	mPoint1 = point1;
	mPoint2 = point2;
}

DrawingItemPointConnectCommand::DrawingItemPointConnectCommand(
	const DrawingItemPointConnectCommand& command, QUndoCommand* parent)
	: DrawingUndoCommand(command, parent)
{
	mDrawing = command.mDrawing;
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
	if (mDrawing) mDrawing->connectItemPoints(mPoint1, mPoint2);
	DrawingUndoCommand::redo();
}

void DrawingItemPointConnectCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mDrawing) mDrawing->disconnectItemPoints(mPoint1, mPoint2);
}

//==================================================================================================

DrawingItemPointDisconnectCommand::DrawingItemPointDisconnectCommand(DrawingWidget* drawing, 
	DrawingItemPoint* point1, DrawingItemPoint* point2, QUndoCommand* parent)
	: DrawingUndoCommand("Disconnect Points", parent)
{
	mDrawing = drawing;
	mPoint1 = point1;
	mPoint2 = point2;
}

DrawingItemPointDisconnectCommand::DrawingItemPointDisconnectCommand(
	const DrawingItemPointDisconnectCommand& command, QUndoCommand* parent)
	: DrawingUndoCommand(command, parent)
{
	mDrawing = command.mDrawing;
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
	if (mDrawing) mDrawing->disconnectItemPoints(mPoint1, mPoint2);
	DrawingUndoCommand::redo();
}

void DrawingItemPointDisconnectCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mDrawing) mDrawing->connectItemPoints(mPoint1, mPoint2);
}
