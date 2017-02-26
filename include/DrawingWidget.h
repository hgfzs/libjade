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

#include <DrawingTypes.h>

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
 * selectItem().  To clear the current selection, call clearSelection(). Call selectedItems() to
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
 * new items around the scene.  The new items are added to the scene on a
 * mouse release event.  Copies of the new items can continue to be placed until the user
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
 * The following operations are undoable:
 * \li Adding items to the widget using #PlaceMode
 * \li Deleting items using deleteSelection()
 * \li Moving items around the scene using mouse events or moveSelection()
 * \li Resizing items within the scene using mouse events or resizeSelection()
 * \li Rotating or flipping items in the scene using rotateSelection(), rotateBackSelection() and
 *     flipSelection().
 * \li Cut/copy/paste of items using cut(), copy(), and paste()
 * \li Reordering the items in the scene using bringForward(), sendBackward(), bringToFront(), and
 *     sendToBack()
 * \li Inserting and removing item points using insertItemPoint() and removeItemPoint()
 * \li Grouping and ungrouping items using group() and ungroup()
 *
 * If the #UndoableSelectCommands flag is set, then the following additional operations are undoable:
 * \li Selecting items using mouse events or selectAll(), selectArea(), or selectNone()
 *
 * Call the setClean() function to mark the undo stack as clean (i.e. if a document is saved).  Call
 * isClean() to determine the current clean status.
 *
 * Custom undo events may be pushed on to the internal undo stack by calling pushUndoCommand().
 */
class DrawingWidget : public QAbstractScrollArea
{
	Q_OBJECT

	friend class DrawingAddItemsCommand;
	friend class DrawingRemoveItemsCommand;
	friend class DrawingMoveItemsCommand;
	friend class DrawingResizeItemCommand;
	friend class DrawingRotateItemsCommand;
	friend class DrawingRotateBackItemsCommand;
	friend class DrawingFlipItemsCommand;
	friend class DrawingReorderItemsCommand;
	friend class DrawingSelectItemsCommand;
	friend class DrawingItemInsertPointCommand;
	friend class DrawingItemRemovePointCommand;
	friend class DrawingItemPointConnectCommand;
	friend class DrawingItemPointDisconnectCommand;

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

	/*! \brief Enum used to modify the default behavior of DrawingWidget.
	 */
	enum Flag
	{
		UndoableSelectCommands = 0x0001,	//!< Selecting and deselecting items are commands that
											//!< the user can undo() and redo().
		SendsMouseMoveInfo = 0x0002			//!< Emits the mouseInfoChanged() signal when the mouse
											//!< is moved within the scene.
	};
	Q_DECLARE_FLAGS(Flags, Flag)

private:
	enum MouseState { MouseReady, MouseSelect, MouseMoveItems, MouseResizeItem, MouseRubberBand };

private:
	QRectF mSceneRect;
	qreal mGrid;
	QBrush mBackgroundBrush;

	QList<DrawingItem*> mItems;
	QList<DrawingItem*> mSelectedItems;
	DrawingItemPoint* mSelectedItemPoint;
	DrawingItem* mNewItem;
	DrawingItem* mMouseDownItem;
	DrawingItem* mFocusItem;

	Flags mFlags;
	Qt::ItemSelectionMode mItemSelectionMode;

	Mode mMode;
	qreal mScale;

	// Internal variables
	QTransform mViewportTransform;
	QTransform mSceneTransform;

	QUndoStack mUndoStack;
	QList<DrawingItem*> mClipboardItems;
	DrawingMouseEvent mMouseEvent;
	MouseState mMouseState;
	QHash<DrawingItem*,QPointF> mInitialPositions;
	QPointF mSelectedItemPointOriginalPos;
	QRect mRubberBandRect;
	int mScrollButtonDownHorizontalScrollValue;
	int mScrollButtonDownVerticalScrollValue;
	QPoint mPanStartPos;
	QPoint mPanCurrentPos;
	QTimer mPanTimer;
	int mConsecutivePastes;
	QPointF mSelectionCenter;

public:
	/*! \brief Create a new DrawingWidget with default settings.
	 *
	 * The new widget does not contain any items.  Items can be added by calling addItem() or
	 * insertItem().
	 */
	DrawingWidget();

	/*! \brief Delete an existing DrawingWidget object.
	 *
	 * Also deletes all items contained in the widget.
	 */
	~DrawingWidget();


	/*! \brief Sets the bounding rectangle of the scene.
	 *
	 * The scene rectangle defines the extent of the scene.  It is used by DrawingWidget to
	 * determine the scrollable area of the viewport.
	 *
	 * The scene rectangle must be set to a valid QRectF.  The behavior of DrawingWidget is
	 * undefined if its sceneRect is invalid.
	 *
	 * The default scene rectangle is set to QRectF(-5000, -3750, 10000, 7500).
	 *
	 * \sa sceneRect()
	 */
	void setSceneRect(const QRectF& rect);

	/*! \brief Sets the bounding rectangle of the scene.
	 *
	 * This convenience function is equivalent to calling setSceneRect(QRectF(left, top, width, height)).
	 *
	 * \sa sceneRect(), width(), height()
	 */
	void setSceneRect(qreal left, qreal top, qreal width, qreal height);

	/*! \brief Returns the widget's bounding rectangle.
	 *
	 * \sa setSceneRect(), width(), height()
	 */
	QRectF sceneRect() const;

	/*! \brief Returns the width of the widget's bounding rectangle.
	 *
	 * \sa setSceneRect(), sceneRect(), height()
	 */
	qreal width() const;

	/*! \brief Returns the height of the widget's bounding rectangle.
	 *
	 * \sa setSceneRect(), sceneRect(), width()
	 */
	qreal height() const;


	/*! \brief Sets the widget's grid.
	 *
	 * Snap to grid is enabled when the grid is set to a value greater than 0.  With snap to grid,
	 * DrawingWidget will force items to be aligned on a grid when moved around the scene or
	 * resized.
	 *
	 * Set the grid to 0 (or a negative number) to disable snap to grid.
	 *
	 * The default grid is set to 50.
	 *
	 * \sa grid(), roundToGrid()
	 */
	void setGrid(qreal grid);

