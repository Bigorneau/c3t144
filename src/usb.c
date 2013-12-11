#include <stdio.h>
#include <libusb-1.0/libusb.h>

#define VENDOR_ID 0x2304 /* Pinnacle Systems, Inc. */
#define PRODUCT_ID 0x0223 /* DazzleTv Sat DBA Device */

int check_vidpid(struct libusb_device *dev)
{
	int rc;
	struct libusb_device_descriptor desc;
	if ((rc = libusb_get_device_descriptor(dev, &desc)) < 0) {
		printf("libusb_get_device_descriptor failed: %s\n", libusb_strerror(rc));
		return -1;
	}
	if (desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID)
		return 0;
	return -2;
}

int main()
{
	int i, rc, active = 0;
	size_t size;
	struct libusb_context *ctx;
	struct libusb_device **list;
	struct libusb_device *dev, *found;
	static struct libusb_device_handle *h;

	if ((rc = libusb_init(&ctx)) < 0) {
		printf("libusb_init failed: %s\n", libusb_strerror(rc));
		goto exit;
	}

	libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_DEBUG);

	if ((size = libusb_get_device_list(ctx, &list)) < 0) {
		printf("libusb_get_device_list failed: %s\n", libusb_strerror(size));
		return -1;
	}

	found = NULL;
	for (i = 0; i < size; i++) {
		dev = list[i];
		if (check_vidpid(dev) == 0) {
			found = dev;
			break;
		}
	}

	if (!found) {
		printf("device(vid=0x%04x, pid=0x04%x) not found", VENDOR_ID, PRODUCT_ID);
		goto free;
	}

	if ((rc = libusb_open(found, &h)) < 0) {
		printf("libusb_open failed: %s\n", libusb_strerror(rc));
		goto free;
	}

	if (libusb_kernel_driver_active(h, 0)) {
		printf("device busy\n");
		libusb_detach_kernel_driver(h, 0);
		active = 1;
	}

	if ((rc = libusb_claim_interface(h, 0)) < 0) {
		printf("libusb_claim_interface failed: %s\n", libusb_strerror(rc));
		goto close;
	}

	sleep(10);

	/* XXX: WTF? */
	if (active)
		libusb_attach_kernel_driver(h, 0);


	if ((rc = libusb_release_interface(h, 0)) < 0) {
		printf("libusb_release_interface failed: %s\n", libusb_strerror(rc));
	}

close:
	libusb_close(h);
free:
	libusb_free_device_list(list, 1);
exit:
	libusb_exit(ctx);

	return rc;
}
