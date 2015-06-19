/*
 *
 *
 *
 */

// import the thrift headers
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/TToString.h>
#include <thrift/transport/TSocket.h>

// import common utility headers
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <fstream>
#include <sys/time.h>
#include <cstdlib> // 06-12-15 for taking cmd line args

//boost threading libraries
#include <boost/thread.hpp>
#include <boost/bind.hpp>

// import the service headers
#include "gen-cpp/ImageMatchingService.h"
#include "../common/detect.h"
//#include "~/sirius/sirius-application/command-center/gen-cpp/CommandCenter.h"
//#include "~/sirius/sirius-application/command-center/gen-cpp/commandcenter_types.h"
#include "CommandCenter.h"
#include "commandcenter_types.h"

// define the namespace
using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace cmdcenterstubs;

// define the constant
#define THREAD_WORKS 16

class ImageMatchingServiceHandler : public ImageMatchingServiceIf {
	public:
		// put the model training here so that it only needs to
		// be trained once
		ImageMatchingServiceHandler(){
			this->matcher = new FlannBasedMatcher();
			cout << "building the image matching model..." << endl;
			build_model(this->matcher, &(this->trainImgs));
		}
		
		void match_img(string &response, const string &image){
			gettimeofday(&tp, NULL);
			long int timestamp = tp.tv_sec * 1000 + tp.tv_usec / 1000;
			string image_path = "input-" + to_string(timestamp) + ".jpg";
			ofstream imagefile(image_path.c_str(), ios::binary);
			imagefile.write(image.c_str(), image.size());
			imagefile.close();
			response = exec_match(image_path, this->matcher, &(this->trainImgs));
		}

		void ping() {
			cout << "pinged" << endl;
		}
	private:
		struct timeval tp;
		DescriptorMatcher *matcher;
		vector<string> trainImgs;
};

int main(int argc, char **argv){
	int port = 9082;
	//Register with the command center 
	int cmdcenterport = 8081;
	if (argv[1]) {
	port = atoi(argv[1]);
	} else {
	std::cout << "Using default port for imm..." << std::endl;
	}

	if (argv[2]) {
	cmdcenterport = atoi(argv[2]);
	} else {
	std::cout << "Using default port for cc..." << std::endl;
	}

	// initial the transport factory
	boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
	boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
	// initial the protocal factory
	boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
	// initial the request handler
	boost::shared_ptr<ImageMatchingServiceHandler> handler(new ImageMatchingServiceHandler());
	// initial the processor
	boost::shared_ptr<TProcessor> processor(new ImageMatchingServiceProcessor(handler));
	// initial the thread manager and factory
	boost::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(THREAD_WORKS);
	boost::shared_ptr<PosixThreadFactory> threadFactory = boost::shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
	threadManager->threadFactory(threadFactory);
	threadManager->start();

	// initial the image matching server
	TThreadPoolServer server(processor, serverTransport, transportFactory, protocolFactory, threadManager);

	cout << "Starting the image matching server..." << endl;
	boost::thread *serverThread = new boost::thread(boost::bind(&TThreadPoolServer::serve, &server));


	//register with command center
	boost::shared_ptr<TTransport> cmdsocket(new TSocket("localhost", cmdcenterport));
	boost::shared_ptr<TTransport> cmdtransport(new TBufferedTransport(cmdsocket));
	boost::shared_ptr<TProtocol> cmdprotocol(new TBinaryProtocol(cmdtransport));
	CommandCenterClient cmdclient(cmdprotocol);
	cmdtransport->open();
	cout<<"Registering image matching server with command center..."<<endl;
	MachineData mDataObj;
	mDataObj.name="localhost";
	mDataObj.port=port;
	cmdclient.registerService("IMM", mDataObj);
	cmdtransport->close();



	serverThread->join();
	// server.serve();
	cout << "Done..." << endl;
	return 0;
}
