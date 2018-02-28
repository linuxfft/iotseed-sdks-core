//
// Created by gubin on 17-11-27.
//

#include "mqtt.h"

#include "time_utils.h"

#if !defined(_WIN32)
#include <signal.h>
#endif

#include "recipe.h"


static const char *rootCAPath = "./mqtt-ca-server.pem";
static const char *privateKeyPath = "mqtt-client1-key.pem";
static const char *certificatePath = "./mqtt-client1.pem";


static const char *s_address = "mqtt.hub.cloudahead.net:38883"; // mqtt broker by CloudAhead
//static const char *s_address = "180.175.136.183:31883";
static const char *sDeviceId = "deviceid";
static const char *sAccessToken = "accesstoken";

static char *s_device_id = NULL;

static char *s_token = NULL;

static volatile int end = 0;

// set_recipe
void fn_set_recipe(void *request, void *user_data)
{
    char response_payload[10086];
    memset(response_payload, 0, 10086);

    char response_topic[128];
    memset(response_topic, 0, sizeof(response_topic));

    JSONRPCRequest *rpc_request = (JSONRPCRequest *)request;
    IOSSEED_MQTT_CONFIG* config = (IOSSEED_MQTT_CONFIG*)user_data;

    // parse request
    IOTSEED_RPC_SET_RECIPE_PARAM param;
    IOTSEED_RECIPE *recipe = handler_set_recipe_params(&param, rpc_request->params);

    if(recipe)
    {
        // return ok
        IOTSEED_RPC_SET_RECIPE_RESULT result;
        result.code = 0;
        result.Recipes[0].Index = recipe->Index;

        serializer_set_recipe_result(response_payload, &result, rpc_request->id);

    }
    else
    {
        // return error
        IOTSEED_RPC_ERROR_MESSAGE error;
        error.code = -100;
        strcpy(error.message.Message, "cannot find recipe");

        serializer_error(response_payload, &error, rpc_request->id);
    }

    sprintf(response_topic, "empoweriot/devices/23f901e0-ccc3-11e7-bf1a-59e9355b22c6/rpc/response/%d", rpc_request->id);
    iotseed_mqtt_publish_msg(config->nc, response_topic, response_payload, rpc_request->id, MG_MQTT_QOS(0));
}

// active recipe
void fn_active_recipe(void *request, void *user_data)
{
    char response_payload[10086];
    memset(response_payload, 0, 10086);

    char response_topic[128];
    memset(response_topic, 0, sizeof(response_topic));

    JSONRPCRequest *rpc_request = (JSONRPCRequest *)request;
    IOSSEED_MQTT_CONFIG* config = (IOSSEED_MQTT_CONFIG*)user_data;

    IOTSEED_RPC_ACTIVE_RECIPE_PARAM param;
    if(handler_active_recipe_params(&param, rpc_request->params) == SD_FAILURE)
    {
        // return error
        IOTSEED_RPC_ERROR_MESSAGE error;
        error.code = -100;
        strcpy(error.message.Message, "active recipe failed");

        serializer_error(response_payload, &error, rpc_request->id);
    }
    else
    {
        // return ok
        IOTSEED_RPC_ACTIVE_RECIPE_RESULT result;
        result.code = 0;
        serializer_active_recipe_result(response_payload, &result, rpc_request->id);
    }

    sprintf(response_topic, "empoweriot/devices/23f901e0-ccc3-11e7-bf1a-59e9355b22c6/rpc/response/%d", rpc_request->id);
    iotseed_mqtt_publish_msg(config->nc, response_topic, response_payload, rpc_request->id, MG_MQTT_QOS(0));
}

// handle rpc request
void handle_msg(const char *msg, ST_INT32 msg_len)
{

    JSONRPCRequest rpc_request;

    char buf[10086];
    memset(buf, 0, sizeof(buf));
    memcpy(buf, msg, msg_len);

    deserializer_jsonrpc_request(buf, &rpc_request);

    dispatch_rpc_method(&rpc_request);

    show_all_recipes();
}