	/*! \brief Returns the widget's grid.
	 *
	 * \sa setGrid(), roundToGrid()
	 */
	qreal grid() const;

	/*! \brief Rounds to specified value to be aligned on the grid and returns the rounded value.
	 *
	 * If the grid is 0 or a negative number, this function simply returns the original value.
	 *
	 * \sa grid()
	 */
	qreal roundToGrid(qreal value) const;

	/*! \brief Rounds to specified point to be aligned on the grid and returns the rounded value.
	 *
	 * If the grid is 0 or a negative number, this function simply returns the original point.
	 *
	 * \sa grid()
	 */
	QPointF roundToGrid(const QPointF& scenePos) const;


	/*! \brief Sets the widget's background brush.
	 *
	 * The background brush is used to fill the background of the widget by the default
	 * implementation of drawBackground().  It is also used to determine the outline color when
	 * rendering DrawingItemPoint objects.
	 *
	 * The default background brush is set to white.
	 *
	 * \sa backgroundBrush()
	 */
	void setBackgroundBrush(const QBrush& brush);

	/*! \brief Returns the widget's background brush.
	 *
	 * \sa setBackgroundBrush()
	 */
	QBrush backgroundBrush() const;


	/*! \brief Set the maximum depth of the internal undo stack of the widget.
	 *
	 * When the number of commands on the stack exceeds the undo limit, commands are deleted from
	 * the bottom of the stack.
	 *
	 * This property may only be set when the undo stack is empty, since setting it on a non-empty
	 * stack might delete the command at the current index. Calling setUndoLimit() on a non-empty
	 * stack does nothing.
	 *
	 * The default undo limit is set to 64.
	 *
	 * \sa undoLimit(), pushUndoCommand()
	 */
	void setUndoLimit(int undoLimit);

	/*! \brief Pushes the specified command onto the widget's internal undo stack.
	 *
	 * This function either adds the command to the stack or merges it with the most recently
	 * executed command. In either case, executes the command by calling its redo function.
	 *
	 * If commands were undone before the command was pushed, the current command and all commands
	 * above it are deleted. Hence command always ends up being the top-most on the stack.
	 *
	 * Once a command is pushed, the stack takes ownership of it. There are no getters to return
	 * the command, since modifying it after it has been executed will almost always lead to
	 * corruption of the document's state.
	 */
	void pushUndoCommand(QUndoCommand* command);

	/*! \brief Returns the maximum number of undo commands that can be stored in the widget.
	 *
	 * \sa setUndoLimit()
	 */
	int undoLimit() const;

	/*! \brief Returns true if the widget's internal undo stack is in a clean state, false
	 * otherwise.
	 *
	 * \sa setClean()
	 */
	bool isClean() const;

	/*! \brief Returns true if there is a command available for undo; otherwise returns false.
	 *
	 * \sa undoText(), canRedo()
	 */
	bool canUndo() const;

	/*! \brief Returns true if there is a command available for redo; otherwise returns false.
	 *
	 * \sa redoText(), canUndo()
	 */
	bool canRedo() const;

	/*! \brief Returns the text of the command which will be redone in the next call to undo().
	 *
	 * \sa canUndo(), redoText()
	 */
	QString undoText() const;

	/*! \brief Returns the text of the command which will be redone in the next call to redo().
	 *
	 * \sa canRedo(), undoText()
	 */
	QString redoText() const;


	/*! \brief Modifies the default behavior of DrawingWidget through a combination of flags.
	 *
	 * The default flags are set to 0.  Applications can set any combination of flags to set the
	 * desired behavior of DrawingWidget.
	 *
	 * \sa flags()
	 */
	void setFlags(Flags flags);

	/*! \brief Sets the widget's item selection mode.
	 *
	 * The item selection mode affects how items are selected with a rubber band select box:
	 * \li Qt::ContainsItemBoundingRect - only items whose bounding rectangle is fully contained
	 * inside the selection area are selected
	 * \li Qt::ContainsItemShape - only items whose shape is fully contained inside the
	 * selection area are selected
	 * \li Qt::IntersectsItemBoundingRect - all items whose bounding rectangle intersects with the
	 * selection area are selected
	 * \li Qt::IntersectsItemShape - all items whose shape intersects with the selection area are
	 * selected
	 *
	 * This also affects which items are returned by items(const QRectF&) const.
	 *
	 * The default mode is set to Qt::ContainsItemBoundingRect.
	 *
	 * \sa itemSelectionMode()
	 */
	void setItemSelectionMode(Qt::ItemSelectionMode mode);

	/*! \brief Returns any modifications to DrawingWidget's default behavior represented as a
	 * combination of flags.
	 *
	 * \sa setFlags()
	 */
	Flags flags() const;

	/*! \brief Returns the widget's item selection mode.
	 *
	 * \sa setItemSelectionMode()
	 */
	Qt::ItemSelectionMode itemSelectionMode() const;


	/*! \brief Returns the widget's current operating mode.
	 *
	 * The default operating mode is #DefaultMode.
	 *
	 * \sa setDefaultMode(), setScrollMode(), setZoomMode(), setPlaceMode()
	 */
	Mode mode() const;

	/*! \brief Returns the current scale between the widget viewport and the scene.
	 *
	 * The default scale is set to 1.0 (no scaling between viewport and scene).
	 *
	 * \sa zoomIn(), zoomOut(), zoomFit(), scaleBy(), fitToView()
	 */
	qreal scale() const;


	/*! \brief Adds an existing item to the widget.
	 *
	 * This convenience function is equivalent to calling #insertItem(items().size(), item).
	 *
	 * \sa removeItem()
	 */
	void addItem(DrawingItem* item);

	/*! \brief Inserts an existing item to the widget at the specified index.
	 *
	 * If a valid item is passed to this function, DrawingWidget will insert it into its list of
	 * items() at the specified index.  DrawingItem takes ownership of the item and will
	 * delete it as necessary.
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * does nothing.  This function also does nothing if the item is already one of the widget's
	 * items().
	 *
	 * \sa addItem(), removeItem()
	 */
	void insertItem(int index, DrawingItem* item);

