//
// Created by gubin on 17-11-20.
//

#include "mqtt.h"
#include "str_util.h"


struct mg_mgr* g_pMqttMsg = nullptr;

struct mg_connection *g_pConnection = nullptr;

static int create_global_msg(void) {
    int ret = 0;
    if (nullptr != g_pMqttMsg) {
        return 0;
    }
    g_pMqttMsg = (mg_mgr*)calloc(1, sizeof(struct mg_mgr));
    if (nullptr == g_pMqttMsg) {
        ret = -1;
    }

    mg_mgr_init(g_pMqttMsg, nullptr);

    return ret;
}

static ST_RET free_global_msg() {
    ST_RET ret = 0;
    if (nullptr != g_pMqttMsg) {
        return ret;
    }

    mg_mgr_free(g_pMqttMsg);

    free(g_pMqttMsg);

    g_pMqttMsg = nullptr;

    return ret;
}


int destory_mqtt_config(MQTT_CONFIG** config){
    auto lConfig = *config;
    if (nullptr == lConfig){
        return 0;
    }

    if(lConfig->s_address){
        free_string(&(lConfig->s_address));
    }

    free(*config);
    *config = nullptr;
    return 0;

}


MQTT_CONFIG* init_mqtt_config(const char* s_address, const char* s_username= nullptr, const char* s_password= nullptr){
    if(nullptr == s_address || isAllWhitespace(s_address)){
        return nullptr;
    }
    auto config = (MQTT_CONFIG*)calloc(1, sizeof(MQTT_CONFIG));

    config->s_address_len = strlen(s_address) + 1;
    config->s_address = (char *)create_string(config->s_address_len, s_address);


    if(nullptr != s_username && !isAllWhitespace(s_username)){
        config->s_username_len = strlen(s_username) + 1;
        config->s_username = (char *)create_string(config->s_username_len, s_username);
    }


    if(nullptr != s_password && !isAllWhitespace(s_password)){
        config->s_password_len = strlen(s_password) + 1;
        config->s_password = (char *)create_string(config->s_password_len, s_password);
    }


    return config;

destory:
    return nullptr;
}


static void ev_handler(struct mg_connection *nc, int ev, void *p) {
    struct mg_mqtt_message *msg = (struct mg_mqtt_message *) p;
    (void) nc;

    if (ev != MG_EV_POLL) printf("USER HANDLER GOT EVENT %d\n", ev);

    switch (ev) {
        case MG_EV_CONNECT: {
            struct mg_send_mqtt_handshake_opts opts;
            memset(&opts, 0, sizeof(opts));
            opts.user_name = s_user_name;
            opts.password = s_password;

            mg_set_protocol_mqtt(nc);
            mg_send_mqtt_handshake_opt(nc, "dummy", opts);
            break;
        }
        case MG_EV_MQTT_CONNACK:
            if (msg->connack_ret_code != MG_EV_MQTT_CONNACK_ACCEPTED) {
                printf("Got mqtt connection error: %d\n", msg->connack_ret_code);
                exit(1);
            }
            s_topic_expr.topic = rpc_s_topic;
            printf("Subscribing to '%s'\n", rpc_s_topic);
            mg_mqtt_subscribe(nc, &s_topic_expr, 1, 42);
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


int create_mqtt_client(const MQTT_CONFIG* config){
    if(nullptr == config){
        return SD_FAILURE;
    }
    int ret = create_global_msg();
    if (ret < 0) {
        printf("创建全局Msg失败\n");
        return ret;
    }

    if ((g_pConnection = mg_connect(g_pMqttMsg, config->s_address, ev_handler)) == nullptr) {
        fprintf(stderr, "mg_connect(%s) failed\n", config->s_address);
        exit(EXIT_FAILURE);
    }


    return ret;
}


//非线程安全
ST_RET mqtt_publish_msg(const char* topic, const char* msg, const int qos=0){
    if(nullptr == g_pConnection)
        return SD_FAILURE;
    mg_mqtt_publish(g_pConnection, topic, 65, MG_MQTT_QOS(qos), msg,
                    strlen(msg) + 1);

    return 0;
}

int mqtt_wait_disconnect(void){

}

ST_RET destory_mqtt_client(){

}

