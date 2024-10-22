// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: elastic_job.proto

#include "elastic_job.pb.h"
#include "elastic_job.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace elasticjob {
namespace proto {

static const char* ElasticJobService_method_names[] = {
  "/elasticjob.proto.ElasticJobService/Ping",
  "/elasticjob.proto.ElasticJobService/AddJob",
  "/elasticjob.proto.ElasticJobService/DeleteJob",
  "/elasticjob.proto.ElasticJobService/GetJobResult",
};

std::unique_ptr< ElasticJobService::Stub> ElasticJobService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< ElasticJobService::Stub> stub(new ElasticJobService::Stub(channel));
  return stub;
}

ElasticJobService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_Ping_(ElasticJobService_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_AddJob_(ElasticJobService_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_DeleteJob_(ElasticJobService_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetJobResult_(ElasticJobService_method_names[3], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status ElasticJobService::Stub::Ping(::grpc::ClientContext* context, const ::elasticjob::proto::PingRequest& request, ::elasticjob::proto::PongResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Ping_, context, request, response);
}

void ElasticJobService::Stub::experimental_async::Ping(::grpc::ClientContext* context, const ::elasticjob::proto::PingRequest* request, ::elasticjob::proto::PongResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Ping_, context, request, response, std::move(f));
}

void ElasticJobService::Stub::experimental_async::Ping(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::elasticjob::proto::PongResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Ping_, context, request, response, std::move(f));
}

void ElasticJobService::Stub::experimental_async::Ping(::grpc::ClientContext* context, const ::elasticjob::proto::PingRequest* request, ::elasticjob::proto::PongResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Ping_, context, request, response, reactor);
}

void ElasticJobService::Stub::experimental_async::Ping(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::elasticjob::proto::PongResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Ping_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::elasticjob::proto::PongResponse>* ElasticJobService::Stub::AsyncPingRaw(::grpc::ClientContext* context, const ::elasticjob::proto::PingRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::elasticjob::proto::PongResponse>::Create(channel_.get(), cq, rpcmethod_Ping_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::elasticjob::proto::PongResponse>* ElasticJobService::Stub::PrepareAsyncPingRaw(::grpc::ClientContext* context, const ::elasticjob::proto::PingRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::elasticjob::proto::PongResponse>::Create(channel_.get(), cq, rpcmethod_Ping_, context, request, false);
}

::grpc::Status ElasticJobService::Stub::AddJob(::grpc::ClientContext* context, const ::elasticjob::proto::AddJobRequest& request, ::elasticjob::proto::AddJobReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_AddJob_, context, request, response);
}

void ElasticJobService::Stub::experimental_async::AddJob(::grpc::ClientContext* context, const ::elasticjob::proto::AddJobRequest* request, ::elasticjob::proto::AddJobReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_AddJob_, context, request, response, std::move(f));
}

void ElasticJobService::Stub::experimental_async::AddJob(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::elasticjob::proto::AddJobReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_AddJob_, context, request, response, std::move(f));
}

void ElasticJobService::Stub::experimental_async::AddJob(::grpc::ClientContext* context, const ::elasticjob::proto::AddJobRequest* request, ::elasticjob::proto::AddJobReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_AddJob_, context, request, response, reactor);
}

void ElasticJobService::Stub::experimental_async::AddJob(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::elasticjob::proto::AddJobReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_AddJob_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::elasticjob::proto::AddJobReply>* ElasticJobService::Stub::AsyncAddJobRaw(::grpc::ClientContext* context, const ::elasticjob::proto::AddJobRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::elasticjob::proto::AddJobReply>::Create(channel_.get(), cq, rpcmethod_AddJob_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::elasticjob::proto::AddJobReply>* ElasticJobService::Stub::PrepareAsyncAddJobRaw(::grpc::ClientContext* context, const ::elasticjob::proto::AddJobRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::elasticjob::proto::AddJobReply>::Create(channel_.get(), cq, rpcmethod_AddJob_, context, request, false);
}

::grpc::Status ElasticJobService::Stub::DeleteJob(::grpc::ClientContext* context, const ::elasticjob::proto::DeleteJobRequest& request, ::elasticjob::proto::DeleteJobReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_DeleteJob_, context, request, response);
}

void ElasticJobService::Stub::experimental_async::DeleteJob(::grpc::ClientContext* context, const ::elasticjob::proto::DeleteJobRequest* request, ::elasticjob::proto::DeleteJobReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_DeleteJob_, context, request, response, std::move(f));
}

void ElasticJobService::Stub::experimental_async::DeleteJob(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::elasticjob::proto::DeleteJobReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_DeleteJob_, context, request, response, std::move(f));
}

void ElasticJobService::Stub::experimental_async::DeleteJob(::grpc::ClientContext* context, const ::elasticjob::proto::DeleteJobRequest* request, ::elasticjob::proto::DeleteJobReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_DeleteJob_, context, request, response, reactor);
}

