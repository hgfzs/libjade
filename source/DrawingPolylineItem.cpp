/* DrawingPolylineItem.cpp
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

#include "DrawingPolylineItem.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"

DrawingPolylineItem::DrawingPolylineItem() : DrawingItem()
{
	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanInsertPoints | CanRemovePoints | CanSelect | PlaceByMousePressAndRelease | AdjustPositionOnResize);

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
		points[i]->setPosition(polygon[i]);
}

QPolygonF DrawingPolylineItem::polyline() const
{
	QPolygonF polygon;

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	for(int i = 0; i < points.size(); i++)
		polygon.append(points[i]->position());

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
		QPointF p0 = points[0]->position();
		QPointF p1 = points[1]->position();
		QPointF p2 = points[points.size()-2]->position();
		QPointF p3 = points[points.size()-1]->position();
		qreal firstLineLength = qSqrt((p1.x() - p0.x()) * (p1.x() - p0.x()) + (p1.y() - p0.y()) * (p1.y() - p0.y()));
		qreal lastLineLength = qSqrt((p3.x() - p2.x()) * (p3.x() - p2.x()) + (p3.y() - p2.y()) * (p3.y() - p2.y()));
		qreal firstLineAngle = 180 * qAtan2(p1.y() - p0.y(), p1.x() - p0.x()) / 3.141592654;
		qreal lastLineAngle = 180 * qAtan2(p3.y() - p2.y(), p3.x() - p2.x()) / 3.141592654;

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		DrawingItemStyle::ArrowStyle startArrowStyle = style->startArrowStyle();
		DrawingItemStyle::ArrowStyle endArrowStyle = style->endArrowStyle();
		qreal startArrowSize = style->startArrowSize();
		qreal endArrowSize = style->endArrowSize();

		// Add line
		drawPath.moveTo(points.first()->position());
		for(auto pointIter = points.begin() + 1; pointIter != points.end(); pointIter++)
		{
			drawPath.lineTo((*pointIter)->position());
			drawPath.moveTo((*pointIter)->position());
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
		shape = strokePath(drawPath, pen);
	}

	return shape;
}

bool DrawingPolylineItem::isValid() const
{
	bool superfluous = true;

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	QPointF position = points.first()->position();

	for(auto pointIter = points.begin() + 1; superfluous && pointIter != points.end(); pointIter++)
		superfluous = (position == (*pointIter)->position());

	return !superfluous;
}

//==================================================================================================

void DrawingPolylineItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
		QPointF p0 = points[0]->position();
		QPointF p1 = points[1]->position();
		QPointF p2 = points[points.size()-2]->position();
		QPointF p3 = points[points.size()-1]->position();
		qreal firstLineLength = qSqrt((p1.x() - p0.x()) * (p1.x() - p0.x()) + (p1.y() - p0.y()) * (p1.y() - p0.y()));
		qreal lastLineLength = qSqrt((p3.x() - p2.x()) * (p3.x() - p2.x()) + (p3.y() - p2.y()) * (p3.y() - p2.y()));
		qreal firstLineAngle = 180 * qAtan2(p1.y() - p0.y(), p1.x() - p0.x()) / 3.141592654;
		qreal lastLineAngle = 180 * qAtan2(p3.y() - p2.y(), p3.x() - p2.x()) / 3.141592654;

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		DrawingItemStyle::ArrowStyle startArrowStyle = style->startArrowStyle();
		DrawingItemStyle::ArrowStyle endArrowStyle = style->endArrowStyle();
		qreal startArrowSize = style->startArrowSize();
		qreal endArrowSize = style->endArrowSize();

		// Draw line
		QPainterPath drawPath;
		drawPath.moveTo(points.first()->position());
		for(auto pointIter = points.begin() + 1; pointIter != points.end(); pointIter++)
		{
			drawPath.lineTo((*pointIter)->position());
			drawPath.moveTo((*pointIter)->position());
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
}

//==================================================================================================

DrawingItemPoint* DrawingPolylineItem::itemPointToInsert(const QPointF& itemPos, int& index)
{
	DrawingItemPoint* pointToInsert = new DrawingItemPoint(
		itemPos, DrawingItemPoint::Control | DrawingItemPoint::Connection);

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	qreal distance = 0;
	qreal minimumDistance = distanceFromPointToLineSegment(pointToInsert->position(),
		QLineF(points[0]->position(), points[1]->position()));

	index = 1;

	for(int i = 1; i < points.size() - 1; i++)
	{
		distance = distanceFromPointToLineSegment(pointToInsert->position(),
			QLineF(points[i]->position(), points[i+1]->position()));
		if (distance < minimumDistance)
		{
			index = i+1;
			minimumDistance = distance;
		}
	}

	return pointToInsert;
}

DrawingItemPoint* DrawingPolylineItem::itemPointToRemove(const QPointF& itemPos)
{
	DrawingItemPoint* pointToRemove = nullptr;

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	if (points.size() > 2)
	{
		DrawingItemPoint* pointToRemove = pointNearest(itemPos);

		if (pointToRemove && (pointToRemove == points.first() || pointToRemove == points.last()))
			pointToRemove = nullptr;
	}

	return pointToRemove;
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
