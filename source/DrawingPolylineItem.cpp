/* DrawingPolylineItem.cpp
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

#include "DrawingPolylineItem.h"
#include "DrawingWidget.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"
#include "DrawingUndo.h"

DrawingPolylineItem::DrawingPolylineItem() : DrawingItem()
{
	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanInsertPoints | CanRemovePoints | CanSelect);

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

DrawingPolylineItem::DrawingPolylineItem(const DrawingPolylineItem& item) : DrawingItem(item) { }

DrawingPolylineItem::~DrawingPolylineItem() { }

//==================================================================================================

DrawingItem* DrawingPolylineItem::copy() const
{
	return new DrawingPolylineItem(*this);
}

//==================================================================================================

void DrawingPolylineItem::setPolyline(const QPolygonF& polygon)
{
	if (polygon.size() >= 2)
	{
		while (points().size() < polygon.size())
			insertPoint(1, new DrawingItemPoint(QPointF(), DrawingItemPoint::Control | DrawingItemPoint::Connection));

		while (points().size() > polygon.size())
		{
			DrawingItemPoint* point = points()[1];
			removePoint(point);
			delete point;
		}
	}

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	for(int i = 0; i < polygon.size(); i++)
		points[i]->setPos(polygon[i]);
}

QPolygonF DrawingPolylineItem::polyline() const
{
	QPolygonF polygon;

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	for(int i = 0; i < points.size(); i++)
		polygon.append(points[i]->pos());

	return polygon;
}

//==================================================================================================

QRectF DrawingPolylineItem::boundingRect() const
{
	QRectF rect;

	if (isValid())
	{
		qreal penWidth = style()->valueLookup(DrawingItemStyle::PenWidth).toReal();

		rect = polyline().boundingRect();
		rect.adjust(-penWidth/2, -penWidth/2, penWidth/2, penWidth/2);
	}

	return rect;
}

QPainterPath DrawingPolylineItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;

		QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
		QPointF p0 = points[0]->pos();
		QPointF p1 = points[1]->pos();
		QPointF p2 = points[points.size()-2]->pos();
		QPointF p3 = points[points.size()-1]->pos();
		qreal firstLineLength = qSqrt((p1.x() - p0.x()) * (p1.x() - p0.x()) + (p1.y() - p0.y()) * (p1.y() - p0.y()));
		qreal lastLineLength = qSqrt((p3.x() - p2.x()) * (p3.x() - p2.x()) + (p3.y() - p2.y()) * (p3.y() - p2.y()));
		qreal firstLineAngle = 180 * qAtan2(p1.y() - p0.y(), p1.x() - p0.x()) / 3.141592654;
		qreal lastLineAngle = 180 * qAtan2(p3.y() - p2.y(), p3.x() - p2.x()) / 3.141592654;

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		DrawingItemStyle::ArrowStyle startArrowStyle = style->startArrowStyle();
		DrawingItemStyle::ArrowStyle endArrowStyle = style->endArrowStyle();
		qreal startArrowSize = style->startArrowSize();
		qreal endArrowSize = style->startArrowSize();
		
		// Add line
		drawPath.moveTo(points.first()->pos());
		for(auto pointIter = points.begin() + 1; pointIter != points.end(); pointIter++)
		{
			drawPath.lineTo((*pointIter)->pos());
			drawPath.moveTo((*pointIter)->pos());
		}

		// Add arrows
		if (pen.style() != Qt::NoPen)
		{
			if (firstLineLength > startArrowSize)
				drawPath.addPath(style->arrowShape(startArrowStyle, startArrowSize, p0, firstLineAngle));
			if (lastLineLength > endArrowSize)
				drawPath.addPath(style->arrowShape(endArrowStyle, endArrowSize, p3, 180 + lastLineAngle));
		}

		// Determine outline path
		pen.setWidthF(qMax(pen.widthF(), minimumPenWidth()));
		shape = strokePath(drawPath, pen);
	}

	return shape;
}

bool DrawingPolylineItem::isValid() const
{
	bool superfluous = true;

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	QPointF pos = points.first()->pos();

	for(auto pointIter = points.begin() + 1; superfluous && pointIter != points.end(); pointIter++)
		superfluous = (pos == (*pointIter)->pos());

	return !superfluous;
}

//==================================================================================================

void DrawingPolylineItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
		QPointF p0 = points[0]->pos();
		QPointF p1 = points[1]->pos();
		QPointF p2 = points[points.size()-2]->pos();
		QPointF p3 = points[points.size()-1]->pos();
		qreal firstLineLength = qSqrt((p1.x() - p0.x()) * (p1.x() - p0.x()) + (p1.y() - p0.y()) * (p1.y() - p0.y()));
		qreal lastLineLength = qSqrt((p3.x() - p2.x()) * (p3.x() - p2.x()) + (p3.y() - p2.y()) * (p3.y() - p2.y()));
		qreal firstLineAngle = 180 * qAtan2(p1.y() - p0.y(), p1.x() - p0.x()) / 3.141592654;
		qreal lastLineAngle = 180 * qAtan2(p3.y() - p2.y(), p3.x() - p2.x()) / 3.141592654;

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		DrawingItemStyle::ArrowStyle startArrowStyle = style->startArrowStyle();
		DrawingItemStyle::ArrowStyle endArrowStyle = style->endArrowStyle();
		qreal startArrowSize = style->startArrowSize();
		qreal endArrowSize = style->startArrowSize();

		// Draw line
		QPainterPath drawPath;
		drawPath.moveTo(points.first()->pos());
		for(auto pointIter = points.begin() + 1; pointIter != points.end(); pointIter++)
		{
			drawPath.lineTo((*pointIter)->pos());
			drawPath.moveTo((*pointIter)->pos());
		}

		painter->setBrush(Qt::transparent);
		painter->setPen(pen);
		painter->drawPath(drawPath);

		// Draw arrows
		if (pen.style() != Qt::NoPen)
		{
			if (firstLineLength > startArrowSize)
				style->drawArrow(painter, startArrowStyle, startArrowSize, p0, firstLineAngle, pen, sceneBrush);
			if (lastLineLength > endArrowSize)
				style->drawArrow(painter, endArrowStyle, endArrowSize, p3, 180 + lastLineAngle, pen, sceneBrush);
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

void DrawingPolylineItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	DrawingItem::resizeItem(itemPoint, scenePos);

	// Adjust position of item and item points so that point(0)->pos() == QPointF(0, 0)
	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	QPointF deltaPos = -points.first()->pos();
	QPointF pointScenePos = mapToScene(points.first()->pos());

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		(*pointIter)->setPos((*pointIter)->pos() + deltaPos);

	setPos(pointScenePos);
}

void DrawingPolylineItem::insertItemPoint(const QPointF& scenePos)
{
	DrawingWidget* drawing = DrawingPolylineItem::drawing();

	if (drawing)
	{
		DrawingItemPoint* newPoint = new DrawingItemPoint(
			mapFromScene(drawing->roundToGrid(scenePos)),
			DrawingItemPoint::Control | DrawingItemPoint::Connection);

		QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
		int index = 1;
		qreal distance = 0;
		qreal minimumDistance = distanceFromPointToLineSegment(newPoint->pos(),
			QLineF(points[0]->pos(), points[1]->pos()));

		for(int i = 1; i < points.size() - 1; i++)
		{
			distance = distanceFromPointToLineSegment(newPoint->pos(),
				QLineF(points[i]->pos(), points[i+1]->pos()));
			if (distance < minimumDistance)
			{
				index = i+1;
				minimumDistance = distance;
			}
		}

		drawing->pushUndoCommand(new DrawingItemInsertPointCommand(drawing, this, newPoint, index));
	}
}

void DrawingPolylineItem::removeItemPoint(const QPointF& scenePos)
{
	DrawingWidget* drawing = DrawingPolylineItem::drawing();
	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();

	if (drawing && points.size() > 2)
	{
		DrawingItemPoint* pointToRemove = pointNearest(mapFromScene(scenePos));

		if (pointToRemove && pointToRemove != points.first() && pointToRemove != points.last())
			drawing->pushUndoCommand(new DrawingItemRemovePointCommand(drawing, this, pointToRemove));
	}
}

//==================================================================================================

bool DrawingPolylineItem::newItemCopyEvent()
{
	QList<DrawingItemPoint*> points = DrawingItem::points();

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		(*pointIter)->setPos(0, 0);

	return true;
}

void DrawingPolylineItem::newMouseMoveEvent(DrawingMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		DrawingWidget* drawing = DrawingItem::drawing();
		QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
		DrawingItemPoint* endPoint = points.last();

		QPointF newPos = event->scenePos();
		if (drawing) newPos = drawing->roundToGrid(newPos);

		resizeItem(endPoint, newPos);
	}
	else DrawingItem::newMouseMoveEvent(event);
}

bool DrawingPolylineItem::newMouseReleaseEvent(DrawingMouseEvent* event)
{
	Q_UNUSED(event);
	return isValid();
}

//==================================================================================================

qreal DrawingPolylineItem::distanceFromPointToLineSegment(const QPointF& point, const QLineF& line) const
{
	qreal distance = 1E10;
	qreal dotAbBc, dotBaAc, crossABC, distanceAB, distanceAC, distanceBC;

	// Let A = line point 0, B = line point 1, and C = point
	dotAbBc = (line.x2() - line.x1()) * (point.x() - line.x2()) +
			  (line.y2() - line.y1()) * (point.y() - line.y2());
	dotBaAc = (line.x1() - line.x2()) * (point.x() - line.x1()) +
			  (line.y1() - line.y2()) * (point.y() - line.y1());
	crossABC = (line.x2() - line.x1()) * (point.y() - line.y1()) -
			   (line.y2() - line.y1()) * (point.x() - line.x1());
	distanceAB = qSqrt( (line.x2() - line.x1()) * (line.x2() - line.x1()) +
						(line.y2() - line.y1()) * (line.y2() - line.y1()) );
	distanceAC = qSqrt( (point.x() - line.x1()) * (point.x() - line.x1()) +
						(point.y() - line.y1()) * (point.y() - line.y1()) );
	distanceBC = qSqrt( (point.x() - line.x2()) * (point.x() - line.x2()) +
						(point.y() - line.y2()) * (point.y() - line.y2()) );

	if (distanceAB != 0)
	{
		if (dotAbBc > 0) distance = distanceBC;
		else if (dotBaAc > 0) distance = distanceAC;
		else distance = qAbs(crossABC) / distanceAB;
	}

	return distance;
}