void ElasticJobService::Stub::experimental_async::DeleteJob(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::elasticjob::proto::DeleteJobReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_DeleteJob_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::elasticjob::proto::DeleteJobReply>* ElasticJobService::Stub::AsyncDeleteJobRaw(::grpc::ClientContext* context, const ::elasticjob::proto::DeleteJobRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::elasticjob::proto::DeleteJobReply>::Create(channel_.get(), cq, rpcmethod_DeleteJob_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::elasticjob::proto::DeleteJobReply>* ElasticJobService::Stub::PrepareAsyncDeleteJobRaw(::grpc::ClientContext* context, const ::elasticjob::proto::DeleteJobRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::elasticjob::proto::DeleteJobReply>::Create(channel_.get(), cq, rpcmethod_DeleteJob_, context, request, false);
}

::grpc::Status ElasticJobService::Stub::GetJobResult(::grpc::ClientContext* context, const ::elasticjob::proto::GetJobResultRequest& request, ::elasticjob::proto::GetJobResultReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetJobResult_, context, request, response);
}

void ElasticJobService::Stub::experimental_async::GetJobResult(::grpc::ClientContext* context, const ::elasticjob::proto::GetJobResultRequest* request, ::elasticjob::proto::GetJobResultReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetJobResult_, context, request, response, std::move(f));
}

void ElasticJobService::Stub::experimental_async::GetJobResult(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::elasticjob::proto::GetJobResultReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetJobResult_, context, request, response, std::move(f));
}

void ElasticJobService::Stub::experimental_async::GetJobResult(::grpc::ClientContext* context, const ::elasticjob::proto::GetJobResultRequest* request, ::elasticjob::proto::GetJobResultReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_GetJobResult_, context, request, response, reactor);
}

void ElasticJobService::Stub::experimental_async::GetJobResult(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::elasticjob::proto::GetJobResultReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_GetJobResult_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::elasticjob::proto::GetJobResultReply>* ElasticJobService::Stub::AsyncGetJobResultRaw(::grpc::ClientContext* context, const ::elasticjob::proto::GetJobResultRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::elasticjob::proto::GetJobResultReply>::Create(channel_.get(), cq, rpcmethod_GetJobResult_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::elasticjob::proto::GetJobResultReply>* ElasticJobService::Stub::PrepareAsyncGetJobResultRaw(::grpc::ClientContext* context, const ::elasticjob::proto::GetJobResultRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::elasticjob::proto::GetJobResultReply>::Create(channel_.get(), cq, rpcmethod_GetJobResult_, context, request, false);
}

ElasticJobService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      ElasticJobService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< ElasticJobService::Service, ::elasticjob::proto::PingRequest, ::elasticjob::proto::PongResponse>(
          [](ElasticJobService::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::elasticjob::proto::PingRequest* req,
             ::elasticjob::proto::PongResponse* resp) {
               return service->Ping(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      ElasticJobService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< ElasticJobService::Service, ::elasticjob::proto::AddJobRequest, ::elasticjob::proto::AddJobReply>(
          [](ElasticJobService::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::elasticjob::proto::AddJobRequest* req,
             ::elasticjob::proto::AddJobReply* resp) {
               return service->AddJob(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      ElasticJobService_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< ElasticJobService::Service, ::elasticjob::proto::DeleteJobRequest, ::elasticjob::proto::DeleteJobReply>(
          [](ElasticJobService::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::elasticjob::proto::DeleteJobRequest* req,
             ::elasticjob::proto::DeleteJobReply* resp) {
               return service->DeleteJob(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      ElasticJobService_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< ElasticJobService::Service, ::elasticjob::proto::GetJobResultRequest, ::elasticjob::proto::GetJobResultReply>(
          [](ElasticJobService::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::elasticjob::proto::GetJobResultRequest* req,
             ::elasticjob::proto::GetJobResultReply* resp) {
               return service->GetJobResult(ctx, req, resp);
             }, this)));
}

ElasticJobService::Service::~Service() {
}

::grpc::Status ElasticJobService::Service::Ping(::grpc::ServerContext* context, const ::elasticjob::proto::PingRequest* request, ::elasticjob::proto::PongResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status ElasticJobService::Service::AddJob(::grpc::ServerContext* context, const ::elasticjob::proto::AddJobRequest* request, ::elasticjob::proto::AddJobReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status ElasticJobService::Service::DeleteJob(::grpc::ServerContext* context, const ::elasticjob::proto::DeleteJobRequest* request, ::elasticjob::proto::DeleteJobReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status ElasticJobService::Service::GetJobResult(::grpc::ServerContext* context, const ::elasticjob::proto::GetJobResultRequest* request, ::elasticjob::proto::GetJobResultReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace elasticjob
}  // namespace proto

