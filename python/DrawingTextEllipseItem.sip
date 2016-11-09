// DrawingTextEllipseItem.sip

class DrawingTextEllipseItem : public DrawingItem
{
private:
	QString mCaption;

public:
	DrawingTextEllipseItem();
	DrawingTextEllipseItem(const DrawingTextEllipseItem& item);
	virtual ~DrawingTextEllipseItem();

	virtual DrawingItem* copy() const;

	void setEllipse(const QRectF& rect);
	void setEllipse(qreal left, qreal top, qreal width, qreal height);
	QRectF ellipse() const;

	void setCaption(const QString& caption);
	QString caption() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);

private:
	QRectF calculateTextRect(const QString& caption, const QFont& font) const;
};
