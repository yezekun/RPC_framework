#include <stdio.h>
#include "common/log.hh"
#include "node/node.hh"

auto main(int argc, char **argv) -> int
{
    printf("rpc node\n");
    RPC_FRAMEWORK_LOG::GlobalLogger::init(spdlog::level::trace);
    RPC_FRAMEWORK_LOG::info("info");
    RPC_FRAMEWORK_LOG::warn("warn");
    return 0;
}
