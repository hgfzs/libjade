/* DrawingPathItem.cpp
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

#include "DrawingPathItem.h"
#include "DrawingItemPoint.h"
#include <QPainter>

DrawingPathItem::DrawingPathItem() : DrawingItem()
{
	mRect = QRectF(0, 0, 0, 0);
	mPen = QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

	mName = "Path";

	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanSelect | CanDelete);

	DrawingItemPoint::Flags flags = (DrawingItemPoint::Control);
	for(int i = 0; i < 8; i++) addPoint(new DrawingItemPoint(QPointF(0, 0), flags));

	updateGeometry();
}

DrawingPathItem::DrawingPathItem(const DrawingPathItem& item) : DrawingItem(item)
{
	mRect = item.mRect;
	mPen = item.mPen;

	mName = item.mName;
	mPath = item.mPath;
	mPathRect = item.mPathRect;

	QList<DrawingItemPoint*> points = DrawingPathItem::points();
	QList<DrawingItemPoint*> otherItemPoints = item.points();
	for(int i = 8; i < points.size(); i++)
		mPathConnectionPoints[points[i]] = item.mPathConnectionPoints[otherItemPoints[i]];

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
	mTransformedPath = item.mTransformedPath;
}

DrawingPathItem::~DrawingPathItem() { }

//==================================================================================================

DrawingItem* DrawingPathItem::copy() const
{
	return new DrawingPathItem(*this);
}

//==================================================================================================

void DrawingPathItem::setRect(const QRectF& rect)
{
	mRect = rect;

	// Update points
	QList<DrawingItemPoint*> points = DrawingPathItem::points();
	if (points.size() >= 8)
	{
		points[TopLeft]->setPosition(rect.left(), rect.top());
		points[TopMiddle]->setPosition(rect.center().x(), rect.top());
		points[TopRight]->setPosition(rect.right(), rect.top());
		points[MiddleRight]->setPosition(rect.right(), rect.center().y());
		points[BottomRight]->setPosition(rect.right(), rect.bottom());
		points[BottomMiddle]->setPosition(rect.center().x(), rect.bottom());
		points[BottomLeft]->setPosition(rect.left(), rect.bottom());
		points[MiddleLeft]->setPosition(rect.left(), rect.center().y());
	}

	updateGeometry();
}

void DrawingPathItem::setRect(qreal left, qreal top, qreal width, qreal height)
{
	setRect(QRectF(left, top, width, height));
}

QRectF DrawingPathItem::rect() const
{
	return mRect;
}

//==================================================================================================

void DrawingPathItem::setPen(const QPen& pen)
{
	mPen = pen;
	updateGeometry();
}

QPen DrawingPathItem::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingPathItem::setProperties(const QHash<QString,QVariant>& properties)
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

	updateGeometry();
}

QHash<QString,QVariant> DrawingPathItem::properties() const
{
	QHash<QString,QVariant> properties;

	properties["pen-style"] = static_cast<uint>(mPen.style());
	properties["pen-color"] = mPen.brush().color();
	properties["pen-width"] = mPen.widthF();
	properties["pen-cap-style"] = static_cast<uint>(mPen.capStyle());
	properties["pen-join-style"] = static_cast<uint>(mPen.joinStyle());

	return properties;
}

//==================================================================================================

void DrawingPathItem::setName(const QString& name)
{
	mName = name;
}

QString DrawingPathItem::name() const
{
	return mName;
}

//==================================================================================================

void DrawingPathItem::setPath(const QPainterPath& path, const QRectF& pathRect)
{
	mPath = path;
	mPathRect = pathRect;
	updateGeometry();
}

QPainterPath DrawingPathItem::path() const
{
	return mPath;
}

QRectF DrawingPathItem::pathRect() const
{
	return mPathRect;
}

//==================================================================================================

void DrawingPathItem::addConnectionPoint(const QPointF& pathPos)
{
	bool existingPointFound = false;
	QPointF itemPos = mapFromPath(pathPos);
	QList<DrawingItemPoint*> points = DrawingPathItem::points();

	for(auto pointIter = points.begin(); !existingPointFound && pointIter != points.end(); pointIter++)
	{
		existingPointFound = ((*pointIter)->position() == itemPos);
		if (existingPointFound) (*pointIter)->setFlags((*pointIter)->flags() | DrawingItemPoint::Connection);
	}

	if (!existingPointFound)
	{
		DrawingItemPoint* newPoint = new DrawingItemPoint(itemPos, DrawingItemPoint::Connection);
		mPathConnectionPoints[newPoint] = pathPos;
		addPoint(newPoint);
	}
}

	void DrawingPathItem::addConnectionPoints(const QPolygonF& pathPos)
{
	for(auto posIter = pathPos.begin(); posIter != pathPos.end(); posIter++)
		addConnectionPoint(*posIter);
}

QPolygonF DrawingPathItem::connectionPoints() const
{
	QPolygonF pathPos;
	QList<DrawingItemPoint*> points = DrawingPathItem::points();

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
	{
		if ((*pointIter)->flags() & DrawingItemPoint::Connection)
			pathPos.append(mapToPath((*pointIter)->position()));
	}

	return pathPos;
}

//==================================================================================================

QPointF DrawingPathItem::mapToPath(const QPointF& itemPos) const
{
	QPointF pathPos;
	QRectF itemRect = DrawingPathItem::rect();

	pathPos.setX((itemPos.x() - itemRect.left()) / itemRect.width() * mPathRect.width() + mPathRect.left());
	pathPos.setY((itemPos.y() - itemRect.top()) / itemRect.height() * mPathRect.height() + mPathRect.top());

	return pathPos;
}

QRectF DrawingPathItem::mapToPath(const QRectF& itemRect) const
{
	return QRectF(mapToPath(itemRect.topLeft()), mapToPath(itemRect.bottomRight()));
}

QPointF DrawingPathItem::mapFromPath(const QPointF& pathPos) const
{
	QPointF itemPos;
	QRectF itemRect = DrawingPathItem::rect();

	itemPos.setX((pathPos.x() - mPathRect.left()) / mPathRect.width() * itemRect.width() + itemRect.left());
	itemPos.setY((pathPos.y() - mPathRect.top()) / mPathRect.height() * itemRect.height() + itemRect.top());

	return itemPos;
}

QRectF DrawingPathItem::mapFromPath(const QRectF& pathRect) const
{
	return QRectF(mapFromPath(pathRect.topLeft()), mapFromPath(pathRect.bottomRight()));
}

//==================================================================================================

QRectF DrawingPathItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingPathItem::shape() const
{
	return mShape;
}

bool DrawingPathItem::isValid() const
{
	return (mRect.width() != 0 && mRect.height() != 0 && !mPath.isEmpty() && !mPathRect.isNull());
}

//==================================================================================================

void DrawingPathItem::render(QPainter* painter)
{
	if (isValid())
		{
			QBrush sceneBrush = painter->brush();
			QPen scenePen = painter->pen();

			// Draw rect
			painter->setBrush(Qt::transparent);
			painter->setPen(mPen);
			painter->drawPath(mTransformedPath);

			painter->setBrush(sceneBrush);
			painter->setPen(scenePen);
		}
}

//==================================================================================================

void DrawingPathItem::resize(DrawingItemPoint* point, const QPointF& pos)
{
	DrawingItem::resize(point, pos);

	QList<DrawingItemPoint*> points = DrawingPathItem::points();

	if (points.size() >= 8)
	{
		int pointIndex = points.indexOf(point);

		if (0 <= pointIndex && pointIndex < 8)
		{
			QRectF rect;

			rect.setTopLeft(points[0]->position());
			rect.setBottomRight(points[1]->position());

			switch (pointIndex)
			{
			case TopLeft: rect.setTopLeft(point->position()); break;
			case TopMiddle:	rect.setTop(point->y()); break;
			case TopRight: rect.setTopRight(point->position()); break;
			case MiddleRight: rect.setRight(point->x()); break;
			case BottomRight: rect.setBottomRight(point->position()); break;
			case BottomMiddle: rect.setBottom(point->y()); break;
			case BottomLeft: rect.setBottomLeft(point->position()); break;
			case MiddleLeft: rect.setLeft(point->x()); break;
			default: break;
			}

			setRect(rect);
		}
	}

	// Adjust position of connection points
	for(auto keyIter = mPathConnectionPoints.begin(); keyIter != mPathConnectionPoints.end(); keyIter++)
		keyIter.key()->setPosition(mapFromPath(keyIter.value()));
}

//==================================================================================================

void DrawingPathItem::updateGeometry()
{
	mBoundingRect = QRectF();
	mShape = QPainterPath();
	mTransformedPath = QPainterPath();

	if (isValid())
	{
		qreal halfPenWidth = mPen.widthF() / 2;
		QRectF normalizedRect = mRect.normalized();
		QPainterPath drawPath;

		// Transformed path
		mTransformedPath = transformedPath();

		// Bounding rect
		mBoundingRect = normalizedRect;
		mBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		// Shape
		//drawPath = mTransformedPath;
		//mShape = strokePath(drawPath, mPen);
		//mShape.addPath(drawPath);

		mShape.addRect(mBoundingRect);
	}
}

//==================================================================================================

QPainterPath DrawingPathItem::transformedPath() const
{
	QPainterPath transformedPath;
	QList<QPointF> curveDataPoints;

	for(int i = 0; i < mPath.elementCount(); i++)
	{
		QPainterPath::Element element = mPath.elementAt(i);

		switch (element.type)
		{
		case QPainterPath::MoveToElement:
			transformedPath.moveTo(mapFromPath(QPointF(element.x, element.y)));
			break;
		case QPainterPath::LineToElement:
			transformedPath.lineTo(mapFromPath(QPointF(element.x, element.y)));
			break;
		case QPainterPath::CurveToElement:
			curveDataPoints.append(mapFromPath(QPointF(element.x, element.y)));
			break;
		case QPainterPath::CurveToDataElement:
			if (curveDataPoints.size() >= 2)
			{
				transformedPath.cubicTo(curveDataPoints[0], curveDataPoints[1],
					mapFromPath(QPointF(element.x, element.y)));
				curveDataPoints.pop_front();
				curveDataPoints.pop_front();
			}
			else curveDataPoints.append(mapFromPath(QPointF(element.x, element.y)));
			break;
		}
	}

	return transformedPath;
}
