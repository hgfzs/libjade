# libjade
<a href="https://jaallen85.github.io/libjade/annotated.html">Class List</a>

The jade library provides a Qt widget for managing a large number of two-dimensional graphical items within a scene: DrawingWidget.  DrawingWidget supports the following features:

* Add and remove large numbers of items to the scene
* Use the mouse to select items and manipulate them within the scene (move, resize, rotate, flip, etc)
* Undo/redo support
* Cut/copy/paste support
* Select all/select none support
* Snap items to grid
* Reorder items (bring to front, send to back, etc)
* Group and ungroup items together
* Connect items together and resize one when the other is moved
* Zoom in/out/fit support

DrawingItem is the base class for all graphical items in a DrawingWidget.  It provides a lightweight foundation for writing custom items. This includes defining the item's geometry, painting implementation, and item interaction through event handlers.

DrawingWidget and DrawingItem are highly extensible; many functions are virtual and may be overridden in a derived class implementation to alter the default behavior.

libjade is available under the <a href="https://www.gnu.org/licenses/gpl-3.0-standalone.html">GNU General Public License version 3.0</a>.

Full class documentation can be found at: <a href="https://jaallen85.github.io/libjade/">https://jaallen85.github.io/libjade/annotated.html</a>.
