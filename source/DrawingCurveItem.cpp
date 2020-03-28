/* DrawingCurveItem.cpp
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

#include "DrawingCurveItem.h"
#include "DrawingItemPoint.h"
#include <QPainter>
#include <QtMath>

DrawingCurveItem::DrawingCurveItem() : DrawingItem()
{
	mPen = QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	mStartArrow = DrawingArrow(DrawingArrow::None, 100.0);
	mEndArrow = DrawingArrow(DrawingArrow::None, 100.0);
	mStartArrow.setVector(QPointF(0, 0), mPen.widthF(), 0, 0);
	mEndArrow.setVector(QPointF(0, 0), mPen.widthF(), 0, 180);

	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanSelect | CanDelete);

	DrawingItemPoint::Flags flags =
		DrawingItemPoint::Control | DrawingItemPoint::Connection | DrawingItemPoint::Free;
	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));						// start point
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Control));	// control point for start
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Control));	// control point for end
	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));						// end point

	setCurve(QPointF(-250, -250), QPointF(0, -250), QPointF(0, 250), QPointF(250, 250));
}

DrawingCurveItem::DrawingCurveItem(const DrawingCurveItem& item) : DrawingItem(item)
{
	mStartPos = item.mStartPos;
	mStartControlPos = item.mStartControlPos;
	mEndControlPos = item.mEndControlPos;
	mEndPos = item.mEndPos;

	mPen = item.mPen;
	mStartArrow = item.mStartArrow;
	mEndArrow = item.mEndArrow;

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
	mStrokeRect = item.mStrokeRect;
	mStrokeShape = item.mStrokeShape;
}

DrawingCurveItem::~DrawingCurveItem() { }

//==================================================================================================

DrawingItem* DrawingCurveItem::copy() const
{
	return new DrawingCurveItem(*this);
}

//==================================================================================================

void DrawingCurveItem::setCurve(const QPointF& p1, const QPointF& controlP1,
	const QPointF& controlP2, const QPointF& p2)
{
	mStartPos = p1;
	mStartControlPos = controlP1;
	mEndControlPos = controlP2;
	mEndPos = p2;

	// Update points
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();
	if (points.size() >= 4)
	{
		points[0]->setPosition(p1);
		points[1]->setPosition(controlP1);
		points[2]->setPosition(controlP2);
		points[3]->setPosition(p2);
	}

	// Update arrows
	qreal length = qSqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) +
		(p2.y() - p1.y()) * (p2.y() - p1.y()));

	mStartArrow.setVector(p1, mStartArrow.vectorPenWidth(), length, startArrowAngle());
	mEndArrow.setVector(p2, mEndArrow.vectorPenWidth(), length, 180 + endArrowAngle());

	updateGeometry();
}

QPointF DrawingCurveItem::curveStartPos() const
{
	return mStartPos;
}

QPointF DrawingCurveItem::curveStartControlPos() const
{
	return mStartControlPos;
}

QPointF DrawingCurveItem::curveEndControlPos() const
{
	return mEndControlPos;
}

QPointF DrawingCurveItem::curveEndPos() const
{
	return mEndPos;
}

//==================================================================================================

void DrawingCurveItem::setPen(const QPen& pen)
{
	mPen = pen;
	mStartArrow.setVector(mStartArrow.vectorPosition(), mPen.widthF(),
		mStartArrow.vectorLength(), mStartArrow.vectorAngle());
	mEndArrow.setVector(mEndArrow.vectorPosition(), mPen.widthF(),
		mEndArrow.vectorLength(), mEndArrow.vectorAngle());
	updateGeometry();
}

QPen DrawingCurveItem::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingCurveItem::setStartArrow(const DrawingArrow& arrow)
{
	mStartArrow.setStyle(arrow.style());
	mStartArrow.setSize(arrow.size());
	updateGeometry();
}

void DrawingCurveItem::setEndArrow(const DrawingArrow& arrow)
{
	mEndArrow.setStyle(arrow.style());
	mEndArrow.setSize(arrow.size());
	updateGeometry();
}

DrawingArrow DrawingCurveItem::startArrow() const
{
	return mStartArrow;
}

DrawingArrow DrawingCurveItem::endArrow() const
{
	return mEndArrow;
}

//==================================================================================================

void DrawingCurveItem::setProperties(const QHash<QString,QVariant>& properties)
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

QHash<QString,QVariant> DrawingCurveItem::properties() const
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

QRectF DrawingCurveItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingCurveItem::shape() const
{
	return mShape;
}

bool DrawingCurveItem::isValid() const
{
	return (mStrokeRect.width() != 0 || mStrokeRect.height() != 0);
}

//==================================================================================================

void DrawingCurveItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw curve
		painter->setBrush(Qt::transparent);
		painter->setPen(mPen);
		painter->drawPath(mStrokeShape);

		// Draw arrows
		if (mPen.style() != Qt::NoPen)
		{
			mStartArrow.render(painter, sceneBrush);
			mEndArrow.render(painter, sceneBrush);
		}

		// Draw control lines
		QList<DrawingItemPoint*> points = DrawingCurveItem::points();

		if (isSelected() && points.size() >= 4)
		{
			QPen pen = mPen;

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

void DrawingCurveItem::resize(DrawingItemPoint* point, const QPointF& pos)
{
	DrawingItem::resize(point, pos);

	QList<DrawingItemPoint*> points = DrawingCurveItem::points();

	if (points.size() >= 4)
	{
		QPointF p1 = points[0]->position();
		QPointF controlP1 = points[1]->position();
		QPointF controlP2 = points[2]->position();
		QPointF p2 = points[3]->position();
		int pointIndex = points.indexOf(point);

		if (pointIndex == 0)
			controlP1 = p1 + (mStartControlPos - mStartPos);
		else if (pointIndex == 3)
			controlP2 = p2 + (mEndControlPos - mEndPos);

		setCurve(p1, controlP1, controlP2, p2);
	}
}

//==================================================================================================

void DrawingCurveItem::updateGeometry()
{
	mBoundingRect = QRectF();
	mShape = QPainterPath();
	mStrokeRect = QRectF();
	mStrokeShape = QPainterPath();

	// Stroke path and rect
	mStrokeShape.moveTo(mStartPos);
	mStrokeShape.cubicTo(mStartControlPos, mEndControlPos, mEndPos);

	mStrokeRect = mStrokeShape.boundingRect().normalized();

	if (isValid())
	{
		qreal halfPenWidth = mPen.widthF() / 2;

		// Bounding rect
		mBoundingRect = mStrokeRect;
		mBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		// Shape
		mShape = strokePath(mStrokeShape, mPen);

		if (mPen.style() != Qt::NoPen)
		{
			mShape = mShape.united(mStartArrow.shape());
			mShape = mShape.united(mEndArrow.shape());
		}
	}
}

//==================================================================================================

QPointF DrawingCurveItem::pointFromRatio(qreal ratio) const
{
	QPointF position;

	position.setX((1 - ratio)*(1 - ratio)*(1 - ratio) * mStartPos.x() +
		3*ratio*(1 - ratio)*(1 - ratio) * mStartControlPos.x() +
		3*ratio*ratio*(1 - ratio) * mEndControlPos.x() +
		ratio*ratio*ratio * mEndPos.x());

	position.setY((1 - ratio)*(1 - ratio)*(1 - ratio) * mStartPos.y() +
		3*ratio*(1 - ratio)*(1 - ratio) * mStartControlPos.y() +
		3*ratio*ratio*(1 - ratio) * mEndControlPos.y() +
		ratio*ratio*ratio * mEndPos.y());

	return position;
}

qreal DrawingCurveItem::startArrowAngle() const
{
	QLineF startLine(mStartPos, pointFromRatio(0.05));
	return -startLine.angle();
}

qreal DrawingCurveItem::endArrowAngle() const
{
	QLineF endLine(mEndPos, pointFromRatio(0.95));
	return -endLine.angle();
}
