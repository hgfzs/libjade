// DrawingArcItem.sip

class DrawingArcItem : public DrawingItem
{
public:
	DrawingArcItem();
	DrawingArcItem(const DrawingArcItem& item);
	virtual ~DrawingArcItem();

	virtual DrawingItem* copy() const;

	void setArc(const QLineF& line);
	void setArc(qreal x1, qreal y1, qreal x2, qreal y2);
	QLineF arc() const;
	
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	
protected:
	virtual bool newItemCopyEvent();
	virtual void newMouseMoveEvent(DrawingMouseEvent* event);
	virtual bool newMouseReleaseEvent(DrawingMouseEvent* event);

private:
	QRectF arcRect() const;
	qreal arcStartAngle() const;

	QPointF pointFromAngle(qreal angle) const;
	qreal startArrowAngle() const;
	qreal endArrowAngle() const;
};
