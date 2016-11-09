// DrawingTypes.sip

class DrawingMouseEvent : public QEvent
{
	friend class DrawingWidget;
	
private:
	QPoint mPos;
	QPointF mScenePos;

	QPoint mButtonDownPos;
	QPointF mButtonDownScenePos;

	Qt::MouseButton mButton;
	Qt::MouseButtons mButtons;
	Qt::KeyboardModifiers mModifiers;

	bool mDragged;

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
	
private:
	void setFromEvent(QMouseEvent* event, DrawingWidget* widget);
};
