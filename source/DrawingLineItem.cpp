/* DrawingLineItem.cpp
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

#include "DrawingLineItem.h"
#include "DrawingWidget.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"

DrawingLineItem::DrawingLineItem() : DrawingItem()
{
	DrawingItemPoint::Flags flags =
		DrawingItemPoint::Control | DrawingItemPoint::Connection | DrawingItemPoint::Free;
	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));	// start point
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Connection));	// mid point
	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));	// end point

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

	points[0]->setPos(line.p1());
	points[1]->setPos((line.p1() + line.p2()) / 2);
	points[2]->setPos(line.p2());
}

void DrawingLineItem::setLine(qreal x1, qreal y1, qreal x2, qreal y2)
{
	setLine(QLineF(x1, y1, x2, y2));
}

QLineF DrawingLineItem::line() const
{
	QLineF line;

	QList<DrawingItemPoint*> points = DrawingLineItem::points();
	line.setP1(points.first()->pos());
	line.setP2(points.last()->pos());

	return line;
}

//==================================================================================================

QRectF DrawingLineItem::boundingRect() const
{
	QRectF rect;

	if (isValid())
	{
		QList<DrawingItemPoint*> points = DrawingLineItem::points();
		QPointF p1 = points.first()->pos();
		QPointF p2 = points.last()->pos();
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
		QPointF p1 = points.first()->pos();
		QPointF p2 = points.last()->pos();
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
		pen.setWidthF(qMax(pen.widthF(), minimumPenWidth()));
		shape = strokePath(drawPath, pen);
	}

	return shape;
}

bool DrawingLineItem::isValid() const
{
	QList<DrawingItemPoint*> points = DrawingLineItem::points();
	return (points.size() >= 2 && points.first()->pos() != points.last()->pos());
}

//==================================================================================================

void DrawingLineItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		QList<DrawingItemPoint*> points = DrawingLineItem::points();
		QPointF p1 = points.first()->pos();
		QPointF p2 = points.last()->pos();
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

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
}

//==================================================================================================

void DrawingLineItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	QList<DrawingItemPoint*> points = DrawingLineItem::points();
	DrawingItemPoint* startPoint = points[0];
	DrawingItemPoint* midPoint = points[1];
	DrawingItemPoint* endPoint = points[2];

	DrawingItem::resizeItem(itemPoint, scenePos);

	midPoint->setPos((startPoint->pos() + endPoint->pos()) / 2);

	// Adjust position of item and item points so that point(0)->pos() == QPointF(0, 0)
	QPointF deltaPos = -points.first()->pos();
	QPointF pointScenePos = mapToScene(points.first()->pos());

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		(*pointIter)->setPos((*pointIter)->pos() + deltaPos);

	setPos(pointScenePos);
}

//==================================================================================================

bool DrawingLineItem::newItemCopyEvent()
{
	QList<DrawingItemPoint*> points = DrawingItem::points();

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		(*pointIter)->setPos(0, 0);

	return true;
}

void DrawingLineItem::newMouseMoveEvent(DrawingMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		DrawingWidget* drawing = DrawingItem::drawing();
		QList<DrawingItemPoint*> points = DrawingLineItem::points();
		DrawingItemPoint* endPoint = points[2];

		QPointF newPos = event->scenePos();
		if (drawing) newPos = drawing->roundToGrid(newPos);

		resizeItem(endPoint, newPos);
	}
	else DrawingItem::newMouseMoveEvent(event);
}

bool DrawingLineItem::newMouseReleaseEvent(DrawingMouseEvent* event)
{
	Q_UNUSED(event);
	return isValid();
}
