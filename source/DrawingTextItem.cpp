/* DrawingTextItem.cpp
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

#include "DrawingTextItem.h"
#include "DrawingWidget.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"

DrawingTextItem::DrawingTextItem() : DrawingItem()
{
	mCaption = "Label";

	setFlags(CanMove | CanRotate | CanFlip | CanSelect);

	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::NoFlags));

	DrawingItemStyle* style = DrawingItem::style();
	style->setValue(DrawingItemStyle::TextColor,
		style->valueLookup(DrawingItemStyle::TextColor, QVariant(QColor(0, 0, 0))));
	style->setValue(DrawingItemStyle::TextOpacity,
		style->valueLookup(DrawingItemStyle::TextOpacity, QVariant(1.0)));

	style->setValue(DrawingItemStyle::FontName,
		style->valueLookup(DrawingItemStyle::FontName, QVariant("Arial")));
	style->setValue(DrawingItemStyle::FontSize,
		style->valueLookup(DrawingItemStyle::FontSize, QVariant(100.0)));
	style->setValue(DrawingItemStyle::FontBold,
		style->valueLookup(DrawingItemStyle::FontBold, QVariant(false)));
	style->setValue(DrawingItemStyle::FontItalic,
		style->valueLookup(DrawingItemStyle::FontItalic, QVariant(false)));
	style->setValue(DrawingItemStyle::FontUnderline,
		style->valueLookup(DrawingItemStyle::FontUnderline, QVariant(false)));
	style->setValue(DrawingItemStyle::FontOverline,
		style->valueLookup(DrawingItemStyle::FontOverline, QVariant(false)));
	style->setValue(DrawingItemStyle::FontStrikeThrough,
		style->valueLookup(DrawingItemStyle::FontStrikeThrough, QVariant(false)));

	style->setValue(DrawingItemStyle::TextHorizontalAlignment,
		style->valueLookup(DrawingItemStyle::TextHorizontalAlignment, QVariant((uint)Qt::AlignLeft)));
	style->setValue(DrawingItemStyle::TextVerticalAlignment,
		style->valueLookup(DrawingItemStyle::TextVerticalAlignment, QVariant((uint)Qt::AlignBottom)));
}

DrawingTextItem::DrawingTextItem(const DrawingTextItem& item) : DrawingItem(item)
{
	mCaption = item.mCaption;
}

DrawingTextItem::~DrawingTextItem() { }

//==================================================================================================

DrawingItem* DrawingTextItem::copy() const
{
	return new DrawingTextItem(*this);
}

//==================================================================================================

void DrawingTextItem::setCaption(const QString& caption)
{
	mCaption = caption;
}

QString DrawingTextItem::caption() const
{
	return mCaption;
}

//==================================================================================================

QRectF DrawingTextItem::boundingRect() const
{
	DrawingItemStyle* style = DrawingTextItem::style();
	QFont font = style->font();
	Qt::Alignment textAlignment = style->textAlignment();

	return calculateTextRect(mCaption, font, textAlignment);
}

QPainterPath DrawingTextItem::shape() const
{
	QPainterPath path;
	path.addRect(boundingRect());
	return path;
}

QPointF DrawingTextItem::centerPos() const
{
	return QPointF(0, 0);
}

bool DrawingTextItem::isValid() const
{
	return (!mCaption.isEmpty());
}

//==================================================================================================

void DrawingTextItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();
		QFont sceneFont = painter->font();

		DrawingItemStyle* style = DrawingTextItem::style();
		QBrush textBrush = style->textBrush();
		QFont font = style->font();
		Qt::Alignment textAlignment = style->textAlignment();

		// Draw text
		QPen textPen(textBrush, 1, Qt::SolidLine);
		painter->setBrush(Qt::transparent);
		painter->setPen(textPen);
		painter->setFont(font);
		painter->drawText(calculateTextRect(mCaption, font, textAlignment), textAlignment, mCaption);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
		painter->setFont(sceneFont);
	}

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
}

//==================================================================================================

QRectF DrawingTextItem::calculateTextRect(const QString& caption, const QFont& font,
	Qt::Alignment textAlignment) const
{
	qreal textWidth = 0, textHeight = 0;

	QFontMetricsF fontMetrics(font);
	QStringList lines = caption.split("\n");

	for(auto lineIter = lines.begin(); lineIter != lines.end(); lineIter++)
	{
		textWidth = qMax(textWidth, fontMetrics.width(*lineIter));
		textHeight += fontMetrics.lineSpacing();
	}

	textHeight -= fontMetrics.leading();

	// Determine text position
	qreal textLeft = 0, textTop = 0;

	if (textAlignment & Qt::AlignLeft) textLeft = 0;
	else if (textAlignment & Qt::AlignRight) textLeft = -textWidth;
	else textLeft = -textWidth / 2;

	if (textAlignment & Qt::AlignBottom) textTop = -textHeight;
	else if (textAlignment & Qt::AlignTop) textTop = 0;
	else textTop = -textHeight / 2;

	return QRectF(textLeft, textTop, textWidth, textHeight);
}
