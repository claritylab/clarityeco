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

TARGET_CLIENT = imclient 
TARGET_SERVER = imserver

CMDOBJECT_SERVER = $(HOME)/sirius/sirius-application/command-center/gen-cpp/CommandCenter.o \
                  $(HOME)/sirius/sirius-application/command-center/gen-cpp/commandcenter_types.o
# CMDOBJECT_SERVER = CommandCenter.o commandcenter_types.o
OBJECTS_CLIENT = ./gen-cpp/ImageMatchingService.o IMSClient.o 
OBJECTS_SERVER = $(CMDOBJECT_SERVER) \
					./gen-cpp/ImageMatchingService.o \
					../common/detect.o \
					../common/kp_protobuf.o \
					../common/keypoints.pb.cc \
					../common/store_mat.pb.cc \
					IMSDaemon.o

# Needed for finding header files during preprocessing
INCLUDES_DIR = $(HOME)/sirius/sirius-application/command-center/gen-cpp

# Needed for linking
BOOST_LOCATION = /usr/local/boost_1_58_0
BOOST_LIB = -L/usr/local/lib

all: $(TARGET_SERVER) $(TARGET_CLIENT)

$(TARGET_CLIENT): $(OBJECTS_CLIENT)
	$(CXX) -I $(BOOST_LOCATION) $(OBJECTS_CLIENT) -o $(TARGET_CLIENT) $(BOOST_LIB) $(LINK_FLAGS)

$(TARGET_SERVER): $(OBJECTS_SERVER)
	$(CXX) -I $(BOOST_LOCATION) $(OBJECTS_SERVER) -o $(TARGET_SERVER) $(BOOST_LIB) $(LINK_FLAGS)

%.o: %.cpp
	$(CXX) -I$(INCLUDES_DIR) -c $(CXX_FLAGS) $(EXTRA_FLAGS) $< -o $@

clean:
	rm -rf $(TARGET_CLIENT) $(TARGET_SERVER) *.o

.PHONY:	all
