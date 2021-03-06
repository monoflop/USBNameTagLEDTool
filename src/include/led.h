/*******************************************************
 LEDTool
 Alternative tool to upload messages to the VADIV LED Nametag
 Device 12x48 (VID:0x483, PID:0x5750)
 STMicroelectronics 0STM32 Composite MSC+HID

 Philipp Kutsch
********************************************************/

#ifndef LED_H
#define LED_H

/* Unique vendor and product id of the LED tag */
#define DEVICE_VENDOR_ID 0x483
#define DEVICE_PRODUCT_ID 0x5750

#include <stdbool.h>
#include <stdint.h>

/* All supported additional device features */
struct led_options
{
  uint8_t display_time;

  uint8_t animation_type;
  uint8_t animation_speed;

  uint8_t border_type;
  uint8_t border_speed;

  bool blinking;
};

/* Package send by the windows program to display 'test' with default options.
   A large part of the protocol is still unknown so we only change the known parts
   and send the other unknown information every time. */
static const uint8_t default_package[] = {
0x48, 0x44, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x03, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc2, 0xaa, 0x48, 0x44, 0x01, 0x69, 0x00, 0x07, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcf, 0x01, 0x00, 0x00, 0x01, 0xd0, 0xaa, 0x48, 0x44, 0x01, 0x69, 0x00, 0x07, 0x00, 0x02, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6b, 0x00, 0x05, 0x01, 0xd3, 0xaa, 0x48, 0x44, 0x01, 0x69, 0x00, 0x07, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x05, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x00, 0x30, 0xbc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xfe, 0xfe, 0xfe, 0x17, 0x43, 0xaa, 0x48, 0x44, 0x01, 0x69, 0x00, 0x07, 0x00, 0x04, 0x00, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x63, 0xdd, 0xdd, 0xc1, 0xdf, 0xdd, 0x63, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc6, 0xbb, 0xbf, 0xc7, 0xfb, 0xbb, 0xc7, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x2b, 0x5a, 0xaa, 0x48, 0x44, 0x01, 0x69, 0x00, 0x07, 0x00, 0x05, 0x00, 0x16, 0x00, 0x22, 0x00, 0x2e, 0x00, 0x3a, 0x00, 0x46, 0x00, 0x52, 0x00, 0x5e, 0x00, 0x6a, 0x00, 0x76, 0x00, 0x82, 0x00, 0x8e, 0xff, 0xff, 0x8f, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xdf, 0x9f, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0x8f, 0xff, 0xff, 0xff, 0xff, 0x8f, 0x77, 0x77, 0xef, 0xdf, 0x1c, 0x19, 0xaa, 0x48, 0x44, 0x01, 0x69, 0x00, 0x07, 0x00, 0x06, 0x00, 0xbf, 0x7f, 0x07, 0xff, 0xff, 0xff, 0xff, 0x8f, 0x77, 0xf7, 0xcf, 0xf7, 0xf7, 0x77, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xef, 0xcf, 0xaf, 0xaf, 0x6f, 0x87, 0xef, 0xe7, 0xff, 0xff, 0xff, 0xff, 0x07, 0x7f, 0x7f, 0x0f, 0xf7, 0xf7, 0x77, 0x8f, 0xff, 0xff, 0xff, 0xff, 0x8f, 0x6f, 0x7f, 0x0f, 0x77, 0x77, 0x77, 0x8f, 0xff, 0x26, 0x67, 0xaa, 0x48, 0x44, 0x01, 0x69, 0x00, 0x07, 0x00, 0x07, 0x00, 0xff, 0xff, 0xff, 0x07, 0x6f, 0xef, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xff, 0xff, 0xff, 0xff, 0x8f, 0x77, 0x77, 0x8f, 0x77, 0x77, 0x77, 0x8f, 0xff, 0xff, 0xff, 0xff, 0x8f, 0x77, 0x77, 0x77, 0x87, 0xf7, 0xb7, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xb7, 0x57, 0x4f, 0xaf, 0xd7, 0xcb, 0xab, 0xb7, 0xff, 0xff, 0x00, 0x00, 0x00, 0x25, 0x7b, 0xaa
};

void led_tag_enable_verbose_output();
uint8_t led_tag_init();
uint8_t led_tag_exit();
uint8_t led_tag_connect();
uint8_t led_tag_disconnect();
uint8_t led_tag_display_text(char* text, struct led_options* options);

#endif
