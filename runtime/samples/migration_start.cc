#include "../rpc/livesness_check_client.h"
#include <stdio.h>

int main(int argc, char** argv) {
  FLAGS_logtostderr = 1;
  google::InitGoogleLogging(argv[0]);

  LivenessCheckClient checker_10240(grpc::CreateChannel(
      "202.45.128.154:10240", grpc::InsecureChannelCredentials()));
  LivenessCheckClient checker_10241(grpc::CreateChannel(
        "202.45.128.155:10241", grpc::InsecureChannelCredentials()));
  LivenessCheckClient checker_10242(grpc::CreateChannel(
        "202.45.128.156:10242", grpc::InsecureChannelCredentials()));

  // LOG(INFO)<<checker_10240.AddOutputRt();
  // LOG(INFO)<<checker_10240.AddOutputMac(10241);
  getchar();
  LOG(INFO)<<checker_10241.SetMigrationTarget("202.45.128.156",10242,1000);
  LOG(INFO)<<checker_10241.MigrateTo("202.45.128.156",10242,50000);

  return 0;
}
