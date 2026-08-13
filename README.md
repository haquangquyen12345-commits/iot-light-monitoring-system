# Thiết kế hệ thống IoT thu thập dữ liệu cảm biến ánh sáng
## Mục đích
-  Giám sát môi trường.

- Ứng dụng điều chỉnh nguồn sáng thông minh.

- Kết hợp các cảm biến khác đưa ra đánh giá, dự đoán.

## Yêu cầu hệ thống

Thiết kế hệ thống IoT:

- Thu thập dữ liệu cảm biến ánh sáng theo thời gian thực, sử dụng vi điều khiển.

- Gửi và lưu trữ dữ liệu cảm biến lên web server thông qua Wifi.

- Hiển thị và giám sát dữ liệu cảm biến thông qua giao diện web.
### Yêu cầu công nghệ

#### 1. Thiết kế thiết bị cảm biến ánh sáng 

a. Cảm biến: module BH1750 

b. MCU + truyền thông WiFi: Module ESP32, không dùng kit 

c. Có 1 nút nhấn reset, 1 nút nhấn khác, 1 led báo (dự phòng thêm chức năng (mở rộng) 

d. Nguồn: 5V (lấy từ dây usb máy tính hoặc adapter 5V) 

e. Vẽ mạch sử dụng Altium, thiết kế nhỏ gọn, hợp lý, kích thước dưới 10x10cm. 

f. Lập trình vi điều khiển kết nối mạng WiFi, đọc cảm biến và gửi dữ liệu định kỳ tới phần mềm ứng dụng Web trên máy tính thông qua giao thức HTTP. 
#### 2. Phần mềm ứng dụng Web trên máy tính 

a. Chạy 1 HTTP server để cảm biến gửi dữ liệu, dữ liệu được lưu trữ vào 1 Database (SQLite, PostgreSQL hoặc MySQL).

b. Giao diện để hiển thị, giám sát dữ liệu cảm biến:

- Hiển thị, quản lý thông tin: vị trí, v.v.. của (các) cảm biến.

- Hiển thị cường độ ánh sáng tại vị trí theo thời gian thực.

- Thống kê, vẽ đồ thị dữ liệu ánh sáng theo thời gian.

