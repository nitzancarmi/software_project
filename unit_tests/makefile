CC = gcc
OBJS = kdarray_unit_test.o ../SPPoint.o ../SPConfig.o ../SPLogger.o ../SPList.o ../SPListElement.o ../KDArray.o
#The executabel filename
EXEC = TEST
INCLUDEPATH=/usr/local/include/
LIBPATH=/usr/local/lib/
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lm

CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-pedantic-errors -DNDEBUG -lm

C_COMP_FLAG = -std=c99 -Wall -Wextra \
-pedantic-errors -DNDEBUG -lm

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -L$(LIBPATH) $(LIBS) -o $@

kdarray_unit_test.o: kdarray_unit_test.c ../SPLogger.h ../SPPoint.h ../SPConfig.h ../KDArray.h unit_test_util.h
	$(CC) $(C_COMP_FLAG) -c $*.c

KDArray.o: ../KDArray.c ../KDArray.h ../SPPoint.h ../SPConfig.h ../SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPPoint.o: ../SPPoint.c ../SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPConfig.o: ../SPConfig.c ../SPConfig.h ../SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
 
SPLogger.o: ../SPLogger.c ../SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPList.o: ../SPList.c ../SPList.h ../SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPListElement.o: ../SPListElement.c ../SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c

clean:
	rm -f $(OBJS) $(EXEC)
