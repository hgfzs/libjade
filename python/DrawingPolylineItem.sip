// DrawingPolylineItem.sip

class DrawingPolylineItem : public DrawingItem
{

%TypeHeaderCode
#include <DrawingPolylineItem.h>
%End

public:
	DrawingPolylineItem();
	DrawingPolylineItem(const DrawingPolylineItem& item);
	virtual ~DrawingPolylineItem();

	virtual DrawingItem* copy() const /TransferBack/;

	void setPolyline(const QPolygonF& polygon);
	QPolygonF polyline() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	virtual void insertItemPoint(const QPointF& scenePos);
	virtual void removeItemPoint(const QPointF& scenePos);

protected:
	virtual bool newItemCopyEvent();
	virtual void newMouseMoveEvent(DrawingMouseEvent* event);
	virtual bool newMouseReleaseEvent(DrawingMouseEvent* event);
};
