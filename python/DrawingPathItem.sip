// DrawingPathItem.sip

class DrawingPathItem : public DrawingItem
{
private:
	QString mName;
	QPainterPath mPath;
	QRectF mPathRect;
	QHash<DrawingItemPoint*,QPointF> mPathConnectionPoints;

public:
	DrawingPathItem();
	DrawingPathItem(const DrawingPathItem& item);
	virtual ~DrawingPathItem();

	virtual DrawingItem* copy() const;

	void setRect(const QRectF& rect);
	void setRect(qreal left, qreal top, qreal width, qreal height);
	QRectF rect() const;

	void setName(const QString& name);
	QString name() const;

	void setPath(const QPainterPath& path, const QRectF& pathRect);
	QPainterPath path() const;
	QRectF pathRect() const;

	void addConnectionPoint(const QPointF& pathPos);
	void addConnectionPoints(const QPolygonF& pathPos);
	QPolygonF connectionPoints() const;

	QPointF mapToPath(const QPointF& itemPos) const;
	QRectF mapToPath(const QRectF& itemRect) const;
	QPointF mapFromPath(const QPointF& pathPos) const;
	QRectF mapFromPath(const QRectF& pathRect) const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);

private:
	QPainterPath transformedPath() const;
};
