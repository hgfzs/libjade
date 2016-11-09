// DrawingItemGroup.sip

class DrawingItemGroup : public DrawingItem
{
private:
	QList<DrawingItem*> mItems;
	QRectF mItemsRect;

public:
	DrawingItemGroup();
	DrawingItemGroup(const DrawingItemGroup& item);
	~DrawingItemGroup();

	DrawingItem* copy() const;

	void setItems(const QList<DrawingItem*>& items);
	QList<DrawingItem*> items() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	virtual void rotateItem(const QPointF& scenePos);
	virtual void rotateBackItem(const QPointF& scenePos);
	virtual void flipItem(const QPointF& scenePos);

private:
	void recalculateContentsRect();
};
