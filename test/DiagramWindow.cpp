/* DiagramWindow.cpp
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

#include "DiagramWindow.h"
#include "DiagramWidget.h"

DiagramWindow::DiagramWindow() : QMainWindow()
{
	mDiagramWidget = new DiagramWidget();
	mDiagramWidget->setFlags(DrawingWidget::UndoableSelectCommands);
	setCentralWidget(mDiagramWidget);
	connect(mDiagramWidget, SIGNAL(modeChanged(DrawingWidget::Mode)),
		this, SLOT(updateActionFromMode(DrawingWidget::Mode)));

	mModifiedLabel = new QLabel("");
	mModeLabel = new QLabel("Select Mode");
	mNumberOfItemsLabel = new QLabel("0");
	mNumberOfSelectedItemsLabel = new QLabel("0");
	mScaleLabel = new QLabel("100%");
	statusBar()->addWidget(mModeLabel, 100);
	statusBar()->addWidget(mModifiedLabel, 100);
	statusBar()->addWidget(mNumberOfItemsLabel, 100);
	statusBar()->addWidget(mNumberOfSelectedItemsLabel, 100);
	statusBar()->addWidget(mScaleLabel, 100);
	connect(mDiagramWidget, SIGNAL(modeChanged(DrawingWidget::Mode)), this, SLOT(setModeLabel(DrawingWidget::Mode)));
	connect(mDiagramWidget, SIGNAL(cleanChanged(bool)), this, SLOT(setModifiedLabel(bool)));
	connect(mDiagramWidget, SIGNAL(numberOfItemsChanged(int)), this, SLOT(setNumberOfItemsLabel(int)));
	connect(mDiagramWidget, SIGNAL(selectionChanged(const QList<DrawingItem*>&)),
		this, SLOT(setNumberOfSelectedItemsLabel(const QList<DrawingItem*>&)));
	connect(mDiagramWidget, SIGNAL(scaleChanged(qreal)), this, SLOT(setScaleLabel(qreal)));

	createActions();
	createMenus();

	setWindowTitle("libJade test");
	resize(1024, 768);
}

DiagramWindow::~DiagramWindow() { }

//==================================================================================================

void DiagramWindow::setModeFromAction(QAction* action)
{
	if (action->text() == "Scroll Mode") mDiagramWidget->setScrollMode();
	else if (action->text() == "Zoom Mode") mDiagramWidget->setZoomMode();
	else if (action->text() == "Place Arc") mDiagramWidget->setPlaceMode(new DrawingArcItem());
	else if (action->text() == "Place Curve") mDiagramWidget->setPlaceMode(new DrawingCurveItem());
	else if (action->text() == "Place Ellipse") mDiagramWidget->setPlaceMode(new DrawingEllipseItem());
	else if (action->text() == "Place Line") mDiagramWidget->setPlaceMode(new DrawingLineItem());
	else if (action->text() == "Place Polygon") mDiagramWidget->setPlaceMode(new DrawingPolygonItem());
	else if (action->text() == "Place Polyline") mDiagramWidget->setPlaceMode(new DrawingPolylineItem());
	else if (action->text() == "Place Rect") mDiagramWidget->setPlaceMode(new DrawingRectItem());
	//else if (action->text() == "Place Text") mDiagramWidget->setPlaceMode(new DrawingTextItem());
	//else if (action->text() == "Place Path") mDiagramWidget->setPlaceMode(new DrawingPathItem());
	else mDiagramWidget->setDefaultMode();
}

void DiagramWindow::updateActionFromMode(DrawingWidget::Mode mode)
{
	QList<QAction*> modeActions = mModeActionGroup->actions();

	switch (mode)
	{
	case DrawingWidget::ScrollMode: modeActions[ScrollModeAction]->setChecked(true); break;
	case DrawingWidget::ZoomMode: modeActions[ZoomModeAction]->setChecked(true); break;
	case DrawingWidget::DefaultMode: modeActions[DefaultModeAction]->setChecked(true); break;
	}
}

//==================================================================================================

void DiagramWindow::setModeLabel(DrawingWidget::Mode mode)
{
	switch (mode)
	{
	case DrawingWidget::ScrollMode: mModeLabel->setText("Scroll Mode"); break;
	case DrawingWidget::ZoomMode: mModeLabel->setText("Zoom Mode"); break;
	case DrawingWidget::PlaceMode: mModeLabel->setText("Place Mode"); break;
	default: mModeLabel->setText("Select Mode"); break;
	};
}

void DiagramWindow::setModifiedLabel(bool clean)
{
	if (clean) mModifiedLabel->setText("");
	else mModifiedLabel->setText("Modified");
}

void DiagramWindow::setNumberOfItemsLabel(int numberOfItems)
{
	mNumberOfItemsLabel->setText(QString::number(numberOfItems));
}

void DiagramWindow::setNumberOfSelectedItemsLabel(const QList<DrawingItem*>& selectedItems)
{
	mNumberOfSelectedItemsLabel->setText(QString::number(selectedItems.size()));
}

void DiagramWindow::setScaleLabel(qreal scale)
{
	mScaleLabel->setText(QString::number(scale * 100, 'f', 1) + "%");
}

//==================================================================================================

void DiagramWindow::showEvent(QShowEvent* event)
{
	QMainWindow::showEvent(event);
	if (!event->spontaneous()) mDiagramWidget->zoomFit();
}

//==================================================================================================

void DiagramWindow::createActions()
{
	addAction("About Qt...", qApp, SLOT(aboutQt()));
	addAction("Exit", this, SLOT(close()));

	mModeActionGroup = new QActionGroup(this);
	connect(mModeActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(setModeFromAction(QAction*)));

	addModeAction("Select Mode", "", "Escape");
	addModeAction("Scroll Mode", "", "");
	addModeAction("Zoom Mode", "", "");
	addModeAction("Place Arc", "", "");
	addModeAction("Place Curve", "", "");
	addModeAction("Place Ellipse", "", "");
	addModeAction("Place Line", "", "");
	addModeAction("Place Path", "", "");
	addModeAction("Place Polygon", "", "");
	addModeAction("Place Polyline", "", "");
	addModeAction("Place Rect", "", "");
	addModeAction("Place Text", "", "");

	actions()[DefaultModeAction]->setChecked(true);
}

void DiagramWindow::createMenus()
{
	QList<QAction*> actions = DiagramWindow::actions();
	QList<QAction*> diagramActions = mDiagramWidget->actions();
	QList<QAction*> modeActions = mModeActionGroup->actions();
	QMenu* menu;

	menu = menuBar()->addMenu("File");
	menu->addAction(actions[AboutQtAction]);
	menu->addSeparator();
	menu->addAction(actions[ExitAction]);

	menu = menuBar()->addMenu("Edit");
	menu->addAction(diagramActions[DiagramWidget::UndoAction]);
	menu->addAction(diagramActions[DiagramWidget::RedoAction]);
	menu->addSeparator();
	menu->addAction(diagramActions[DiagramWidget::CutAction]);
	menu->addAction(diagramActions[DiagramWidget::CopyAction]);
	menu->addAction(diagramActions[DiagramWidget::PasteAction]);
	menu->addAction(diagramActions[DiagramWidget::DeleteAction]);
	menu->addSeparator();
	menu->addAction(diagramActions[DiagramWidget::SelectAllAction]);
	menu->addAction(diagramActions[DiagramWidget::SelectNoneAction]);

	menu = menuBar()->addMenu("Object");
	menu->addAction(diagramActions[DiagramWidget::RotateAction]);
	menu->addAction(diagramActions[DiagramWidget::RotateBackAction]);
	menu->addAction(diagramActions[DiagramWidget::FlipAction]);
	menu->addSeparator();
	menu->addAction(diagramActions[DiagramWidget::InsertPointAction]);
	menu->addAction(diagramActions[DiagramWidget::RemovePointAction]);
	menu->addSeparator();
	menu->addAction(diagramActions[DiagramWidget::GroupAction]);
	menu->addAction(diagramActions[DiagramWidget::UngroupAction]);
	menu->addSeparator();
	menu->addAction(diagramActions[DiagramWidget::BringForwardAction]);
	menu->addAction(diagramActions[DiagramWidget::SendBackwardAction]);
	menu->addAction(diagramActions[DiagramWidget::BringToFrontAction]);
	menu->addAction(diagramActions[DiagramWidget::SendToBackAction]);

	menu = menuBar()->addMenu("Diagram");
	menu->addAction(modeActions[DefaultModeAction]);
	menu->addAction(modeActions[ScrollModeAction]);
	menu->addAction(modeActions[ZoomModeAction]);
	menu->addSeparator();
	menu->addAction(modeActions[PlaceArcAction]);
	menu->addAction(modeActions[PlaceCurveAction]);
	menu->addAction(modeActions[PlaceEllipseAction]);
	menu->addAction(modeActions[PlaceLineAction]);
	menu->addAction(modeActions[PlacePathAction]);
	menu->addAction(modeActions[PlacePolygonAction]);
	menu->addAction(modeActions[PlacePolylineAction]);
	menu->addAction(modeActions[PlaceRectAction]);
	menu->addAction(modeActions[PlaceTextAction]);

	menu = menuBar()->addMenu("View");
	menu->addAction(diagramActions[DiagramWidget::ZoomInAction]);
	menu->addAction(diagramActions[DiagramWidget::ZoomOutAction]);
	menu->addAction(diagramActions[DiagramWidget::ZoomFitAction]);
}

void DiagramWindow::addAction(const QString& text, QObject* slotObj, const char* slotFunction,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);
	connect(action, SIGNAL(triggered()), slotObj, slotFunction);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	QMainWindow::addAction(action);
}

QAction* DiagramWindow::addModeAction(const QString& text, const QString& iconPath,
	const QString& shortcut)
{
	QAction* action = new QAction(text, this);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	action->setCheckable(true);
	action->setActionGroup(mModeActionGroup);

	return action;
}
