// DrawingLineItem.sip

class DrawingLineItem : public DrawingItem
{

%TypeHeaderCode
#include <DrawingLineItem.h>
%End

public:
	DrawingLineItem();
	DrawingLineItem(const DrawingLineItem& item);
	virtual ~DrawingLineItem();

	virtual DrawingItem* copy() const /TransferBack/;

	void setLine(const QLineF& line);
	void setLine(qreal x1, qreal y1, qreal x2, qreal y2);
	QLineF line() const;
	
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
