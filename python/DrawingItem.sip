// DrawingItem.sip

class DrawingItem
{
	friend class DrawingWidget;
	friend class DrawingItemGroup;

public:
	enum Flag
	{
		CanMove = 0x01,
		CanResize = 0x02,
		CanRotate = 0x04,
		CanFlip = 0x08,
		CanInsertPoints = 0x10,
		CanRemovePoints = 0x20,
		CanSelect = 0x40
	};
	Q_DECLARE_FLAGS(Flags, Flag)

private:
	DrawingWidget* mDrawing;

	QPointF mPos;
	Flags mFlags;

	qreal mRotation;
	bool mFlipped;

	bool mSelected;
	bool mVisible;

	QList<DrawingItemPoint*> mPoints;

	DrawingItemStyle* mStyle;

	// Internal
	QTransform mTransform;
	QTransform mTransformInverse;

public:
	DrawingItem();
	DrawingItem(const DrawingItem& item);
	virtual ~DrawingItem();

	virtual DrawingItem* copy() const = 0;

	DrawingWidget* drawing() const;

	void setPos(const QPointF& pos);
	void setPos(qreal x, qreal y);
	void setX(qreal x);
	void setY(qreal y);
	QPointF pos() const;
	qreal x() const;
	qreal y() const;

	void setFlags(Flags flags);
	Flags flags() const;

	void setRotation(qreal angle);
	void setFlipped(bool flipped);
	qreal rotation() const;
	bool isFlipped() const;
	QTransform transform() const;

	void setSelected(bool select);
	void setVisible(bool visible);
	bool isSelected() const;
	bool isVisible() const;

	void addPoint(DrawingItemPoint* itemPoint);
	void insertPoint(int index, DrawingItemPoint* itemPoint);
	void removePoint(DrawingItemPoint* itemPoint);
	void clearPoints();
	QList<DrawingItemPoint*> points() const;
	DrawingItemPoint* pointAt(const QPointF& itemPos) const;
	DrawingItemPoint* pointNearest(const QPointF& itemPos) const;

	void setStyle(DrawingItemStyle* style);
	DrawingItemStyle* style() const;

	QPointF mapFromScene(const QPointF& point) const;
	QRectF mapFromScene(const QRectF& rect) const;
	QPolygonF mapFromScene(const QPolygonF& polygon) const;
	QPainterPath mapFromScene(const QPainterPath& path) const;
	QPointF mapToScene(const QPointF& point) const;
	QRectF mapToScene(const QRectF& rect) const;
	QPolygonF mapToScene(const QPolygonF& polygon) const;
	QPainterPath mapToScene(const QPainterPath& path) const;

	virtual QRectF boundingRect() const = 0;
	virtual QPainterPath shape() const;
	virtual QPointF centerPos() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter) = 0;

	virtual void moveItem(const QPointF& scenePos);
	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	virtual void rotateItem(const QPointF& scenePos);
	virtual void rotateBackItem(const QPointF& scenePos);
	virtual void flipItem(const QPointF& scenePos);
	virtual void insertItemPoint(const QPointF& scenePos);
	virtual void removeItemPoint(const QPointF& scenePos);

protected:
	virtual void mousePressEvent(DrawingMouseEvent* event);
	virtual void mouseMoveEvent(DrawingMouseEvent* event);
	virtual void mouseReleaseEvent(DrawingMouseEvent* event);
	virtual void mouseDoubleClickEvent(DrawingMouseEvent* event);
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);
	
	virtual void newMousePressEvent(DrawingMouseEvent* event);
	virtual void newMouseMoveEvent(DrawingMouseEvent* event);
	virtual bool newMouseReleaseEvent(DrawingMouseEvent* event);
	virtual bool newMouseDoubleClickEvent(DrawingMouseEvent* event);
	virtual bool newItemCopyEvent();

protected:
	QPainterPath strokePath(const QPainterPath& path, const QPen& pen) const;
	qreal minimumPenWidth() const;

private:
	void recalculateTransform();

public:
	static QList<DrawingItem*> copyItems(const QList<DrawingItem*>& items);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DrawingItem::Flags)
