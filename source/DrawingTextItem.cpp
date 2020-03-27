/* DrawingTextItem.cpp
 *
 * Copyright (C) 2013-2020 Jason Allen
 *
 * This file is part of the libjade library.
 *
 * libjade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libjade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libjade.  If not, see <http://www.gnu.org/licenses/>
 */

#include "DrawingTextItem.h"
#include "DrawingItemPoint.h"
#include <QPainter>
#include <QPaintEngine>

DrawingTextItem::DrawingTextItem() : DrawingItem()
{
	mCaption = "Label";
	mPen = QPen(Qt::black, 1, Qt::SolidLine);
	mFont = QFont("Arial", 100.0, -1, false);
	mAlignment = Qt::AlignCenter;

	setFlags(CanMove | CanRotate | CanFlip | CanSelect | CanDelete);

	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::NoFlags));
}

DrawingTextItem::DrawingTextItem(const DrawingTextItem& item) : DrawingItem(item)
{
	mCaption = item.mCaption;
	mPen = item.mPen;
	mFont = item.mFont;
	mAlignment = item.mAlignment;

	mTextRect = item.mTextRect;
	mTextShape = item.mTextShape;
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

void DrawingTextItem::setTextColor(const QColor& color)
{
	mPen.setBrush(color);
}

QColor DrawingTextItem::textColor() const
{
	return mPen.brush().color();
}

//==================================================================================================

void DrawingTextItem::setFont(const QFont& font)
{
	mFont = font;
}

QFont DrawingTextItem::font() const
{
	return mFont;
}

//==================================================================================================

void DrawingTextItem::setAlignment(Qt::Alignment alignment)
{
	mAlignment = alignment;
}

Qt::Alignment DrawingTextItem::alignment() const
{
	return mAlignment;
}

//==================================================================================================

void DrawingTextItem::setProperties(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("caption"))
		mCaption = properties["caption"].toString();

	if (properties.contains("text-color"))
	{
		QColor color = properties["text-color"].value<QColor>();
		mPen.setBrush(color);
	}

	if (properties.contains("font-family"))
		mFont.setFamily(properties["font-family"].toString());

	if (properties.contains("font-size"))
	{
		bool ok = false;
		qreal value = properties["font-size"].toDouble(&ok);
		if (ok) mFont.setPointSizeF(value);
	}

	if (properties.contains("font-bold"))
		mFont.setBold(properties["font-bold"].toBool());

	if (properties.contains("font-italic"))
		mFont.setItalic(properties["font-italic"].toBool());

	if (properties.contains("font-underline"))
		mFont.setUnderline(properties["font-underline"].toBool());

	if (properties.contains("font-strike-through"))
		mFont.setStrikeOut(properties["font-strike-through"].toBool());

	if (properties.contains("text-alignment-horizontal"))
	{
		bool ok = false;
		uint value = properties["text-alignment-horizontal"].toUInt(&ok);
		if (ok) mAlignment = static_cast<Qt::Alignment>((mAlignment & Qt::AlignVertical_Mask) | value);
	}

	if (properties.contains("text-alignment-vertical"))
	{
		bool ok = false;
		uint value = properties["text-alignment-vertical"].toUInt(&ok);
		if (ok) mAlignment = static_cast<Qt::Alignment>((mAlignment & Qt::AlignHorizontal_Mask) | value);
	}
}

QHash<QString,QVariant> DrawingTextItem::properties() const
{
	QHash<QString,QVariant> properties;

	properties["caption"] = mCaption;

	properties["text-color"] = mPen.brush().color();

	properties["font-family"] = mFont.family();
	properties["font-size"] = mFont.pointSizeF();
	properties["font-bold"] = mFont.bold();
	properties["font-italic"] = mFont.italic();
	properties["font-underline"] = mFont.underline();
	properties["font-strike-through"] = mFont.strikeOut();

	properties["text-alignment-horizontal"] = static_cast<uint>(mAlignment & Qt::AlignHorizontal_Mask);
	properties["text-alignment-vertical"] = static_cast<uint>(mAlignment & Qt::AlignVertical_Mask);

	return properties;
}

//==================================================================================================

QRectF DrawingTextItem::boundingRect() const
{
	return mTextRect;
}

QPainterPath DrawingTextItem::shape() const
{
	return mTextShape;
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

void DrawingTextItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();
		QFont sceneFont = painter->font();

		// Draw text
		QFont font = mFont;
		if (painter->paintEngine()->paintDevice())
			font.setPointSizeF(font.pointSizeF() * 96.0 / painter->paintEngine()->paintDevice()->logicalDpiX());

		updateTextRect(font);

		painter->setBrush(Qt::transparent);
		painter->setPen(mPen);
		painter->setFont(font);
		painter->drawText(mTextRect, mAlignment, mCaption);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
		painter->setFont(sceneFont);
	}
}

//==================================================================================================

void DrawingTextItem::updateTextRect(const QFont& font)
{
	mTextRect = QRectF();
	mTextShape = QPainterPath();

	if (isValid())
	{
		// Bounding rect
		qreal textWidth = 0, textHeight = 0;

		QFontMetricsF fontMetrics(font);
		QStringList lines = mCaption.split("\n");

		for(auto lineIter = lines.begin(); lineIter != lines.end(); lineIter++)
		{
			textWidth = qMax(textWidth, fontMetrics.width(*lineIter));
			textHeight += fontMetrics.lineSpacing();
		}

		textHeight -= fontMetrics.leading();

		// Determine text position
		qreal textLeft = 0, textTop = 0;

		if (mAlignment & Qt::AlignLeft) textLeft = 0;
		else if (mAlignment & Qt::AlignRight) textLeft = -textWidth;
		else textLeft = -textWidth / 2;

		if (mAlignment & Qt::AlignBottom) textTop = -textHeight;
		else if (mAlignment & Qt::AlignTop) textTop = 0;
		else textTop = -textHeight / 2;

		mTextRect = QRectF(textLeft, textTop, textWidth, textHeight);

		// Shape
		mTextShape.addRect(mTextRect);
	}
}
