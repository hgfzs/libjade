/* DiagramWidget.h
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

#ifndef DIAGRAMWIDGET_H
#define DIAGRAMWIDGET_H

#include <Drawing.h>

class DiagramWidget : public DrawingWidget
{
	Q_OBJECT
	
public:
	enum ActionIndex { UndoAction, RedoAction, CutAction, CopyAction, PasteAction, DeleteAction,
		SelectAllAction, SelectNoneAction, RotateAction, RotateBackAction, FlipAction,
		BringForwardAction, SendBackwardAction, BringToFrontAction, SendToBackAction,
		InsertPointAction, RemovePointAction, GroupAction, UngroupAction,
		ZoomInAction, ZoomOutAction, ZoomFitAction, NumberOfActions };

private:
	QMenu mSingleItemContextMenu;
	QMenu mSinglePolyItemContextMenu;
	QMenu mMultipleItemContextMenu;
	QMenu mDiagramContextMenu;

public:
	DiagramWidget();
	~DiagramWidget();

private:
	void mouseReleaseEvent(QMouseEvent* event);

	void addActions();
	void createContextMenus();
	QAction* addAction(const QString& text, QObject* slotObj, const char* slotFunction,
		const QString& iconPath = QString(), const QString& shortcut = QString());
};

#endif
