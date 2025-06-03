#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "vlConfig.h"
#include "esp_netif.h"
#include "afdx.h"
#include"esp_log.h"
#include "crc32.h"

//INSERT YOUR LOCAL NETWORK DATA
#define WIFI_SSID "ESP32_SENDER_AP"
#define WIFI_PASS "123qweasd"
#define WIFI_IP "192.168.1.149"
#define WIFI_GATEWAY "192.168.1.1"
#define WIFI_SUBNET_MASK "255.255.255.0"
#define PORT 7000

#define MAX_ERR_MSG_LENGTH 128

//PROTOTYPES
//
/// @brief INIT WI-FI STATION WITH DEFAULT CONFIGURATION
esp_err_t WifiInitAp(void);

/// @brief SEND TASK OVER VIRTUAL-LINK
/// @param param VIRTUAL-LINK PARAMETERS
void VlSendTask(void *param);

void ReceiverErrorHandlerTask(void* param);

void ConfigureStaticIp()
{
    esp_netif_ip_info_t ipInfo;
    ip4_addr_t  ip,
                gw,
                netmask;
    ip4addr_aton(WIFI_IP, &ip);
    ip4addr_aton(WIFI_GATEWAY, &gw);
    ip4addr_aton(WIFI_SUBNET_MASK, &netmask);

    ipInfo.ip.addr = ip.addr;
    ipInfo.gw.addr = gw.addr;
    ipInfo.netmask.addr = netmask.addr;

    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    if (netif == NULL)
    {
        ESP_LOGE("NETIF", "AP netif not found");
        return;
    }

    ESP_ERROR_CHECK(esp_netif_dhcps_stop(netif));
    ESP_ERROR_CHECK(esp_netif_set_ip_info(netif, &ipInfo));
}

//FUNCTIONS

esp_err_t WifiInitAp(void)
{
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&config);

    wifi_config_t wifiConfig =
    {
        .ap = 
        {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .password = WIFI_PASS,
            .max_connection = 2,
            .authmode = WIFI_AUTH_WPA2_PSK
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifiConfig));
    ESP_ERROR_CHECK(esp_wifi_start());
    return ESP_OK;
}

void VlSendTask(void *param)
{
    VirtualLink_t *virtualLink = (VirtualLink_t *)param;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0)
    {
        ESP_LOGE(virtualLink->name, "Error to create socket");
        vTaskDelete(NULL);
    }

    struct sockaddr_in destAddr = 
    {
        .sin_family = AF_INET,
        .sin_port = htons(virtualLink->port),
        .sin_addr.s_addr = inet_addr(virtualLink->targetIp),

    };
    
    AfdxFrame_t frame;
    char payload[50];
    int missionCode = 101;
    float altitude = 5000.0f;
    TickType_t lastWakeTime = xTaskGetTickCount();

    while (true)
    {        
        uint32_t timeMs = esp_timer_get_time()/1000;

        altitude += 10.0f;
        if (altitude > 10000.0f) altitude = 5000.0f;

        snprintf(payload, sizeof(payload), "M%03d|ALT:%.1f ft|T:%lu ms", missionCode, altitude, timeMs);
        AfdxCreateFrame(&frame, virtualLink->id, payload);
        AfdxPrintFrame(&frame);

        int err = sendto(sock, &frame, sizeof(frame), 0, (struct sockaddr *)&destAddr, sizeof(destAddr));
        if (err < 0)
        {
            ESP_LOGE(virtualLink->name, "Error to send: errno %d", errno);
        }
        else
        {
            ESP_LOGI(virtualLink->name, "Frame sent to  %s:%d", virtualLink->targetIp, virtualLink->port);
        }
        
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(virtualLink->msPeriod));
    }
    close(sock);
    vTaskDelete(NULL);
}

void ReceiverErrorHandlerTask(void* param)
{
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if(sock < 0)
    {
        ESP_LOGE("ERROR_LISTENER", "Failed to create socket: errno %d", errno);
        vTaskDelete(NULL);
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        ESP_LOGE("ERROR_LISTENER", "Bind failed: errno %d", errno);
        close(sock);
        vTaskDelete(NULL);
    }

    ESP_LOGI("ERROR_LISTENER", "Listening for receiver error messages...");

    char buffer[MAX_ERR_MSG_LENGTH];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, NULL, NULL);
        if (len > 0)
        {
            buffer[len] = '\0';
            ESP_LOGW("RECEIVER ERROR", "%s", buffer);
        }
    }
    close(sock);
    vTaskDelete(NULL);
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(WifiInitAp());
    
    ConfigureStaticIp();
    crc32Init();
    xTaskCreate(ReceiverErrorHandlerTask, "ERROR_LISTENER", 4096, NULL, 5, NULL);
    for (int i = 0; i < VL_COUNT; i++)
    {
        xTaskCreate(VlSendTask, vlTable[i].name, 6144, &vlTable[i], vlTable[i].priority, NULL);
    }
}