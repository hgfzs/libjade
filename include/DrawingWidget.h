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
 * \section widget_viewport Viewport
 *
 * DrawingWidget visualizes a scene in a scrollable viewport.  The scrollable area of the scene is
 * determined by the sceneRect().
 *
 * Any position on the scene can be scrolled to by using the scroll bars or by calling centerOn().
 * The area that is currently visible can be obtained by calling visibleRect().
 *
 * The user can zoom in and out on the viewport through zoomIn() and zoomOut().  By default, these
 * functions zoom in or out by a factor of sqrt(2).  The user can zoom out to fit the entire scene
 * in the viewport by calling zoomFit().
 *
 * DrawingWidget visualizes the scene by calling render().  First, the background is drawn by
 * calling drawBackground().  Then the items are drawn by calling drawItems().  Items are drawn in
 * the order they were added to the widget, starting with the first item added and ending with the
 * most recent item added.  Finally, the foreground is drawn by calling drawForeground().  Any of
 * these functions can be overridden in a derived class to modify the default rendering behavior.
 *
 * \section widget_events Event Handling
 *
 * DrawingWidget supports several modes of operation.  The current mode() affects how DrawingWidget
 * handles mouse and keyboard events.
 *
 * In #ScrollMode, the user can pan around the scene.  The cursor turns into a hand, which can
 * grab the scene at any point and drag it around the viewport.  The user cannot interact with any
 * items in this mode.  By default, right-clicking or double clicking will bring the widget back
 * to #DefaultMode.
 *
 * In #ZoomMode, the user can zoom in on an area of the scene.  The cursor turns into a crosshairs,
 * which can be used to draw a rect over a portion of the scene.  The viewport will then zoom in to
 * fit the entire rect into the new view.  Again, the user cannot interact with any
 * items in this mode.  By default, right-clicking or double clicking will bring the widget back
 * to #DefaultMode.
 *
 * #DefaultMode is the most common mode for using a DrawingWidget.  This mode allows full
 * interaction with the items in the scene.  Users can click on items to select them or draw a
 * rubber band over an area to select all items in that area.  Selected items can be moved around
 * the scene.  A single selected item can be resized if the user clicks on a Control
 * DrawingItemPoint.  Clicking on empty space will clear the current selection.
 *
 * On a mouse press event, DrawingWidget updates the current mouseDownItem() to whatever item was
 * clicked.  At this time, DrawingWidget also updates the current focusItem() to match the new
 * mouseDownItem().  After a mouse release event, DrawingWidget sets the current mouseDownItem() to
 * nullptr since the mouse event is over.  Mouse events are forwarded to the mouseDownItem(), while
 * keyboard events are forwarded to the focusItem().
 *
 * #PlaceMode is used to add new items to the widget.  By default, mouse move events move the
 * new item around the scene.  The new item is added to the scene on a
 * mouse release event.  Copies of the new item can continue to be placed until the user
 * right-clicks or until the user enters a different mode.
 *
 * The default event handlers mousePressEvent(), mouseMoveEvent(), mouseReleaseEvent(),
 * mouseDoubleClickEvent(), wheelEvent(), keyPressEvent(), and keyReleaseEvent() have an intelligent
 * base implementation of the behavior described here.  These can be overloaded in a derived class
 * to modify their behavior as needed for the application.
 *
 * \section widget_undo Undo Support
 *
 * DrawingWidget comes with full undo support.  The widget has an internal undo stack that is
 * managed automatically.
 *
 * The following operations support undo through the specified function and associated undo
 * command classes:
 *
 * \li Add a set of items to the widget: addItems(), DrawingAddItemsCommand
 * \li Flip an item horizontally: flipItems(), DrawingFlipItemsCommand
 * \li Insert an item point into an item: insertItemPoint(), DrawingItemInsertPointCommand
 * \li Create a connection between two items: connectItemPoints(), DrawingItemPointConnectCommand
 * \li Break a connection between two items: disconnectItemPoints(), DrawingItemPointDisconnectCommand
 * \li Remove an item point from an item: removeItemPoint(), DrawingItemRemovePointCommand
 * \li Move a set of items within the scene: moveItems(), DrawingMoveItemsCommand
 * \li Remove a set of items from the widget: removeItems(), DrawingRemoveItemsCommand
 * \li Bring items forward in the scene: bringForward(), DrawingReorderItemsCommand
 * \li Send items backward in the scene: sendBackward(), DrawingReorderItemsCommand
 * \li Bring items to the front of the scene: bringToFront(), DrawingReorderItemsCommand
 * \li Send items the the back of the scene: sendToBack(), DrawingReorderItemsCommand
 * \li Resize an item in the scene: resizeItem(), DrawingResizeItemCommand
 * \li Rotate an item backward: rotateBackItems(), DrawingRotateBackItemsCommand
 * \li Rotate an item forward: rotateItems(), DrawingRotateItemsCommand
 *
 * Call the setClean() function to mark the undo stack as clean (i.e. if a document is saved).  Call
 * isClean() to determine the current clean status.
 */
