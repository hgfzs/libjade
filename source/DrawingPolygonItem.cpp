/* DrawingPolygonItem.cpp
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

#include "DrawingPolygonItem.h"
#include "DrawingItemPoint.h"
#include <QPainter>
#include <QtMath>

DrawingPolygonItem::DrawingPolygonItem() : DrawingItem()
{
	mPen = QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	mBrush = Qt::white;

	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanInsertPoints | CanRemovePoints |
			 CanSelect | CanDelete);

	DrawingItemPoint::Flags flags = (DrawingItemPoint::Control | DrawingItemPoint::Connection);
	for(int i = 0; i < 3; i++) addPoint(new DrawingItemPoint(QPointF(0, 0), flags));

	QPolygonF polygon;
	polygon << QPointF(-250, -250) << QPointF(250, 0) << QPointF(-250, 250);
	setPolygon(polygon);
}

DrawingPolygonItem::DrawingPolygonItem(const DrawingPolygonItem& item) : DrawingItem(item)
{
	mPolygon = item.mPolygon;
	mPen = item.mPen;
	mBrush = item.mBrush;

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
}

DrawingPolygonItem::~DrawingPolygonItem() { }

//==================================================================================================

DrawingItem* DrawingPolygonItem::copy() const
{
	return new DrawingPolygonItem(*this);
}

//==================================================================================================

void DrawingPolygonItem::setPolygon(const QPolygonF& polygon)
{
	mPolygon = polygon;

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
	for(int i = 0; i < polygon.size(); i++)	points[i]->setPosition(polygon[i]);

	updateGeometry();
}

QPolygonF DrawingPolygonItem::polygon() const
{
	return mPolygon;
}

//==================================================================================================

void DrawingPolygonItem::setPen(const QPen& pen)
{
	mPen = pen;
	updateGeometry();
}

QPen DrawingPolygonItem::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingPolygonItem::setBrush(const QBrush& brush)
{
	mBrush = brush;
	updateGeometry();
}

QBrush DrawingPolygonItem::brush() const
{
	return mBrush;
}

//==================================================================================================

void DrawingPolygonItem::setProperties(const QHash<QString,QVariant>& properties)
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

	updateGeometry();
}

QHash<QString,QVariant> DrawingPolygonItem::properties() const
{
	QHash<QString,QVariant> properties;

	properties["pen-style"] = static_cast<uint>(mPen.style());
	properties["pen-color"] = mPen.brush().color();
	properties["pen-width"] = mPen.widthF();
	properties["pen-cap-style"] = static_cast<uint>(mPen.capStyle());
	properties["pen-join-style"] = static_cast<uint>(mPen.joinStyle());

	properties["brush-color"] = mBrush.color();

	return properties;
}

//==================================================================================================

QRectF DrawingPolygonItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingPolygonItem::shape() const
{
	return mShape;
}

bool DrawingPolygonItem::isValid() const
{
	QRectF rect = mPolygon.boundingRect();
	return (rect.width() != 0 || rect.height() != 0);
}

//==================================================================================================

void DrawingPolygonItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw polygon
		painter->setBrush(mBrush);
		painter->setPen(mPen);
		painter->drawPolygon(mPolygon);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingPolygonItem::resize(DrawingItemPoint* point, const QPointF& pos)
{
	DrawingItem::resize(point, pos);

	QPolygonF polygon;

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	for(auto pointIter = points.begin(), pointEnd = points.end(); pointIter != pointEnd; pointIter++)
		polygon << (*pointIter)->position();

	setPolygon(polygon);
}

//==================================================================================================

DrawingItemPoint* DrawingPolygonItem::itemPointToInsert(const QPointF& pos, int& index)
{
	DrawingItemPoint* pointToInsert = new DrawingItemPoint(
		pos, DrawingItemPoint::Control | DrawingItemPoint::Connection);

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	qreal distance = 0;
	qreal minimumDistance = distanceFromPointToLineSegment(pointToInsert->position(),
		QLineF(points[points.size()-1]->position(), points[0]->position()));

	index = points.size();

	for(int i = 0; i < points.size() - 1; i++)
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

DrawingItemPoint* DrawingPolygonItem::itemPointToRemove(const QPointF& pos)
{
	DrawingItemPoint* pointToRemove = nullptr;

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	if (points.size() > 3)
	{
		pointToRemove = pointNearest(pos);

		//if (pointToRemove && (pointToRemove == points.first() || pointToRemove == points.last()))
		//	pointToRemove = nullptr;
	}

	return pointToRemove;
}

//==================================================================================================

void DrawingPolygonItem::updateGeometry()
{
	mBoundingRect = QRectF();
	mShape = QPainterPath();

	if (isValid())
	{
		qreal halfPenWidth = mPen.widthF() / 2;
		QPainterPath drawPath;

		// Bounding rect
		mBoundingRect = mPolygon.boundingRect().normalized();
		mBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		// Shape
		drawPath.addPolygon(polygon());
		drawPath.closeSubpath();

		mShape = strokePath(drawPath, mPen);
		if (mBrush.color().alpha() > 0) mShape.addPath(drawPath);
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
