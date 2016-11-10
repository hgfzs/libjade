// DrawingItemPoint.sip

class DrawingItemPoint
{

%TypeHeaderCode
#include <DrawingItemPoint.h>
%End

public:
	enum Flag
	{
		NoFlags = 0x0000,
		Control = 0x0001,
		Connection = 0x0002,
		Free = 0x0004
	};
	typedef QFlags<DrawingItemPoint::Flag> Flags;

public:
	DrawingItemPoint(const QPointF& pos = QPointF(), DrawingItemPoint::Flags flags = Control);
	DrawingItemPoint(const DrawingItemPoint& point);
	virtual ~DrawingItemPoint();

	DrawingItem* item() const;

	void setPos(const QPointF& pos);
	void setPos(qreal x, qreal y);
	void setX(qreal x);
	void setY(qreal y);
	QPointF pos() const;
	qreal x() const;
	qreal y() const;

	void setFlags(DrawingItemPoint::Flags flags);
	DrawingItemPoint::Flags flags() const;
	bool isControlPoint() const;
	bool isConnectionPoint() const;
	bool isFree() const;

	void addConnection(DrawingItemPoint* itemPoint);
	void removeConnection(DrawingItemPoint* itemPoint);
	void clearConnections();
	QList<DrawingItemPoint*> connections() const;
	bool isConnected(DrawingItemPoint* itemPoint) const;
	bool isConnected(DrawingItem* item) const;
};

//Q_DECLARE_OPERATORS_FOR_FLAGS(DrawingItemPoint::Flags)
