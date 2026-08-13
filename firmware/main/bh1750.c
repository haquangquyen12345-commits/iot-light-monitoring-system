#include "bh1750.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"   // in log ra màn hình debug

static const char *TAG = "BH1750_DRV_NEW";//Tạo một cái tên (nhãn) gắn vào mỗi dòng log in ra. 
//Giúp phân biệt log của cảm biến với log của WiFi sau này.

static i2c_master_dev_handle_t bh1750_handle;
//tạo ra một thẻ quản lý thiết bị (Handle) mang tính cục bộ

void bh1750_init(void) {
    // khai báo thông số của tuyến đường i2c(bus)
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_MASTER_NUM,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,//lọc nhiễu tín hiệu
        .flags.enable_internal_pullup = true,// bật điện trở kéo, giúp tự đẩy điện áp lên mức cao
    };
    i2c_master_bus_handle_t bus_handle;//Tạo một cái biến để hứng thẻ quản lý tuyến đường
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));
    //mở tuyến đường I2C vật lý dựa trên bản cấu hình ở trên
    // Hàm ESP_ERROR_CHECK bọc bên ngoài sẽ làm hệ thống tự khởi động lại ngay lập tức nếu việc cấu hình chân cắm bị lỗi
    // khai báo bh1750 lên bus
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BH1750_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    
    // Đăng ký thiết bị và cấp tên vào biến bh1750_handle
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &bh1750_handle));
    
    ESP_LOGI(TAG, "Khoi tao I2C BH1750 thanh cong!");
}

float bh1750_read_lux(void) {
    uint8_t cmd = BH1750_CMD_CONT_H_RES; // Lệnh đo 0x10
    uint8_t data[2] = {0};           // Mảng hứng 2 byte trả về
    
    //Ghi lệnh đo xuống cảm biến
    i2c_master_transmit(bh1750_handle, &cmd, 1, pdMS_TO_TICKS(1000));

    //Chờ 180ms để cảm biến đo ánh sáng
    vTaskDelay(pdMS_TO_TICKS(180));

    //Thu thập 2 byte báo cáo gửi về
    i2c_master_receive(bh1750_handle, data, 2, pdMS_TO_TICKS(1000));

    //Ghép 2 byte và tính ra giá trị Lux
    uint16_t raw_lux = (data[0] << 8) | data[1];//dịch byte cao sang trái 8 bit rồi ghép với byte thấp
    return raw_lux / 1.2;
}
