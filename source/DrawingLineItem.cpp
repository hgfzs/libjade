/* DrawingLineItem.cpp
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

#include "DrawingLineItem.h"
#include "DrawingItemPoint.h"
#include <QPainter>
#include <QtMath>

DrawingLineItem::DrawingLineItem() : DrawingItem()
{
	mLine = QLineF(0, 0, 0, 0);
	mPen = QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	mStartArrow = DrawingArrow(DrawingArrow::None, 100.0);
	mEndArrow = DrawingArrow(DrawingArrow::None, 100.0);
	mStartArrow.setVector(QPointF(0, 0), mPen.widthF(), 0, 0);
	mEndArrow.setVector(QPointF(0, 0), mPen.widthF(), 0, 180);

	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanSelect | CanDelete);

	DrawingItemPoint::Flags flags =
		DrawingItemPoint::Control | DrawingItemPoint::Connection | DrawingItemPoint::Free;
	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));	// start point
	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));	// end point
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Connection));	// mid point

	updateGeometry();
}

DrawingLineItem::DrawingLineItem(const DrawingLineItem& item) : DrawingItem(item)
{
	mLine = item.mLine;
	mPen = item.mPen;
	mStartArrow = item.mStartArrow;
	mEndArrow = item.mEndArrow;

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
}

DrawingLineItem::~DrawingLineItem() { }

//==================================================================================================

DrawingItem* DrawingLineItem::copy() const
{
	return new DrawingLineItem(*this);
}

//==================================================================================================

void DrawingLineItem::setLine(const QLineF& line)
{
	mLine = line;

	// Update points
	QList<DrawingItemPoint*> points = DrawingLineItem::points();
	if (points.size() >= 2)
	{
		points[0]->setPosition(line.p1());
		points[1]->setPosition(line.p2());
	}
	if (points.size() == 3) points[2]->setPosition((line.p1() + line.p2()) / 2);

	// Update arrows
	qreal length = qSqrt((line.x2() - line.x1()) * (line.x2() - line.x1()) +
		(line.y2() - line.y1()) * (line.y2() - line.y1()));
	qreal angle = 180 * qAtan2(line.y2() - line.y1(), line.x2() - line.x1()) / 3.141592654;

	mStartArrow.setVector(line.p1(), mStartArrow.vectorPenWidth(), length, angle);
	mEndArrow.setVector(line.p2(), mEndArrow.vectorPenWidth(), length, 180 + angle);

	updateGeometry();
}

void DrawingLineItem::setLine(qreal x1, qreal y1, qreal x2, qreal y2)
{
	setLine(QLineF(x1, y1, x2, y2));
}

QLineF DrawingLineItem::line() const
{
	return mLine;
}

//==================================================================================================

void DrawingLineItem::setPen(const QPen& pen)
{
	mPen = pen;
	mStartArrow.setVector(mStartArrow.vectorPosition(), mPen.widthF(),
		mStartArrow.vectorLength(), mStartArrow.vectorAngle());
	mEndArrow.setVector(mEndArrow.vectorPosition(), mPen.widthF(),
		mEndArrow.vectorLength(), mEndArrow.vectorAngle());
	updateGeometry();
}

QPen DrawingLineItem::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingLineItem::setStartArrow(const DrawingArrow& arrow)
{
	mStartArrow.setStyle(arrow.style());
	mStartArrow.setSize(arrow.size());
	updateGeometry();
}

void DrawingLineItem::setEndArrow(const DrawingArrow& arrow)
{
	mEndArrow.setStyle(arrow.style());
	mEndArrow.setSize(arrow.size());
	updateGeometry();
}

DrawingArrow DrawingLineItem::startArrow() const
{
	return mStartArrow;
}

DrawingArrow DrawingLineItem::endArrow() const
{
	return mEndArrow;
}

//==================================================================================================

void DrawingLineItem::setProperties(const QHash<QString,QVariant>& properties)
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

	if (properties.contains("start-arrow-style"))
	{
		bool ok = false;
		uint value = properties["start-arrow-style"].toUInt(&ok);
		if (ok) mStartArrow.setStyle(static_cast<DrawingArrow::Style>(value));
	}

	if (properties.contains("start-arrow-size"))
	{
		bool ok = false;
		qreal value = properties["start-arrow-size"].toDouble(&ok);
		if (ok) mStartArrow.setSize(value);
	}

	if (properties.contains("end-arrow-style"))
	{
		bool ok = false;
		uint value = properties["end-arrow-style"].toUInt(&ok);
		if (ok) mEndArrow.setStyle(static_cast<DrawingArrow::Style>(value));
	}

	if (properties.contains("end-arrow-size"))
	{
		bool ok = false;
		qreal value = properties["end-arrow-size"].toDouble(&ok);
		if (ok) mEndArrow.setSize(value);
	}

	updateGeometry();
}

QHash<QString,QVariant> DrawingLineItem::properties() const
{
	QHash<QString,QVariant> properties;

	properties["pen-style"] = static_cast<uint>(mPen.style());
	properties["pen-color"] = mPen.brush().color();
	properties["pen-width"] = mPen.widthF();
	properties["pen-cap-style"] = static_cast<uint>(mPen.capStyle());
	properties["pen-join-style"] = static_cast<uint>(mPen.joinStyle());

	properties["start-arrow-style"] = static_cast<uint>(mStartArrow.style());
	properties["start-arrow-size"] = mStartArrow.size();
	properties["end-arrow-style"] = static_cast<uint>(mEndArrow.style());
	properties["end-arrow-size"] = mEndArrow.size();

	return properties;
}

//==================================================================================================

QRectF DrawingLineItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingLineItem::shape() const
{
	return mShape;
}

bool DrawingLineItem::isValid() const
{
	return (mLine.x1() != mLine.x2() || mLine.y1() != mLine.y2());
}

//==================================================================================================

void DrawingLineItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw line
		painter->setBrush(Qt::transparent);
		painter->setPen(mPen);
		painter->drawLine(mLine);

		// Draw arrows
		if (mPen.style() != Qt::NoPen)
		{
			mStartArrow.render(painter, sceneBrush);
			mEndArrow.render(painter, sceneBrush);
		}

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingLineItem::resize(DrawingItemPoint* point, const QPointF& pos)
{
	DrawingItem::resize(point, pos);

	QList<DrawingItemPoint*> points = DrawingLineItem::points();

	if (points.size() >= 2)
	{
		QLineF line;

		line.setP1(points[0]->position());
		line.setP2(points[1]->position());

		setLine(line);
	}
}

//==================================================================================================

void DrawingLineItem::updateGeometry()
{
	mBoundingRect = QRectF();
	mShape = QPainterPath();

	if (isValid())
	{
		qreal halfPenWidth = mPen.widthF() / 2;
		QPainterPath drawPath;

		// Bounding rect
		mBoundingRect = QRectF(mLine.p1(), mLine.p2()).normalized();
		mBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		// Shape
		drawPath.moveTo(mLine.p1());
		drawPath.lineTo(mLine.p2());

		mShape = strokePath(drawPath, mPen);

		if (mPen.style() != Qt::NoPen)
		{
			mShape = mShape.united(mStartArrow.shape());
			mShape = mShape.united(mEndArrow.shape());
		}
	}
}
