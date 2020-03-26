/* DrawingPathItem.cpp
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

#include "DrawingPathItem.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"

DrawingPathItem::DrawingPathItem() : DrawingItem()
{
	mName = "Path";

	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanSelect | CanDelete);

	for(int i = 0; i < 8; i++) addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Control));

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

DrawingPathItem::DrawingPathItem(const DrawingPathItem& item) : DrawingItem(item)
{
	mName = item.mName;
	mPath = item.mPath;
	mPathRect = item.mPathRect;

	QList<DrawingItemPoint*> points = DrawingPathItem::points();
	QList<DrawingItemPoint*> otherItemPoints = item.points();
	for(int i = 8; i < points.size(); i++)
		mPathConnectionPoints[points[i]] = item.mPathConnectionPoints[otherItemPoints[i]];
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
	QList<DrawingItemPoint*> points = DrawingPathItem::points();
	points[TopLeft]->setPosition(rect.left(), rect.top());
	points[TopMiddle]->setPosition(rect.center().x(), rect.top());
	points[TopRight]->setPosition(rect.right(), rect.top());
	points[MiddleRight]->setPosition(rect.right(), rect.center().y());
	points[BottomRight]->setPosition(rect.right(), rect.bottom());
	points[BottomMiddle]->setPosition(rect.center().x(), rect.bottom());
	points[BottomLeft]->setPosition(rect.left(), rect.bottom());
	points[MiddleLeft]->setPosition(rect.left(), rect.center().y());
}

void DrawingPathItem::setRect(qreal left, qreal top, qreal width, qreal height)
{
	setRect(QRectF(left, top, width, height));
}

QRectF DrawingPathItem::rect() const
{
	QList<DrawingItemPoint*> points = DrawingPathItem::points();
	return (points.size() >= 8) ? QRectF(points[TopLeft]->position(), points[BottomRight]->position()) : QRectF();
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
	QRectF rect;

	if (isValid())
	{
		qreal penWidth = style()->valueLookup(DrawingItemStyle::PenWidth).toReal();

		rect = DrawingPathItem::rect().normalized();
		rect.adjust(-penWidth/2, -penWidth/2, penWidth/2, penWidth/2);
	}

	return rect;
}

QPainterPath DrawingPathItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		/*DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();

		// Add path
		QPainterPath drawPath = transformedPath();

		// Determine outline path
		shape = strokePath(drawPath, pen);*/

		shape.addRect(boundingRect());
	}

	return shape;
}

bool DrawingPathItem::isValid() const
{
	QList<DrawingItemPoint*> points = DrawingPathItem::points();
	return (points.size() >= 8 && points[TopLeft]->position() != points[BottomRight]->position() &&
		!mPathRect.isNull() && !mPath.isEmpty());
}

//==================================================================================================

void DrawingPathItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		QBrush brush = style->brush();

		// Draw path
		painter->setBrush(brush);
		painter->setPen(pen);
		painter->drawPath(transformedPath());

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingPathItem::resizeEvent(DrawingItemPoint* itemPoint, const QPointF& parentPos)
{
	DrawingItem::resizeEvent(itemPoint, parentPos);

	QList<DrawingItemPoint*> points = DrawingPathItem::points();
	if (points.size() >= 8)
	{
		QRectF rect = DrawingPathItem::rect();
		int pointIndex = points.indexOf(itemPoint);

		if (0 <= pointIndex && pointIndex < 8)
		{
			switch (pointIndex)
			{
			case TopLeft: rect.setTopLeft(itemPoint->position()); break;
			case TopMiddle:	rect.setTop(itemPoint->y()); break;
			case TopRight: rect.setTopRight(itemPoint->position()); break;
			case MiddleRight: rect.setRight(itemPoint->x()); break;
			case BottomRight: rect.setBottomRight(itemPoint->position()); break;
			case BottomMiddle: rect.setBottom(itemPoint->y()); break;
			case BottomLeft: rect.setBottomLeft(itemPoint->position()); break;
			case MiddleLeft: rect.setLeft(itemPoint->x()); break;
			default: break;
			}

			points[TopLeft]->setPosition(rect.left(), rect.top());
			points[TopMiddle]->setPosition(rect.center().x(), rect.top());
			points[TopRight]->setPosition(rect.right(), rect.top());
			points[MiddleRight]->setPosition(rect.right(), rect.center().y());
			points[BottomRight]->setPosition(rect.right(), rect.bottom());
			points[BottomMiddle]->setPosition(rect.center().x(), rect.bottom());
			points[BottomLeft]->setPosition(rect.left(), rect.bottom());
			points[MiddleLeft]->setPosition(rect.left(), rect.center().y());
		}
	}

	// Adjust position of connection points
	for(auto keyIter = mPathConnectionPoints.begin(); keyIter != mPathConnectionPoints.end(); keyIter++)
		keyIter.key()->setPosition(mapFromPath(keyIter.value()));
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
