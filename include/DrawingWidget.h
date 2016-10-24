/* DrawingWidget.h
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

#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include <DrawingItem.h>

/*! \brief Widget for managing a large number of two-dimensional DrawingItem objects.
 *
 * DrawingWidget visualizes a scene in a scrollable viewport.  The scrollable area of the scene is
 * determined by the sceneRect().
 *
 * Any position on the scene can be scrolled to by using the scroll bars or by calling centerOn().
 * To ensure that a certain area is visible, but not necessarily centered, call ensureVisible()
 * instead.
 *
 * visibleRect(), scrollBarDefinedRect()
 *
 * DrawingWidget visualizes the scene by calling render().  First, the background is drawn by
 * calling drawBackground().  Then the items are drawn by calling drawItems().  Items are drawn in
 * the order they were added to the widget, starting with the first item added and ending with the
 * most recent item added.  Finally, the foreground is drawn by calling drawForeground().  Any of
 * these functions can be overridden in a derived class to modify the default rendering behavior.
 *
 * \section widget_features Features
 *
 * modes
 * grid
 * undo/redo
 * cut/copy/paste
 * select all/none
 * zoom in/out
 * move, resize, rotate, flip items
 * bring items forward and send items backward
 * insert/remove item points
 * group/ungroup items
 * styles
 *
 * add/remove items
 * select/unselect items
 *
 * \section widget_items Items
 *
 * DrawingWidget serves as a container for DrawingItem objects.  Items can be added to the widget
 * using addItem() or insertItem() and removed using removeItem().
 *
 * DrawingWidget provides several ways to search for items within the scene.  The items() function
 * returns a list of all the items contained within the widget in the order they were added.  The
 * items(const QPointF&) const overload returns a list of all the items whose shape intersects with
 * the given location.  The items(const QRectF&) const overload returns a list of all the items
 * whose boundingRect is fully contained within the specified rect.  The itemAt() function is used
 * to determine which item (if any) was clicked on by the user.
 *
 * DrawingWidget maintains selection information for items within the widget. To select items, call
 * selectItems().  To clear the current selection, call clearSelection(). Call selectedItems() to
 * get the list of all currently selected items.
 *
 * pointSizeHint(), pointRect()
 *
 * \section widget_modes Modes
 *
 *
 *
 * \section widget_events Event Handling and Propagation
 *
 * You can interact with the items on the scene by using the mouse and keyboard. QGraphicsView translates the mouse and key events into scene events, (events that inherit QGraphicsSceneEvent,), and forward them to the visualized scene. In the end, it's the individual item that handles the events and reacts to them. For example, if you click on a selectable item, the item will typically let the scene know that it has been selected, and it will also redraw itself to display a selection rectangle. Similiary, if you click and drag the mouse to move a movable item, it's the item that handles the mouse moves and moves itself. Item interaction is enabled by default, and you can toggle it by calling setInteractive().
 *
 * (default mode)
 * Mouse events are delivered to the mouse down item... mouseDownItem()
 * Key events are delivered to the focus item... focusItem()
 *
 * (place mode)
 * Mouse and key events are delivered to the new item... newItem()
 *
 * You can also provide your own custom scene interaction, by creating a subclass of QGraphicsView, and reimplementing the mouse and key event handlers. To simplify how you programmatically interact with items in the view, QGraphicsView provides the mapping functions mapToScene() and mapFromScene(), and the item accessors items() and itemAt(). These functions allow you to map points, rectangles, polygons and paths between view coordinates and scene coordinates, and to find items on the scene using view coordinates.
 */

	QList<DrawingItem*> items() const;
	QList<DrawingItem*> items(const QPointF& scenePos) const;
	QList<DrawingItem*> items(const QRectF& sceneRect) const;
	DrawingItem* itemAt(const QPointF& scenePos) const;

class DrawingWidget : public QAbstractScrollArea
{
	Q_OBJECT

	friend class DrawingReorderItemsCommand;

public:
	enum Mode { DefaultMode, ScrollMode, ZoomMode, PlaceMode, NumberOfModes };
	enum ActionIndex { UndoAction, RedoAction, CutAction, CopyAction, PasteAction, DeleteAction,
		SelectAllAction, SelectNoneAction, RotateAction, RotateBackAction, FlipAction,
		BringForwardAction, SendBackwardAction, BringToFrontAction, SendToBackAction,
		InsertPointAction, RemovePointAction, GroupAction, UngroupAction,
		ZoomInAction, ZoomOutAction, ZoomFitAction, PropertiesAction, NumberOfActions };

private:
	enum MouseState { MouseReady, MouseSelect, MouseMoveItems, MouseResizeItem, MouseRubberBand };

private:
	QRectF mSceneRect;
	qreal mGrid;

	QBrush mBackgroundBrush;
	DrawingGridStyle mGridStyle;
	QBrush mGridBrush;
	int mGridSpacingMajor, mGridSpacingMinor;

