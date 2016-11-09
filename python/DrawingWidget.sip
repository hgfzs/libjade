// DrawingWidget.sip

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
	enum Mode
	{
		DefaultMode,
		ScrollMode,
		ZoomMode,
		PlaceMode
	};

	enum Flag
	{
		UndoableSelectCommands = 0x0001
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

	void setFlags(Flags flags);
	void setItemSelectionMode(Qt::ItemSelectionMode mode);
	Flags flags() const;
	Qt::ItemSelectionMode itemSelectionMode() const;

	Mode mode() const;
	qreal scale() const;

	void addItem(DrawingItem* item);
	void insertItem(int index, DrawingItem* item);
	void removeItem(DrawingItem* item);
	void clearItems();
	QList<DrawingItem*> items() const;
	QList<DrawingItem*> items(const QPointF& scenePos) const;
	QList<DrawingItem*> items(const QRectF& sceneRect) const;
	DrawingItem* itemAt(const QPointF& scenePos) const;

	void selectItem(DrawingItem* item);
	void deselectItem(DrawingItem* item);
	void clearSelection();
	QList<DrawingItem*> selectedItems() const;

	DrawingItem* newItem() const;
	DrawingItem* mouseDownItem() const;
	DrawingItem* focusItem() const;

	virtual QSize pointSizeHint() const;
	QRect pointRect(DrawingItemPoint* point) const;

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
	void selectArea(const QRectF& rect);
	void selectArea(const QPainterPath& path);
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
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DrawingWidget::Flags)
