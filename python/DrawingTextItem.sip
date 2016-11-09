// DrawingTextItem.sip

class DrawingTextItem : public DrawingItem
{
private:
	QString mCaption;

public:
	DrawingTextItem();
	DrawingTextItem(const DrawingTextItem& item);
	virtual ~DrawingTextItem();

	virtual DrawingItem* copy() const;

	void setCaption(const QString& caption);
	QString caption() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual QPointF centerPos() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

private:
	QRectF calculateTextRect(const QString& caption, const QFont& font,
		Qt::Alignment textAlignment) const;
};
