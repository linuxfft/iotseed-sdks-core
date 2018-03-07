//
// Created by gubin on 17-11-20.
//

#include "mqtt.h"
#include "str_utils.h"
#include "time_utils.h"
#include "uuid/uuid.h"

#include <atomic>
#include <thread>


struct mg_mgr* g_pMqttManager = nullptr;

static std::atomic<bool> end(false); //atomic flag

static std::atomic<bool> connected(false);

static std::thread *g_ThreadPool = nullptr;

static ST_RET create_global_manager(void) {
    if (nullptr != g_pMqttManager) {
        return SD_SUCCESS;
    }
    g_pMqttManager = (mg_mgr*)calloc(1, sizeof(struct mg_mgr));
    if (nullptr == g_pMqttManager) {
        return SD_FAILURE;
    }

    mg_mgr_init(g_pMqttManager, nullptr);

    return SD_SUCCESS;
}

static ST_RET destroy_global_manager(void) {
    ST_RET ret = 0;
    if (nullptr != g_pMqttManager) {
        return ret;
    }

    mg_mgr_free(g_pMqttManager);

    free(g_pMqttManager);

    g_pMqttManager = nullptr;

    return ret;
}

ST_VOID iotseed_mg_set_protocol_mqtt(void *nc){
    struct mg_connection * _nc = (struct mg_connection *)nc;
    mg_set_protocol_mqtt(_nc);
}

ST_VOID iotseed_mg_send_mqtt_handshake_opt(void *nc, const char *client_id,
                                struct iotseed_mg_send_mqtt_handshake_opts* opts){
    struct mg_connection * _nc = (struct mg_connection *)nc;
    struct mg_send_mqtt_handshake_opts _opts = *(struct mg_send_mqtt_handshake_opts*)opts;

    char _random_clientID[36];

    if (nullptr == client_id){
        uuid_t uuid = {0};

        uuid_generate_time_safe(uuid);
        uuid_unparse_lower(uuid, _random_clientID);
        mg_send_mqtt_handshake_opt(_nc,_random_clientID,_opts);
    } else {
        mg_send_mqtt_handshake_opt(_nc,client_id,_opts);
    }


}


ST_RET destory_mqtt_config(IOSSEED_MQTT_CONFIG** config){
    auto lConfig = *config;
    if (nullptr == lConfig){
        return SD_SUCCESS;
    }

    free(*config);
    *config = nullptr;
    return SD_SUCCESS;

}


IOSSEED_MQTT_CONFIG* iotseed_init_mqtt_config(const char *s_address, const char *sDeviceId, const char *sAccessToken,
                                              const TOPIC *s_topics, const int topics_size){
    if(nullptr == s_address || isAllWhitespace(s_address)){
        fprintf(stderr, "MQTT Broker address is required\n");
        exit(1);
    }
    if(strlen(s_address) > ADDRESS_MAX_LEN || strlen(sDeviceId) > DEVICEID_MAX_LEN || strlen(sAccessToken) > ACCESSTOKEN_MAX_LEN || topics_size > TOPICS_MAX_NUM){
        fprintf(stderr, "please check the string length\n");
        exit(1);
    }

    auto config = new IOSSEED_MQTT_CONFIG();

    strncpy(config->s_address, s_address, strlen(s_address) + 1);

    if(nullptr != sDeviceId && !isAllWhitespace(sDeviceId)){
        strncpy(config->sDeviceId, sDeviceId, strlen(sDeviceId) + 1);
    }


    if(nullptr != sAccessToken && !isAllWhitespace(sAccessToken)){
        strncpy(config->sAccessToken, sAccessToken, strlen(sAccessToken) + 1);
    }

    for(int i=0 ; i < topics_size; ++i){
        strncpy(config->sub_topics[i], s_topics[i], strlen(s_topics[i]) + 1);
    }

    config->topics_size = topics_size;

    return config;

}


static void iotseed_ev_handler(struct mg_connection *nc, int ev, void *p, void* user_data) {
    struct mg_mqtt_message *msg = (struct mg_mqtt_message *) p;
    IOSSEED_MQTT_CONFIG* config = (IOSSEED_MQTT_CONFIG*)user_data;
    (void) nc;

    if (ev != MG_EV_POLL) printf("USER HANDLER GOT EVENT %d\n", ev);

    switch (ev) {
        case MG_EV_CONNECT: {
            struct mg_send_mqtt_handshake_opts opts;
            memset(&opts, 0, sizeof(opts));
            opts.user_name = config->sDeviceId;
            opts.password = config->sAccessToken;

            mg_set_protocol_mqtt(nc);
            mg_send_mqtt_handshake_opt(nc, "iotseed", opts);
            break;
        }
        case MG_EV_MQTT_CONNACK:
            if (msg->connack_ret_code != MG_EV_MQTT_CONNACK_ACCEPTED) {
                printf("Got mqtt connection error: %d\n", msg->connack_ret_code);
                exit(1);
            }
            break;
        case MG_EV_MQTT_PUBACK:
            printf("Message publishing acknowledged (msg_id: %d)\n", msg->message_id);
            break;
        case MG_EV_MQTT_SUBACK:
            printf("Subscription acknowledged, forwarding to '/test'\n");
            break;
        case MG_EV_MQTT_PUBLISH: {
#if 0
            char hex[1024] = {0};
        mg_hexdump(nc->recv_mbuf.buf, msg->payload.len, hex, sizeof(hex));
        printf("Got incoming message %.*s:\n%s", (int)msg->topic.len, msg->topic.p, hex);
#else
            printf("Got incoming message %.*s: %.*s\n", (int) msg->topic.len,
                   msg->topic.p, (int) msg->payload.len, msg->payload.p);
#endif

//            printf("Forwarding to /test\n");
//            mg_mqtt_publish(nc, "/test", 65, MG_MQTT_QOS(0), msg->payload.p,
//                            msg->payload.len);
            break;
        }
        case MG_EV_CLOSE:
            printf("Connection closed\n");
            exit(1);
    }
}


