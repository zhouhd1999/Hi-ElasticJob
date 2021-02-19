PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

all: elasticjob_worker_service.grpc.pb.cc elasticjob_worker_service.pb.cc elasticjob_worker_service.grpc.pb.h elasticjob_worker_service.grpc.pb.h

%.grpc.pb.cc: %.proto
	$(PROTOC) -I. --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	$(PROTOC) -I. --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h
