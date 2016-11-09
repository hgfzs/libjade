// DrawingTypes.sip

class DrawingMouseEvent : public QEvent
{

%TypeHeaderCode
#include <DrawingTypes.h>
%End
	
public:
	DrawingMouseEvent();
	~DrawingMouseEvent();

	QPoint pos() const;
	QPointF scenePos() const;
	QPoint buttonDownPos() const;
	QPointF buttonDownScenePos() const;
	Qt::MouseButton button() const;
	Qt::MouseButtons buttons() const;
	Qt::KeyboardModifiers modifiers() const;	
	bool isDragged() const;
};
