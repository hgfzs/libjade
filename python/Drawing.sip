// Drawing.sip

%Module jade

%Import QtCore/QtCoremod.sip

%If (Qt_5_0_0 -)
%Import QtWidgets/QtWidgetsmod.sip
%Else
%Import QtGui/QtGuimod.sip
%End

%Include DrawingArcItem.sip
%Include DrawingCurveItem.sip
%Include DrawingEllipseItem.sip
%Include DrawingItem.sip
%Include DrawingItemGroup.sip
%Include DrawingItemPoint.sip
%Include DrawingItemStyle.sip
%Include DrawingLineItem.sip
%Include DrawingPathItem.sip
%Include DrawingPolygonItem.sip
%Include DrawingPolylineItem.sip
%Include DrawingRectItem.sip
%Include DrawingTextEllipseItem.sip
%Include DrawingTextItem.sip
%Include DrawingTextPolygonItem.sip
%Include DrawingTextRectItem.sip
%Include DrawingTypes.sip
%Include DrawingWidget.sip
