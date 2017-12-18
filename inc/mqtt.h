//
// Created by gubin on 17-11-20.
//

#ifndef IOTSEED_SDKS_CORE_MQTT_H
#define IOTSEED_SDKS_CORE_MQTT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sysinc.h"

#include "glbtypes.h"

#include "mongoose.h"

#define IOTSEED_MG_EV_POLL              MG_EV_POLL
#define IOTSEED_MG_EV_CONNECT           MG_EV_CONNECT
#define IOTSEED_MG_EV_MQTT_CONNACK      MG_EV_MQTT_CONNACK
#define IOTSEED_MG_EV_MQTT_PUBACK       MG_EV_MQTT_PUBACK
#define IOTSEED_MG_EV_MQTT_SUBACK       MG_EV_MQTT_SUBACK
#define IOTSEED_MG_EV_MQTT_PUBLISH      MG_EV_MQTT_PUBLISH
#define IOTSEED_MG_EV_CLOSE             MG_EV_CLOSE

#ifndef USENAME_MAX_LEN
#define USENAME_MAX_LEN 63
#endif

#ifndef ADDRESS_MAX_LEN
#define ADDRESS_MAX_LEN 63
#endif

#ifndef PWD_MAX_LEN
#define PWD_MAX_LEN    63
#endif

#ifndef MG_MQTT_QOS
#define MG_MQTT_QOS(qos) ((qos) << 1)
#endif

#ifndef TOPICS_MAX_NUM
#define TOPICS_MAX_NUM  5
#endif

#ifndef TOPIC_MAX_LEN
#define TOPIC_MAX_LEN   127
#endif

typedef char TOPIC[TOPIC_MAX_LEN +1];


/*
 * Callback function (event handler) prototype. Must be defined by the user.
 * Mongoose calls the event handler, passing the events defined below.
 */
typedef void (*iotseed_mg_event_handler_t)(void *nc, int ev,
                                   void *ev_data, void *user_data) ;


typedef  void* IOTSEED_THREAD_ID;

typedef struct _mqtt_config_struct_t {
    char    s_address[ADDRESS_MAX_LEN + 1];
    char    s_username[USENAME_MAX_LEN + 1];
    char    s_password[PWD_MAX_LEN + 1];
    TOPIC   sub_topics[TOPICS_MAX_NUM];
    int     topics_size;
    void    * nc;
}IOSSEED_MQTT_CONFIG;

struct iotseed_mg_send_mqtt_handshake_opts {
    unsigned char flags;
    uint16_t keep_alive;
    const char *will_topic;
    const char *will_message;
    const char *user_name;
    const char *password;
};


struct iotseed_mg_str {
    const char *p; /* Memory chunk pointer */
    size_t len;    /* Memory chunk length */
};

struct iotseed_mg_mqtt_message {
    int cmd;
    int qos;
    int len; /* message length in the IO buffer */
    struct iotseed_mg_str topic;
    struct iotseed_mg_str payload;

    uint8_t connack_ret_code; /* connack */
    uint16_t message_id;      /* puback */

    /* connect */
    uint8_t protocol_version;
    uint8_t connect_flags;
    uint16_t keep_alive_timer;
    struct iotseed_mg_str protocol_name;
    struct iotseed_mg_str client_id;
    struct iotseed_mg_str will_topic;
    struct iotseed_mg_str will_message;
    struct iotseed_mg_str user_name;
    struct iotseed_mg_str password;
};


IOTSEED_API IOSSEED_MQTT_CONFIG* iotseed_init_mqtt_config(const char *s_address, const char *s_username,
                                                     const char *s_password, const TOPIC *s_topics,
                                                     const int topics_size);


// client
IOTSEED_API ST_RET iotseed_create_mqtt_client(const IOSSEED_MQTT_CONFIG *config);

IOTSEED_API ST_RET iotseed_destory_mqtt_client(IOSSEED_MQTT_CONFIG *config);


//connect
IOTSEED_API ST_RET iotseed_set_connected(void);

IOTSEED_API ST_RET iotseed_is_connected(void);

/*!
 *
 * @param config
 * @param handler handler 
 * @return SD_SUCCESS: success
 *         SD_FAILURE: fail
 */
IOTSEED_API ST_RET iotseed_mqtt_connect(IOSSEED_MQTT_CONFIG *config, iotseed_mg_event_handler_t handler);

IOTSEED_API ST_VOID iotseed_mg_set_protocol_mqtt(void *nc);

IOTSEED_API ST_VOID iotseed_mg_send_mqtt_handshake_opt(void *nc, const char *client_id,
                                                  struct iotseed_mg_send_mqtt_handshake_opts* opts);



//thread
IOTSEED_API IOTSEED_THREAD_ID iotseed_mg_start_thread(void *(*f)(void *), void *p);

//publish subscribe
IOTSEED_API ST_RET iotseed_mqtt_publish_msg(void *nc, const char *topic, const char *msg, const int msg_id, const int qos);

IOTSEED_API ST_RET iotseed_mqtt_subscribe_msg(void *nc, const char *topic, const int msg_id, const int qos);


#ifdef __cplusplus
}
#endif

#endif //IOTSEED_SDKS_CORE_MQTT_H
