BLENDER=/usr/bin/blender
QTDIR:=$(shell ls -d /usr/qt/3)

MOC=$(QTDIR)/bin/moc
IFLAGS=-I$(QTDIR)/include -I../include
LFLAGS=-L$(QTDIR)/lib -L/usr/X11R6/lib -L../lib -lqt-mt

%.o : %.cc
	g++ -g -c $*.cc -I. $(IFLAGS)

%.moc: %.cc
	$(MOC) $*.cc -o $*.moc

%.moh: ../include/%.h
	$(MOC) ../include/$*.h -o $*.moh

all: CoastRoll

CoastRoll.o: CoastRoll.moc

CoastRoll: CoastRoll.o Mesh.o Geometry.o Curve.o BezierSpline.o
	g++ -g $+ -o CoastRoll -lGL -lGLU $(LFLAGS)

