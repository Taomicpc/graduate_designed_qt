######################################################################
# Automatically generated by qmake (2.01a) ?? 4? 9 11:22:12 2015
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += dialogAddTable1.h dialogAddTable2.h dialogModifyTable1.h dialogModifyTable2.h dialogShowVoice.h TQInputMethod.h voiceIden.h
FORMS += dialogAddTable1.ui \
	 dialogAddTable2.ui \
	 dialogModifyTable1.ui \
	 dialogModifyTable2.ui \
	 dialogShowVoice.ui \
         voiceIden.ui
SOURCES += dialogAddTable1.cpp dialogAddTable2.cpp dialogModifyTable1.cpp dialogModifyTable2.cpp dialogShowVoice.cpp main.cpp voiceIden.cpp
QT += sql
LIBS += -L. -lTQInputMethod