static void thread_task(){
    while(!end.load()) {
        mg_mgr_poll(g_pMqttManager, 1000);
    }
}


ST_RET iotseed_set_connected(void){
    connected.exchange(true);
    return SD_SUCCESS;
}

ST_RET iotseed_set_disconnected(void){
    connected.exchange(false);
    return SD_SUCCESS;
}

ST_RET iotseed_is_connected(void){
    return connected.load();
}


ST_RET iotseed_create_mqtt_client(const IOSSEED_MQTT_CONFIG *config){
    if(nullptr == config){
        return SD_FAILURE;
    }
    ST_RET ret = create_global_manager();
    if (ret < 0) {
        printf("create global mgr manager fail\n");
        return ret;
    }

    return ret;
}


ST_RET iotseed_mqtt_connect_ssl(IOSSEED_MQTT_CONFIG *config, iotseed_mg_event_handler_t handler, const char* ca_cert, const char* cert, const char* key)
{
    struct mg_connect_opts opts;
    memset(&opts, 0, sizeof(opts));
    opts.ssl_cert = (cert != NULL && strlen(cert) > 0 ? cert : NULL);
    opts.ssl_key = (key != NULL && strlen(key) > 0 ? key : NULL);
    opts.ssl_ca_cert = (ca_cert != NULL && strlen(ca_cert) > 0 ? ca_cert : NULL);

    if ((config->nc = (void *)mg_connect_opt(g_pMqttManager, config->s_address, (mg_event_handler_t)handler, (void*)config,opts)) == nullptr) {
        fprintf(stderr, "mg_connect(%s) failed\n", config->s_address);
        return SD_FAILURE;
    }

    if(nullptr == g_ThreadPool){
        g_ThreadPool = new std::thread(thread_task);

    }

    return SD_SUCCESS;
}


ST_RET iotseed_mqtt_connect(IOSSEED_MQTT_CONFIG *config, iotseed_mg_event_handler_t handler)
{
    if ((config->nc = (void *)mg_connect(g_pMqttManager, config->s_address, (mg_event_handler_t)handler, (void*)config)) == nullptr) {
        fprintf(stderr, "mg_connect(%s) failed\n", config->s_address);
        return SD_FAILURE;
    }

    if(nullptr == g_ThreadPool){
        g_ThreadPool = new std::thread(thread_task);

    }

    return SD_SUCCESS;
}

ST_VOID iotseed_mqtt_disconnect(void *nc){
    mg_mqtt_disconnect((struct mg_connection*)nc); //try to send the DISCONNECT REQ
}


static ST_RET mqtt_wait_disconnect(IOSSEED_MQTT_CONFIG *config){

    end.exchange(true); //set the global END flag is true

    if(nullptr != config->nc){
        mg_mqtt_disconnect((struct mg_connection*)(config->nc)); //try to send the DISCONNECT REQ
    }

    iotseed_set_disconnected();

    //等待连接断开
    while(iotseed_is_connected() == true){
        iotseed_msSleep(100);
    }

    if(nullptr != g_ThreadPool){
        g_ThreadPool->join();
        g_ThreadPool = nullptr;
    }

    return SD_SUCCESS;
}


IOTSEED_THREAD_ID iotseed_mg_start_thread(void *(*f)(void *), void *p){
    return mg_start_thread(f,p);
}



/*!
 *
 * @param config mqtt config the mqtt pointer
 * @return
 */
ST_RET iotseed_destory_mqtt_client(IOSSEED_MQTT_CONFIG *config){

    mqtt_wait_disconnect(config);

    if(nullptr != config){
        destory_mqtt_config(&config);
    }

    ST_RET ret = destroy_global_manager();
    if (ret != SD_SUCCESS) {
#ifdef IOTSEED_DEBUG
        fprintf(stderr,"clear the global mgr manager fail\n");
#endif
        return ret;
    }
    return SD_SUCCESS;
}


//no thread safe
ST_RET iotseed_mqtt_publish_msg(void *nc, const char *topic, const char *msg, const int msg_id, const int qos){
    struct mg_connection *_nc = (struct mg_connection *)nc;
    if(nullptr == _nc)
        return SD_FAILURE;
    mg_mqtt_publish(_nc, topic, (uint16_t)msg_id, MG_MQTT_QOS(qos), msg,
                    strlen(msg));//cannot add the \0 to the payload

    return SD_SUCCESS;
}

ST_RET iotseed_mqtt_subscribe_msg(void *nc, const char *topic, const int msg_id, const int qos){
    struct mg_connection *_nc = (struct mg_connection *)nc;
    struct mg_mqtt_topic_expression s_topic_expr = {topic, (uint8_t)qos};

    if(nullptr == nc || nullptr == topic){
        fprintf(stderr,"mqtt Subscribing fail\n");
        return SD_FAILURE;
    }

    printf("Subscribing to '%s'\n", topic);
    mg_mqtt_subscribe(_nc, &s_topic_expr, 1, (uint16_t)msg_id);
    return SD_SUCCESS;
}



//ST_VOID iotseed_mqtt_ping(void * nc){
//    struct mg_connection *_nc = (struct mg_connection *)nc;
//    return mg_mqtt_ping(_nc);
//}





