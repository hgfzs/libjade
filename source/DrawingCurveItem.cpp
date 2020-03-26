/* DrawingCurveItem.cpp
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

#include "DrawingCurveItem.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"

DrawingCurveItem::DrawingCurveItem() : DrawingItem()
{
	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanSelect | CanDelete);

	DrawingItemPoint::Flags flags =
		DrawingItemPoint::Control | DrawingItemPoint::Connection | DrawingItemPoint::Free;
	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));						// start point
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Control));	// control point for start
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Control));	// control point for end
	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));						// end point
	setCurve(QPointF(-200, -200), QPointF(0, -200), QPointF(0, 200), QPointF(200, 200));

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

DrawingCurveItem::DrawingCurveItem(const DrawingCurveItem& item) : DrawingItem(item) { }

DrawingCurveItem::~DrawingCurveItem() { }

//==================================================================================================

DrawingItem* DrawingCurveItem::copy() const
{
	return new DrawingCurveItem(*this);
}

//==================================================================================================

void DrawingCurveItem::setCurve(const QPointF& p1, const QPointF& controlP1, const QPointF& controlP2, const QPointF& p2)
{
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();
	points[0]->setPosition(p1);
	points[1]->setPosition(controlP1);
	points[2]->setPosition(controlP2);
	points[3]->setPosition(p2);
}

QPointF DrawingCurveItem::curveStartPos() const
{
	return points()[0]->position();
}

QPointF DrawingCurveItem::curveStartControlPos() const
{
	return points()[1]->position();
}

QPointF DrawingCurveItem::curveEndControlPos() const
{
	return points()[2]->position();
}

QPointF DrawingCurveItem::curveEndPos() const
{
	return points()[3]->position();
}

//==================================================================================================

QRectF DrawingCurveItem::boundingRect() const
{
	QPainterPath drawPath;
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();

	drawPath.moveTo(points[0]->position());
	drawPath.cubicTo(points[1]->position(), points[2]->position(), points[3]->position());

	qreal penWidth = style()->valueLookup(DrawingItemStyle::PenWidth).toReal();
	QRectF rect = drawPath.boundingRect();
	rect.adjust(-penWidth/2, -penWidth/2, penWidth/2, penWidth/2);

	return rect;
}

QPainterPath DrawingCurveItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;

		QList<DrawingItemPoint*> points = DrawingCurveItem::points();
		QPointF p1 = points.first()->position();
		QPointF p2 = points.last()->position();
		qreal lineLength = qSqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y()));

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		DrawingItemStyle::ArrowStyle startArrowStyle = style->startArrowStyle();
		DrawingItemStyle::ArrowStyle endArrowStyle = style->endArrowStyle();
		qreal startArrowSize = style->startArrowSize();
		qreal endArrowSize = style->endArrowSize();

		// Add arc
		drawPath.moveTo(points[0]->position());
		drawPath.cubicTo(points[1]->position(), points[2]->position(), points[3]->position());

		// Add arrows
		if (pen.style() != Qt::NoPen)
		{
			if (lineLength > startArrowSize)
				drawPath.addPath(style->arrowShape(startArrowStyle, startArrowSize, p1, startArrowAngle()));
			if (lineLength > endArrowSize)
				drawPath.addPath(style->arrowShape(endArrowStyle, endArrowSize, p2, endArrowAngle()));
		}

		// Determine outline path
		shape = strokePath(drawPath, pen);
	}

	return shape;
}

bool DrawingCurveItem::isValid() const
{
	return !boundingRect().isNull();
}

//==================================================================================================

void DrawingCurveItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		QList<DrawingItemPoint*> points = DrawingCurveItem::points();
		QPointF p1 = points.first()->position();
		QPointF p2 = points.last()->position();
		qreal lineLength = qSqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y()));

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		DrawingItemStyle::ArrowStyle startArrowStyle = style->startArrowStyle();
		DrawingItemStyle::ArrowStyle endArrowStyle = style->endArrowStyle();
		qreal startArrowSize = style->startArrowSize();
		qreal endArrowSize = style->endArrowSize();

		// Draw curve
		QPainterPath drawPath;
		drawPath.moveTo(points[0]->position());
		drawPath.cubicTo(points[1]->position(), points[2]->position(), points[3]->position());

		painter->setBrush(Qt::transparent);
		painter->setPen(pen);
		painter->drawPath(drawPath);

		// Draw arrows
		if (pen.style() != Qt::NoPen)
		{
			if (lineLength > startArrowSize)
				style->drawArrow(painter, startArrowStyle, startArrowSize, p1, startArrowAngle(), pen, sceneBrush);
			if (lineLength > endArrowSize)
				style->drawArrow(painter, endArrowStyle, endArrowSize, p2, endArrowAngle(), pen, sceneBrush);
		}

		// Draw control lines
		if (isSelected())
		{
			pen.setStyle(Qt::DotLine);
			pen.setWidthF(pen.widthF() * 0.75);
			painter->setBrush(Qt::transparent);
			painter->setPen(pen);
			painter->drawLine(points[0]->position(), points[1]->position());
			painter->drawLine(points[3]->position(), points[2]->position());
		}

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingCurveItem::resizeEvent(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();

	int pointIndex = points.indexOf(itemPoint);

	if (pointIndex == 0)
	{
		QPointF difference = points[1]->position() - points[0]->position();
		points[1]->setPosition(mapFromScene(scenePos) + difference);

	}
	else if (pointIndex == 3)
	{
		QPointF difference = points[2]->position() - points[3]->position();
		points[2]->setPosition(mapFromScene(scenePos) + difference);
	}

	DrawingItem::resizeEvent(itemPoint, scenePos);
}

//==================================================================================================

QPointF DrawingCurveItem::pointFromRatio(qreal ratio) const
{
	QPointF position;
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();

	QPointF p0 = points[0]->position();
	QPointF p1 = points[1]->position();
	QPointF p2 = points[2]->position();
	QPointF p3 = points[3]->position();

	position.setX((1 - ratio)*(1 - ratio)*(1 - ratio) * p0.x() +
		3*ratio*(1 - ratio)*(1 - ratio) * p1.x() +
		3*ratio*ratio*(1 - ratio) * p2.x() +
		ratio*ratio*ratio * p3.x());

	position.setY((1 - ratio)*(1 - ratio)*(1 - ratio) * p0.y() +
		3*ratio*(1 - ratio)*(1 - ratio) * p1.y() +
		3*ratio*ratio*(1 - ratio) * p2.y() +
		ratio*ratio*ratio * p3.y());

	return position;
}

qreal DrawingCurveItem::startArrowAngle() const
{
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();
	QLineF startLine(points[0]->position(), pointFromRatio(0.05));
	return -startLine.angle();
}

qreal DrawingCurveItem::endArrowAngle() const
{
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();
	QLineF endLine(points[3]->position(), pointFromRatio(0.95));
	return -endLine.angle();
}
