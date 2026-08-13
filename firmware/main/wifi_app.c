#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi_app.h"

static const char *TAG = "WIFI_APP";

// Thêm biến cờ toàn cục ở đầu file (dưới các thư viện)
bool is_wifi_connected = false;

// HÀM LẮNG NGHE SỰ KIỆN NÂNG CẤP
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // Sự kiện: Bị rớt mạng, mất sóng, khởi động lại cục Router
        is_wifi_connected = false; // Hạ cờ báo hiệu mất mạng
        
        ESP_LOGW(TAG, "Bi mat ket noi WiFi! ESP32 dang tu dong thu lai...");
        
        // Cố tình delay 2 giây để tránh bị spam liên tục gây treo chip
        vTaskDelay(pdMS_TO_TICKS(2000));
        esp_wifi_connect(); // Ra lệnh thử kết nối lại vô hạn, bỏ luôn s_retry_num
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        is_wifi_connected = true; // Dựng cờ báo hiệu có mạng

        ESP_LOGI(TAG, "             KET NOI THANH CONG!               ");
        ESP_LOGI(TAG, "- Dia chi IPv4   : " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG, "===============================================\n");
        ESP_LOGI(TAG, "- Subnet Mask    : " IPSTR, IP2STR(&event->ip_info.netmask));
        ESP_LOGI(TAG, "- Default Gateway: " IPSTR, IP2STR(&event->ip_info.gw));
        ESP_LOGI(TAG, "===============================================\n");
    }
}

// ==========================================================
// HÀM KHỞI TẠO WIFI CHẾ ĐỘ STATION (MÁY TRẠM)
// ==========================================================
void wifi_init_sta(void) {
    // 1. Khởi tạo bộ nhớ NVS (Bắt buộc để WiFi lưu dữ liệu hiệu chuẩn)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Cấu hình mạng cơ bản
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // 3. Khởi tạo WiFi với cấu hình mặc định của hãng
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 4. Đăng ký hàm Lắng nghe sự kiện (Đã viết ở trên)
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    // 5. Nạp cấu hình SSID và Mật khẩu
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            // Authmode mặc định là WPA2 PSK
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    
    // 6. Kích hoạt card WiFi
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Da khoi tao xong Module WiFi. Dang cho ket noi...");
}