	qreal mScale;
	Mode mMode;

	QList<DrawingItem*> mItems;

	// Internal variables
	QTransform mViewportTransform;
	QTransform mSceneTransform;

	QList<DrawingItem*> mSelectedItems;
	DrawingItem* mNewItem;
	DrawingItem* mMouseDownItem;
	DrawingItem* mFocusItem;

	QUndoStack mUndoStack;
	DrawingMouseEvent mMouseEvent;
	MouseState mMouseState;
	QMap<DrawingItem*,QPointF> mInitialPositions;
	QRect mRubberBandRect;
	int mScrollButtonDownHorizontalScrollValue;
	int mScrollButtonDownVerticalScrollValue;
	QPoint mPanStartPos;
	QPoint mPanCurrentPos;
	QTimer mPanTimer;
	int mConsecutivePastes;
	QPointF mSelectionCenter;

	QMenu mSingleItemContextMenu;
	QMenu mSinglePolyItemContextMenu;
	QMenu mMultipleItemContextMenu;
	QMenu mDrawingContextMenu;

public:
	DrawingWidget();
	~DrawingWidget();

	// Selectors
	void setSceneRect(const QRectF& rect);
	void setSceneRect(qreal left, qreal top, qreal width, qreal height);
	QRectF sceneRect() const;
	qreal width() const;
	qreal height() const;

	void setGrid(qreal grid);
	qreal grid() const;
	qreal roundToGrid(qreal value) const;
	QPointF roundToGrid(const QPointF& scenePos) const;

	void setBackgroundBrush(const QBrush& brush);
	QBrush backgroundBrush() const;

	void setGridStyle(DrawingGridStyle style);
	void setGridBrush(const QBrush& brush);
	void setGridSpacing(int majorSpacing, int minorSpacing = 1);
	DrawingGridStyle gridStyle() const;
	QBrush gridBrush() const;
	int gridSpacingMajor() const;
	int gridSpacingMinor() const;

	void setUndoLimit(int undoLimit);
	void pushUndoCommand(QUndoCommand* command);
	int undoLimit() const;
	bool isClean() const;
	bool canUndo() const;
	bool canRedo() const;
	QString undoText() const;
	QString redoText() const;

	qreal scale() const;
	Mode mode() const;

	// Items
	void addItem(DrawingItem* item, bool place = false);
	void insertItem(int index, DrawingItem* item, bool place = false);
	void removeItem(DrawingItem* item);
	void clearItems();
	QList<DrawingItem*> items() const;
	QList<DrawingItem*> items(const QPointF& scenePos) const;
	QList<DrawingItem*> items(const QRectF& sceneRect) const;
	DrawingItem* itemAt(const QPointF& scenePos) const;

	void selectItem(DrawingItem* item);
	void deselectItem(DrawingItem* item);
	void selectItems(const QRectF& sceneRect, Qt::ItemSelectionMode mode);
	void clearSelection();
	QList<DrawingItem*> selectedItems() const;

	void setNewItem(DrawingItem* item);
	DrawingItem* newItem() const;
	DrawingItem* mouseDownItem() const;
	DrawingItem* focusItem() const;

	virtual QSize pointSizeHint() const;
	QRect pointRect(DrawingItemPoint* point) const;

	// View mapping
	QPointF mapToScene(const QPoint& screenPos) const;
	QRectF mapToScene(const QRect& screenRect) const;
	QPoint mapFromScene(const QPointF& scenePos) const;
	QRect mapFromScene(const QRectF& sceneRect) const;
	QRectF visibleRect() const;
	QRectF scrollBarDefinedRect() const;

	void centerOn(const QPointF& scenePos);
	void centerOnCursor(const QPointF& scenePos);
	void fitToView(const QRectF& sceneRect);
	void scaleBy(qreal scale);

	// Rendering
	virtual void render(QPainter* painter);
	virtual void renderExport(QPainter* painter);

public slots:
	void zoomIn();
	void zoomOut();
	void zoomFit();

	void setDefaultMode();
	void setScrollMode();
	void setZoomMode();
	void setPlaceMode(DrawingItem* newItem);

	void undo();
	void redo();
	void setClean();

	void cut();
	void copy();
	void paste();
	void deleteSelection();

	void selectAll();
	void selectNone();