	/*! \brief Removes an existing item from the widget.
	 *
	 * If a valid item is passed to this function, DrawingWidget will remove it from its list of
	 * items().  DrawingWidget relinquishes ownership of the item and does not delete the
	 * item from memory.
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * does nothing.  This function also does nothing if the item is not one of the widget's
	 * items().
	 *
	 * \sa addPoint(), insertPoint(), clearPoints()
	 */
	void removeItem(DrawingItem* item);

	/*! \brief Removes and deletes all items from the widget.
	 *
	 * This function removes and deletes all of the widgets's items() from memory.
	 *
	 * \sa removeItem()
	 */
	void clearItems();

	/*! \brief Returns a list of all items added to the widget.
	 *
	 * \sa addItem(), insertItem(), removeItem()
	 */
	QList<DrawingItem*> items() const;

	/*! \brief Returns a list of all visible items added to the widget that are at the specified
	 * position.
	 *
	 * This function uses DrawingItem::shape() to determine the exact shape of each item to test
	 * against the specified position.
	 *
	 * \sa items(const QRectF&) const, itemAt()
	 */
	QList<DrawingItem*> items(const QPointF& scenePos) const;

	/*! \brief Returns a list of all visible items added to the widget that are inside the
	 * specified rectangle.
	 *
	 * This function uses the current itemSelectionMode() to affect how it matches items to the rect:
	 * \li Qt::ContainsItemBoundingRect - only items whose bounding rectangle is fully contained
	 * inside the specified rect are included in the list
	 * \li Qt::ContainsItemShape - only items whose shape is fully contained inside the
	 * specified rect are included in the list
	 * \li Qt::IntersectsItemBoundingRect - all items whose bounding rectangle intersects with the
	 * specified rect are included in the list
	 * \li Qt::IntersectsItemShape - all items whose shape intersects with the specified rect are
	 * included in the list
	 *
	 * \sa items(const QPointF&) const
	 */
	QList<DrawingItem*> items(const QRectF& sceneRect) const;

	/*! \brief Returns the topmost visible item at the specified position, or nulltr if there are
	 * no items at this position.
	 *
	 * This function favors already selected items in its search.  First it searches through all
	 * of the selected items for a match.  If none of the selected items is at the specified
	 * position, then this function searches through all of the widget items.
	 *
	 * This function uses DrawingItem::shape() to determine the exact shape of each item to test
	 * against the specified position.  It returns immediately once it finds the first item that
	 * matches the specified position.
	 *
	 * \sa items(const QPointF&) const
	 */
	DrawingItem* itemAt(const QPointF& scenePos) const;


	/*! \brief Adds the specified item to the selection.
	 *
	 * If a valid item is passed to this function, DrawingWidget will set it as selected and
	 * append it into its list of selectedItems() at the specified index.
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * does nothing.  This function also does nothing if the item is already one of the widget's
	 * selectedItems().
	 *
	 * \sa selectItems(), deselectItem()
	 */
	void selectItem(DrawingItem* item);

	/*! \brief Removes the specified item from the selection.
	 *
	 * If a valid item is passed to this function, DrawingWidget will set it as unselected and
	 * remove it from its list of selectedItems().
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * does nothing.  This function also does nothing if the item is not one of the widget's
	 * selectedItems().
	 *
	 * \sa selectItem(), clearSelection()
	 */
	void deselectItem(DrawingItem* item);

	/*! \brief Removes all items from the selection.
	 *
	 * This function calls deselectItem() on each item in the current selection.  After running this
	 * function, no items will be selected.
	 */
	void clearSelection();

	/*! \brief Returns a list of all currently selected items in the widget.
	 *
	 * \sa addItem(), insertItem(), removeItem()
	 */
	QList<DrawingItem*> selectedItems() const;


	/*! \brief Returns the widget's new item, or nullptr if no new item is set.
	 *
	 * The new item is set when entering #PlaceMode.  It is used to place new items within the
	 * widget's scene.
	 *
	 * \sa setPlaceMode()
	 */
	DrawingItem* newItem() const;

	/*! \brief Returns the widget's mouse down item, or nullpty if no mouse down item is set.
	 *
	 * The mouse down item is the item that receives all mouse events sent to the scene.
	 *
	 * An item becomes a mouse down when it receives a mouse press event, and it stays
	 * the mouse down until it receives a mouse release event.
	 */
	DrawingItem* mouseDownItem() const;

	/*! \brief Returns the widget's focus item, or nullpty if no focus item is set.
	 *
	 * The focus item is the item that receives all keyboard events sent to the scene.
	 *
	 * An item becomes a focus item when it receives a mouse press event, and it stays
	 * the mouse down until the next mouse press event.
	 */
	DrawingItem* focusItem() const;


	/*! \brief Returns the size to use when rendering DrawingItemPoint objects in the scene.
	 *
	 * The default width and height are both set to 8 * devicePixelRatio().
	 *
	 * \sa pointRect()
	 */
	virtual QSize pointSizeHint() const;

	/*! \brief Returns the screen rect used to render a DrawingItemPoint in the scene.
	 *
	 * The rect will be centered on the item point's position and have size as set by the
	 * pointSizeHint().
	 */
	QRect pointRect(DrawingItemPoint* point) const;


	/*! \brief Scrolls the contents of the viewport to ensure that specified scenePos is centered
	 * in the view.
	 *
	 * Because scenePos is a floating point coordinate and the scroll bars operate on integer
	 * coordinates, the centering is only an approximation.
	 *
	 * Note: If the item is close to or outside the border, it will be visible in the view, but not
	 * centered.
	 *
	 * \sa centerOnCursor(), fitToView()
	 */
	void centerOn(const QPointF& scenePos);

	/*! \brief Scrolls the contents of the viewport to ensure that specified scenePos is centered
	 * under the cursor.
	 *
	 * Because scenePos is a floating point coordinate and the scroll bars operate on integer
	 * coordinates, the centering is only an approximation.
	 *
	 * Note: If the item is close to or outside the border, it will be visible in the view, but not
	 * centered under the cursor.
	 *
	 * \sa centerOnCursor(), fitToView()
	 */
	void centerOnCursor(const QPointF& scenePos);

	/*! \brief Scales the view and scrolls the scroll bars to ensure that the sceneRect fits inside
	 * the viewport.
	 *
	 * The specified rect must be inside the sceneRect(), otherwise fitInView() cannot guarantee
	 * that the whole rect is visible.
	 *
	 * \sa zoomFit()
	 */
	void fitToView(const QRectF& sceneRect);

