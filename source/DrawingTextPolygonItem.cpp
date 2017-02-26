/* DrawingTextPolygonItem.cpp
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

#include "DrawingTextPolygonItem.h"
#include "DrawingWidget.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"
#include "DrawingUndo.h"

DrawingTextPolygonItem::DrawingTextPolygonItem() : DrawingItem()
{
	mCaption = "Label";

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

	style->setValue(DrawingItemStyle::TextColor,
		style->valueLookup(DrawingItemStyle::TextColor, QVariant(QColor(0, 0, 0))));
	style->setValue(DrawingItemStyle::TextOpacity,
		style->valueLookup(DrawingItemStyle::TextOpacity, QVariant(1.0)));

	style->setValue(DrawingItemStyle::FontName,
		style->valueLookup(DrawingItemStyle::FontName, QVariant("Arial")));
	style->setValue(DrawingItemStyle::FontSize,
		style->valueLookup(DrawingItemStyle::FontSize, QVariant(100.0)));
	style->setValue(DrawingItemStyle::FontBold,
		style->valueLookup(DrawingItemStyle::FontBold, QVariant(false)));
	style->setValue(DrawingItemStyle::FontItalic,
		style->valueLookup(DrawingItemStyle::FontItalic, QVariant(false)));
	style->setValue(DrawingItemStyle::FontUnderline,
		style->valueLookup(DrawingItemStyle::FontUnderline, QVariant(false)));
	style->setValue(DrawingItemStyle::FontOverline,
		style->valueLookup(DrawingItemStyle::FontOverline, QVariant(false)));
	style->setValue(DrawingItemStyle::FontStrikeThrough,
		style->valueLookup(DrawingItemStyle::FontStrikeThrough, QVariant(false)));
}

DrawingTextPolygonItem::DrawingTextPolygonItem(const DrawingTextPolygonItem& item) : DrawingItem(item)
{
	mCaption = item.mCaption;
}

DrawingTextPolygonItem::~DrawingTextPolygonItem() { }

//==================================================================================================

DrawingItem* DrawingTextPolygonItem::copy() const
{
	return new DrawingTextPolygonItem(*this);
}

//==================================================================================================

void DrawingTextPolygonItem::setPolygon(const QPolygonF& polygon)
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

	QList<DrawingItemPoint*> points = DrawingTextPolygonItem::points();
	for(int i = 0; i < polygon.size(); i++)
		points[i]->setPos(polygon[i]);
}

QPolygonF DrawingTextPolygonItem::polygon() const
{
	QPolygonF polygon;

	QList<DrawingItemPoint*> points = DrawingTextPolygonItem::points();
	for(int i = 0; i < points.size(); i++) polygon.append(points[i]->pos());

	return polygon;
}

//==================================================================================================

void DrawingTextPolygonItem::setCaption(const QString& caption)
{
	mCaption = caption;
}

QString DrawingTextPolygonItem::caption() const
{
	return mCaption;
}

//==================================================================================================

QRectF DrawingTextPolygonItem::boundingRect() const
{
	QRectF rect;

	if (isValid())
	{
		qreal penWidth = style()->valueLookup(DrawingItemStyle::PenWidth).toReal();

		rect = polygon().boundingRect();
		rect.adjust(-penWidth/2, -penWidth/2, penWidth/2, penWidth/2);

		rect = rect.united(calculateTextRect(mCaption, style()->font()));
	}

	return rect;
}

QPainterPath DrawingTextPolygonItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		QBrush brush = style->brush();
		QFont font = style->font();

		// Add polygon
		drawPath.addPolygon(polygon());
		drawPath.closeSubpath();

		// Determine outline path
		pen.setWidthF(qMax(pen.widthF(), minimumPenWidth()));
		shape = strokePath(drawPath, pen);

		if (brush.color().alpha() > 0) shape = shape.united(drawPath);

		// Add text
		QPainterPath textPath;
		textPath.addRect(calculateTextRect(mCaption, font));

		shape = shape.united(textPath);
	}

	return shape;
}

bool DrawingTextPolygonItem::isValid() const
{
	bool superfluous = true;

	QList<DrawingItemPoint*> points = DrawingTextPolygonItem::points();
	QPointF pos = points.first()->pos();

	for(auto pointIter = points.begin() + 1; superfluous && pointIter != points.end(); pointIter++)
		superfluous = (pos == (*pointIter)->pos());

	return (!superfluous && !mCaption.isEmpty());
}

//==================================================================================================

void DrawingTextPolygonItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();
		QFont sceneFont = painter->font();

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		QBrush brush = style->brush();
		QFont font = style->font();
		QBrush textBrush = style->textBrush();

		// Draw polygon
		painter->setBrush(brush);
		painter->setPen(pen);
		painter->drawPolygon(polygon());

		// Draw text
		QFont painterFont = font;
		if (painter->paintEngine()->paintDevice())
			painterFont.setPointSizeF(painterFont.pointSizeF() * 96.0 / painter->paintEngine()->paintDevice()->logicalDpiX());

		QPen textPen(textBrush, 1, Qt::SolidLine);
		painter->setBrush(Qt::transparent);
		painter->setPen(textPen);
		painter->setFont(painterFont);
		painter->drawText(calculateTextRect(mCaption, font), Qt::AlignCenter, mCaption);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
		painter->setFont(sceneFont);
	}

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
	//painter->drawRect(boundingRect());
}

//==================================================================================================

void DrawingTextPolygonItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	DrawingItem::resizeItem(itemPoint, scenePos);

	// Adjust position of item and item points so that point(0)->pos() == QPointF(0, 0)
	QList<DrawingItemPoint*> points = DrawingTextPolygonItem::points();
	QPointF deltaPos = -points.first()->pos();
	QPointF pointScenePos = mapToScene(points.first()->pos());

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		(*pointIter)->setPos((*pointIter)->pos() + deltaPos);

	setPos(pointScenePos);
}

void DrawingTextPolygonItem::insertItemPoint(const QPointF& scenePos)
{
	DrawingWidget* drawing = DrawingTextPolygonItem::drawing();

	if (drawing)
	{
		DrawingItemPoint* newPoint = new DrawingItemPoint(
			mapFromScene(drawing->roundToGrid(scenePos)),
		DrawingItemPoint::Control | DrawingItemPoint::Connection);

		QList<DrawingItemPoint*> points = DrawingTextPolygonItem::points();
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

void DrawingTextPolygonItem::removeItemPoint(const QPointF& scenePos)
{
	DrawingWidget* drawing = DrawingTextPolygonItem::drawing();
	QList<DrawingItemPoint*> points = DrawingTextPolygonItem::points();

	if (drawing && points.size() > 3)
	{
		DrawingItemPoint* pointToRemove = pointNearest(mapFromScene(scenePos));

		if (pointToRemove)
			drawing->pushUndoCommand(new DrawingItemRemovePointCommand(drawing, this, pointToRemove));
	}
}

//==================================================================================================

qreal DrawingTextPolygonItem::distanceFromPointToLineSegment(const QPointF& point, const QLineF& line) const
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

QRectF DrawingTextPolygonItem::calculateTextRect(const QString& caption, const QFont& font) const
{
	qreal textWidth = 0, textHeight = 0;

	QPolygonF polygon = DrawingTextPolygonItem::polygon();
	//QPointF polygonCenter = polygon.boundingRect().center();
	QPointF polygonCenter;
	int polygonCount = 0;
	for(auto pointIter = polygon.begin(); pointIter != polygon.end(); pointIter++)
	{
		polygonCenter += *pointIter;
		polygonCount++;
	}
	if (polygonCount > 0) polygonCenter = QPointF(polygonCenter.x() / polygonCount, polygonCenter.y() / polygonCount);

	QFontMetricsF fontMetrics(font);
	QStringList lines = caption.split("\n");

	for(auto lineIter = lines.begin(); lineIter != lines.end(); lineIter++)
	{
		textWidth = qMax(textWidth, fontMetrics.width(*lineIter));
		textHeight += fontMetrics.lineSpacing();
	}

	textHeight -= fontMetrics.leading();

	return QRectF(-textWidth / 2, -textHeight / 2, textWidth, textHeight).translated(polygonCenter);
}
