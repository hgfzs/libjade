# jade-test.py

import os, sys
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from jade import *

class DiagramWidget(DrawingWidget):
	( UndoAction, RedoAction, CutAction, CopyAction, PasteAction, DeleteAction,
		SelectAllAction, SelectNoneAction, RotateAction, RotateBackAction, FlipAction,
		BringForwardAction, SendBackwardAction, BringToFrontAction, SendToBackAction,
		InsertPointAction, RemovePointAction, GroupAction, UngroupAction,
		ZoomInAction, ZoomOutAction, ZoomFitAction, NumberOfActions ) = range(23)
	
	def __init__(self):
		DrawingWidget.__init__(self)
		self.addActions()
		self.createContextMenus()
	
	def mouseReleaseEvent(self, event):
		if (event.button() == Qt.RightButton):
			if (self.mode() == DrawingWidget.DefaultMode):
				mouseDownItem = self.mouseDownItem()

				if (mouseDownItem and mouseDownItem.isSelected() and len(self.selectedItems()) == 1):
					if ((mouseDownItem.flags() & DrawingItem.CanInsertPoints) or
						(mouseDownItem.flags() & DrawingItem.CanRemovePoints)):
						self._singlePolyItemContextMenu.popup(event.globalPos() + QPoint(2,2))
					else:
						self._singleItemContextMenu.popup(event.globalPos() + QPoint(2,2))
				elif (mouseDownItem and mouseDownItem.isSelected()):
					self._multipleItemContextMenu.popup(event.globalPos() + QPoint(2,2))
				else:
					if (mouseDownItem == None):
						self.clearSelection()
					self._diagramContextMenu.popup(event.globalPos() + QPoint(2,2))
			else:
				self.setDefaultMode()

		DrawingWidget.mouseReleaseEvent(self, event)

	def addActions(self):
		self.addAction("Undo", self.undo, "", "Ctrl+Z")
		self.addAction("Redo", self.redo, "", "Ctrl+Shift+Z")
		self.addAction("Cut", self.cut, "", "Ctrl+X")
		self.addAction("Copy", self.copy, "", "Ctrl+C")
		self.addAction("Paste", self.paste, "", "Ctrl+V")
		self.addAction("Delete", self.deleteSelection, "", "Delete")
		self.addAction("Select All", self.selectAll, "", "Ctrl+A")
		self.addAction("Select None", self.selectNone, "", "Ctrl+Shift+A")

		self.addAction("Rotate", self.rotateSelection, "", "R")
		self.addAction("Rotate Back", self.rotateBackSelection, "", "Shift+R")
		self.addAction("Flip", self.flipSelection, "", "F")

		self.addAction("Bring Forward", self.bringForward, "")
		self.addAction("Send Backward", self.sendBackward, "")
		self.addAction("Bring to Front", self.bringToFront, "")
		self.addAction("Send to Back", self.sendToBack, "")

		self.addAction("Insert Point", self.insertItemPoint, "")
		self.addAction("Remove Point", self.removeItemPoint, "")

		self.addAction("Group", self.group, "", "Ctrl+G")
		self.addAction("Ungroup", self.ungroup, "", "Ctrl+Shift+G")

		self.addAction("Zoom In", self.zoomIn, "", ".")
		self.addAction("Zoom Out", self.zoomOut, "", ",")
		self.addAction("Zoom Fit", self.zoomFit, "", "/")

	def createContextMenus(self):
		actions = self.actions()
		
		self._singleItemContextMenu = QMenu(self)
		self._singleItemContextMenu.addAction(actions[DiagramWidget.RotateAction])
		self._singleItemContextMenu.addAction(actions[DiagramWidget.RotateBackAction])
		self._singleItemContextMenu.addAction(actions[DiagramWidget.FlipAction])
		self._singleItemContextMenu.addAction(actions[DiagramWidget.DeleteAction])
		self._singleItemContextMenu.addSeparator()
		self._singleItemContextMenu.addAction(actions[DiagramWidget.GroupAction])
		self._singleItemContextMenu.addAction(actions[DiagramWidget.UngroupAction])
		self._singleItemContextMenu.addSeparator()
		self._singleItemContextMenu.addAction(actions[DiagramWidget.CutAction])
		self._singleItemContextMenu.addAction(actions[DiagramWidget.CopyAction])
		self._singleItemContextMenu.addAction(actions[DiagramWidget.PasteAction])

		self._singlePolyItemContextMenu = QMenu(self)
		self._singlePolyItemContextMenu.addAction(actions[DiagramWidget.InsertPointAction])
		self._singlePolyItemContextMenu.addAction(actions[DiagramWidget.RemovePointAction])
		self._singlePolyItemContextMenu.addSeparator()
		self._singlePolyItemContextMenu.addAction(actions[DiagramWidget.RotateAction])
		self._singlePolyItemContextMenu.addAction(actions[DiagramWidget.RotateBackAction])
		self._singlePolyItemContextMenu.addAction(actions[DiagramWidget.FlipAction])
		self._singlePolyItemContextMenu.addAction(actions[DiagramWidget.DeleteAction])
		self._singlePolyItemContextMenu.addSeparator()
		self._singlePolyItemContextMenu.addAction(actions[DiagramWidget.CutAction])
		self._singlePolyItemContextMenu.addAction(actions[DiagramWidget.CopyAction])
		self._singlePolyItemContextMenu.addAction(actions[DiagramWidget.PasteAction])

		self._multipleItemContextMenu = QMenu(self)
		self._multipleItemContextMenu.addAction(actions[DiagramWidget.RotateAction])
		self._multipleItemContextMenu.addAction(actions[DiagramWidget.RotateBackAction])
		self._multipleItemContextMenu.addAction(actions[DiagramWidget.FlipAction])
		self._multipleItemContextMenu.addAction(actions[DiagramWidget.DeleteAction])
		self._multipleItemContextMenu.addSeparator()
		self._multipleItemContextMenu.addAction(actions[DiagramWidget.GroupAction])
		self._multipleItemContextMenu.addAction(actions[DiagramWidget.UngroupAction])
		self._multipleItemContextMenu.addSeparator()
		self._multipleItemContextMenu.addAction(actions[DiagramWidget.CutAction])
		self._multipleItemContextMenu.addAction(actions[DiagramWidget.CopyAction])
		self._multipleItemContextMenu.addAction(actions[DiagramWidget.PasteAction])

		self._diagramContextMenu = QMenu(self)
		self._diagramContextMenu.addAction(actions[DiagramWidget.UndoAction])
		self._diagramContextMenu.addAction(actions[DiagramWidget.RedoAction])
		self._diagramContextMenu.addSeparator()
		self._diagramContextMenu.addAction(actions[DiagramWidget.CutAction])
		self._diagramContextMenu.addAction(actions[DiagramWidget.CopyAction])
		self._diagramContextMenu.addAction(actions[DiagramWidget.PasteAction])
		self._diagramContextMenu.addSeparator()
		self._diagramContextMenu.addAction(actions[DiagramWidget.ZoomInAction])
		self._diagramContextMenu.addAction(actions[DiagramWidget.ZoomOutAction])
		self._diagramContextMenu.addAction(actions[DiagramWidget.ZoomFitAction])

	def addAction(self, text, slot, iconPath="", shortcut=""):
		action = QAction(text, self)
		action.triggered.connect(slot)

		if (iconPath != ""):
			action.setIcon(QIcon(iconPath))
		if (shortcut != ""):
			action.setShortcut(QKeySequence(shortcut))

		DrawingWidget.addAction(self, action)
		return action

