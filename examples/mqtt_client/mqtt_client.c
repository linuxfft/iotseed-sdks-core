//
// Created by gubin on 17-11-27.
//

#include "mqtt.h"

#include "mongoose.h"

#include <signal.h>

static const char *s_address = "mqtt.hub.cloudahead.net:31883"; // mqtt broker by CloudAhead
static const char *s_user_name = "cloudahead";
static const char *s_password = "alano+1234567";
static char *s_device_id;

static int end = 0;

static void ev_handler(struct mg_connection *nc, int ev, void *p,void* user_data) {
    struct mg_mqtt_message *msg = (struct mg_mqtt_message *) p;
    MQTT_CONFIG* config = (MQTT_CONFIG*)user_data;
    (void) nc;

    if (ev != MG_EV_POLL) printf("USER HANDLER GOT EVENT %d\n", ev);

    switch (ev) {
        case MG_EV_CONNECT: {
            struct mg_send_mqtt_handshake_opts opts;
            memset(&opts, 0, sizeof(opts));
            opts.user_name = config->s_username;
            opts.password = config->s_password;

            mg_set_protocol_mqtt(nc);
            mg_send_mqtt_handshake_opt(nc, "iotseed", opts);
            break;
        }
        case MG_EV_MQTT_CONNACK:
            if (msg->connack_ret_code != MG_EV_MQTT_CONNACK_ACCEPTED) {
                printf("Got mqtt connection error: %d\n", msg->connack_ret_code);
                exit(1);
            }
            int qos = MG_MQTT_QOS(0);

            for(int i = 0; i < config->topics_size; ++i){
                mqtt_subscribe_msg(config->nc,config->sub_topics[i],1000, qos);
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
//            mqtt_publish_msg(config->nc,"empoweriot/devices/123/rpc/requests","test",0);
//            mg_mqtt_publish(nc, "empoweriot/devices/123/rpc/requests", 65, MG_MQTT_QOS(0), msg->payload.p,
//                            msg->payload.len);
            break;
        }
        case MG_EV_CLOSE:
            printf("Connection closed\n");
            exit(1);
    }
}

static void signal_handler(int sig_num) {
    signal(sig_num, signal_handler);  // Reinstantiate signal handler
    end = 1;
}

int main(int argc, char **argv) {
    ST_RET ret;
//    int msg_id = rand();

//    char s_topic[256] = {0};
    TOPIC sub_topics[2] = {"empoweriot/devices/123/rpc/requests","empoweriot/devices/+/telemetry"};

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    /* Parse command line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            s_device_id = argv[++i];
        }
    }


    MQTT_CONFIG* config = init_mqtt_config(s_address, s_user_name, s_password,sub_topics,2);
    if (NULL == config){
        fprintf(stderr, "初始化mqtt配置失败\n");
        exit(1);
    }

    ret = create_mqtt_client(config);

    if(ret != SD_SUCCESS){
        fprintf(stderr, "创建mqtt客户端失败\n");
        exit(1);
    }

    ret = mqtt_connect(config, ev_handler);

    if(ret != SD_SUCCESS){
        fprintf(stderr, "连接mqtt broker失败\n");
        exit(1);
    }


    while (!end){
        sleep(1);
    }

    destory_mqtt_client(config);

}