	/*! \brief Scales the view by the specified scaling factor.
	 *
	 * The scaling factor must be greater than zero; if not, then this function does nothing.
	 *
	 * \sa zoomFit()
	 */
	void scaleBy(qreal scale);


	/*! \brief Maps the point from the coordinate system of the viewport to the scene's
	 * coordinate system.
	 *
	 * \sa mapFromScene(const QPointF&) const
	 */
	QPointF mapToScene(const QPoint& screenPos) const;

	/*! \brief Maps the rect from the coordinate system of the viewport to the scene's
	 * coordinate system.
	 *
	 * \sa mapFromScene(const QRectF&) const
	 */
	QRectF mapToScene(const QRect& screenRect) const;

	/*! \brief Maps the point from the scene's coordinate system to the coordinate system of the
	 * viewport.
	 *
	 * \sa mapToScene(const QPoint&) const
	 */
	QPoint mapFromScene(const QPointF& scenePos) const;

	/*! \brief Maps the rect from the scene's coordinate system to the coordinate system of the
	 * viewport.
	 *
	 * \sa mapToScene(const QRect&) const
	 */
	QRect mapFromScene(const QRectF& sceneRect) const;

	/*! \brief Returns a rect representing the currently visible area of the scene.
	 *
	 * \sa scrollBarDefinedRect()
	 */
	QRectF visibleRect() const;

	/*! \brief Returns a rect representing the scrollable area of the scene.
	 *
	 * Normally this function returns the widget's sceneRect().  However, if the viewport is zoomed
	 * out enough that more of the scene can be seen than the sceneRect(), this function will
	 * return a rect representing the entire area.
	 *
	 * \sa visibleRect()
	 */
	QRectF scrollBarDefinedRect() const;


	/*! \brief Renders the scene using the specified painter.
	 *
	 * This function simply calls drawBackground(), drawItems(), and drawForeground() in succession
	 * using the specified painter.
	 */
	virtual void render(QPainter* painter);

public slots:
	/*! \brief Zooms in on the scene.
	 *
	 * Zooms in on the scene by scaling the view by a factor of sqrt(2) and sends the
	 * scaleChanged() signal to indicate the new scale factor.
	 *
	 * \sa zoomOut(), zoomFit(), scaleBy()
	 */
	void zoomIn();

	/*! \brief Zooms out from the scene.
	 *
	 * Zooms out on the scene by scaling the view by a factor of sqrt(2)/2 and sends the
	 * scaleChanged() signal to indicate the new scale factor.
	 *
	 * \sa zoomOut(), zoomFit(), scaleBy()
	 */
	void zoomOut();

	/*! \brief Zooms to fit the entire sceneRect() within the view.
	 *
	 * Scales the view and scrolls the scroll bars to ensure that the sceneRect() fits inside
	 * the viewport, then sends the scaleChanged() signal to indicate the new scale factor.
	 *
	 * \sa zoomOut(), zoomFit(), scaleBy()
	 */
	void zoomFit();


	/*! \brief Sets the current operating mode to #DefaultMode.
	 *
	 * #DefaultMode is used for normal interaction with items in the scene.  Users can click on
	 * items to select them or draw a rubber band over an area to select all items in that area.
	 * Selected items can be moved around the scene.  A single selected item can be resized if the
	 * user clicks on a Control DrawingItemPoint.  Clicking on empty space will clear the current
	 * selection.
	 *
	 * This slot emits the modeChanged() signal to indicate that the mode has changed.
	 *
	 * \sa setScrollMode(), setZoomMode(), setPlaceMode()
	 */
	void setDefaultMode();

	/*! \brief Sets the current operating mode to #ScrollMode.
	 *
	 * #ScrollMode is used for panning around the scene.  The cursor turns into a hand, which can
	 * grab the scene at any point and drag it around the viewport.  Right-clicking or
	 * double-clicking will set the mode back to #DefaultMode.
	 *
	 * This slot emits the modeChanged() signal to indicate that the mode has changed.
	 *
	 * \sa setDefaultMode(), setZoomMode(), setPlaceMode()
	 */
	void setScrollMode();

	/*! \brief Sets the current operating mode to #ZoomMode.
	 *
	 * #ZoomMode is used for selecting an area of the scene to zoom in on.  The cursor turns into a
	 * crosshairs, which can be used to draw a rect over a portion of the scene.  The
	 * viewport will then zoom in to fit the entire rect into the new view.  Right-clicking or
	 * double-clicking will set the mode back to #DefaultMode.
	 *
	 * This slot emits the modeChanged() signal to indicate that the mode has changed.
	 *
	 * \sa setDefaultMode(), setScrollMode(), setPlaceMode()
	 */
	void setZoomMode();

	/*! \brief Sets the current operating mode to #ScrollMode.
	 *
	 * #PlaceMode is used to add new items to the widget.  By default, mouse move events move the
	 * new item around the scene.  The new item is added to the scene on a
	 * mouse release event.  Copies of the new item can continue to be placed until the user
	 * right-clicks or until the user enters a different mode.
	 *
	 * DrawingWidget is expecting to receive a pointer to a DrawingItem that is not already
	 * one of the widget's items().  DrawingWidget takes ownership of the item and will delete
	 * it upon exiting #PlaceMode.
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * sets the operating mode to #DefaultMode.
	 *
	 * This slot emits the modeChanged() signal to indicate that the mode has changed.
	 *
	 * \sa setDefaultMode(), setScrollMode(), setZoomMode()
	 */
	void setPlaceMode(DrawingItem* newItem);


	/*! \brief Undoes the previous command.
	 *
	 * The undo operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * This function will send the undoEvent() signal if there was a command to undo.
	 *
	 * If the #UndoableSelectCommands flag is not set, this function will clear any selected items
	 * before performing the undo operation.
	 *
	 * \sa redo(), setClean(), isClean()
	 */
	void undo();

	/*! \brief Redoes the previous command.
	 *
	 * The redo operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * This function will send the redoEvent() signal if there was a command to redo.
	 *
	 * If the #UndoableSelectCommands flag is not set, this function will clear any selected items
	 * before performing the redo operation.
	 *
	 * \sa undo(), setClean(), isClean()
	 */
	void redo();

