/* DiagramWindow.h
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

#ifndef DIAGRAMWINDOW_H
#define DIAGRAMWINDOW_H

#include <Drawing.h>

class DiagramWidget;

class DiagramWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	enum ActionIndex { AboutQtAction, ExitAction, NumberOfActions };
	enum ModeActionIndex { DefaultModeAction, ScrollModeAction, ZoomModeAction,
		PlaceArcAction, PlaceCurveAction, PlaceEllipseAction, PlaceLineAction, PlacePathAction,
		PlacePolygonAction, PlacePolylineAction, PlaceRectAction, PlaceTextAction,
		NumberOfModeActions };

private:
	DiagramWidget* mDiagramWidget;

	QLabel* mModifiedLabel;
	QLabel* mModeLabel;
	QLabel* mNumberOfItemsLabel;
	QLabel* mNumberOfSelectedItemsLabel;
	QLabel* mScaleLabel;

	QActionGroup* mModeActionGroup;

public:
	DiagramWindow();
	~DiagramWindow();

private slots:
	void setModeFromAction(QAction* action);
	void updateActionFromMode(DrawingWidget::Mode mode);

	void setModifiedLabel(bool clean);
	void setModeLabel(DrawingWidget::Mode mode);
	void setNumberOfItemsLabel(int numberOfItems);
	void setNumberOfSelectedItemsLabel(const QList<DrawingItem*>& selectedItems);
	void setScaleLabel(qreal scale);

private:
	void showEvent(QShowEvent* event);

	void createActions();
	void createMenus();
	void addAction(const QString& text, QObject* slotObj, const char* slotFunction,
		const QString& iconPath = QString(), const QString& shortcut = QString());
	QAction* addModeAction(const QString& text,
		const QString& iconPath = QString(), const QString& shortcut = QString());
};

#endif
