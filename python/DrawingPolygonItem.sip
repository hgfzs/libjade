// DrawingPolygonItem.sip

class DrawingPolygonItem : public DrawingItem
{

%TypeHeaderCode
#include <DrawingPolygonItem.h>
%End

public:
	DrawingPolygonItem();
	DrawingPolygonItem(const DrawingPolygonItem& item);
	virtual ~DrawingPolygonItem();

	virtual DrawingItem* copy() const /TransferBack/;

	void setPolygon(const QPolygonF& polygon);
	QPolygonF polygon() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	virtual void insertItemPoint(const QPointF& scenePos);
	virtual void removeItemPoint(const QPointF& scenePos);
};