static void ev_handler(void *nc, int ev, void *p,void* user_data) {
    struct iotseed_mg_mqtt_message *msg = (struct iotseed_mg_mqtt_message *) p;
    IOSSEED_MQTT_CONFIG* config = (IOSSEED_MQTT_CONFIG*)user_data;
    (void) nc;

//    if (ev != IOTSEED_MG_EV_POLL) printf("USER HANDLER GOT EVENT %d\n", ev);

    switch (ev) {
        case IOTSEED_MG_EV_CONNECT: {
            struct iotseed_mg_send_mqtt_handshake_opts opts;
            memset(&opts, 0, sizeof(opts));
            opts.device_id = config->sDeviceId;
            opts.access_token = config->sAccessToken;

            iotseed_mg_set_protocol_mqtt(nc);
            iotseed_mg_send_mqtt_handshake_opt(nc, "iotseed", &opts);
            break;
        }
        case IOTSEED_MG_EV_MQTT_CONNACK:
            if (msg->connack_ret_code != MG_EV_MQTT_CONNACK_ACCEPTED) {
                printf("Got mqtt connection error: %d\n", msg->connack_ret_code);
                exit(1);
            }
            int qos = MG_MQTT_QOS(0);

            for(int i = 0; i < config->topics_size; ++i){
                iotseed_mqtt_subscribe_msg(config->nc,config->sub_topics[i],1000, qos);
            }
            iotseed_set_connected();
            break;
        case IOTSEED_MG_EV_MQTT_PUBACK:
            printf("Message publishing acknowledged (msg_id: %d)\n", msg->message_id);
            break;
        case IOTSEED_MG_EV_MQTT_SUBACK:
            printf("Subscription acknowledged, forwarding to '/test'\n");
            break;
        case IOTSEED_MG_EV_MQTT_PUBLISH: {
#if 0
            char hex[1024] = {0};
        mg_hexdump(nc->recv_mbuf.buf, msg->payload.len, hex, sizeof(hex));
        printf("Got incoming message %.*s:\n%s", (int)msg->topic.len, msg->topic.p, hex);
#else
            printf("Got incoming message %.*s: %.*s\n", (int) msg->topic.len,
                   msg->topic.p, (int) msg->payload.len, msg->payload.p);

#ifdef RPC_TEST
            handle_msg(msg->payload.p, (int) msg->payload.len);
#endif

#endif
//            mqtt_publish_msg(config->nc,"empoweriot/devices/123/rpc/requests","test",0);
//            mg_mqtt_publish(nc, "empoweriot/devices/123/rpc/requests", 65, MG_MQTT_QOS(0), msg->payload.p,
//                            msg->payload.len);
            break;
        }
        case IOTSEED_MG_EV_CLOSE:
            printf("Connection closed\n");
            if(!end){
                iotseed_mqtt_connect_ssl(config, ev_handler,rootCAPath,certificatePath,privateKeyPath); //重新连接
            }
        case IOTSEED_MG_EV_MQTT_DISCONNECT:
            printf("Disconnect from Broker\n");
            if(!end){
                iotseed_mqtt_connect_ssl(config, ev_handler,rootCAPath,certificatePath,privateKeyPath); //重新连接
            }
    }
}



#if !defined(_WIN32)
static void signal_handler(int sig_num) {
    signal(sig_num, signal_handler);  // Reinstantiate signal handler
    end = 1;
}
#endif

void *worker_thread_proc(void *param) {
    IOSSEED_MQTT_CONFIG *config = (IOSSEED_MQTT_CONFIG *) param;

    while (!end) {
        char* msg = "demo data";
        if (iotseed_is_connected()){
            iotseed_mqtt_publish_msg(config->nc, "test/echo", msg, 16, MG_MQTT_QOS(0));
        }
        iotseed_msSleep(1000);
    }
    return NULL;
}



int main(int argc, char **argv) {

    init_device_recipes(sDeviceId); //传入的是设备id

    ST_RET ret;
//    int msg_id = rand();

//    char s_topic[256] = {0};
    TOPIC sub_topics[2] = {"empoweriot/devices/23f901e0-ccc3-11e7-bf1a-59e9355b22c6/rpc/request/+","test/echo"};

#if !defined(_WIN32)
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
#endif

    /* Parse command line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            s_device_id = argv[++i];
        }
        if (strcmp(argv[i], "-a") == 0) {
            s_token = argv[++i];
        }
    }

    if (s_device_id == NULL){
        s_device_id = sDeviceId;
    }

    if (s_token == NULL){
        s_token = sAccessToken;
    }

    IOSSEED_MQTT_CONFIG* config = iotseed_init_mqtt_config(s_address, s_device_id, s_token,sub_topics,sizeof(sub_topics)/ sizeof(TOPIC));
    if (NULL == config){
        fprintf(stderr, "initial mqtt config failed\n");
        exit(1);
    }

    registry_iotseed_recipe_rpc_method(RPC_METHOD_SETRECIPE, fn_set_recipe, config);
    registry_iotseed_recipe_rpc_method(RPC_METHOD_ACTIVERECIPE, fn_active_recipe, config);

    ret = iotseed_create_mqtt_client(config);

    if(ret != SD_SUCCESS){
        fprintf(stderr, "create mqtt client failed\n");
        exit(1);
    }

    ret = iotseed_mqtt_connect_ssl(config, ev_handler,rootCAPath,certificatePath,privateKeyPath);

    if(ret != SD_SUCCESS){
        fprintf(stderr, "connect failed\n");
        exit(1);
    }

    iotseed_mg_start_thread(worker_thread_proc, config);



    while (!end){
        //critical 
        if (iotseed_is_connected()){
//            iotseed_mqtt_publish_msg(config->nc, "empower234sts", "demo data master process", 16, MG_MQTT_QOS(0));
        }
        iotseed_msSleep(1000);
    }

    iotseed_destory_mqtt_client(config);

}