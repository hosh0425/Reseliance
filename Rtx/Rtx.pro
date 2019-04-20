TEMPLATE = app
TARGET = Rtx

QT += core gui widgets network script scripttools opengl svg sql printsupport webengine webenginewidgets

#CONFIG += debug
#CONFIG += release
CONFIG += debug_and_release
CONFIG += build_all

CONFIG(debug, debug|release) {
	CONFIGNAME = Debug
} else {
	CONFIGNAME = Release
}

win32 {
	CONFIG += console

	# Indicate if Visual Studio is used as COMPILER
	CONFIG += visualStudioCompiler

	GSLDir = "../gsl-1.16"

	# Indicate if GSL has been built statically
	#CONFIG += staticGSL
} else {
	GSLDir = /opt/local
}
	
PLATFORM = 

win32 {
	contains(CONFIG, staticGSL) {
	} else {
		DEFINES += GSL_DLL
	}
	
	!contains(QMAKE_HOST.arch, x86_64) {
		PLATFORM = Win32
	} else {
		PLATFORM = x64
	}
	
	contains(CONFIG, visualStudioCompiler) {
		# This branch is for developers on Windows who are using Visual Studio as COMPILER, regardless of IDE
		contains(CONFIG, staticGSL) {
			LIBS += $${GSLDir}/build.vc10/dll/$${PLATFORM}/$${CONFIGNAME}/gsl.lib
			LIBS += $${GSLDir}/build.vc10/dll/$${PLATFORM}/$${CONFIGNAME}/cblas.lib
		} else {
			LIBS += $${GSLDir}/build.vc10/dll/$${PLATFORM}/$${CONFIGNAME}/gsl.lib
			LIBS += $${GSLDir}/build.vc10/dll/$${PLATFORM}/$${CONFIGNAME}/cblas.lib
		}
		
		# Required by QCustomPlot
		LIBS += opengl32.lib
		
		# NOTICE 1: In case of building Qt statically, the DEFINES directives should be modified and at least, QT_DLL should be removed (not sure about others).
		# NOTICE 2: Add more if more modules of Qt are put to use.
		DEFINES += QT_LARGEFILE_SUPPORT QT_DLL QT_NETWORK_LIB QT_SCRIPT_LIB QT_SCRIPTTOOLS_LIB QT_OPENGL_LIB QT_SVG_LIB QT_SQL_LIB QT_PRINTSUPPORT_LIB QT_WEBENGINE_LIB QT_WEBENGINEWIDGETS_LIB
		
		# Enables the use of multi-processor compiling
		QMAKE_CXXFLAGS += /MP
		
		# Disables prevention of the conversion of char * to QString
		QMAKE_CXXFLAGS -= -Zc:strictStrings

	} else {
		# This branch is for developers on Windows who are using MinGW as COMPILER (This usually means that the IDE is QtCreator)
		LIBS += -L$${GSLDir}/lib -lgsl -lgslcblas
	}
} else {
	!contains(QMAKE_HOST.arch, x86_64) {
		PLATFORM = x86
	} else {
		PLATFORM = x86_64
	}
	
	LIBS += -L$${GSLDir}/lib -lgsl -lgslcblas
	#LIBS += $${GSLDir}/lib/libgsl.a
	#LIBS += $${GSLDir}/lib/libgslcblas.a
}

INCLUDEPATH += . \
	./Source/RDomain \
	./Source/RDomain/RParameter \
	./Source/RDomain/RParameter/RRandomVariable \
	./Source/RDomain/RParameter/RResponse \
	./Source/RDomain/RParameter/RConstant \
	./Source/RDomain/RParameter/RDecisionVariable \
	./Source/RDomain/RParameter/RLocation \
	./Source/RDomain/RParameter/RCorrelation \
	./Source/RDomain/RParameter/RTime \
	./Source/RDomain/RFunction \
	./Source/RDomain/RModel \
	./Source/RDomain/RModel/Event \
	./Source/RDomain/RModel/Event/Operation \
	./Source/RDomain/RModel/Event/Time \
	./Source/RDomain/RModel/Occurrence \
	./Source/RDomain/RModel/Consequence \
	./Source/RDomain/RModel/ExternalSoftware \
	./Source/RDomain/RModel/Hazard \
	./Source/RDomain/RModel/Infrastructure \
	./Source/RDomain/RModel/Infrastructure/St \
	./Source/RDomain/RModel/Infrastructure/Building \
	./Source/RDomain/RModel/Infrastructure/Power \
	./Source/RDomain/RModel/Infrastructure/Commercial \
	./Source/RDomain/RModel/Infrastructure/Water \
	./Source/RDomain/RModel/Infrastructure/Healthcare \
	./Source/RDomain/RModel/Generic \
	./Source/RMPI \
	./Source/RAnalysis \
	./Source/RAnalysis/RAnalyzer \
	./Source/RAnalysis/RNonlinSingleConstrSolver \
	./Source/RAnalysis/RConvergenceChecker \
	./Source/RAnalysis/RStepDirectionSearcher \
	./Source/RAnalysis/RStepSizeSearcher \
	./Source/RAnalysis/RTransformer \
	./Source/RAnalysis/RRandomNumberGenerator \
	./Source/RAnalysis/RAccumulator \
	./Source/RAnalysis/RMeritChecker \
	./Source/RGUI \
	./Source/RGUI/RAboutDialog \
	./Source/RGUI/RMapWidget \
	./Source/RGUI/RObjectWidget \
	./Source/RGUI/RPlotWidget \
	./Source/RGUI/RFlowchartWidget \
	./Source/RGUI/RGLWidget \
	./Source/RGUI/RAnalysisDialog \
	./Source/RGUI/RParametricAnalysisDialog \
	./Source/RGUI/RVariableInferenceDialog \
	./Source/RGUI/RModelInferenceDialog \
	./Source/RGUI/RPropertyWidget \
	./Source/RGUI/RPropertyWidget/RDelegate \
	./Source/Templates \
	./Source/Tools \
	./Source/Tools/MuParser \
	./Source/Tools/Sparse \
	./$${PLATFORM}/$${CONFIGNAME}

win32 {
	INCLUDEPATH += $${GSLDir} \
} else {
	INCLUDEPATH += $${GSLDir}/include \
}
	
#DEPENDPATH += INCLUDEPATH

MOC_DIR = ./GeneratedFiles/$${CONFIGNAME}
OBJECTS_DIR = ./$${PLATFORM}/$${CONFIGNAME}
UI_DIR = ./GeneratedFiles
RCC_DIR = ./GeneratedFiles
DESTDIR = ./Bin/$${PLATFORM}/$${CONFIGNAME}

#Include file(s)
include(Rtx.pri)

#Windows resource file for icon
win32:RC_FILE = ./Source/RGUI/RIcon/Rtx.rc

#Mac icon
mac:ICON = ./Source/RGUI/RIcon/Rtx.icns

#Mac machine type
#mac:CONFIG += x86_64
