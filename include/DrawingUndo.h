/* DrawingUndo.h
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

#ifndef DRAWINGUNDO_H
#define DRAWINGUNDO_H

#include <QHash>
#include <QList>
#include <QPointF>
#include <QString>
#include <QTransform>
#include <QUndoCommand>

class DrawingWidget;
class DrawingItem;
class DrawingItemPoint;

class DrawingUndoCommand : public QUndoCommand
{
public:
	enum Type { AddItemsType, RemoveItemsType, MoveItemsType, ResizeItemType,
		RotateItemsType, RotateBackItemsType, FlipItemsHorizontalType, FlipItemsVerticalType,
		ReorderItemsType, SelectItemsType, SetItemsVisibilityType,
		InsertItemPointType, RemoveItemPointType,
		PointConnectType, PointDisconnectType, NumberOfCommands };

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
	DrawingWidget* mWidget;
	QList<DrawingItem*> mItems;
	bool mUndone;

public:
	DrawingAddItemsCommand(DrawingWidget* widget, const QList<DrawingItem*>& items,
		QUndoCommand* parent = nullptr);
	~DrawingAddItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingRemoveItemsCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mWidget;
	QList<DrawingItem*> mItems;
	QHash<DrawingItem*,int> mItemIndex;
	bool mUndone;

public:
	DrawingRemoveItemsCommand(DrawingWidget* widget, const QList<DrawingItem*>& items,
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
	DrawingWidget* mWidget;
	QList<DrawingItem*> mItems;
	QHash<DrawingItem*,QPointF> mPos;
	QHash<DrawingItem*,QPointF> mOriginalPos;
	bool mFinalMove;

public:
	DrawingMoveItemsCommand(DrawingWidget* widget, const QList<DrawingItem*>& items,
		const QHash<DrawingItem*,QPointF>& pos, bool finalMove, QUndoCommand* parent = nullptr);
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
	DrawingWidget* mWidget;
	DrawingItemPoint* mPoint;
	QPointF mPos;
	QPointF mOriginalPos;
	bool mFinalResize;

public:
	DrawingResizeItemCommand(DrawingWidget* widget, DrawingItemPoint* point,
		const QPointF& pos, bool finalResize, QUndoCommand* parent = nullptr);
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
	DrawingWidget* mWidget;
	QList<DrawingItem*> mItems;
	QPointF mPos;

public:
	DrawingRotateItemsCommand(DrawingWidget* widget, const QList<DrawingItem*>& items,
		const QPointF& pos, QUndoCommand* parent = nullptr);
	~DrawingRotateItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingRotateBackItemsCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mWidget;
	QList<DrawingItem*> mItems;
	QPointF mPos;

public:
	DrawingRotateBackItemsCommand(DrawingWidget* widget, const QList<DrawingItem*>& items,
		const QPointF& pos, QUndoCommand* parent = nullptr);
	~DrawingRotateBackItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingFlipItemsHorizontalCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mWidget;
	QList<DrawingItem*> mItems;
	QPointF mPos;

public:
	DrawingFlipItemsHorizontalCommand(DrawingWidget* widget, const QList<DrawingItem*>& items,
		const QPointF& pos, QUndoCommand* parent = nullptr);
	~DrawingFlipItemsHorizontalCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingFlipItemsVerticalCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mWidget;
	QList<DrawingItem*> mItems;
	QPointF mPos;

public:
	DrawingFlipItemsVerticalCommand(DrawingWidget* widget, const QList<DrawingItem*>& items,
		const QPointF& pos, QUndoCommand* parent = nullptr);
	~DrawingFlipItemsVerticalCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingReorderItemsCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mWidget;
	QList<DrawingItem*> mItemOrder;
	QList<DrawingItem*> mOriginalItemOrder;

public:
	DrawingReorderItemsCommand(DrawingWidget* widget,
		const QList<DrawingItem*>& itemOrder, QUndoCommand* parent = nullptr);
	~DrawingReorderItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingSelectItemsCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mWidget;
	QList<DrawingItem*> mSelectedItems;
	QList<DrawingItem*> mOriginalSelectedItems;
	bool mFinalSelect;
	
public:
	DrawingSelectItemsCommand(DrawingWidget* widget, const QList<DrawingItem*>& selectedItems,
		bool finalSelect, QUndoCommand* parent = nullptr);
	~DrawingSelectItemsCommand();

	int id() const;
	bool mergeWith(const QUndoCommand* command);

	void redo();
	void undo();
};

//==================================================================================================

class DrawingItemSetVisibilityCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mWidget;
	QHash<DrawingItem*,bool> mVisibility;
	QHash<DrawingItem*,bool> mOriginalVisibility;

public:
	DrawingItemSetVisibilityCommand(DrawingWidget* widget, const QList<DrawingItem*>& items,
		bool visible, QUndoCommand* parent = nullptr);
	~DrawingItemSetVisibilityCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingItemInsertPointCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mWidget;
	DrawingItem* mItem;
	DrawingItemPoint* mPoint;
	int mPointIndex;
	bool mUndone;

public:
	DrawingItemInsertPointCommand(DrawingWidget* widget, DrawingItem* item, DrawingItemPoint* point,
		int index, QUndoCommand* parent = nullptr);
	~DrawingItemInsertPointCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingItemRemovePointCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mWidget;
	DrawingItem* mItem;
	DrawingItemPoint* mPoint;
	int mPointIndex;
	bool mUndone;

public:
	DrawingItemRemovePointCommand(DrawingWidget* widget, DrawingItem* item, DrawingItemPoint* point,
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
	DrawingWidget* mWidget;
	DrawingItemPoint* mPoint1;
	DrawingItemPoint* mPoint2;

public:
	DrawingItemPointConnectCommand(DrawingWidget* widget, DrawingItemPoint* point1,
		DrawingItemPoint* point2, QUndoCommand* parent = nullptr);
	DrawingItemPointConnectCommand(const DrawingItemPointConnectCommand& command,
		QUndoCommand* parent = nullptr);
	~DrawingItemPointConnectCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingItemPointDisconnectCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mWidget;
	DrawingItemPoint* mPoint1;
	DrawingItemPoint* mPoint2;

public:
	DrawingItemPointDisconnectCommand(DrawingWidget* widget, DrawingItemPoint* point1,
		DrawingItemPoint* point2, QUndoCommand* parent = nullptr);
	DrawingItemPointDisconnectCommand(const DrawingItemPointDisconnectCommand& command,
		QUndoCommand* parent = nullptr);
	~DrawingItemPointDisconnectCommand();

	int id() const;

	void redo();
	void undo();
};

#endif
