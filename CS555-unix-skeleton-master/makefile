LIB_PATHS = -L/usr/local/lib
LIBS = -lopencv_gapi -lopencv_stitching -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_core
INCLUDE = -I/usr/local/include/opencv4
main:
	g++ -std=c++11 skeleton.cpp -o skeleton ${INCLUDE} ${LIB_PATHS} ${LIBS}
run:
	./skeleton
clean:
	rm skeleton output.tif