#===================================================================================================

class DiagramWindow(QMainWindow):
	( AboutQtAction, ExitAction, NumberOfActions ) = range(3)
	( DefaultModeAction, ScrollModeAction, ZoomModeAction,
		PlaceArcAction, PlaceCurveAction, PlaceEllipseAction, PlaceLineAction, PlacePathAction,
		PlacePolygonAction, PlacePolylineAction, PlaceRectAction, PlaceTextAction,
		PlaceTextRectAction, PlaceTextEllipseAction, PlaceTextPolygonAction,
		NumberOfModeActions ) = range(16)
	
	def __init__(self):
		QMainWindow.__init__(self)
		
		self._diagramWidget = DiagramWidget()
		self._diagramWidget.setFlags(DrawingWidget.UndoableSelectCommands)
		self.setCentralWidget(self._diagramWidget)

		self._modifiedLabel = QLabel("")
		self._modeLabel = QLabel("Select Mode")
		self._numberOfItemsLabel = QLabel("0")
		self._numberOfSelectedItemsLabel = QLabel("0")
		self._scaleLabel = QLabel("100%")
		self.statusBar().addWidget(self._modeLabel, 100)
		self.statusBar().addWidget(self._modifiedLabel, 100)
		self.statusBar().addWidget(self._numberOfItemsLabel, 100)
		self.statusBar().addWidget(self._numberOfSelectedItemsLabel, 100)
		self.statusBar().addWidget(self._scaleLabel, 100)
		
		self._diagramWidget.modeChanged.connect(self.updateActionFromMode)
		self._diagramWidget.modeChanged.connect(self.setModeLabel)
		self._diagramWidget.cleanChanged.connect(self.setModifiedLabel)
		self._diagramWidget.numberOfItemsChanged.connect(self.setNumberOfItemsLabel)
		self._diagramWidget.selectionChanged.connect(self.setNumberOfSelectedItemsLabel)
		self._diagramWidget.scaleChanged.connect(self.setScaleLabel)

		self.createActions()
		self.createMenus()

		self.setWindowTitle("libJade test")
		self.resize(1024, 768)

	def setModeFromAction(self, action):
		if (action.text() == "Scroll Mode"):
			self._diagramWidget.setScrollMode()
		elif (action.text() == "Zoom Mode"):
			self._diagramWidget.setZoomMode()
		elif (action.text() == "Place Arc"):
			self._diagramWidget.setPlaceMode(DrawingArcItem())
		elif (action.text() == "Place Curve"):
			self._diagramWidget.setPlaceMode(DrawingCurveItem())
		elif (action.text() == "Place Ellipse"):
			self._diagramWidget.setPlaceMode(DrawingEllipseItem())
		elif (action.text() == "Place Line"):
			self._diagramWidget.setPlaceMode(DrawingLineItem())
		elif (action.text() == "Place Polygon"):
			self._diagramWidget.setPlaceMode(DrawingPolygonItem())
		elif (action.text() == "Place Polyline"):
			self._diagramWidget.setPlaceMode(DrawingPolylineItem())
		elif (action.text() == "Place Rect"):
			self._diagramWidget.setPlaceMode(DrawingRectItem())
		elif (action.text() == "Place Text"):
			self._diagramWidget.setPlaceMode(DrawingTextItem())
		elif (action.text() == "Place Path"):
			path = QPainterPath()
			path.addRect(-200, -100, 400, 200)
			item = DrawingPathItem()
			item.setRect(-200, -100, 400, 200)
			item.setPath(path, QRectF(-200, -100, 400, 200))
			self._diagramWidget.setPlaceMode(item)
		elif (action.text() == "Place Text Rect"):
			self._diagramWidget.setPlaceMode(DrawingTextRectItem())
		elif (action.text() == "Place Text Ellipse"):
			self._diagramWidget.setPlaceMode(DrawingTextEllipseItem())
		elif (action.text() == "Place Text Polygon"):
			self._diagramWidget.setPlaceMode(DrawingTextPolygonItem())
		else:
			self._diagramWidget.setDefaultMode()

	def updateActionFromMode(self, mode):
		modeActions = self._modeActionGroup.actions()

		if (mode == DrawingWidget.ScrollMode):
			modeActions[DiagramWindow.ScrollModeAction].setChecked(True)
		elif (mode == DrawingWidget.ZoomMode):
			modeActions[DiagramWindow.ZoomModeAction].setChecked(True)
		elif (mode == DrawingWidget.DefaultMode):
			modeActions[DiagramWindow.DefaultModeAction].setChecked(True)

	def setModeLabel(self, mode):
		if (mode == DrawingWidget.ScrollMode):
			self._modeLabel.setText("Scroll Mode")
		elif (mode == DrawingWidget.ZoomMode):
			self._modeLabel.setText("Zoom Mode")
		elif (mode == DrawingWidget.PlaceMode):
			self._modeLabel.setText("Place Mode")
		else:
			self._modeLabel.setText("Select Mode")
	
	def setModifiedLabel(self, clean):
		if (clean):
			self._modifiedLabel.setText("")
		else:
			self._modifiedLabel.setText("Modified")
	
	def setNumberOfItemsLabel(self, numberOfItems):
		self._numberOfItemsLabel.setText("{:d}".format(numberOfItems))
			
	def setNumberOfSelectedItemsLabel(self, selectedItems):
		self._numberOfSelectedItemsLabel.setText("{:d}".format(len(selectedItems)))
		
	def setScaleLabel(self, scale):
		self._scaleLabel.setText("{:.1f}".format(scale))

	def showEvent(self, event):
		QMainWindow.showEvent(self, event)
		if (not event.spontaneous()):
			self._diagramWidget.zoomFit()

	def createActions(self):
		self.addAction("About Qt...", qApp.aboutQt);
		self.addAction("Exit", self.close)

		self._modeActionGroup = QActionGroup(self)
		self._modeActionGroup.triggered.connect(self.setModeFromAction)

		self.addModeAction("Select Mode", "", "Escape")
		self.addModeAction("Scroll Mode", "", "")
		self.addModeAction("Zoom Mode", "", "")
		self.addModeAction("Place Arc", "", "")
		self.addModeAction("Place Curve", "", "")
		self.addModeAction("Place Ellipse", "", "")
		self.addModeAction("Place Line", "", "")
		self.addModeAction("Place Path", "", "")
		self.addModeAction("Place Polygon", "", "")
		self.addModeAction("Place Polyline", "", "")
		self.addModeAction("Place Rect", "", "")
		self.addModeAction("Place Text", "", "")
		self.addModeAction("Place Text Rect", "", "")
		self.addModeAction("Place Text Ellipse", "", "")
		self.addModeAction("Place Text Polygon", "", "")

		self._modeActionGroup.actions()[DiagramWindow.DefaultModeAction].setChecked(True)

	def createMenus(self):
		actions = self.actions()
		diagramActions = self._diagramWidget.actions()
		modeActions = self._modeActionGroup.actions()

		menu = self.menuBar().addMenu("File")
		menu.addAction(actions[DiagramWindow.AboutQtAction])
		menu.addSeparator()
		menu.addAction(actions[DiagramWindow.ExitAction])

		menu = self.menuBar().addMenu("Edit")
		menu.addAction(diagramActions[DiagramWidget.UndoAction])
		menu.addAction(diagramActions[DiagramWidget.RedoAction])
		menu.addSeparator()
		menu.addAction(diagramActions[DiagramWidget.CutAction])
		menu.addAction(diagramActions[DiagramWidget.CopyAction])
		menu.addAction(diagramActions[DiagramWidget.PasteAction])
		menu.addAction(diagramActions[DiagramWidget.DeleteAction])
		menu.addSeparator()
		menu.addAction(diagramActions[DiagramWidget.SelectAllAction])
		menu.addAction(diagramActions[DiagramWidget.SelectNoneAction])

		menu = self.menuBar().addMenu("Object")
		menu.addAction(diagramActions[DiagramWidget.RotateAction])
		menu.addAction(diagramActions[DiagramWidget.RotateBackAction])
		menu.addAction(diagramActions[DiagramWidget.FlipAction])
		menu.addSeparator()
		menu.addAction(diagramActions[DiagramWidget.InsertPointAction])
		menu.addAction(diagramActions[DiagramWidget.RemovePointAction])
		menu.addSeparator()
		menu.addAction(diagramActions[DiagramWidget.GroupAction])
		menu.addAction(diagramActions[DiagramWidget.UngroupAction])
		menu.addSeparator()
		menu.addAction(diagramActions[DiagramWidget.BringForwardAction])
		menu.addAction(diagramActions[DiagramWidget.SendBackwardAction])
		menu.addAction(diagramActions[DiagramWidget.BringToFrontAction])
		menu.addAction(diagramActions[DiagramWidget.SendToBackAction])

		menu = self.menuBar().addMenu("Diagram")
		menu.addAction(modeActions[DiagramWindow.DefaultModeAction])
		menu.addAction(modeActions[DiagramWindow.ScrollModeAction])
		menu.addAction(modeActions[DiagramWindow.ZoomModeAction])
		menu.addSeparator()
		menu.addAction(modeActions[DiagramWindow.PlaceArcAction])
		menu.addAction(modeActions[DiagramWindow.PlaceCurveAction])
		menu.addAction(modeActions[DiagramWindow.PlaceEllipseAction])
		menu.addAction(modeActions[DiagramWindow.PlaceLineAction])
		menu.addAction(modeActions[DiagramWindow.PlacePathAction])
		menu.addAction(modeActions[DiagramWindow.PlacePolygonAction])
		menu.addAction(modeActions[DiagramWindow.PlacePolylineAction])
		menu.addAction(modeActions[DiagramWindow.PlaceRectAction])
		menu.addAction(modeActions[DiagramWindow.PlaceTextAction])
		menu.addAction(modeActions[DiagramWindow.PlaceTextRectAction])
		menu.addAction(modeActions[DiagramWindow.PlaceTextEllipseAction])
		menu.addAction(modeActions[DiagramWindow.PlaceTextPolygonAction])

		menu = self.menuBar().addMenu("View")
		menu.addAction(diagramActions[DiagramWidget.ZoomInAction])
		menu.addAction(diagramActions[DiagramWidget.ZoomOutAction])
		menu.addAction(diagramActions[DiagramWidget.ZoomFitAction])
		
	def addAction(self, text, slot, iconPath="", shortcut=""):
		action = QAction(text, self)
		action.triggered.connect(slot)

		if (iconPath != ""):
			action.setIcon(QIcon(iconPath))
		if (shortcut != ""):
			action.setShortcut(QKeySequence(shortcut))

		QMainWindow.addAction(self, action)
		return action
	
	def addModeAction(self, text, slot, iconPath="", shortcut=""):
		action = QAction(text, self)

		if (iconPath != ""):
			action.setIcon(QIcon(iconPath))
		if (shortcut != ""):
			action.setShortcut(QKeySequence(shortcut))

		action.setCheckable(True)
		action.setActionGroup(self._modeActionGroup)
		return action
	
#===================================================================================================

if __name__ == "__main__":
	app = QApplication(sys.argv)
	window = DiagramWindow()
	window.show()
	sys.exit(app.exec_())
