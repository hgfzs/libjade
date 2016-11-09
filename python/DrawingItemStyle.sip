// DrawingItemStyle.sip

class DrawingItemStyle
{
public:
	enum Property 
	{ 
		PenStyle,
		PenColor,
		PenOpacity,
		PenWidth,
		PenCapStyle,
		PenJoinStyle,
		BrushStyle,
		BrushColor,	
		BrushOpacity,
		FontName,
		FontSize,
		FontBold,
		FontItalic,
		FontUnderline,
		FontOverline,
		FontStrikeThrough,
		TextColor,
		TextOpacity,
		TextHorizontalAlignment,
		TextVerticalAlignment,
		StartArrowStyle,
		StartArrowSize,
		EndArrowStyle,
		EndArrowSize,
		NumberOfProperties
	};
	
	enum ArrowStyle
	{
		ArrowNone,
		ArrowNormal,
		ArrowReverse,
		ArrowTriangle,
		ArrowTriangleFilled,
		ArrowConcave,
		ArrowConcaveFilled,
		ArrowCircle,
		ArrowCircleFilled,
		ArrowDiamond,
		ArrowDiamondFilled,
		ArrowHarpoon,
		ArrowHarpoonMirrored,
		ArrowX
	};

private:
	QHash<Property,QVariant> mProperties;

public:
	DrawingItemStyle();
	DrawingItemStyle(const DrawingItemStyle& style);
	~DrawingItemStyle();

	void setValues(const QHash<Property,QVariant>& values);
	QHash<Property,QVariant> values() const;

	void setValue(Property index, const QVariant& value);
	void unsetValue(Property index);
	void clearValues();	
	bool hasValue(Property index) const;
	QVariant value(Property index) const;
	
	QVariant valueLookup(Property index) const;
	QVariant valueLookup(Property index, const QVariant& fallbackValue) const;
	
	QPen pen() const;
	QBrush brush() const;
	QFont font() const;
	QBrush textBrush() const;
	Qt::Alignment textAlignment() const;
	ArrowStyle startArrowStyle() const;
	qreal startArrowSize() const;
	ArrowStyle endArrowStyle() const;
	qreal endArrowSize() const;

	void drawArrow(QPainter* painter, ArrowStyle style, qreal size,
		const QPointF& pos, qreal direction, const QPen& pen, const QBrush& backgroundBrush);
	QPainterPath arrowShape(ArrowStyle style, qreal size,
		const QPointF& pos, qreal direction) const;

private:
	QPolygonF calculateArrowPoints(ArrowStyle style, qreal size,
		const QPointF& pos, qreal direction) const;


private:
	static QHash<Property,QVariant> mDefaultProperties;
	
public:
	static void setDefaultValues(const QHash<Property,QVariant>& values);
	static QHash<Property,QVariant> defaultValues();

	static void setDefaultValue(Property index, const QVariant& value);
	static void unsetDefaultValue(Property index);
	static void clearDefaultValues();	
	static bool hasDefaultValue(Property index);
	static QVariant defaultValue(Property index);
};
