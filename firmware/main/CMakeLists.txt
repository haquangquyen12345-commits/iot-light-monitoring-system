idf_component_register(SRCS "wifi_app.c" "main.c" "bh1750.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_wifi esp_event nvs_flash esp_netif esp_http_client driver esp_driver_i2c)
