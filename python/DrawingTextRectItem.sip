// DrawingTextRectItem.sip

class DrawingTextRectItem : public DrawingItem
{

%TypeHeaderCode
#include <DrawingTextRectItem.h>
%End

public:
	DrawingTextRectItem();
	DrawingTextRectItem(const DrawingTextRectItem& item);
	virtual ~DrawingTextRectItem();

	virtual DrawingItem* copy() const /TransferBack/;

	void setRect(const QRectF& rect);
	void setRect(qreal left, qreal top, qreal width, qreal height);
	QRectF rect() const;

	void setCornerRadii(qreal radiusX, qreal radiusY);
	qreal cornerRadiusX() const;
	qreal cornerRadiusY() const;

	void setCaption(const QString& caption);
	QString caption() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);
};
