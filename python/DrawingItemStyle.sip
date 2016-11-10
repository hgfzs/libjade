// DrawingItemStyle.sip

class DrawingItemStyle
{

%TypeHeaderCode
#include <DrawingItemStyle.h>
%End

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

public:
	DrawingItemStyle();
	DrawingItemStyle(const DrawingItemStyle& style);
	~DrawingItemStyle();

	void setValues(const QHash<DrawingItemStyle::Property,QVariant>& values);
	QHash<DrawingItemStyle::Property,QVariant> values() const;

	void setValue(DrawingItemStyle::Property index, const QVariant& value);
	void unsetValue(DrawingItemStyle::Property index);
	void clearValues();	
	bool hasValue(DrawingItemStyle::Property index) const;
	QVariant value(DrawingItemStyle::Property index) const;
	
	QVariant valueLookup(DrawingItemStyle::Property index) const;
	QVariant valueLookup(DrawingItemStyle::Property index, const QVariant& fallbackValue) const;
	
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


public:
	static void setDefaultValues(const QHash<DrawingItemStyle::Property,QVariant>& values);
	static QHash<DrawingItemStyle::Property,QVariant> defaultValues();

	static void setDefaultValue(DrawingItemStyle::Property index, const QVariant& value);
	static void unsetDefaultValue(DrawingItemStyle::Property index);
	static void clearDefaultValues();	
	static bool hasDefaultValue(DrawingItemStyle::Property index);
	static QVariant defaultValue(DrawingItemStyle::Property index);
};
