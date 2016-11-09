// DrawingPolygonItem.sip

class DrawingPolygonItem : public DrawingItem
{
public:
	DrawingPolygonItem();
	DrawingPolygonItem(const DrawingPolygonItem& item);
	virtual ~DrawingPolygonItem();

	virtual DrawingItem* copy() const;

	void setPolygon(const QPolygonF& polygon);
	QPolygonF polygon() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	virtual void insertItemPoint(const QPointF& scenePos);
	virtual void removeItemPoint(const QPointF& scenePos);

private:
	qreal distanceFromPointToLineSegment(const QPointF& point, const QLineF& line) const;
};
