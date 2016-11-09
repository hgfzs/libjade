// DrawingTextItem.sip

class DrawingTextItem : public DrawingItem
{

%TypeHeaderCode
#include <DrawingTextItem.h>
%End

public:
	DrawingTextItem();
	DrawingTextItem(const DrawingTextItem& item);
	virtual ~DrawingTextItem();

	virtual DrawingItem* copy() const /TransferBack/;

	void setCaption(const QString& caption);
	QString caption() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual QPointF centerPos() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);
};
