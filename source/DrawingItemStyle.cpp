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
