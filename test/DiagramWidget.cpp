/* DiagramWidget.cpp
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

#include "DiagramWidget.h"

DiagramWidget::DiagramWidget() : DrawingWidget()
{
	addActions();
	createContextMenus();
}

DiagramWidget::~DiagramWidget() { }

//==================================================================================================

void DiagramWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton)
	{
		if (mode() == DefaultMode)
		{
			DrawingItem* mouseDownItem = itemAt(mapToScene(event->pos()));

			if (mouseDownItem && mouseDownItem->isSelected() && selectedItems().size() == 1)
			{
				if ((mouseDownItem->flags() & DrawingItem::CanInsertPoints) ||
					(mouseDownItem->flags() & DrawingItem::CanRemovePoints))
					mSinglePolyItemContextMenu.popup(event->globalPos() + QPoint(2, 2));
				else
					mSingleItemContextMenu.popup(event->globalPos() + QPoint(2, 2));
			}
			else if (mouseDownItem && mouseDownItem->isSelected())
			{
				mMultipleItemContextMenu.popup(event->globalPos() + QPoint(2, 2));
			}
			else
			{
				if (mouseDownItem == nullptr) clearSelection();
				mDiagramContextMenu.popup(event->globalPos() + QPoint(2, 2));
			}
		}
		else setDefaultMode();
	}

	DrawingWidget::mouseReleaseEvent(event);
}

//==================================================================================================

void DiagramWidget::addActions()
{
//	void 	moveSelection (const QPointF &newPos)
//	void 	resizeSelection (DrawingItemPoint *itemPoint, const QPointF &scenePos)
//	void 	selectArea (const QRectF &rect)
//	void 	selectArea (const QPainterPath &path)
//	void 	setDefaultMode ()
//	void 	setPlaceMode (DrawingItem *newItem)
//	void 	setScrollMode ()
//	void 	setZoomMode ()



	addAction("Undo", this, SLOT(undo()), "", "Ctrl+Z");
	addAction("Redo", this, SLOT(redo()), "", "Ctrl+Shift+Z");
	addAction("Cut", this, SLOT(cut()), "", "Ctrl+X");
	addAction("Copy", this, SLOT(copy()), "", "Ctrl+C");
	addAction("Paste", this, SLOT(paste()), "", "Ctrl+V");
	addAction("Delete", this, SLOT(deleteSelection()), "", "Delete");
	addAction("Select All", this, SLOT(selectAll()), "", "Ctrl+A");
	addAction("Select None", this, SLOT(selectNone()), "", "Ctrl+Shift+A");

	addAction("Rotate", this, SLOT(rotateSelection()), "", "R");
	addAction("Rotate Back", this, SLOT(rotateBackSelection()), "", "Shift+R");
	addAction("Flip", this, SLOT(flipSelection()), "", "F");

	addAction("Bring Forward", this, SLOT(bringForward()), "");
	addAction("Send Backward", this, SLOT(sendBackward()), "");
	addAction("Bring to Front", this, SLOT(bringToFront()), "");
	addAction("Send to Back", this, SLOT(sendToBack()), "");

	addAction("Insert Point", this, SLOT(insertItemPoint()), "");
	addAction("Remove Point", this, SLOT(removeItemPoint()), "");

	addAction("Group", this, SLOT(group()), "", "Ctrl+G");
	addAction("Ungroup", this, SLOT(ungroup()), "", "Ctrl+Shift+G");

	addAction("Zoom In", this, SLOT(zoomIn()), "", ".");
	addAction("Zoom Out", this, SLOT(zoomOut()), "", ",");
	addAction("Zoom Fit", this, SLOT(zoomFit()), "", "/");
}

void DiagramWidget::createContextMenus()
{
	QList<QAction*> actions = DrawingWidget::actions();

	mSingleItemContextMenu.addAction(actions[RotateAction]);
	mSingleItemContextMenu.addAction(actions[RotateBackAction]);
	mSingleItemContextMenu.addAction(actions[FlipAction]);
	mSingleItemContextMenu.addAction(actions[DeleteAction]);
	mSingleItemContextMenu.addSeparator();
	mSingleItemContextMenu.addAction(actions[GroupAction]);
	mSingleItemContextMenu.addSeparator();
	mSingleItemContextMenu.addAction(actions[UngroupAction]);
	mSingleItemContextMenu.addAction(actions[CutAction]);
	mSingleItemContextMenu.addAction(actions[CopyAction]);
	mSingleItemContextMenu.addAction(actions[PasteAction]);

	mSinglePolyItemContextMenu.addAction(actions[InsertPointAction]);
	mSinglePolyItemContextMenu.addAction(actions[RemovePointAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actions[RotateAction]);
	mSinglePolyItemContextMenu.addAction(actions[RotateBackAction]);
	mSinglePolyItemContextMenu.addAction(actions[FlipAction]);
	mSinglePolyItemContextMenu.addAction(actions[DeleteAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actions[CutAction]);
	mSinglePolyItemContextMenu.addAction(actions[CopyAction]);
	mSinglePolyItemContextMenu.addAction(actions[PasteAction]);

	mMultipleItemContextMenu.addAction(actions[RotateAction]);
	mMultipleItemContextMenu.addAction(actions[RotateBackAction]);
	mMultipleItemContextMenu.addAction(actions[FlipAction]);
	mMultipleItemContextMenu.addAction(actions[DeleteAction]);
	mMultipleItemContextMenu.addSeparator();
	mMultipleItemContextMenu.addAction(actions[GroupAction]);
	mMultipleItemContextMenu.addAction(actions[UngroupAction]);
	mMultipleItemContextMenu.addSeparator();
	mMultipleItemContextMenu.addAction(actions[CutAction]);
	mMultipleItemContextMenu.addAction(actions[CopyAction]);
	mMultipleItemContextMenu.addAction(actions[PasteAction]);

	mDiagramContextMenu.addAction(actions[UndoAction]);
	mDiagramContextMenu.addAction(actions[RedoAction]);
	mDiagramContextMenu.addSeparator();
	mDiagramContextMenu.addAction(actions[CutAction]);
	mDiagramContextMenu.addAction(actions[CopyAction]);
	mDiagramContextMenu.addAction(actions[PasteAction]);
	mDiagramContextMenu.addSeparator();
	mDiagramContextMenu.addAction(actions[ZoomInAction]);
	mDiagramContextMenu.addAction(actions[ZoomOutAction]);
	mDiagramContextMenu.addAction(actions[ZoomFitAction]);
}

QAction* DiagramWidget::addAction(const QString& text, QObject* slotObj, const char* slotFunction,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);
	if (slotObj) connect(action, SIGNAL(triggered()), slotObj, slotFunction);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	QAbstractScrollArea::addAction(action);
	return action;
}
