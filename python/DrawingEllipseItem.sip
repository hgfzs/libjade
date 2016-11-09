// DrawingEllipseItem.sip

class DrawingEllipseItem : public DrawingItem
{
public:
	DrawingEllipseItem();
	DrawingEllipseItem(const DrawingEllipseItem& item);
	virtual ~DrawingEllipseItem();

	virtual DrawingItem* copy() const;

	void setEllipse(const QRectF& rect);
	void setEllipse(qreal left, qreal top, qreal width, qreal height);
	QRectF ellipse() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);

protected:
	virtual bool newItemCopyEvent();
	virtual void newMouseMoveEvent(DrawingMouseEvent* event);
	virtual bool newMouseReleaseEvent(DrawingMouseEvent* event);
};
