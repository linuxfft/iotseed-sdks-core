//
// Created by gubin on 17-11-20.
//

#ifndef IOTSEED_SDKS_CORE_MQTT_H
#define IOTSEED_SDKS_CORE_MQTT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mqtt_config_struct_t {
    char*
}MQTT_CONFIG;

extern MQTT_CONFIG* init_mqtt_config();

#ifdef __cplusplus
}
#endif

#endif //IOTSEED_SDKS_CORE_MQTT_H
