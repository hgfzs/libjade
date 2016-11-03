/* DrawingItemStyle.cpp
 *
 * Copyright (C) 2013-2016 Jason Allen
 *
 * This file is part of the jade library.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#include "DrawingItemStyle.h"

QHash<DrawingItemStyle::Property,QVariant> DrawingItemStyle::mDefaultProperties;

DrawingItemStyle::DrawingItemStyle() { }

DrawingItemStyle::DrawingItemStyle(const DrawingItemStyle& style)
{
	mProperties = style.mProperties;
}

DrawingItemStyle::~DrawingItemStyle() { }

//==================================================================================================

void DrawingItemStyle::setValues(const QHash<Property,QVariant>& values)
{
	mProperties = values;
}

QHash<DrawingItemStyle::Property,QVariant> DrawingItemStyle::values() const
{
	return mProperties;
}

//==================================================================================================

void DrawingItemStyle::setValue(Property index, const QVariant& value)
{
	mProperties.insert(index, value);
}

void DrawingItemStyle::unsetValue(Property index)
{
	mProperties.remove(index);
}

void DrawingItemStyle::clearValues()
{
	mProperties.clear();
}

bool DrawingItemStyle::hasValue(Property index) const
{
	return mProperties.contains(index);
}

QVariant DrawingItemStyle::value(Property index) const
{
	return mProperties.value(index, QVariant());
}

//==================================================================================================

QVariant DrawingItemStyle::valueLookup(Property index) const
{
	QVariant value;
	
	value = mDefaultProperties.value(index, value);
	value = mProperties.value(index, value);
	
	return value;
}
	
QVariant DrawingItemStyle::valueLookup(Property index, const QVariant& fallbackValue) const
{
	QVariant value = fallbackValue;
	
	value = mDefaultProperties.value(index, value);
	value = mProperties.value(index, value);
	
	return value;
}
	
//==================================================================================================

QPen DrawingItemStyle::pen() const
{
	Qt::PenStyle style = (Qt::PenStyle)valueLookup(PenStyle, QVariant((uint)Qt::SolidLine)).toUInt();
	QColor color = valueLookup(PenColor, QVariant(QColor(0, 0, 0))).value<QColor>();
	qreal opacity = valueLookup(PenOpacity, QVariant(1.0)).toReal();
	qreal width = valueLookup(PenWidth, QVariant(1.0)).toReal();
	Qt::PenCapStyle capStyle = (Qt::PenCapStyle)valueLookup(PenCapStyle, QVariant((uint)Qt::RoundCap)).toUInt();
	Qt::PenJoinStyle joinStyle = (Qt::PenJoinStyle)valueLookup(PenJoinStyle, QVariant((uint)Qt::RoundJoin)).toUInt();
	
	color.setAlphaF(opacity);
	
	return QPen(QBrush(color), width, style, capStyle, joinStyle);
}

QBrush DrawingItemStyle::brush() const
{
	Qt::BrushStyle style = (Qt::BrushStyle)valueLookup(BrushStyle, QVariant((uint)Qt::SolidPattern)).toUInt();
	QColor color = valueLookup(BrushColor, QVariant(QColor(255, 255, 255))).value<QColor>();
	qreal opacity = valueLookup(BrushOpacity, QVariant(1.0)).toReal();
	
	color.setAlphaF(opacity);
	
	return QBrush(color, style);
}

QFont DrawingItemStyle::font() const
{
	QString name = valueLookup(FontName, QVariant("Arial")).toString();
	qreal size = valueLookup(FontSize, QVariant(1.0)).toReal();
	bool bold = valueLookup(FontBold, QVariant(false)).toBool();
	bool italic = valueLookup(FontItalic, QVariant(false)).toBool();
	bool underline = valueLookup(FontUnderline, QVariant(false)).toBool();
	bool overline = valueLookup(FontOverline, QVariant(false)).toBool();
	bool strikeThrough = valueLookup(FontStrikeThrough, QVariant(false)).toBool();

	QFont font;
	font.setFamily(name);
	font.setPointSizeF(size);
	font.setBold(bold);
	font.setItalic(italic);
	font.setUnderline(underline);
	font.setOverline(overline);
	font.setStrikeOut(strikeThrough);

	return font;
}

QBrush DrawingItemStyle::textBrush() const
{
	QColor color = valueLookup(TextColor, QVariant(QColor(0, 0, 0))).value<QColor>();
	qreal opacity = valueLookup(TextOpacity, QVariant(1.0)).toReal();
	
	color.setAlphaF(opacity);
	
	return QBrush(color);
}

Qt::Alignment DrawingItemStyle::textAlignment() const
{	
	Qt::Alignment horizontalAlignment =
		(Qt::Alignment)valueLookup(TextHorizontalAlignment, QVariant((uint)Qt::AlignHCenter)).toUInt();
	Qt::Alignment verticalAlignment =
		(Qt::Alignment)valueLookup(TextVerticalAlignment, QVariant((uint)Qt::AlignVCenter)).toUInt();
	
	return ((horizontalAlignment & Qt::AlignHorizontal_Mask) | (verticalAlignment & Qt::AlignVertical_Mask));
}

DrawingItemStyle::ArrowStyle DrawingItemStyle::startArrowStyle() const
{
	return (ArrowStyle)valueLookup(StartArrowStyle, QVariant((uint)ArrowNone)).toUInt();
}

qreal DrawingItemStyle::startArrowSize() const
{
	return valueLookup(StartArrowSize, QVariant(0.0)).toReal();
}

DrawingItemStyle::ArrowStyle DrawingItemStyle::endArrowStyle() const
{
	return (ArrowStyle)valueLookup(EndArrowStyle, QVariant((uint)ArrowNone)).toUInt();
}

qreal DrawingItemStyle::endArrowSize() const
{
	return valueLookup(EndArrowSize, QVariant(0.0)).toReal();
}

//==================================================================================================

void DrawingItemStyle::drawArrow(QPainter* painter, ArrowStyle style, qreal size,
	const QPointF& pos, qreal direction, const QPen& pen, const QBrush& backgroundBrush)
{
	if (style != ArrowNone)
	{
		QBrush originalBrush = painter->brush();
		QPen originalPen = painter->pen();

		QPolygonF polygon = calculateArrowPoints(style, size, pos, direction);
		QPen arrowPen = pen;

		if (arrowPen.style() != Qt::NoPen) arrowPen.setStyle(Qt::SolidLine);
		painter->setPen(arrowPen);

		switch (style)
		{
		case ArrowTriangleFilled:
		case ArrowCircleFilled:
		case ArrowDiamondFilled:
		case ArrowConcaveFilled:
			painter->setBrush(pen.brush());
			break;
		case ArrowTriangle:
		case ArrowCircle:
		case ArrowDiamond:
		case ArrowConcave:
			painter->setBrush(backgroundBrush);
			break;
		default:
			painter->setBrush(Qt::transparent);
			break;
		}

		switch (style)
		{
		case ArrowNormal:
		case ArrowReverse:
			painter->drawLine(polygon[0], polygon[1]);
			painter->drawLine(polygon[0], polygon[2]);
			break;
		case ArrowCircle:
		case ArrowCircleFilled:
			painter->drawEllipse(pos, size / 2, size / 2);
			break;
		case ArrowHarpoon:
		case ArrowHarpoonMirrored:
			painter->drawLine(polygon[0], polygon[1]);
			break;
		case ArrowX:
			painter->drawLine(polygon[0], polygon[1]);
			painter->drawLine(polygon[2], polygon[3]);
			break;
		default:
			painter->drawPolygon(polygon);
			break;
		}

		painter->setPen(originalPen);
		painter->setBrush(originalBrush);
	}
}

QPainterPath DrawingItemStyle::arrowShape(ArrowStyle style, qreal size, const QPointF& pos,
	qreal direction) const
{
	QPainterPath path;
	QPolygonF polygon;

	if (style != ArrowNone)
	{
		switch (style)
		{
		case ArrowCircle:
		case ArrowCircleFilled:
			path.addEllipse(pos, size / 2, size / 2);
			break;
		case ArrowHarpoon:
		case ArrowHarpoonMirrored:
			polygon = calculateArrowPoints(style, size, pos, direction);
			path.moveTo(polygon[0]);
			path.lineTo(polygon[1]);
			break;
		case ArrowX:
			polygon = calculateArrowPoints(style, size, pos, direction);
			path.moveTo(polygon[0]);
			path.lineTo(polygon[1]);
			path.moveTo(polygon[2]);
			path.lineTo(polygon[3]);
			break;
		default:
			polygon = calculateArrowPoints(style, size, pos, direction);
			path.moveTo(polygon[0]);
			path.addPolygon(polygon);
			path.closeSubpath();
			break;
		}
	}

	return path;
}

QPolygonF DrawingItemStyle::calculateArrowPoints(ArrowStyle style, qreal size,
	const QPointF& pos, qreal direction) const
{
	QPolygonF polygon;
	const qreal sqrt2 = qSqrt(2);
	qreal angle = 0;

	direction = direction * 3.141592654 / 180;

	switch (style)
	{
	case ArrowNormal:
	case ArrowTriangle:
	case ArrowTriangleFilled:
		angle = 3.141592654 / 6;
		polygon.append(pos);
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction - angle),
							   pos.y() + size / sqrt2 * qSin(direction - angle)));
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction + angle),
							   pos.y() + size / sqrt2 * qSin(direction + angle)));
		break;
	case ArrowDiamond:
	case ArrowDiamondFilled:
		angle = 3.141592654;
		polygon.append(QPointF(pos.x() + size / 2 * qCos(direction),
							   pos.y() + size / 2 * qSin(direction)));
		polygon.append(QPointF(pos.x() + size / 2 * qCos(direction - angle / 2),
							   pos.y() + size / 2 * qSin(direction - angle / 2)));
		polygon.append(QPointF(pos.x() + size / 2 * qCos(direction - angle),
							   pos.y() + size / 2 * qSin(direction - angle)));
		polygon.append(QPointF(pos.x() + size / 2 * qCos(direction + angle / 2),
							   pos.y() + size / 2 * qSin(direction + angle / 2)));
		break;
	case ArrowHarpoon:
		angle = 3.141592654 / 6;
		polygon.append(pos);
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction - angle),
							   pos.y() + size / sqrt2 * qSin(direction - angle)));
		break;
	case ArrowHarpoonMirrored:
		angle = 3.141592654 / 6;
		polygon.append(pos);
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction + angle),
							   pos.y() + size / sqrt2 * qSin(direction + angle)));
		break;
	case ArrowConcave:
	case ArrowConcaveFilled:
		angle = 3.141592654 / 6;
		polygon.append(pos);
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction - angle),
							   pos.y() + size / sqrt2 * qSin(direction - angle)));
		polygon.append(QPointF(pos.x() + size / sqrt2 / 2 * qCos(direction),
							   pos.y() + size / sqrt2 / 2 * qSin(direction)));
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction + angle),
							   pos.y() + size / sqrt2 * qSin(direction + angle)));
		break;
	case ArrowReverse:
		angle = 3.141592654 / 6;
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction),
							   pos.y() + size / sqrt2 * qSin(direction)));
		polygon.append(QPointF(polygon[0].x() - size / sqrt2 * qCos(direction - angle),
							   polygon[0].y() - size / sqrt2 * qSin(direction - angle)));
		polygon.append(QPointF(polygon[0].x() - size / sqrt2 * qCos(direction + angle),
							   polygon[0].y() - size / sqrt2 * qSin(direction + angle)));
		break;
	case ArrowX:
		angle = 3.141592654 / 4;
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction + angle),
							   pos.y() + size / sqrt2 * qSin(direction + angle)));
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction + 5 * angle),
							   pos.y() + size / sqrt2 * qSin(direction + 5 * angle)));
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction + 3 * angle),
							   pos.y() + size / sqrt2 * qSin(direction + 3 * angle)));
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction + 7 * angle),
							   pos.y() + size / sqrt2 * qSin(direction + 7 * angle)));
		break;
	default:
		break;
	}

	return polygon;
}

//==================================================================================================

void DrawingItemStyle::setDefaultValues(const QHash<Property,QVariant>& values)
{
	mDefaultProperties = values;
}

QHash<DrawingItemStyle::Property,QVariant> DrawingItemStyle::defaultValues()
{
	return mDefaultProperties;
}

//==================================================================================================

void DrawingItemStyle::setDefaultValue(Property index, const QVariant& value)
{
	mDefaultProperties.insert(index, value);
}

void DrawingItemStyle::unsetDefaultValue(Property index)
{
	mDefaultProperties.remove(index);
}

void DrawingItemStyle::clearDefaultValues()
{
	mDefaultProperties.clear();
}

bool DrawingItemStyle::hasDefaultValue(Property index)
{
	return mDefaultProperties.contains(index);
}

QVariant DrawingItemStyle::defaultValue(Property index)
{
	return mDefaultProperties.value(index, QVariant());
}
