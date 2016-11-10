// DrawingWidget.sip

class DrawingWidget : public QAbstractScrollArea
{

%TypeHeaderCode
#include <DrawingWidget.h>
%End

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
	typedef QFlags<DrawingWidget::Flag> Flags;

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
	void pushUndoCommand(QUndoCommand* command /Transfer/);
	int undoLimit() const;
	bool isClean() const;
	bool canUndo() const;
	bool canRedo() const;
	QString undoText() const;
	QString redoText() const;

	void setFlags(DrawingWidget::Flags flags);
	void setItemSelectionMode(Qt::ItemSelectionMode mode);
	DrawingWidget::Flags flags() const;
	Qt::ItemSelectionMode itemSelectionMode() const;

	DrawingWidget::Mode mode() const;
	qreal scale() const;

	void addItem(DrawingItem* item /Transfer/);
	void insertItem(int index, DrawingItem* item /Transfer/);
	void removeItem(DrawingItem* item /TransferBack/);
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
	void setPlaceMode(DrawingItem* newItem /Transfer/);

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
};

//Q_DECLARE_OPERATORS_FOR_FLAGS(DrawingWidget::Flags)
