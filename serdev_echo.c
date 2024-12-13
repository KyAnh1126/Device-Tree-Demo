#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

#include <linux/serdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KyAnh@Debian");
MODULE_DESCRIPTION("Simple UART device driver for Raspberry Pi");

static int serdev_echo_probe(struct serdev_device* serdev);
static void serdev_echo_remove(struct serdev_device* serdev);

// định nghĩa serdev_echo_ids 
static struct of_device_id serdev_echo_ids[] = {
    {
        .compatible = "brightlight,echodev", // trùng với .compatible trong serdev_overlay.dts
    },
};

MODULE_DEVICE_TABLE(of, serdev_echo_ids);


// định nghĩa driver cho UART device
static struct serdev_echo_driver serdev_echo_driver = {
    .probe = serdev_echo_probe,
    .remove = serdev_echo_remove,
    .driver = {
        .name = "server_echo_driver",
        .of_match_table = serdev_echo_ids, // nhận diện thiết bị đầu vào (ie: UART device) để kernel tìm driver tương ứng là serdev_echo_driver
    },
};

// serdev_echo_recv() được gọi mỗi khi nhận được ký tự
static int serdev_echo_recv(struct serdev_device* serdev, const unsigned char* buffer, size_t size) {
    printk("serdev_echo_recv: Receive %ld bytes with \"%s\"\n", size, buffer);
    return serdev_device_write_buf(serdev, buffer, size); 
}

// định nghĩa các callbacks
static const struct serdev_device_ops serdev_echo_ops = {   
    .receive_buf = serdev_echo_recv,
};

// được gọi khi kernel nhận diện & tìm thấy driver tương ứng cho thiết bị, thực hiện đăng ký callback, mở serial port,
// thiết lập thông số kết nối,...
static int serdev_echo_probe(struct serdev_device* serdev) {
    int status;
    printk("serdev_echo_probe: Ready to probe!\n");

    serdev_device_set_client_ops(serdev, serdev_echo_ops); // đăng ký callback
    status = serdev_device_open(serdev); // mở serial port
    if(status) {
        printk("serdev_echo: Error opening serial port!\n");
        return -status;
    }

    serdev_device_set_baudrate(serdev, 9600); // 9600 Bps
    serdev_device_set_flow_control(serdev, false); 
    serdev_device_set_parity(serdev, SERDEV_PARITY_NONE);

    status = serdev_device_write_buf(serdev, "Type somthing: ", sizeof("Type something: ")); // gửi tin nhắn tới UART device
    printk("serdev_echo: Wrote %d bytes \n", status);
    return 0;
}

// được gọi khi kernel nhận diện thiết bị serial bị ngắt kết nối
static void serdev_echo_remove(struct serdev_device* serdev) {
    printk("serdev_echo_remove: Ready to remove!\n");
    serdev_device_close(serdev);
}

static int __init my_init(void) {
    printk("serdev_echo: Loading the driver...\n");
    if(serdev_device_driver_register(&serdev_echo_driver)) {
        printk("serdev_echo: Error: Could not load driver!\n");
        return -1;
    }
    return 0;
}

static void __init my_exit(void) {
    printk("serdev_echo: Unloading the driver...\n");
    serdev_device_driver_unregister(&serdev_echo_driver);
    return 0;
}

module_init(my_init);
module_exit(my_exit);