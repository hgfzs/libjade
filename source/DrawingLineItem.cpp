/* DrawingLineItem.cpp
 *
 * Copyright (C) 2013-2017 Jason Allen
 *
 * This file is part of the jade application.
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

#include "DrawingLineItem.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"

DrawingLineItem::DrawingLineItem() : DrawingItem()
{
	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanSelect | CanDelete);

	DrawingItemPoint::Flags flags =
		DrawingItemPoint::Control | DrawingItemPoint::Connection | DrawingItemPoint::Free;
	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));	// start point
	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));	// end point
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Connection));	// mid point

	DrawingItemStyle* style = DrawingItem::style();
	style->setValue(DrawingItemStyle::PenStyle,
		style->valueLookup(DrawingItemStyle::PenStyle, QVariant((uint)Qt::SolidLine)));
	style->setValue(DrawingItemStyle::PenColor,
		style->valueLookup(DrawingItemStyle::PenColor, QVariant(QColor(0, 0, 0))));
	style->setValue(DrawingItemStyle::PenOpacity,
		style->valueLookup(DrawingItemStyle::PenOpacity, QVariant(1.0)));
	style->setValue(DrawingItemStyle::PenWidth,
		style->valueLookup(DrawingItemStyle::PenWidth, QVariant(12.0)));
	style->setValue(DrawingItemStyle::PenCapStyle,
		style->valueLookup(DrawingItemStyle::PenCapStyle, QVariant((uint)Qt::RoundCap)));
	style->setValue(DrawingItemStyle::PenJoinStyle,
		style->valueLookup(DrawingItemStyle::PenJoinStyle, QVariant((uint)Qt::RoundJoin)));
	style->setValue(DrawingItemStyle::StartArrowStyle,
		style->valueLookup(DrawingItemStyle::StartArrowStyle, QVariant((uint)DrawingItemStyle::ArrowNone)));
	style->setValue(DrawingItemStyle::StartArrowSize,
		style->valueLookup(DrawingItemStyle::StartArrowSize, QVariant(100.0)));
	style->setValue(DrawingItemStyle::EndArrowStyle,
		style->valueLookup(DrawingItemStyle::EndArrowStyle, QVariant((uint)DrawingItemStyle::ArrowNone)));
	style->setValue(DrawingItemStyle::EndArrowSize,
		style->valueLookup(DrawingItemStyle::EndArrowSize, QVariant(100.0)));
}

DrawingLineItem::DrawingLineItem(const DrawingLineItem& item) : DrawingItem(item) { }

DrawingLineItem::~DrawingLineItem() { }

//==================================================================================================

DrawingItem* DrawingLineItem::copy() const
{
	return new DrawingLineItem(*this);
}

//==================================================================================================

void DrawingLineItem::setLine(const QLineF& line)
{
	QList<DrawingItemPoint*> points = DrawingLineItem::points();

	points[0]->setPosition(line.p1());
	points[1]->setPosition(line.p2());
	points[2]->setPosition((line.p1() + line.p2()) / 2);
}

void DrawingLineItem::setLine(qreal x1, qreal y1, qreal x2, qreal y2)
{
	setLine(QLineF(x1, y1, x2, y2));
}

QLineF DrawingLineItem::line() const
{
	QLineF line;

	QList<DrawingItemPoint*> points = DrawingLineItem::points();
	line.setP1(points[0]->position());
	line.setP2(points[1]->position());

	return line;
}

//==================================================================================================

QRectF DrawingLineItem::boundingRect() const
{
	QRectF rect;

	if (isValid())
	{
		QList<DrawingItemPoint*> points = DrawingLineItem::points();
		QPointF p1 = points[0]->position();
		QPointF p2 = points[1]->position();
		qreal penWidth = style()->valueLookup(DrawingItemStyle::PenWidth).toReal();

		rect = QRectF(qMin(p1.x(), p2.x()), qMin(p1.y(), p2.y()), qAbs(p1.x() - p2.x()), qAbs(p1.y() - p2.y()));
		rect.adjust(-penWidth/2, -penWidth/2, penWidth/2, penWidth/2);
	}

	return rect;
}

QPainterPath DrawingLineItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;

		QList<DrawingItemPoint*> points = DrawingLineItem::points();
		QPointF p1 = points[0]->position();
		QPointF p2 = points[1]->position();
		qreal lineLength = qSqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y()));
		qreal lineAngle = 180 * qAtan2(p2.y() - p1.y(), p2.x() - p1.x()) / 3.141592654;

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		DrawingItemStyle::ArrowStyle startArrowStyle = style->startArrowStyle();
		DrawingItemStyle::ArrowStyle endArrowStyle = style->endArrowStyle();
		qreal startArrowSize = style->startArrowSize();
		qreal endArrowSize = style->endArrowSize();

		// Add line
		drawPath.moveTo(p1);
		drawPath.lineTo(p2);

		// Add arrows
		if (pen.style() != Qt::NoPen)
		{
			if (lineLength > startArrowSize)
				drawPath.addPath(style->arrowShape(startArrowStyle, startArrowSize, p1, lineAngle));
			if (lineLength > endArrowSize)
				drawPath.addPath(style->arrowShape(endArrowStyle, endArrowSize, p2, 180 + lineAngle));
		}

		// Determine outline path
		shape = strokePath(drawPath, pen);
	}

	return shape;
}

bool DrawingLineItem::isValid() const
{
	QList<DrawingItemPoint*> points = DrawingLineItem::points();
	return (points.size() >= 2 && points[0]->position() != points[1]->position());
}

//==================================================================================================

void DrawingLineItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		QList<DrawingItemPoint*> points = DrawingLineItem::points();
		QPointF p1 = points[0]->position();
		QPointF p2 = points[1]->position();
		qreal lineLength = qSqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y()));
		qreal lineAngle = 180 * qAtan2(p2.y() - p1.y(), p2.x() - p1.x()) / 3.141592654;

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		DrawingItemStyle::ArrowStyle startArrowStyle = style->startArrowStyle();
		DrawingItemStyle::ArrowStyle endArrowStyle = style->endArrowStyle();
		qreal startArrowSize = style->startArrowSize();
		qreal endArrowSize = style->endArrowSize();

		// Draw line
		painter->setBrush(Qt::transparent);
		painter->setPen(pen);
		painter->drawLine(p1, p2);

		// Draw arrows
		if (pen.style() != Qt::NoPen)
		{
			if (lineLength > startArrowSize)
				style->drawArrow(painter, startArrowStyle, startArrowSize, p1, lineAngle, pen, sceneBrush);
			if (lineLength > endArrowSize)
				style->drawArrow(painter, endArrowStyle, endArrowSize, p2, 180 + lineAngle, pen, sceneBrush);
		}

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingLineItem::resize(DrawingItemPoint* itemPoint, const QPointF& parentPos)
{
	DrawingItem::resize(itemPoint, parentPos);

	QList<DrawingItemPoint*> points = DrawingLineItem::points();
	DrawingItemPoint* startPoint = points[0];
	DrawingItemPoint* endPoint = points[1];
	DrawingItemPoint* midPoint = points[2];

	midPoint->setPosition((startPoint->position() + endPoint->position()) / 2);
}
