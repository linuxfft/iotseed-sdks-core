//
// Created by gubin on 17-12-5.
//

#ifndef IOTSEED_SDKS_CORE_JSONRPC_H
#define IOTSEED_SDKS_CORE_JSONRPC_H


#ifdef __cplusplus
extern "C" {
#endif

#ifndef JSONRPC_VERSION
#define JSONRPC_VERSION "2.0"
#endif


struct _JSONRPCRequest_common_struct_t {

    int32_t id;
    char    *jsonrpc; // always = "2.0"
    char    *method;
}JSONRPCRequest;


struct _JSONRPCResponse_commonstruct_t {
    int32_t id;
    char    *jsonrpc; // always = "2.0"
};

#ifdef __cplusplus
}
#endif


#endif //IOTSEED_SDKS_CORE_JSONRPC_H
