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

typedef struct _mqtt_config_struct_t {
    char    s_address[ADDRESS_MAX_LEN + 1];
    char    s_username[USENAME_MAX_LEN + 1];
    char    s_password[PWD_MAX_LEN + 1];
    TOPIC   sub_topics[TOPICS_MAX_NUM];
    int     topics_size;
    struct mg_connection * nc;
}MQTT_CONFIG;

extern MQTT_CONFIG* init_mqtt_config(const char* s_address, const char* s_username, const char* s_password, const TOPIC *s_topics, const int topics_size);
extern ST_RET create_mqtt_client(const MQTT_CONFIG* config);

extern ST_RET destory_mqtt_client(MQTT_CONFIG* config);

extern ST_RET mqtt_publish_msg(struct mg_connection * nc,const char* topic, const char* msg, const int qos);

extern ST_RET set_connected(void);

extern ST_RET is_connected(void);


/*!
 *
 * @param config
 * @param handler 回调句柄
 * @return SD_SUCCESS: 成功
 *         SD_FAILURE: 失败
 */
extern ST_RET mqtt_connect(MQTT_CONFIG* config, mg_event_handler_t handler);

extern ST_RET mqtt_subscribe_msg(struct mg_connection * nc,const char* topic,const int msg_id, const int qos);

#ifdef __cplusplus
}
#endif

#endif //IOTSEED_SDKS_CORE_MQTT_H
