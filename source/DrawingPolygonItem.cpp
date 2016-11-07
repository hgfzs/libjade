/* DrawingPolygonItem.cpp
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

#include "DrawingPolygonItem.h"
#include "DrawingWidget.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"
#include "DrawingUndo.h"

DrawingPolygonItem::DrawingPolygonItem() : DrawingItem()
{
	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanInsertPoints | CanRemovePoints | CanSelect);

	DrawingItemPoint::Flags flags = DrawingItemPoint::Control | DrawingItemPoint::Connection;
	addPoint(new DrawingItemPoint(QPointF(-200, -200), flags));
	addPoint(new DrawingItemPoint(QPointF(200, 0), flags));
	addPoint(new DrawingItemPoint(QPointF(-200, 200), flags));

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

	style->setValue(DrawingItemStyle::BrushStyle,
		style->valueLookup(DrawingItemStyle::BrushStyle, QVariant((uint)Qt::SolidPattern)));
	style->setValue(DrawingItemStyle::BrushColor,
		style->valueLookup(DrawingItemStyle::BrushColor, QVariant(QColor(255, 255, 255))));
	style->setValue(DrawingItemStyle::BrushOpacity,
		style->valueLookup(DrawingItemStyle::BrushOpacity, QVariant(1.0)));
}

DrawingPolygonItem::DrawingPolygonItem(const DrawingPolygonItem& item) : DrawingItem(item) { }

DrawingPolygonItem::~DrawingPolygonItem() { }

//==================================================================================================

DrawingItem* DrawingPolygonItem::copy() const
{
	return new DrawingPolygonItem(*this);
}

//==================================================================================================

void DrawingPolygonItem::setPolygon(const QPolygonF& polygon)
{
	if (polygon.size() >= 3)
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

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	for(int i = 0; i < polygon.size(); i++)
		points[i]->setPos(polygon[i]);
}

QPolygonF DrawingPolygonItem::polygon() const
{
	QPolygonF polygon;

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	for(int i = 0; i < points.size(); i++) polygon.append(points[i]->pos());

	return polygon;
}


//==================================================================================================

QRectF DrawingPolygonItem::boundingRect() const
{
	QRectF rect;

	if (isValid())
	{
		qreal penWidth = style()->valueLookup(DrawingItemStyle::PenWidth).toReal();

		rect = polygon().boundingRect();
		rect.adjust(-penWidth/2, -penWidth/2, penWidth/2, penWidth/2);
	}

	return rect;
}

QPainterPath DrawingPolygonItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		QBrush brush = style->brush();

		// Add polygon
		drawPath.addPolygon(polygon());
		drawPath.closeSubpath();

		// Determine outline path
		pen.setWidthF(qMax(pen.widthF(), minimumPenWidth()));
		shape = strokePath(drawPath, pen);

		if (brush.color().alpha() > 0) shape = shape.united(drawPath);
	}

	return shape;
}

bool DrawingPolygonItem::isValid() const
{
	bool superfluous = true;

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	QPointF pos = points.first()->pos();

	for(auto pointIter = points.begin() + 1; superfluous && pointIter != points.end(); pointIter++)
		superfluous = (pos == (*pointIter)->pos());

	return !superfluous;
}

//==================================================================================================

void DrawingPolygonItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		QBrush brush = style->brush();

		// Draw polygon
		painter->setBrush(brush);
		painter->setPen(pen);
		painter->drawPolygon(polygon());

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
}

//==================================================================================================

void DrawingPolygonItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	DrawingItem::resizeItem(itemPoint, scenePos);

	// Adjust position of item and item points so that point(0)->pos() == QPointF(0, 0)
	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	QPointF deltaPos = -points.first()->pos();
	QPointF pointScenePos = mapToScene(points.first()->pos());

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		(*pointIter)->setPos((*pointIter)->pos() + deltaPos);

	setPos(pointScenePos);
}

void DrawingPolygonItem::insertItemPoint(const QPointF& scenePos)
{
	DrawingWidget* drawing = DrawingPolygonItem::drawing();

	if (drawing)
	{
		DrawingItemPoint* newPoint = new DrawingItemPoint(
			mapFromScene(drawing->roundToGrid(scenePos)),
		DrawingItemPoint::Control | DrawingItemPoint::Connection);

		QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
		int index = 1;
		qreal distance = 0;
		qreal minimumDistance = distanceFromPointToLineSegment(newPoint->pos(),
			QLineF(points[points.size()-1]->pos(), points[0]->pos()));

		index = points.size();
		for(int i = 0; i < points.size() - 1; i++)
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

void DrawingPolygonItem::removeItemPoint(const QPointF& scenePos)
{
	DrawingWidget* drawing = DrawingPolygonItem::drawing();
	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();

	if (drawing && points.size() > 3)
	{
		DrawingItemPoint* pointToRemove = pointNearest(mapFromScene(scenePos));

		if (pointToRemove)
			drawing->pushUndoCommand(new DrawingItemRemovePointCommand(drawing, this, pointToRemove));
	}
}

//==================================================================================================

qreal DrawingPolygonItem::distanceFromPointToLineSegment(const QPointF& point, const QLineF& line) const
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
