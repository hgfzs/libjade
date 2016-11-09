// DrawingCurveItem.sip

class DrawingCurveItem : public DrawingItem
{
public:
	DrawingCurveItem();
	DrawingCurveItem(const DrawingCurveItem& item);
	virtual ~DrawingCurveItem();

	virtual DrawingItem* copy() const;

	void setCurve(const QPointF& p1, const QPointF& controlP1, const QPointF& controlP2, const QPointF& p2);
	QPointF curveStartPos() const;
	QPointF curveEndPos() const;
	QPointF curveStartControlPos() const;
	QPointF curveEndControlPos() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);

private:
	QPointF pointFromRatio(qreal ratio) const;
	qreal startArrowAngle() const;
	qreal endArrowAngle() const;
};
