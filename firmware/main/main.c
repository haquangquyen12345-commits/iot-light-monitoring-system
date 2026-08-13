#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "wifi_app.h" 
#include "bh1750.h"

static const char *TAG = "APP_MAIN";

// Địa chỉ IP của máy tính
#define SERVER_URL "http://10.218.44.166:8000/api/data"
// TASK ĐỌC CẢM BIẾN & GỬI LÊN SERVER
void bh1750_task(void *pvParameters) {
    // Khởi tạo phần cứng cảm biến BH1750
    bh1750_init();
    // Chờ 10 giây để WiFi kết nối ổn định và nhận IP trước khi bắt đầu gửi dữ liệu
    ESP_LOGI(TAG, "Dang cho WiFi on dinh...");
    vTaskDelay(pdMS_TO_TICKS(10000)); 

    while (1) {
        // Đọc cảm biến bình thường
        float lux = bh1750_read_lux();
        
        if (lux < 0) {
            ESP_LOGE(TAG, "Khong doc duoc cam bien, Vui long kiem tra lai day cam I2C.");
        } else {
            ESP_LOGI(TAG, "Cuong do anh sang: %.2f Lux", lux);

            // BƯỚC KIỂM TRA MẠNG CHỐNG CRASH HỆ THỐNG
            if (!is_wifi_connected) {
                ESP_LOGW(TAG, "Khong co WiFi. Tam ngung day du lieu len Server...");
            } 
            else {
                // CHỈ THỰC HIỆN HTTP POST KHI CÓ MẠNG
                char post_data[100];
                sprintf(post_data, "{\"lux\": %.2f}", lux);

                esp_http_client_config_t config = {
                    .url = SERVER_URL,
                    .method = HTTP_METHOD_POST,
                    .timeout_ms = 5000, 
                };
                esp_http_client_handle_t client = esp_http_client_init(&config);

                esp_http_client_set_header(client, "Content-Type", "application/json");
                esp_http_client_set_post_field(client, post_data, strlen(post_data));

                esp_err_t err = esp_http_client_perform(client);
                if (err == ESP_OK) {
                    ESP_LOGI(TAG, " Da gui len Server, HTTP Status: %d", esp_http_client_get_status_code(client));
                } else {
                    ESP_LOGE(TAG, " Loi ket noi den Server: %s", esp_err_to_name(err));
                }
                esp_http_client_cleanup(client);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}

// ==============================================================================
// HÀM MAIN CHÍNH
// ==============================================================================
void app_main(void) {
    ESP_LOGI(TAG, "He thong dang khoi dong...");
    wifi_init_sta();

    //  Tạo Task chạy độc lập để xử lý dữ liệu cảm biến
    // Cấp phát 4096 bytes RAM là mức an toàn cho HTTP Client
    xTaskCreate(bh1750_task, "BH1750_Task", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "Da giao viec cho FreeRTOS, app_main ket thuc");
}