	void moveSelection(const QPointF& newPos);
	void resizeSelection(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	void rotateSelection();
	void rotateBackSelection();
	void flipSelection();
	void updateSelectionProperties(const QMap<QString,QVariant>& properties);

	void bringForward();
	void sendBackward();
	void bringToFront();
	void sendToBack();

	void insertItemPoint();
	void removeItemPoint();

	void group();
	void ungroup();

	void properties();
	void updateProperties(const QRectF& sceneRect, qreal grid, const QBrush& backgroundBrush,
		DrawingGridStyle gridStyle, const QBrush& gridBrush, int gridSpacingMajor, int gridSpacingMinor);

signals:
	void scaleChanged(qreal scale);
	void modeChanged(DrawingWidget::Mode mode);

	void cleanChanged(bool clean);
	void canUndoChanged(bool canUndo);
	void canRedoChanged(bool canRedo);

	void propertiesTriggered();
	void selectionChanged(const QList<DrawingItem*>& items);
	void newItemChanged(DrawingItem* item);
	void itemsGeometryChanged(const QList<DrawingItem*>& items);
	void itemGeometryChanged(DrawingItem* item);
	void numberOfItemsChanged(int itemCount);

	void mouseInfoChanged(const QString& infoText);

	void propertiesChanged(const QRectF& sceneRect, qreal grid, const QBrush& backgroundBrush,
		DrawingGridStyle gridStyle, const QBrush& gridBrush, int gridSpacingMajor, int gridSpacingMinor);

protected:
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
	virtual void wheelEvent(QWheelEvent* event);

	virtual void defaultMousePressEvent(DrawingMouseEvent* event);
	virtual void defaultMouseMoveEvent(DrawingMouseEvent* event);
	virtual void defaultMouseReleaseEvent(DrawingMouseEvent* event);
	virtual void defaultMouseDoubleClickEvent(DrawingMouseEvent* event);

	virtual void placeModeMousePressEvent(DrawingMouseEvent* event);
	virtual void placeModeMouseMoveEvent(DrawingMouseEvent* event);
	virtual void placeModeMouseReleaseEvent(DrawingMouseEvent* event);
	virtual void placeModeMouseDoubleClickEvent(DrawingMouseEvent* event);

	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);

	virtual void paintEvent(QPaintEvent* event);
	virtual void drawBackground(QPainter* painter);
	virtual void drawItems(QPainter* painter);
	virtual void drawForeground(QPainter* painter);

	virtual void resizeEvent(QResizeEvent* event);

protected slots:
	void mousePanEvent();

	void updateSelectionCenter();
	void updateActionsFromSelection();

protected:
	void addItems(const QList<DrawingItem*>& items, bool place, QUndoCommand* command = nullptr);
	void addItems(DrawingItem* item, bool place, QUndoCommand* command = nullptr);
	void removeItems(const QList<DrawingItem*>& items, QUndoCommand* command = nullptr);
	void removeItems(DrawingItem* item, QUndoCommand* command = nullptr);

	void moveItems(const QList<DrawingItem*>& items, const QMap<DrawingItem*,QPointF>& newPos,
		const QMap<DrawingItem*,QPointF>& initialPos, bool place, QUndoCommand* command = nullptr);
	void moveItems(const QList<DrawingItem*>& items, const QMap<DrawingItem*,QPointF>& newPos,
		bool place, QUndoCommand* command = nullptr);
	void moveItems(DrawingItem* item, const QPointF& newPos, bool place, QUndoCommand* command = nullptr);
	void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos,
		bool place, bool disconnect, QUndoCommand* command = nullptr);

	void rotateItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command = nullptr);
	void rotateBackItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command = nullptr);
	void flipItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command = nullptr);

	void placeItems(const QList<DrawingItem*>& items, QUndoCommand* command);
	void placeItems(DrawingItem* item, QUndoCommand* command);
	void unplaceItems(const QList<DrawingItem*>& items, QUndoCommand* command);
	void unplaceItems(DrawingItem* item, QUndoCommand* command);

	void tryToMaintainConnections(const QList<DrawingItem*>& items, bool allowResize,
		bool checkControlPoints, DrawingItemPoint* pointToSkip, QUndoCommand* command);
	void tryToMaintainConnections(DrawingItem* item, bool allowResize,
		bool checkControlPoints, DrawingItemPoint* pointToSkip, QUndoCommand* command);
	void disconnectAll(DrawingItemPoint* itemPoint, QUndoCommand* command);

	void connectItemPoints(DrawingItemPoint* point0, DrawingItemPoint* point1, QUndoCommand* command = nullptr);
	void disconnectItemPoints(DrawingItemPoint* point0, DrawingItemPoint* point1, QUndoCommand* command = nullptr);

	void updateItemProperties(const QList<DrawingItem*>& items, const QMap<QString,QVariant>& properties,
		QUndoCommand* command = nullptr);

protected:
	bool itemMatchesPoint(DrawingItem* item, const QPointF& scenePos) const;
	bool itemMatchesRect(DrawingItem* item, const QRectF& rect, Qt::ItemSelectionMode mode) const;

	bool shouldConnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const;
	bool shouldDisconnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const;

	void recalculateContentSize(const QRectF& targetSceneRect = QRectF());

	void updateMouseInfo(const QPointF& pos);
	void updateMouseInfo(const QPointF& p1, const QPointF& p2);

	void addActions();
	void createContextMenu();
	QAction* addAction(const QString& text, QObject* slotObj, const char* slotFunction,
		const QString& iconPath = QString(), const QString& shortcut = QString());
};

#endif
