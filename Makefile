obj-m += serdev_echo.o

all: module dt

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

dt: serdev_overlay.dts
	dtc -@ -I dts -O dtb -o serdev_overlay.dtbo serdev_overlay.dts
	# -@ : bật chế độ hỗ trợ symbol khi tạo overlay
	# -I dts: chỉ định đầu vào là Device Tree Source
	# -O dtb: chỉ định đầu ra là Device Tree Blob
	# -o serdev_overlay.dtbo: chỉ định tên tệp đầu ra
	# serdev_overlay.dts: tên tệp đầu vào để biển dịch

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -rf serdev_overlay.dtbo
