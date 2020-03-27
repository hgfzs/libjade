/* DrawingPolylineItem.cpp
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

#include "DrawingPolylineItem.h"
#include "DrawingItemPoint.h"
#include <QPainter>
#include <QtMath>

DrawingPolylineItem::DrawingPolylineItem() : DrawingItem()
{
	mPen = QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	mStartArrow = DrawingArrow(DrawingArrow::None, 100.0);
	mEndArrow = DrawingArrow(DrawingArrow::None, 100.0);
	mStartArrow.setVector(QPointF(0, 0), mPen.widthF(), 0, 0);
	mEndArrow.setVector(QPointF(0, 0), mPen.widthF(), 0, 180);

	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanInsertPoints | CanRemovePoints |
			 CanSelect | CanDelete);

	DrawingItemPoint::Flags flags =
		DrawingItemPoint::Control | DrawingItemPoint::Connection | DrawingItemPoint::Free;
	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));	// start point
	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));	// end point

	updateGeometry();
}

DrawingPolylineItem::DrawingPolylineItem(const DrawingPolylineItem& item) : DrawingItem(item)
{
	mPolyline = item.mPolyline;
	mPen = item.mPen;
	mStartArrow = item.mStartArrow;
	mEndArrow = item.mEndArrow;

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
	mStrokePath = item.mStrokePath;
}

DrawingPolylineItem::~DrawingPolylineItem() { }

//==================================================================================================

DrawingItem* DrawingPolylineItem::copy() const
{
	return new DrawingPolylineItem(*this);
}

//==================================================================================================

void DrawingPolylineItem::setPolyline(const QPolygonF& polygon)
{
	mPolyline = polygon;

	// Update points
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
	for(int i = 0; i < polygon.size(); i++) points[i]->setPosition(polygon[i]);

	// Update arrows
	QPointF p0 = mPolyline[0];
	QPointF p1 = mPolyline[1];
	QPointF p2 = mPolyline[mPolyline.size()-2];
	QPointF p3 = mPolyline[mPolyline.size()-1];
	qreal firstLineLength = qSqrt((p1.x() - p0.x()) * (p1.x() - p0.x()) + (p1.y() - p0.y()) * (p1.y() - p0.y()));
	qreal lastLineLength = qSqrt((p3.x() - p2.x()) * (p3.x() - p2.x()) + (p3.y() - p2.y()) * (p3.y() - p2.y()));
	qreal firstLineAngle = 180 * qAtan2(p1.y() - p0.y(), p1.x() - p0.x()) / 3.141592654;
	qreal lastLineAngle = 180 * qAtan2(p3.y() - p2.y(), p3.x() - p2.x()) / 3.141592654;

	mStartArrow.setVector(p0, mStartArrow.vectorPenWidth(), firstLineLength, firstLineAngle);
	mEndArrow.setVector(p3, mEndArrow.vectorPenWidth(), lastLineLength, 180 + lastLineAngle);

	updateGeometry();
}

QPolygonF DrawingPolylineItem::polyline() const
{
	return mPolyline;
}

//==================================================================================================

void DrawingPolylineItem::setPen(const QPen& pen)
{
	mPen = pen;
	mStartArrow.setVector(mStartArrow.vectorPosition(), mPen.widthF(),
		mStartArrow.vectorLength(), mStartArrow.vectorAngle());
	mEndArrow.setVector(mEndArrow.vectorPosition(), mPen.widthF(),
		mEndArrow.vectorLength(), mEndArrow.vectorAngle());
	updateGeometry();
}

QPen DrawingPolylineItem::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingPolylineItem::setStartArrow(const DrawingArrow& arrow)
{
	mStartArrow.setStyle(arrow.style());
	mStartArrow.setSize(arrow.size());
	updateGeometry();
}

void DrawingPolylineItem::setEndArrow(const DrawingArrow& arrow)
{
	mEndArrow.setStyle(arrow.style());
	mEndArrow.setSize(arrow.size());
	updateGeometry();
}

DrawingArrow DrawingPolylineItem::startArrow() const
{
	return mStartArrow;
}

DrawingArrow DrawingPolylineItem::endArrow() const
{
	return mEndArrow;
}

//==================================================================================================

void DrawingPolylineItem::setProperties(const QHash<QString,QVariant>& properties)
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

QHash<QString,QVariant> DrawingPolylineItem::properties() const
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

QRectF DrawingPolylineItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingPolylineItem::shape() const
{
	return mShape;
}

bool DrawingPolylineItem::isValid() const
{
	QRectF rect = mPolyline.boundingRect();
	return (rect.width() != 0 || rect.height() != 0);
}

//==================================================================================================

void DrawingPolylineItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw line
		painter->setBrush(Qt::transparent);
		painter->setPen(mPen);
		painter->drawPath(mStrokePath);

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

void DrawingPolylineItem::resize(DrawingItemPoint* point, const QPointF& pos)
{
	DrawingItem::resize(point, pos);

	QPolygonF polyline;

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	for(auto pointIter = points.begin(), pointEnd = points.end(); pointIter != pointEnd; pointIter++)
		polyline << (*pointIter)->position();

	setPolyline(polyline);
}

//==================================================================================================

DrawingItemPoint* DrawingPolylineItem::pointToInsert(const QPointF& pos, int& index)
{
	DrawingItemPoint* pointToInsert = new DrawingItemPoint(
		pos, DrawingItemPoint::Control | DrawingItemPoint::Connection);

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

DrawingItemPoint* DrawingPolylineItem::pointToRemove(const QPointF& pos)
{
	DrawingItemPoint* pointToRemove = nullptr;

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	if (points.size() > 2)
	{
		pointToRemove = pointNearest(pos);

		if (pointToRemove && (pointToRemove == points.first() || pointToRemove == points.last()))
			pointToRemove = nullptr;
	}

	return pointToRemove;
}

//==================================================================================================

void DrawingPolylineItem::updateGeometry()
{
	mBoundingRect = QRectF();
	mShape = QPainterPath();
	mStrokePath = QPainterPath();

	if (isValid())
	{
		qreal halfPenWidth = mPen.widthF() / 2;

		// Bounding rect
		mBoundingRect = mPolyline.boundingRect().normalized();
		mBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		// Shape
		mStrokePath.moveTo(mPolyline.first());
		for(auto pointIter = mPolyline.begin() + 1; pointIter != mPolyline.end(); pointIter++)
		{
			mStrokePath.lineTo(*pointIter);
			mStrokePath.moveTo(*pointIter);
		}

		mShape = strokePath(mStrokePath, mPen);

		if (mPen.style() != Qt::NoPen)
		{
			mShape = mShape.united(mStartArrow.shape());
			mShape = mShape.united(mEndArrow.shape());
		}
	}
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
