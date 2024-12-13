- Đoạn code trên là một UART device driver mô tả cách giao tiếp giữa raspberry pi và client thông qua UART device
- Cụ thể, pi sẽ đọc được các dữ liệu được chuyển tới (từng ký tự một) từ UART device
- Cần sửa device tree (bằng overlay để không thay đổi source gốc) để cấu hình thêm phần cứng của thiết bị

- UART device hoạt động theo asynchronous, đồng bộ hóa dữ liệu bằng cách thiết lập tốc độ giao tiếp bằng nhau giữa thiết bị gửi và nhận (ie: 9600 bits per second)

Quá trình kết nối & tương tác giữa raspberry pi (thiết bị nhận) & laptop (thiết bị gửi) thông qua UART device:
1. laptop nhập lệnh "screen /dev/ttyUSB0 9600" để thiết lập screen tương tác với UART device với baud rate = 9600 bps (bắt buộc bằng với baud rate được thiết lập tại driver) 
2. Load LKM (serdev_echo.c) vào kernel, thực thi my_init() để đăng ký serdev_device_driver 
3. Khi kernel nhận diện được thiết bị (như USB-to-UART device) thì kernel tìm driver tương ứng là serdev_device_driver
4. Kích hoạt hàm probe để khởi tạo tài nguyên, thiết lập kết nối (ie: baud rate = 9600 bps) và gửi tín hiệu tới UART device và tới laptop bằng hàm serdev_device_write_buf()
5. Laptop nhận được tín hiệu & có thể tương tác, gửi tín hiệu tới pi thông qua screen
6. Pi nhận được tin nhắn, lúc này, driver gọi callback tương ứng là serdev_echo_recv() để xử lý tin nhắn nhận được

- Cần chỉnh /boot/config.txt:
    + enable_uart=1 để enable kết nối tới UART device
    + dtoverlay=serdev_overlay để thêm cấu hình từ serdev_overlay vào Device Tree gốc mà không cần chỉnh source gốc