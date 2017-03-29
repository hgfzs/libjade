/* DrawingUndo.h
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

#ifndef DRAWINGUNDO_H
#define DRAWINGUNDO_H

#include <QtWidgets>

class DrawingView;
class DrawingItem;
class DrawingItemPoint;

class DrawingUndoCommand : public QUndoCommand
{
public:
	enum Type { AddItemsType, RemoveItemsType, MoveItemsType,
		RotateItemsType, RotateBackItemsType, FlipItemsHorizontalType, FlipItemsVerticalType,
		ItemResizeType, ReorderItemsType, SelectItemsType,
		InsertItemPointType, RemoveItemPointType,
		PointConnectType, PointDisconnectType,
		UpdateItemPropertiesType, UpdatePropertiesType, NumberOfCommands };

public:
	DrawingUndoCommand(const QString& title = QString(), QUndoCommand* parent = nullptr);
	DrawingUndoCommand(const DrawingUndoCommand& command, QUndoCommand* parent = nullptr);
	virtual ~DrawingUndoCommand();

protected:
	virtual void mergeChildren(const QUndoCommand* command);
};

//==================================================================================================

class DrawingAddItemsCommand : public DrawingUndoCommand
{
private:
	DrawingView* mView;
	QList<DrawingItem*> mItems;
	bool mUndone;

public:
	DrawingAddItemsCommand(DrawingView* view, const QList<DrawingItem*>& items, QUndoCommand* parent = nullptr);
	~DrawingAddItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingRemoveItemsCommand : public DrawingUndoCommand
{
private:
	DrawingView* mView;
	QList<DrawingItem*> mItems;
	bool mUndone;
	QHash<DrawingItem*,int> mItemIndex;

public:
	DrawingRemoveItemsCommand(DrawingView* view, const QList<DrawingItem*>& items,
		QUndoCommand* parent = nullptr);
	~DrawingRemoveItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingMoveItemsCommand : public DrawingUndoCommand
{
private:
	DrawingView* mView;
	QList<DrawingItem*> mItems;
	QHash<DrawingItem*,QPointF> mScenePos;
	QHash<DrawingItem*,QPointF> mOriginalScenePos;
	bool mFinalMove;

public:
	DrawingMoveItemsCommand(DrawingView* view, const QList<DrawingItem*>& items,
		const QHash<DrawingItem*,QPointF>& newPos, bool finalMove, QUndoCommand* parent = nullptr);
	~DrawingMoveItemsCommand();

	int id() const;
	bool mergeWith(const QUndoCommand* command);

	void redo();
	void undo();
};

//==================================================================================================

class DrawingResizeItemCommand : public DrawingUndoCommand
{
private:
	DrawingView* mView;
	DrawingItemPoint* mPoint;
	QPointF mScenePos;
	QPointF mOriginalScenePos;
	bool mFinalResize;

public:
	DrawingResizeItemCommand(DrawingView* view, DrawingItemPoint* point,
		const QPointF& scenePos, bool finalResize, QUndoCommand* parent = nullptr);
	DrawingResizeItemCommand(const DrawingResizeItemCommand& command, QUndoCommand* parent = nullptr);
	~DrawingResizeItemCommand();

	int id() const;
	bool mergeWith(const QUndoCommand* command);

	void redo();
	void undo();
};

//==================================================================================================

class DrawingRotateItemsCommand : public DrawingUndoCommand
{
private:
	DrawingView* mView;
	QList<DrawingItem*> mItems;
	QPointF mScenePos;

public:
	DrawingRotateItemsCommand(DrawingView* view, const QList<DrawingItem*>& items,
		const QPointF& scenePos, QUndoCommand* parent = nullptr);
	~DrawingRotateItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingRotateBackItemsCommand : public DrawingUndoCommand
{
private:
	DrawingView* mView;
	QList<DrawingItem*> mItems;
	QPointF mScenePos;

public:
	DrawingRotateBackItemsCommand(DrawingView* view, const QList<DrawingItem*>& items,
		const QPointF& scenePos, QUndoCommand* parent = nullptr);
	~DrawingRotateBackItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingFlipItemsHorizontalCommand : public DrawingUndoCommand
{
private:
	DrawingView* mView;
	QList<DrawingItem*> mItems;
	QPointF mScenePos;

public:
	DrawingFlipItemsHorizontalCommand(DrawingView* view, const QList<DrawingItem*>& items,
		const QPointF& scenePos, QUndoCommand* parent = nullptr);
	~DrawingFlipItemsHorizontalCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingFlipItemsVerticalCommand : public DrawingUndoCommand
{
private:
	DrawingView* mView;
	QList<DrawingItem*> mItems;
	QPointF mScenePos;

public:
	DrawingFlipItemsVerticalCommand(DrawingView* view, const QList<DrawingItem*>& items,
		const QPointF& scenePos, QUndoCommand* parent = nullptr);
	~DrawingFlipItemsVerticalCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingReorderItemsCommand : public DrawingUndoCommand
{
private:
	DrawingView* mView;
	QList<DrawingItem*> mNewItemOrder;
	QList<DrawingItem*> mOriginalItemOrder;

public:
	DrawingReorderItemsCommand(DrawingView* view,
		const QList<DrawingItem*>& newItemOrder, QUndoCommand* parent = nullptr);
	~DrawingReorderItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingSelectItemsCommand : public DrawingUndoCommand
{
private:
	DrawingView* mView;
	QList<DrawingItem*> mSelectedItems;
	QList<DrawingItem*> mOriginalSelectedItems;
	bool mFinalSelect;
	
public:
	DrawingSelectItemsCommand(DrawingView* view, const QList<DrawingItem*>& newSelectedItems,
		bool finalSelect, QUndoCommand* parent = nullptr);
	~DrawingSelectItemsCommand();

	int id() const;
	bool mergeWith(const QUndoCommand* command);

	void redo();
	void undo();
};

//==================================================================================================

class DrawingItemInsertPointCommand : public DrawingUndoCommand
{
private:
	DrawingView* mView;
	DrawingItem* mItem;
	DrawingItemPoint* mPoint;
	int mPointIndex;
	bool mUndone;

public:
	DrawingItemInsertPointCommand(DrawingView* view, DrawingItem* item, DrawingItemPoint* point,
		int pointIndex, QUndoCommand* parent = nullptr);
	~DrawingItemInsertPointCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingItemRemovePointCommand : public DrawingUndoCommand
{
private:
	DrawingView* mView;
	DrawingItem* mItem;
	DrawingItemPoint* mPoint;
	int mPointIndex;
	bool mUndone;

public:
	DrawingItemRemovePointCommand(DrawingView* view, DrawingItem* item, DrawingItemPoint* point,
		QUndoCommand* parent = nullptr);
	~DrawingItemRemovePointCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingItemPointConnectCommand : public DrawingUndoCommand
{
private:
	DrawingView* mView;
	DrawingItemPoint* mPoint1;
	DrawingItemPoint* mPoint2;

public:
	DrawingItemPointConnectCommand(DrawingView* view, DrawingItemPoint* point1,
		DrawingItemPoint* point2, QUndoCommand* parent = NULL);
	DrawingItemPointConnectCommand(const DrawingItemPointConnectCommand& command,
		QUndoCommand* parent = NULL);
	~DrawingItemPointConnectCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingItemPointDisconnectCommand : public DrawingUndoCommand
{
private:
	DrawingView* mView;
	DrawingItemPoint* mPoint1;
	DrawingItemPoint* mPoint2;

public:
	DrawingItemPointDisconnectCommand(DrawingView* view, DrawingItemPoint* point1,
		DrawingItemPoint* point2, QUndoCommand* parent = NULL);
	DrawingItemPointDisconnectCommand(const DrawingItemPointDisconnectCommand& command,
		QUndoCommand* parent = NULL);
	~DrawingItemPointDisconnectCommand();

	int id() const;

	void redo();
	void undo();
};

#endif
