/* DrawingTextEllipseItem.cpp
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

#include "DrawingTextEllipseItem.h"
#include "DrawingItemPoint.h"
#include <QPainter>
#include <QPaintEngine>

DrawingTextEllipseItem::DrawingTextEllipseItem() : DrawingItem()
{
	mRect = QRectF(0, 0, 0, 0);
	mPen = QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	mBrush = Qt::white;

	mCaption = "Label";
	mTextPen = QPen(Qt::black, 1, Qt::SolidLine);
	mFont = QFont("Arial", 100.0, -1, false);

	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanSelect | CanDelete);

	DrawingItemPoint::Flags flags = (DrawingItemPoint::Control | DrawingItemPoint::Connection);
	for(int i = 0; i < 8; i++) addPoint(new DrawingItemPoint(QPointF(0, 0), flags));
	setEllipse(QRectF(-500, -250, 1000, 500));

	updateGeometry();
}

DrawingTextEllipseItem::DrawingTextEllipseItem(const DrawingTextEllipseItem& item) : DrawingItem(item)
{
	mRect = item.mRect;
	mPen = item.mPen;
	mBrush = item.mBrush;

	mCaption = item.mCaption;
	mTextPen = item.mTextPen;
	mFont = item.mFont;

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
	mRectBoundingRect = item.mRectBoundingRect;
	mRectShape = item.mRectShape;
	mTextBoundingRect = item.mTextBoundingRect;
}

DrawingTextEllipseItem::~DrawingTextEllipseItem() { }

//==================================================================================================

DrawingItem* DrawingTextEllipseItem::copy() const
{
	return new DrawingTextEllipseItem(*this);
}

//==================================================================================================

void DrawingTextEllipseItem::setEllipse(const QRectF& rect)
{
	mRect = rect;

	// Update points
	QList<DrawingItemPoint*> points = DrawingTextEllipseItem::points();
	if (points.size() >= 8)
	{
		points[TopLeft]->setPosition(rect.left(), rect.top());
		points[TopMiddle]->setPosition(rect.center().x(), rect.top());
		points[TopRight]->setPosition(rect.right(), rect.top());
		points[MiddleRight]->setPosition(rect.right(), rect.center().y());
		points[BottomRight]->setPosition(rect.right(), rect.bottom());
		points[BottomMiddle]->setPosition(rect.center().x(), rect.bottom());
		points[BottomLeft]->setPosition(rect.left(), rect.bottom());
		points[MiddleLeft]->setPosition(rect.left(), rect.center().y());
	}

	updateGeometry();
}

void DrawingTextEllipseItem::setEllipse(qreal left, qreal top, qreal width, qreal height)
{
	setEllipse(QRectF(left, top, width, height));
}

QRectF DrawingTextEllipseItem::ellipse() const
{
	return mRect;
}

//==================================================================================================

void DrawingTextEllipseItem::setPen(const QPen& pen)
{
	mPen = pen;
	updateGeometry();
}

QPen DrawingTextEllipseItem::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingTextEllipseItem::setBrush(const QBrush& brush)
{
	mBrush = brush;
	updateGeometry();
}

QBrush DrawingTextEllipseItem::brush() const
{
	return mBrush;
}

//==================================================================================================

void DrawingTextEllipseItem::setCaption(const QString& caption)
{
	mCaption = caption;
}

QString DrawingTextEllipseItem::caption() const
{
	return mCaption;
}

//==================================================================================================

void DrawingTextEllipseItem::setTextColor(const QColor& color)
{
	mPen.setBrush(color);
}

QColor DrawingTextEllipseItem::textColor() const
{
	return mPen.brush().color();
}

//==================================================================================================

void DrawingTextEllipseItem::setFont(const QFont& font)
{
	mFont = font;
}

QFont DrawingTextEllipseItem::font() const
{
	return mFont;
}

//==================================================================================================

void DrawingTextEllipseItem::setProperties(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("pen-style"))
	{
		bool ok = false;
		uint value = properties["pen-style"].toUInt(&ok);
		if (ok) mPen.setStyle(static_cast<Qt::PenStyle>(value));
	}

	if (properties.contains("pen-color"))
	{
		QColor color = properties["pen-color"].value<QColor>();
		mPen.setBrush(color);
	}

	if (properties.contains("pen-width"))
	{
		bool ok = false;
		qreal value = properties["pen-width"].toDouble(&ok);
		if (ok) mPen.setWidthF(value);
	}

	if (properties.contains("pen-cap-style"))
	{
		bool ok = false;
		uint value = properties["pen-cap-style"].toUInt(&ok);
		if (ok) mPen.setCapStyle(static_cast<Qt::PenCapStyle>(value));
	}

	if (properties.contains("pen-join-style"))
	{
		bool ok = false;
		uint value = properties["pen-join-style"].toUInt(&ok);
		if (ok) mPen.setJoinStyle(static_cast<Qt::PenJoinStyle>(value));
	}

	if (properties.contains("brush-color"))
	{
		QColor color = properties["brush-color"].value<QColor>();
		mBrush = QBrush(color);
	}

	if (properties.contains("caption"))
		mCaption = properties["caption"].toString();

	if (properties.contains("text-color"))
	{
		QColor color = properties["text-color"].value<QColor>();
		mTextPen.setBrush(color);
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

	updateGeometry();
}

QHash<QString,QVariant> DrawingTextEllipseItem::properties() const
{
	QHash<QString,QVariant> properties;

	properties["pen-style"] = static_cast<uint>(mPen.style());
	properties["pen-color"] = mPen.brush().color();
	properties["pen-width"] = mPen.widthF();
	properties["pen-cap-style"] = static_cast<uint>(mPen.capStyle());
	properties["pen-join-style"] = static_cast<uint>(mPen.joinStyle());

	properties["brush-color"] = mBrush.color();

	properties["caption"] = mCaption;

	properties["text-color"] = mTextPen.brush().color();

	properties["font-family"] = mFont.family();
	properties["font-size"] = mFont.pointSizeF();
	properties["font-bold"] = mFont.bold();
	properties["font-italic"] = mFont.italic();
	properties["font-underline"] = mFont.underline();
	properties["font-strike-through"] = mFont.strikeOut();

	return properties;
}

//==================================================================================================

QRectF DrawingTextEllipseItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingTextEllipseItem::shape() const
{
	return mShape;
}

bool DrawingTextEllipseItem::isValid() const
{
	return ((mRect.width() != 0 && mRect.height() != 0) || !mCaption.isEmpty());
}

//==================================================================================================

void DrawingTextEllipseItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();
		QFont sceneFont = painter->font();

		// Draw rect
		painter->setBrush(mBrush);
		painter->setPen(mPen);
		painter->drawEllipse(mRect);

		// Draw text
		QFont font = mFont;
		if (painter->paintEngine()->paintDevice())
			font.setPointSizeF(font.pointSizeF() * 96.0 / painter->paintEngine()->paintDevice()->logicalDpiX());

		updateTextRect(font);

		painter->setBrush(Qt::transparent);
		painter->setPen(mTextPen);
		painter->setFont(font);
		painter->drawText(mTextBoundingRect, Qt::AlignCenter, mCaption);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
		painter->setFont(sceneFont);
	}
}

//==================================================================================================

void DrawingTextEllipseItem::resize(DrawingItemPoint* point, const QPointF& pos)
{
	DrawingItem::resize(point, pos);

	QList<DrawingItemPoint*> points = DrawingTextEllipseItem::points();

	if (points.size() >= 8)
	{
		int pointIndex = points.indexOf(point);

		if (0 <= pointIndex && pointIndex < 8)
		{
			QRectF rect;

			rect.setTopLeft(points[0]->position());
			rect.setBottomRight(points[1]->position());

			switch (pointIndex)
			{
			case TopLeft: rect.setTopLeft(point->position()); break;
			case TopMiddle:	rect.setTop(point->y()); break;
			case TopRight: rect.setTopRight(point->position()); break;
			case MiddleRight: rect.setRight(point->x()); break;
			case BottomRight: rect.setBottomRight(point->position()); break;
			case BottomMiddle: rect.setBottom(point->y()); break;
			case BottomLeft: rect.setBottomLeft(point->position()); break;
			case MiddleLeft: rect.setLeft(point->x()); break;
			default: break;
			}

			setEllipse(rect);
		}
	}
}

//==================================================================================================

void DrawingTextEllipseItem::updateGeometry()
{
	mBoundingRect = QRectF();
	mShape = QPainterPath();
	mRectBoundingRect = QRectF();
	mRectShape = QPainterPath();

	if (isValid())
	{
		qreal halfPenWidth = mPen.widthF() / 2;
		QRectF normalizedRect = mRect.normalized();
		QPainterPath drawPath;

		// Bounding rect
		mRectBoundingRect = normalizedRect;
		mRectBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		mBoundingRect = mRectBoundingRect;
		mBoundingRect = mBoundingRect.united(mTextBoundingRect);

		// Shape
		drawPath.addEllipse(normalizedRect);

		mRectShape = strokePath(drawPath, mPen);
		if (mBrush.color().alpha() > 0) mRectShape.addPath(drawPath);

		mShape = mRectShape;
		mShape.addRect(mTextBoundingRect);
	}
}

void DrawingTextEllipseItem::updateTextRect(const QFont& font)
{
	mTextBoundingRect = QRectF();

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
		qreal textLeft = -textWidth / 2;
		qreal textTop = -textHeight / 2;

		mTextBoundingRect = QRectF(textLeft, textTop, textWidth, textHeight).translated(mRect.center());

		// Bounding rect and shape
		mBoundingRect = mRectBoundingRect;
		mBoundingRect = mBoundingRect.united(mTextBoundingRect);

		mShape = mRectShape;
		mShape.addRect(mTextBoundingRect);
	}
}
