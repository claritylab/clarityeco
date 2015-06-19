/*
 *
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "ImageMatchingService.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

int main(int argc, char** argv) {
	boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9082));
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	ImageMatchingServiceClient client(protocol);
	
	struct timeval tv1, tv2;
	try{
		ifstream fin("test.jpg", ios::binary);
		ostringstream ostrm;
                ostrm << fin.rdbuf();
		string image(ostrm.str());
		if (!fin) std::cerr << "Could not open the file!" << std::endl;

		transport->open();
		string response;

		gettimeofday(&tv1, NULL);
		client.match_img(response, image);
		gettimeofday(&tv2, NULL);
		unsigned int query_latency = (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec);		

		 cout << "client send the image successfully..." << endl;
		 cout << response << endl;
		// cout << "answer replied within " << fixed << setprecision(2) << (double)query_latency / 1000 << " ms" << endl;
		cout << fixed << setprecision(2) << (double)query_latency / 1000 << endl;
		
		transport->close();
	}catch(TException &tx){
		cout << "ERROR: " << tx.what() << endl;
	}

	return 0;
}
