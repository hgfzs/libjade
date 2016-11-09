# libjade.py

import os, shutil
import sipconfig

# Build settings
master_input_file = "Drawing.sip"

other_input_files = [ 
	"DrawingArcItem.sip", 
	"DrawingCurveItem.sip", 
	"DrawingEllipseItem.sip", 
	"DrawingItem.sip", 
	"DrawingItemGroup.sip", 
	"DrawingItemPoint.sip", 
	"DrawingItemStyle.sip", 
	"DrawingLineItem.sip", 
	"DrawingPathItem.sip", 
	"DrawingPolygonItem.sip", 
	"DrawingPolylineItem.sip", 
	"DrawingRectItem.sip", 
	"DrawingTextEllipseItem.sip", 
	"DrawingTextItem.sip", 
	"DrawingTextPolygonItem.sip", 
	"DrawingTextRectItem.sip", 
	"DrawingTypes.sip", 
	"DrawingWidget.sip"
]
	
build_file = "jade.sbf"

output_dir = "codegen"

# SIP settings
sip_bin = r"C:\Development\Python34\sip.exe"
sip_includedir_pyqt = r"C:\Development\Python34\Lib\site-packages\PyQt5\sip\PyQt5"
sip_flags_pyqt = "-t WS_WIN"

# Run SIP
if (os.path.exists(output_dir)):
	shutil.rmtree(output_dir)
os.mkdir(output_dir)

os.system(" ".join([sip_bin, "-c", output_dir, "-b", os.path.join(output_dir, build_file), "-I", sip_includedir_pyqt, sip_flags_pyqt, master_input_file]))







# # Get the PyQt4 configuration information.
# config = pyqtconfig.Configuration()

# # Get the extra SIP flags needed by the imported PyQt4 modules.  Note that
# # this normally only includes those flags (-x and -t) that relate to SIP's
# # versioning system.
# pyqt_sip_flags = config.pyqt_sip_flags

# # Run SIP to generate the code.  Note that we tell SIP where to find the qt
# # module's specification files using the -I flag.
# os.system(" ".join([config.sip_bin, "-c", ".", "-b", build_file, "-I", config.pyqt_sip_dir, pyqt_sip_flags, "Drawing.sip"]))

# # We are going to install the SIP specification file for this module and
# # its configuration module.
# installs = []

# installs.append([master_input_file, os.path.join(config.default_sip_dir, "jade")])

# for filename in other_input_files:
	# installs.append([filename, os.path.join(config.default_sip_dir, "jade")])

# # Create the Makefile.  The QtWidgetsModuleMakefile class provided by the
# # pyqtconfig module takes care of all the extra preprocessor, compiler and
# # linker flags needed by the Qt library.
# makefile = pyqtconfig.QtWidgetsModuleMakefile(
    # configuration=config,
    # build_file=build_file,
    # installs=installs
# )

# # Add the library we are wrapping.  The name doesn't include any platform
# # specific prefixes or extensions (e.g. the "lib" prefix on UNIX, or the
# # ".dll" extension on Windows).
# makefile.extra_libs = [ "jade" ]

# # Generate the Makefile itself.
# makefile.generate()
