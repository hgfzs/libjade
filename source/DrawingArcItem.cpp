/* DrawingArcItem.cpp
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

#include "DrawingArcItem.h"
#include "DrawingWidget.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"

DrawingArcItem::DrawingArcItem() : DrawingItem()
{
	DrawingItemPoint::Flags flags =
		DrawingItemPoint::Control | DrawingItemPoint::Connection | DrawingItemPoint::Free;
	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));	// start point
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

DrawingArcItem::DrawingArcItem(const DrawingArcItem& item) : DrawingItem(item) { }

DrawingArcItem::~DrawingArcItem() { }

//==================================================================================================

DrawingItem* DrawingArcItem::copy() const
{
	return new DrawingArcItem(*this);
}

//==================================================================================================

void DrawingArcItem::setArc(const QLineF& line)
{
	QList<DrawingItemPoint*> points = DrawingArcItem::points();

	points[0]->setPos(line.p1());
	points[1]->setPos(line.p2());
}

void DrawingArcItem::setArc(qreal x1, qreal y1, qreal x2, qreal y2)
{
	setArc(QLineF(x1, y1, x2, y2));
}

QLineF DrawingArcItem::arc() const
{
	QLineF line;

	QList<DrawingItemPoint*> points = DrawingArcItem::points();
	line.setP1(points.first()->pos());
	line.setP2(points.last()->pos());

	return line;
}

//==================================================================================================

QRectF DrawingArcItem::boundingRect() const
{
	QRectF rect;

	if (isValid())
	{
		QList<DrawingItemPoint*> points = DrawingArcItem::points();
		QPointF p1 = points.first()->pos();
		QPointF p2 = points.last()->pos();
		qreal penWidth = style()->valueLookup(DrawingItemStyle::PenWidth).toReal();

		rect = QRectF(qMin(p1.x(), p2.x()), qMin(p1.y(), p2.y()), qAbs(p1.x() - p2.x()), qAbs(p1.y() - p2.y()));
		rect.adjust(-penWidth/2, -penWidth/2, penWidth/2, penWidth/2);
	}

	return rect;
}

QPainterPath DrawingArcItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;

		QList<DrawingItemPoint*> points = DrawingArcItem::points();
		QPointF p1 = points.first()->pos();
		QPointF p2 = points.last()->pos();
		qreal lineLength = qSqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y()));
		QRectF arcRect = DrawingArcItem::arcRect();
		qreal arcStartAngle = DrawingArcItem::arcStartAngle();

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		DrawingItemStyle::ArrowStyle startArrowStyle = style->startArrowStyle();
		DrawingItemStyle::ArrowStyle endArrowStyle = style->endArrowStyle();
		qreal startArrowSize = style->startArrowSize();
		qreal endArrowSize = style->endArrowSize();
		
		// Add arc
		drawPath.arcMoveTo(arcRect, arcStartAngle);
		drawPath.arcTo(arcRect, arcStartAngle, 90);

		// Add arrows
		if (pen.style() != Qt::NoPen)
		{
			if (lineLength > startArrowSize)
				drawPath.addPath(style->arrowShape(startArrowStyle, startArrowSize, p1, startArrowAngle()));
			if (lineLength > endArrowSize)
				drawPath.addPath(style->arrowShape(endArrowStyle, endArrowSize, p2, endArrowAngle()));
		}

		// Determine outline path
		pen.setWidthF(qMax(pen.widthF(), minimumPenWidth()));
		shape = strokePath(drawPath, pen);
	}

	return shape;
}

bool DrawingArcItem::isValid() const
{
	QList<DrawingItemPoint*> points = DrawingArcItem::points();
	return (points.size() >= 2 && points.first()->pos() != points.last()->pos());
}

//==================================================================================================

void DrawingArcItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		QList<DrawingItemPoint*> points = DrawingArcItem::points();
		QPointF p1 = points.first()->pos();
		QPointF p2 = points.last()->pos();
		qreal lineLength = qSqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y()));
		QRectF arcRect = DrawingArcItem::arcRect();
		qreal arcStartAngle = DrawingArcItem::arcStartAngle();

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		DrawingItemStyle::ArrowStyle startArrowStyle = style->startArrowStyle();
		DrawingItemStyle::ArrowStyle endArrowStyle = style->endArrowStyle();
		qreal startArrowSize = style->startArrowSize();
		qreal endArrowSize = style->endArrowSize();

		// Draw line
		QPainterPath drawPath;
		drawPath.arcMoveTo(arcRect, arcStartAngle);
		drawPath.arcTo(arcRect, arcStartAngle, 90);

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

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
}

//==================================================================================================

void DrawingArcItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	DrawingItem::resizeItem(itemPoint, scenePos);

	// Adjust position of item and item points so that point(0)->pos() == QPointF(0, 0)
	QList<DrawingItemPoint*> points = DrawingArcItem::points();
	QPointF deltaPos = -points.first()->pos();
	QPointF pointScenePos = mapToScene(points.first()->pos());

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		(*pointIter)->setPos((*pointIter)->pos() + deltaPos);

	setPos(pointScenePos);
}

//==================================================================================================

bool DrawingArcItem::newItemCopyEvent()
{
	QList<DrawingItemPoint*> points = DrawingItem::points();

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		(*pointIter)->setPos(0, 0);

	return true;
}

void DrawingArcItem::newMouseMoveEvent(DrawingMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		DrawingWidget* drawing = DrawingItem::drawing();
		QList<DrawingItemPoint*> points = DrawingArcItem::points();
		DrawingItemPoint* endPoint = points[1];

		QPointF newPos = event->scenePos();
		if (drawing) newPos = drawing->roundToGrid(newPos);

		resizeItem(endPoint, newPos);
	}
	else DrawingItem::newMouseMoveEvent(event);
}

bool DrawingArcItem::newMouseReleaseEvent(DrawingMouseEvent* event)
{
	Q_UNUSED(event);
	return isValid();
}

//==================================================================================================

QRectF DrawingArcItem::arcRect() const
{
	QLineF line = arc();

	qreal width = 2 * qAbs(line.x2() - line.x1());
	qreal height = 2 * qAbs(line.y2() - line.y1());

	qreal left = qMin(line.x1(), line.x2());
	qreal top = qMin(line.y1(), line.y2());
	if (line.y1() >= line.y2()) left -= width / 2;
	if (line.x1() < line.x2()) top -= height / 2;

	return QRectF(left, top, width, height);
}

qreal DrawingArcItem::arcStartAngle() const
{
	qreal startAngle = 0;
	QLineF line = arc();

	if (line.x1() < line.x2())
	{
		if (line.y1() < line.y2()) startAngle = 180;
		else startAngle = 270;
	}
	else
	{
		if (line.y1() < line.y2()) startAngle = 90;
		else startAngle = 0;
	}

	return startAngle;
}

//==================================================================================================

QPointF DrawingArcItem::pointFromAngle(qreal angle) const
{
	QPointF ellipsePoint;

	QRectF rect = arcRect();

	angle *= 3.141592654 / 180;

	ellipsePoint.setX(qAbs(rect.width() / 2) * qCos(angle) + rect.center().x());
	ellipsePoint.setY(-qAbs(rect.height() / 2) * qSin(angle) + rect.center().y());

	return ellipsePoint;
}

qreal DrawingArcItem::startArrowAngle() const
{
	QLineF startLine(arc().p1(), pointFromAngle(arcStartAngle() + 20));
	return -startLine.angle();
}

qreal DrawingArcItem::endArrowAngle() const
{
	QLineF endLine(arc().p2(), pointFromAngle(arcStartAngle() + 70));
	return -endLine.angle();
}