class DrawingWidget : public QAbstractScrollArea
{
	Q_OBJECT

	friend class DrawingReorderItemsCommand;

public:
	/*! \brief Enum used to set the current mode of the DrawingWidget.  See the \ref widget_events
	 * section above for more information.
	 */
	enum Mode
	{
		DefaultMode,	//!< The normal mode for interacting with items in a DrawingWidget.
		ScrollMode,		//!< Mode for the user to pan around the scene.  No interaction with items.
		ZoomMode,		//!< Mode for the user to zoom in on an area of the scene.  No interaction
						//!< with items.
		PlaceMode		//!< Mode for placing new items into a DrawingWidget.
	};

private:
	enum MouseState { MouseReady, MouseSelect, MouseMoveItems, MouseResizeItem, MouseRubberBand };

private:
	// future: can place multiple new items at once
	// future: selecting/unselecting items are undoable events, item has CanSelect flag
	// add support for item styles
	// items must accept mouseReleaseEvent or mouseDoubleClickEvent to place item in scene
	QRectF mSceneRect;
	qreal mGrid;
	QBrush mBackgroundBrush;

	QList<DrawingItem*> mItems;
	QList<DrawingItem*> mSelectedItems;
	DrawingItemPoint* mSelectedItemPoint;
	DrawingItem* mNewItem;
	DrawingItem* mMouseDownItem;
	DrawingItem* mFocusItem;

	Mode mMode;
	Qt::ItemSelectionMode mItemSelectionMode;

	qreal mScale;

	// Internal variables
	QTransform mViewportTransform;
	QTransform mSceneTransform;

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

	void setUndoLimit(int undoLimit);
	void pushUndoCommand(QUndoCommand* command);
	int undoLimit() const;
	bool isClean() const;
	bool canUndo() const;
	bool canRedo() const;
	QString undoText() const;
	QString redoText() const;

	void setItemSelectionMode(Qt::ItemSelectionMode mode);
	Qt::ItemSelectionMode itemSelectionMode() const;

	Mode mode() const;
	qreal scale() const;

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

	DrawingItem* newItem() const;
	DrawingItem* mouseDownItem() const;
	DrawingItem* focusItem() const;

	// Item points
	virtual QSize pointSizeHint() const;
	QRect pointRect(DrawingItemPoint* point) const;

	// View mapping
	void centerOn(const QPointF& scenePos);
	void centerOnCursor(const QPointF& scenePos);
	void fitToView(const QRectF& sceneRect);
	void scaleBy(qreal scale);

	QPointF mapToScene(const QPoint& screenPos) const;
	QRectF mapToScene(const QRect& screenRect) const;
	QPoint mapFromScene(const QPointF& scenePos) const;
	QRect mapFromScene(const QRectF& sceneRect) const;
	QRectF visibleRect() const;
	QRectF scrollBarDefinedRect() const;

	// Rendering
	virtual void render(QPainter* painter);

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

	void bringForward();
	void sendBackward();
	void bringToFront();
	void sendToBack();

	void insertItemPoint();
	void removeItemPoint();

	void group();
	void ungroup();

signals:
	void scaleChanged(qreal scale);
	void modeChanged(DrawingWidget::Mode mode);

	void cleanChanged(bool clean);
	void canUndoChanged(bool canUndo);
	void canRedoChanged(bool canRedo);

	void numberOfItemsChanged(int itemCount);
	void itemsGeometryChanged(const QList<DrawingItem*>& items);
	void itemGeometryChanged(DrawingItem* item);
	void selectionChanged(const QList<DrawingItem*>& items);
	void newItemChanged(DrawingItem* item);

protected:
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
	virtual void wheelEvent(QWheelEvent* event);

	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);

	virtual void paintEvent(QPaintEvent* event);
	virtual void drawBackground(QPainter* painter);
	virtual void drawItems(QPainter* painter);
	virtual void drawForeground(QPainter* painter);

	virtual void resizeEvent(QResizeEvent* event);

private:
	void defaultMousePressEvent(DrawingMouseEvent* event);
	void defaultMouseMoveEvent(DrawingMouseEvent* event);
	void defaultMouseReleaseEvent(DrawingMouseEvent* event);
	void defaultMouseDoubleClickEvent(DrawingMouseEvent* event);

	void placeModeMousePressEvent(DrawingMouseEvent* event);
	void placeModeMouseMoveEvent(DrawingMouseEvent* event);
	void placeModeMouseReleaseEvent(DrawingMouseEvent* event);
	void placeModeMouseDoubleClickEvent(DrawingMouseEvent* event);

private slots:
	void updateSelectionCenter();
	void mousePanEvent();

private:
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

	bool itemMatchesPoint(DrawingItem* item, const QPointF& scenePos) const;
	bool itemMatchesRect(DrawingItem* item, const QRectF& rect, Qt::ItemSelectionMode mode) const;

	bool shouldConnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const;
	bool shouldDisconnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const;

	void recalculateContentSize(const QRectF& targetSceneRect = QRectF());
};

#endif
