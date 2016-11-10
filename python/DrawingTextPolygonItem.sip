// DrawingTextPolygonItem.sip

class DrawingTextPolygonItem : public DrawingItem
{

%TypeHeaderCode
#include <DrawingTextPolygonItem.h>
%End

public:
	DrawingTextPolygonItem();
	DrawingTextPolygonItem(const DrawingTextPolygonItem& item);
	virtual ~DrawingTextPolygonItem();

	virtual DrawingItem* copy() const /TransferBack/;

	void setPolygon(const QPolygonF& polygon);
	QPolygonF polygon() const;

	void setCaption(const QString& caption);
	QString caption() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	virtual void insertItemPoint(const QPointF& scenePos);
	virtual void removeItemPoint(const QPointF& scenePos);
};
