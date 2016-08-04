CC = gcc
CPP = g++
#put your object files here
OBJS = main.o SPImageProc.o SPPoint.o SPConfig.o SPLogger.o SPBPriorityQueue.o SPList.o SPListElement.o KDArray.o KDTree.o
#The executabel filename
EXEC = SPCBIR
INCLUDEPATH=/usr/local/lib/opencv-3.1.0/include/
LIBPATH=/usr/local/lib/opencv-3.1.0/lib/
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core
LDFLAGS = -Wl,-rpath-link,/usr/local/lib/opencv-3.1.0/lib/

CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-pedantic-errors -DNDEBUG -g				#ADD Werror remove g

C_COMP_FLAG = -std=c99 -Wall -Wextra \
-pedantic-errors -DNDEBUG -g				#ADD Werror remove g

$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -L$(LIBPATH) $(LDFLAGS) $(LIBS) -o $@
main.o: main.cpp SPImageProc.h SPConfig.h SPLogger.h SPPoint.h KDArray.h
	$(CPP)  $(CPP_COMP_FLAG) $(LDFLAGS)  -I$(INCLUDEPATH) -c $*.cpp
SPImageProc.o: SPImageProc.cpp SPImageProc.h SPConfig.h SPLogger.h SPPoint.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
#a rule for building a simple c souorce file
#use gcc -MM SPPoint.c to see the dependencies

SPPoint.o: SPPoint.c SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPConfig.o: SPConfig.c SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
 
SPLogger.o: SPLogger.c SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPBPriorityQueue.o: SPBPriorityQueue.c SPList.h SPListElement.h SPBPriorityQueue.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPList.o: SPList.c SPList.h SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPListElement.o: SPListElement.c SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c

KDArray.o: KDArray.c KDArray.h SPPoint.h SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

KDTree.o: KDTree.c KDTree.h KDArray.h SPPoint.h SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

clean:
	rm -f $(OBJS) $(EXEC)