	/*! \brief Marks the widget's internal undo stack as clean.
	 *
	 * This is typically called when a document is saved, for example.
	 *
	 * This function also emits cleanChanged() if the stack was not already clean.
	 *
	 * Whenever the stack returns to this state through the use of undo/redo commands, it emits the
	 * signal cleanChanged(). This signal is also emitted when the stack leaves the clean state.
	 *
	 * \sa isClean()
	 */
	void setClean();


	/*! \brief Copies the selected items to an internal clipboard, then deletes them from the scene.
	 *
	 * This function does not use the standard clipboard (i.e. QApplication::clipboard()).  Instead,
	 * DrawingWidget maintains its own internal list of items.  This means that items cannot be
	 * copied between two different DrawingWidget objects.
	 *
	 * The cut operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa copy(), paste(), deleteSelection()
	 */
	void cut();

	/*! \brief Copies the selected items to an internal clipboard.
	 *
	 * This function does not use the standard clipboard (i.e. QApplication::clipboard()).  Instead,
	 * DrawingWidget maintains its own internal list of items.  This means that items cannot be
	 * copied between two different DrawingWidget objects.
	 *
	 * The cut operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa cut(), paste(), deleteSelection()
	 */
	void copy();

	/*! \brief Pastes copies of any items on the clipboard into the scene.
	 *
	 * This function emits the numberOfItemsChanged() signal to indicate that items were added
	 * to the widget.
	 *
	 * Any existing selection is cleared before the new items are added.  The new items become
	 * the new selection.  This function  emits the selectionChanged() signal to indicate the new
	 * selection has changed.
	 *
	 * This function does not use the standard clipboard (i.e. QApplication::clipboard()).  Instead,
	 * DrawingWidget maintains its own internal list of items.  This means that items cannot be
	 * pasted between two different DrawingWidget objects.
	 *
	 * The cut operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa cut(), copy(), deleteSelection()
	 */
	void paste();

	/*! \brief Deletes the selected items from the scene.
	 *
	 * This function emits the numberOfItemsChanged() signal to indicate that items were removed
	 * from the widget.  This function also emits the selectionChanged() signal because the
	 * selection is cleared when the items are deleted.
	 *
	 * If no items are currently selected, this function does nothing.
	 *
	 * The delete operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function calls setDefaultMode().
	 *
	 * \sa cut(), copy(), paste()
	 */
	void deleteSelection();


	/*! \brief Selects all the items in the scene.
	 *
	 * This function emits the selectionChanged() signal after the items have been selected.
	 *
	 * The select operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa selectArea(const QRectF&), selectArea(const QPainterPath&), selectNone()
	 */
	void selectAll();

	/*! \brief Sets the selection to all visible items added to the widget that are inside the
	 * specified rectangle.
	 *
	 * This function first clears the current selection, then searches for items that match
	 * the specified rect.
	 *
	 * This function uses the widget's itemSelectionMode() to affect how it matches items to the rect:
	 * \li Qt::ContainsItemBoundingRect - only items whose bounding rectangle is fully contained
	 * inside the specified rect are included in the selection
	 * \li Qt::ContainsItemShape - only items whose shape is fully contained inside the
	 * specified rect are included in the selection
	 * \li Qt::IntersectsItemBoundingRect - all items whose bounding rectangle intersects with the
	 * specified rect are included in the selection
	 * \li Qt::IntersectsItemShape - all items whose shape intersects with the specified rect are
	 * included in the selection
	 *
	 * The selectionChanged() signal is emitted after the items have been selected.
	 *
	 * The select operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa selectAll(), selectArea(const QPainterPath&), selectNone()
	 */
	void selectArea(const QRectF& rect);

	/*! \brief Sets the selection to all visible items added to the widget that are inside the
	 * specified path.
	 *
	 * This function first clears the current selection, then searches for items that match
	 * the specified path.
	 *
	 * This function uses the widget's itemSelectionMode() to affect how it matches items to the path:
	 * \li Qt::ContainsItemBoundingRect - only items whose bounding rectangle is fully contained
	 * inside the specified path are included in the selection
	 * \li Qt::ContainsItemShape - only items whose shape is fully contained inside the
	 * specified path are included in the selection
	 * \li Qt::IntersectsItemBoundingRect - all items whose bounding rectangle intersects with the
	 * specified path are included in the selection
	 * \li Qt::IntersectsItemShape - all items whose shape intersects with the specified path are
	 * included in the selection
	 *
	 * The selectionChanged() signal is emitted after the items have been selected.
	 *
	 * The select operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa selectAll(), selectArea(const QRectF&), selectNone()
	 */
	void selectArea(const QPainterPath& path);

	/*! \brief Deselects all the items in the scene.
	 *
	 * This function emits the selectionChanged() signal after the items have been deselected.
	 *
	 * The select operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa selectAll(), selectArea(const QRectF&), selectArea(const QPainterPath&)
	 */
	void selectNone();


	/*! \brief Moves the selected items to the specified position.
	 *
	 * This function emits the itemsGeometryChanged() signal after the items have been moved.
	 *
	 * If multiple items are selected, this function sets the position of the first item to newPos,
	 * then moves the remaining items such that the change in position for all items is the same.
	 *
	 * Only items with the CanMove flag set are moved.  If no items are movable, or if no items 
	 * are selected, this function does nothing.
	 *
	 * The move operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa resizeSelection()
	 */
	void moveSelection(const QPointF& newPos);
	
