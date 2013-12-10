#include <stdio.h>
#include <libusb-1.0/libusb.h>

#define VENDOR_ID 0x2304 /* Pinnacle Systems, Inc. */
#define PRODUCT_ID 0x0223 /* DazzleTv Sat DBA Device */

int main()
{
	int rc;
	struct libusb_context *ctx;
	static struct libusb_device_handle *h;

	if ((rc = libusb_init(&ctx)) < 0) {
		printf("libusb_init failed: %s\n", libusb_strerror(rc));
		return rc;
	}

	libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_DEBUG);

	h = libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID);
	if (!h) {
		printf("libusb_open_device(0x%04x, 0x%04x) failed\n", VENDOR_ID, PRODUCT_ID);
		return -1;
	}

	if ((rc = libusb_claim_interface(h, 0)) < 0) {
		printf("libusb_claim_interface failed: %s\n", libusb_strerror(rc));
		return rc;
	}

	sleep(10);

	if ((rc = libusb_release_interface(h, 0)) < 0) {
		printf("libusb_release_interface failed: %s\n", libusb_strerror(rc));
		return rc;
	}

	libusb_close(h);
	libusb_exit(ctx);

	return 0;
}
