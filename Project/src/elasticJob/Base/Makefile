PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = ./

all: elastic_job.grpc.pb.cc elastic_job.pb.cc elastic_job.grpc.pb.h elastic_job.pb.h

%.grpc.pb.cc: %.proto
	$(PROTOC) -I. --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	$(PROTOC) -I. --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h