	/*! \brief Resizes the selected item by moving the itemPoint to the specified position.
	 *
	 * This function emits the itemsGeometryChanged() signal after the item has been resized.
	 *
	 * Exactly one item must be selected; if multiple items or no items are selected, this function
	 * does nothing.  The item must have the CanResize flag set.
	 *
	 * The resize operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa moveSelection()
	 */
	void resizeSelection(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	
	/*! \brief Rotates the selected items 90 degrees counter-clockwise.
	 *
	 * This function emits the itemsGeometryChanged() signal after the items have been rotated.
	 *
	 * Items are rotated about the center of the selection, which is calculated by taking the
	 * average of each selected item's centerPos().
	 *
	 * Only items with the CanRotate flag set are rotated.  If no items can be rotated, or if 
	 * no items are selected, this function does nothing.
	 *
	 * The rotate operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa rotateBackSelection(), flipSelection()
	 */
	void rotateSelection();
	
	/*! \brief Rotates the selected items 90 degrees clockwise.
	 *
	 * This function emits the itemsGeometryChanged() signal after the items have been rotated.
	 *
	 * Items are rotated about the center of the selection, which is calculated by taking the
	 * average of each selected item's centerPos().
	 *
	 * Only items with the CanRotate flag set are rotated.  If no items can be rotated, or if 
	 * no items are selected, this function does nothing.
	 *
	 * The rotate operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa rotateSelection(), flipSelection()
	 */
	void rotateBackSelection();
	
	/*! \brief Flips the selected items horizontally.
	 *
	 * This function emits the itemsGeometryChanged() signal after the items have been flipped.
	 *
	 * Items are flipped over the center of the selection, which is calculated by taking the
	 * average of each selected item's centerPos().
	 *
	 * Only items with the CanFlip flag set are rotated.  If no items can be flipped, or if 
	 * no items are selected, this function does nothing.
	 *
	 * The flip operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa rotateSelection(), rotateBackSelection()
	 */
	void flipSelection();

	
	/*! \brief Brings each selected item forward in the widget's stacking order.
	 *
	 * If no items are selected, this function does nothing.
	 *
	 * The reordering operation is only performed if the mode() is #DefaultMode.  If the mode() is 
	 * any other mode, this function does nothing.
	 *
	 * \sa sendBackward(), bringToFront(), sendToBack()
	 */
	void bringForward();
	
	/*! \brief Sends each selected item backward in the widget's stacking order.
	 *
	 * If no items are selected, this function does nothing.
	 *
	 * The reordering operation is only performed if the mode() is #DefaultMode.  If the mode() is 
	 * any other mode, this function does nothing.
	 *
	 * \sa bringForward(), bringToFront(), sendToBack()
	 */
	void sendBackward();
	
	/*! \brief Brings all of the selected items to the front of the widget's stacking order.
	 *
	 * If no items are selected, this function does nothing.
	 *
	 * The reordering operation is only performed if the mode() is #DefaultMode.  If the mode() is 
	 * any other mode, this function does nothing.
	 *
	 * \sa bringForward(), sendBackward(), sendToBack()
	 */
	void bringToFront();
	
	/*! \brief Sends all of the selected items to the back of the widget's stacking order.
	 *
	 * If no items are selected, this function does nothing.
	 *
	 * The reordering operation is only performed if the mode() is #DefaultMode.  If the mode() is 
	 * any other mode, this function does nothing.
	 *
	 * \sa bringForward(), sendBackward(), bringToFront()
	 */
	void sendToBack();

	
	/*! \brief Inserts a new DrawingItemPoint into an item at the specified position.
	 *
	 * This function emits the itemsGeometryChanged() signal after the new item point has been 
	 * added.
	 *
	 * Exactly one item must be selected; if multiple items or no items are selected, this function
	 * does nothing.  The item must have the CanInsertPoints flag set.
	 *
	 * The insert operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa removeItemPoint()
	 */
	void insertItemPoint();
	
	/*! \brief Removes an existing DrawingItemPoint from an item at the specified position.
	 *
	 * This function emits the itemsGeometryChanged() signal after the new item point has been 
	 * removed.
	 *
	 * Exactly one item must be selected; if multiple items or no items are selected, this function
	 * does nothing.  The item must have the CanRemovePoints flag set.
	 *
	 * The remove operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa insertItemPoint()
	 */
	void removeItemPoint();
	

	/*! \brief Combines all selected items into a new DrawingItemGroup and adds it to the widget.
	 *
	 * This function emits the numberOfItemsChanged() signal after the group operation is complete.
	 *
	 * More than one item must be selected; if one or zero items are selected, this function
	 * does nothing.
	 *
	 * The group operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa removeItemPoint()
	 */
	void group();
	
	/*! \brief Splits the selected DrawingItemGroup into its constituent items and adds them to
	 * the widget.
	 *
	 * This function emits the numberOfItemsChanged() signal after the ungroup operation is 
	 * complete.
	 *
	 * Exactly one item must be selected and it must be a DrawingItemGroup, otherwise this function
	 * does nothing.
	 *
	 * The ungroup operation is only performed if the mode() is #DefaultMode.  If the mode() is any
	 * other mode, this function does nothing.
	 *
	 * \sa removeItemPoint()
	 */
	void ungroup();

signals:
	/*! \brief Emitted whenever the scale of the viewport changes.
	 *
	 * This signal is emitted whenever the scale is changed from the zoomIn(), zoomOut(), and
	 * zoomFit() slots.  It is also emitted in #ZoomMode when the view is changed to zoom in on
	 * a rectangular area of the scene.
	 *
	 * This signal is not emitted when using the fitToView() or scaleBy() functions directly.
	 */
	void scaleChanged(qreal scale);

	/*! \brief Emitted whenever the operating mode of the widget changes.
	 *
	 * This signal is emitted whenever the mode is changed using the setDefaultMode(),
	 * setScrollMode(), setZoomMode(), and setPlaceMode() slots.  It is also emitted in when
	 * exiting other operating modes by right-clicking or double-clicking.
	 */
	void modeChanged(DrawingWidget::Mode mode);

	/*! \brief Emitted whenever the stack enters or leaves the clean state.
	 *
	 * If clean is true, the stack is in a clean state; otherwise this signal indicates that it
	 * has left the clean state.
	 *
	 * \sa setClean(), isClean()
	 */
	void cleanChanged(bool clean);

	/*! \brief Emitted whenever the value of canUndo() changes.
	 *
	 * This signal may be used to enable or disable an undo action associated with this widget.
	 */
	void canUndoChanged(bool canUndo);

	/*! \brief Emitted whenever the value of canRedo() changes.
	 *
	 * This signal may be used to enable or disable a redo action associated with this widget.
	 */
	void canRedoChanged(bool canRedo);

	/*! \brief Emitted whenever the number of items in the widget changes.
	 *
	 * This signal is emitted whenever the user adds items using mouse events in #PlaceMode,
	 * as well as when the user removes items using deleteSelection().  It is also emitted
	 * when performing cut() and paste() operations as well as group() and ungroup() operations.
	 *
	 * This signal is not emitted when using the addItem(), removeItem(), or clearItems() 
	 * functions directly.
	 */
	void numberOfItemsChanged(int itemCount);
	
	/*! \brief Emitted whenever items' geometry changes.
	 *
	 * This signal is emitted whenever the user moves, resizes, rotates, or flips items using 
	 * mouse events in #DefaultMode, as well as when using moveSelection(), resizeSelection(),
	 * rotateSelection(), rotateBackSelection(), and flipSelection().  It is also emitted when
	 * new DrawingItemPoint objects are inserted or removed from items that support it using
	 * insertItemPoint() and removeItemPoint().
	 
	 * This signal is emitted whenever the newItem() is moved, resized, rotated, or flipped using
	 * mouse events in #PlaceMode.
	 *
	 * This signal is not emitted when using functions in DrawingItem to directly manipulate the
	 * position, geometry, or transform of the item.
	 */
	void itemsGeometryChanged(const QList<DrawingItem*>& items);
	
	/*! \brief Emitted whenever the widget's list of selectedItems() changes.
	 *
	 * This signal is emitted whenever the user selects or deselects items using mouse events in 
	 * #DefaultMode, as well as when the user selects items using selectAll(), 
	 * selectArea(const QRectF&), selectArea(const QPainterPath&), and selectNone().  It is also 
	 * emitted when performing cut(), paste(), and deleteSelection() operations as well as 
	 * group() and ungroup() operations.
	 *
	 * This signal is not emitted when using the selectItem(), deselectItem(), or clearSelection()
	 * functions directly.
	 */
	void selectionChanged(const QList<DrawingItem*>& items);
	
	/*! \brief Emitted whenever the widget's newItem() changes.
	 *
	 * This signal is emitted whenever a different newItem() is set on the widget, either through
	 * mouse events in #PlaceMode or using setPlaceMode() directly.  This signal is also emitted
	 * when the newItem() is deleted when switching modes using setScrollMode(), setZoomMode(),
	 * and setDefaultMode(), so the item pointer returned could be nullptr.
	 */
	void newItemChanged(DrawingItem* item);

	/*! \brief Emitted whenever the mouse position changes in the scene.
	 *
	 * This signal is emitted whenever the mouse moves in the scene.  It provides context as to
	 * what the user is currently doing (moving/resizing items, doing a rubber band select, etc).
	 *
	 * This feature must be enabled by setting the #SendsMouseMoveInfo flag.
	 */
	void mouseInfoChanged(const QString& mouseInfo);

protected:
	/*! \brief Handles mouse press events for the widget.
	 *
	 * The default implementation handles mouse press events differently based on the current
	 * mode() of operation.  This event handler can be reimplemented in a derived class to
	 * modify the default behavior.
	 *
	 * In #DefaultMode, the widget updates its mouseDownItem() and focusItem().  The mouse press
	 * event is forwarded to the mouseDownItem() if it is the only selected item.
	 *
	 * In #PlaceMode, the mouse press event is forwarded to the widget's newItem().  Since the
	 * geometry of the newItem() may have changed, the itemsGeometryChanged() signal is sent.
	 *
	 * \sa mouseMoveEvent(), mouseReleaseEvent(), mouseDoubleClickEvent(), wheelEvent()
	 */
	virtual void mousePressEvent(QMouseEvent* event);
	
	/*! \brief Handles mouse move events for the widget.
	 *
	 * The default implementation handles mouse press events differently based on the current
	 * mode() of operation.  This event handler can be reimplemented in a derived class to
	 * modify the default behavior.
	 *
	 * In #DefaultMode, the widget determines whether to move, resize, or select items based on
	 * where the mouse was clicked.
	 *
	 * In #PlaceMode, the mouse move event is forwarded to the widget's newItem().  Since the
	 * geometry of the newItem() may have changed, the itemsGeometryChanged() signal is sent.
	 *
	 * \sa mousePressEvent(), mouseReleaseEvent(), mouseDoubleClickEvent(), wheelEvent()
	 */
	virtual void mouseMoveEvent(QMouseEvent* event);
	
	/*! \brief Handles mouse release events for the widget.
	 *
	 * The default implementation handles mouse press events differently based on the current
	 * mode() of operation.  This event handler can be reimplemented in a derived class to
	 * modify the default behavior.
	 *
	 * In #DefaultMode, the widget finishes moving, resizing, or selecting items based on
	 * where the mouse was clicked and how much it has moved.
	 *
	 * In #PlaceMode, the mouse move event is forwarded to the widget's newItem().  If the
	 * newItem() accepts the event by returning true, then the item is added to the widget and 
	 * placed in the scene.  Then a copy of the item is set as the newItem() and the newItemChanged() 
	 * signal is sent.  The new newItem() is forwarded a newItemCopyEvent().  If true is returned
	 * from this event, the widget continues in #PlaceMode; otherwise, the widget is returned to 
	 * #DefaultMode.
	 *
	 * \sa mousePressEvent(), mouseMoveEvent(), mouseDoubleClickEvent(), wheelEvent()
	 */
	virtual void mouseReleaseEvent(QMouseEvent* event);
	
	/*! \brief Handles mouse double-click events for the widget.
	 *
	 * The default implementation handles mouse double-click events differently based on the current
	 * mode() of operation.  This event handler can be reimplemented in a derived class to
	 * modify the default behavior.
	 *
	 * In #DefaultMode, the widget updates its mouseDownItem() and focusItem().  The mouse 
	 * double-click event is forwarded to the mouseDownItem() if it is the only selected item.
	 *
	 * In #PlaceMode, the mouse double-click event is forwarded to the widget's newItem().  If the
	 * newItem() accepts the event by returning true, then the item is added to the widget and 
	 * placed in the scene.  Then a copy of the item is set as the newItem() and the newItemChanged() 
	 * signal is sent.  The new newItem() is forwarded a newItemCopyEvent().  If true is returned
	 * from this event, the widget continues in #PlaceMode; otherwise, the widget is returned to 
	 * #DefaultMode.
	 *
	 * \sa mouseMoveEvent(), mouseMoveEvent(), mouseReleaseEvent(), wheelEvent()
	 */
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
	
	/*! \brief Handles mouse wheel events for the widget.
	 *
	 * The default implementation zooms in on the scene if the scroll wheel is moved up with
	 * the Control key held down and zooms out on the scene if the scroll wheel is moved down with
	 * the Control key held down.
	 *
	 * \sa mouseMoveEvent(), mouseMoveEvent(), mouseReleaseEvent(), mouseDoubleClickEvent()
	 */
	virtual void wheelEvent(QWheelEvent* event);

	/*! \brief Handles key press events for the widget.
	 *
	 * The default implementation forwards the key press event to the current focusItem().
	 *
	 * \sa keyReleaseEvent()
	 */
	virtual void keyPressEvent(QKeyEvent* event);
	
	/*! \brief Handles key release events for the widget.
	 *
	 * The default implementation forwards the key release event to the current focusItem().
	 *
	 * \sa keyPressEvent()
	 */
	virtual void keyReleaseEvent(QKeyEvent* event);

	
	/*! \brief Handles paint events for the widget.
	 *
	 * The default implementation paints the scene by calling drawBackground(), drawItems(), 
	 * and drawForeground() in order.
	 */
	virtual void paintEvent(QPaintEvent* event);
	
	/*! \brief Renders the background of the scene using the specified painter.
	 *
	 * The default implementation fills the background of the scene using the current 
	 * backgroundBrush().  This function may be overridden in a derived class to provide a 
	 * custom background for the scene.
	 *
	 * This function is called before rendering the widget's items.
	 *
	 * \sa drawItems(), drawForeground()
	 */
	virtual void drawBackground(QPainter* painter);
	
	/*! \brief Renders the widget's items into the scene using the specified painter.
	 *
	 * The default implementation renders items the order they were added to the widget, starting 
	 * with the first item added and ending with the most recent item added.  Any newItem() is
	 * rendered last.
	 *
	 * DrawingWidget handles all rendering of item's points.  The default implementation is to draw
	 * item points only when the associated item is selected.
	 
	 * This function may be overridden in a derived class to provide a custom rendering 
	 * implementation for items in the scene.
	 *
	 * This function is called after rendering the background of the scene and before rendering
	 * the foreground.
	 *
	 * \sa drawBackground(), drawForeground()
	 */
	virtual void drawItems(QPainter* painter);
	
	/*! \brief Renders the foreground of the scene using the specified painter.
	 *
	 * The default implementation does nothing.  This function may be overridden in a derived
	 * class to provide a custom foreground for the scene.
	 *
	 * This function is called after rendering the widget's items.
	 *
	 * \sa drawBackground(), drawItems()
	 */
	virtual void drawForeground(QPainter* painter);

	
	/*! \brief Handles resize events for the widget.
	 *
	 * The default implementation updates the scroll bars to reflect the new size of the viewport.
	 */
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

protected:
	// Functions that generate undo commands
	void addItemsCommand(const QList<DrawingItem*>& items, bool place, QUndoCommand* command = nullptr);
	void removeItemsCommand(const QList<DrawingItem*>& items, QUndoCommand* command = nullptr);
	void moveItemsCommand(const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& newPos,
		bool place, QUndoCommand* command = nullptr);
	void resizeItemCommand(DrawingItemPoint* itemPoint, const QPointF& scenePos,
		bool place, bool disconnect, QUndoCommand* command = nullptr);
	void rotateItemsCommand(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command = nullptr);
	void rotateBackItemsCommand(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command = nullptr);
	void flipItemsCommand(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command = nullptr);
	void reorderItemsCommand(const QList<DrawingItem*>& itemsOrdered, QUndoCommand* command = nullptr);
	void selectItemsCommand(const QList<DrawingItem*>& items, bool finalSelect = true, QUndoCommand* command = nullptr);
	void connectItemPointsCommand(DrawingItemPoint* point1, DrawingItemPoint* point2, QUndoCommand* command = nullptr);
	void disconnectItemPointsCommand(DrawingItemPoint* point1, DrawingItemPoint* point2, QUndoCommand* command = nullptr);

	void placeItems(const QList<DrawingItem*>& items, QUndoCommand* command);
	void unplaceItems(const QList<DrawingItem*>& items, QUndoCommand* command);
	void tryToMaintainConnections(const QList<DrawingItem*>& items, bool allowResize,
		bool checkControlPoints, DrawingItemPoint* pointToSkip, QUndoCommand* command);
	void disconnectAll(DrawingItemPoint* itemPoint, QUndoCommand* command);
	
private:
	// Functions called by undo command classes
	void addItems(const QList<DrawingItem*>& items);
	void insertItems(const QList<DrawingItem*>& items, const QHash<DrawingItem*,int>& index);
	void removeItems(const QList<DrawingItem*>& items);
	void moveItems(const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& scenePos);
	void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	void rotateItems(const QList<DrawingItem*>& items, const QPointF& scenePos);
	void rotateBackItems(const QList<DrawingItem*>& items, const QPointF& scenePos);
	void flipItems(const QList<DrawingItem*>& items, const QPointF& scenePos);
	void reorderItems(const QList<DrawingItem*>& items);
	void selectItems(const QList<DrawingItem*>& items);
	void insertItemPoint(DrawingItem* item, DrawingItemPoint* itemPoint, int pointIndex);
	void removeItemPoint(DrawingItem* item, DrawingItemPoint* itemPoint);
	void connectItemPoints(DrawingItemPoint* point1, DrawingItemPoint* point2);
	void disconnectItemPoints(DrawingItemPoint* point1, DrawingItemPoint* point2);

	// Miscellaneous helper functions
	bool itemMatchesPoint(DrawingItem* item, const QPointF& scenePos) const;
	bool itemMatchesRect(DrawingItem* item, const QRectF& rect, Qt::ItemSelectionMode mode) const;
	bool itemMatchesPath(DrawingItem* item, const QPainterPath& path, Qt::ItemSelectionMode mode) const;

	bool shouldConnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const;
	bool shouldDisconnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const;

	void recalculateContentSize(const QRectF& targetSceneRect = QRectF());
	
	void sendMouseInfoText(const QPointF& pos);
	void sendMouseInfoText(const QPointF& p1, const QPointF& p2);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DrawingWidget::Flags)

#endif