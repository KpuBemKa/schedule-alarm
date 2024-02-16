/* HTTP Restful API Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "driver/gpio.h"
#include "sdkconfig.h"
// #include "esp_vfs_semihost.h"
// #include "esp_vfs_fat.h"
// #include "esp_spiffs.h"
// #include "sdmmc_cmd.h"
// #include "nvs_flash.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"

#include "http_server.hpp"

// #define MDNS_INSTANCE "esp home web server"

static const char* TAG = "MAIN";

server::HttpServer http_server;

extern "C" void
app_main(void)
{
  // ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  // initialise_mdns();
//   netbiosns_init();
//   netbiosns_set_name(CONFIG_EXAMPLE_MDNS_HOST_NAME);

//   ESP_ERROR_CHECK(example_connect());
  // ESP_ERROR_CHECK(init_fs());
  ESP_ERROR_CHECK(http_server.StartServer("../front/web-demo"));
}
