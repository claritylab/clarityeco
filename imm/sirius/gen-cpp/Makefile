CXX = g++
CXX_FLAGS = -I/usr/local/boost_1_58_0

LINK_FLAGS = -L/usr/local/boost_1_58_0/stage/lib \
					-lopencv_core \
					-lopencv_highgui \
					-lopencv_imgproc \
					-lopencv_nonfree \
					-lopencv_flann \
					-lopencv_objdetect \
					-lopencv_features2d \
					-lopencv_gpu \
					-lrt \
					-lboost_program_options \
					-lboost_filesystem \
					-lboost_system \
					-lboost_thread \
					-lboost_regex \
					-lprotobuf \
					-ltesseract \
					-lthrift

%.o: %.cpp
	$(CXX) -c $(CXX_FLAGS) $(EXTRA_FLAGS) $< -o $@

clean:
	rm -rf *.o

.PHONY:	all