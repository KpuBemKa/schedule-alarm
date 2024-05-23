#include <string.h>

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_netif_net_stack.h"
#include "esp_wifi.h"
// #include "file_serving_example_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "nvs_flash.h"

#include "http_server.hpp"
#include "wifi_manager.hpp"

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static const char TAG[] = "MAIN";

#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

const char s_base_path[] = "/spiffs";

// static int s_retry_num = 0;

// static EventGroupHandle_t s_wifi_event_group;

server::HttpServer http_server;
wifi::WiFiManager wifi_manager;

esp_err_t
example_mount_storage(const char* base_path);

extern "C" void
app_main(void)
{
  ESP_LOGI(TAG, "Starting example");

  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  /* Initialize file storage */
  ESP_ERROR_CHECK(example_mount_storage(s_base_path));

  /* Initialize event group */
  // s_wifi_event_group = xEventGroupCreate();

  /* Register Event handler */
  // ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
  // ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

  LOG_I("WiFiManager::StartAP(): %s", esp_err_to_name(wifi_manager.StartAP("test_ssid", "12345678")));

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(1'000));
  }
}
