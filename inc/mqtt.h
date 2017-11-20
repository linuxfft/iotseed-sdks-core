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

//mqtt client 库头文件
#include "../lib/mongoose/mongoose.h"


typedef struct _mqtt_config_struct_t {
    char*   s_address;
    size_t  s_address_len;
    char*   s_username;
    size_t  s_username_len;
    char*   s_password;
    size_t  s_password_len;

}MQTT_CONFIG;

extern MQTT_CONFIG* init_mqtt_config();

#ifdef __cplusplus
}
#endif

#endif //IOTSEED_SDKS_CORE_MQTT_H
