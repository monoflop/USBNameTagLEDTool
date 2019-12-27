/*******************************************************
 LEDTool
 Alternative tool to upload messages to the VADIV LED Nametag
 Device 12x48 (VID:0x483, PID:0x5750)
 STMicroelectronics 0STM32 Composite MSC+HID

 Philipp Kutsch
********************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "led.h"
#include "font.h"
#include "hidapi.h"

void send_package_bundle(uint8_t* package_bundle, int package_bundle_size);
void fix_checksum(uint8_t* target_package_bundle, uint8_t index);
void build_package_bundle(uint8_t* target_package_bundle, uint8_t* bitmap, struct led_options* options);
void package_update_byte(uint8_t* target_package_bundle, uint8_t package_index, uint8_t byte_index, uint8_t byte);
void print_character_to_bitmap(uint8_t* bitmap, uint8_t x_offset, char character);
void print_package_bundle(uint8_t* target_package_bundle);

/* Display all debug messages */
bool isverbose = false;

/* Is LED tag initialised */
bool initialised = false;

/* Is LED tag connected */
bool connected = false;

/* LED tag HID device handle */
hid_device *handle;

/**
 * @brief Enable all debug messages
 */
void led_tag_enable_verbose_output()
{
  isverbose = true;
}

/**
 * @brief Initialise LED tag. At the moment we only initialise the HID library
 */
uint8_t led_tag_init()
{
  int res = hid_init();
  if(res == 0)
  {
    initialised = true;
  }
  return res;
}

/**
 * @brief Finalize LED tag. Finalize HID library
 */
uint8_t led_tag_exit()
{
  return hid_exit();
}

/**
 * @brief Connect LED tag. First enumerate all attached HID devices
 * and search for our VID and PID. If our LED tag was found
 * we try to connect.
 */
uint8_t led_tag_connect()
{
  if(!initialised)return 1;

  /* Enumerate all attached HID-Devices */
  if(isverbose)printf("Connected HID devices:\n");
  if(isverbose)printf("path | vendor_id | product_id | release_number | interface_number\n");
  int device_connected = 0;
  struct hid_device_info* devices = hid_enumerate(0, 0);
  while(devices != NULL)
  {
    if(isverbose)printf("%s 0x%x 0x%x %d %d\n", devices->path, devices->vendor_id, devices->product_id, devices->release_number, devices->interface_number);
    if(devices->vendor_id == DEVICE_VENDOR_ID && devices->product_id == DEVICE_PRODUCT_ID)
    {
      device_connected = 1;
    }
    devices = devices->next;
  }
  hid_free_enumeration(devices);

  if(!device_connected)
  {
    if(isverbose)fprintf(stderr, "Device not connected.\n");
    return 2;
  }

  /* Open the device using the VID, PID,
     and optionally the Serial number. */
  if(isverbose)printf("Try to open device\n");
  handle = hid_open(DEVICE_VENDOR_ID, DEVICE_PRODUCT_ID, NULL);
  if(handle == NULL)
  {
    if(isverbose)fprintf(stderr, "Failed to open HID device.\n");
    return 3;
  }

  if(isverbose)printf("Device connected\n");
  connected = true;
  return 0;
}

/**
 * @brief Disconnect LED tag.
 */
uint8_t led_tag_disconnect()
{
  if(!initialised)return 1;
  if(!connected)return 2;

  /* Close device */
  hid_close(handle);

  connected = false;

  return 0;
}

/**
 * @brief Display text on the LED tag
 * @param text Text
 * @param led_options Optional device behavior like blinking etc.
 * @return 0 Success
 * @return 1 Library not initialised
 * @return 2 Device not connected
 */
uint8_t led_tag_display_text(char* text, struct led_options* options)
{
  if(!initialised)return 1;
  if(!connected)return 2;

  /* Create data package bundle and bitmap */
  uint8_t package_bundle[512];
  uint8_t bitmap[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
  };

  /* @TODO At the moment we can only display 8 characters. check size */
  for (int i = 0; i < strlen(text); i++)
  {
    print_character_to_bitmap(bitmap, i * 8, text[i]);
  }

  /* Write bitmap and options into the package and send the package to the device */
  build_package_bundle(package_bundle, bitmap, options);
  send_package_bundle(package_bundle, 512);

  return 0;
}

/**
 * @brief Internal method : Send package to the device
 * @param package_bundle Package data
 * @param package_bundle_size Package size
 */
void send_package_bundle(uint8_t* package_bundle, int package_bundle_size)
{
  uint8_t complete_package[package_bundle_size + 1];
  complete_package[0] = 0x00; //Report ID
  memcpy(&complete_package[1], package_bundle, package_bundle_size);

  int bytes_written = hid_write(handle, complete_package, package_bundle_size + 1);
  if(isverbose)printf("Wrote %d bytes\n", bytes_written);
}

/**
 * @brief Internal method : Write bitmap and options into the default package
 * @param target_package_bundle Package data (512 Byte)
 * @param bitmap Bitmap data (72 Byte)
 * @param options Additional options
 */
void build_package_bundle(uint8_t* target_package_bundle, uint8_t* bitmap, struct led_options* options)
{
  /* Copy default data */
  memcpy(&target_package_bundle[0], default_package, 512);

  /* Write first part of bitmap */
  memcpy(&target_package_bundle[232], &bitmap[0], 21);

  /* Write second part of bitmap */
  memcpy(&target_package_bundle[265], &bitmap[21], 51);

  /* Apply options */
  if(options == NULL)
  {
    /* Default options */
    package_update_byte(target_package_bundle, 3, 9, 3);
    package_update_byte(target_package_bundle, 3, 10, 0);
    package_update_byte(target_package_bundle, 2, 59, 0);
    package_update_byte(target_package_bundle, 2, 60, 5);
    package_update_byte(target_package_bundle, 3, 11, 0);
    package_update_byte(target_package_bundle, 3, 12, 5);
  }
  else
  {
    package_update_byte(target_package_bundle, 3, 9, options->display_time);
    package_update_byte(target_package_bundle, 3, 10, options->blinking);
    package_update_byte(target_package_bundle, 2, 59, options->animation_type);
    package_update_byte(target_package_bundle, 2, 60, options->animation_speed);
    package_update_byte(target_package_bundle, 3, 11, options->border_type);
    package_update_byte(target_package_bundle, 3, 12, options->border_speed);
  }

  /* Fix checksums */
  fix_checksum(target_package_bundle, 3);
  fix_checksum(target_package_bundle, 4);
}


/**
 * @brief Internal method : Update a specific byte inside the package and fix the checksum
 * @param target_package_bundle Package data
 * @param package_index Package index inside the package bundle
 * @param byte_index Byte index inside the package
 * @param byte value
 */
void package_update_byte(uint8_t* target_package_bundle, uint8_t package_index, uint8_t byte_index, uint8_t byte)
{
  if(byte_index <= 1)
  {
    printf("Magic number cannot be changed\n");
    return;
  }
  else if(byte_index >= 61)
  {
    printf("Checksum and last byte cannot be changed\n");
    return;
  }

  target_package_bundle[package_index * 64 + byte_index] = byte;
  fix_checksum(target_package_bundle, package_index);
}

/**
 * @brief Internal method : Print a character to a bitmap @TODO Improve & Fix
 * @param bitmap Bitmap
 * @param x_offset X pixel offset (@TODO)
 * @param character Target character
 */
void print_character_to_bitmap(uint8_t* bitmap, uint8_t x_offset, char character)
{
  uint8_t tmp_bitmap[512];
  memset(&tmp_bitmap[0], 0xff, 512);
  uint8_t block = x_offset / 8;
  for(int i = 0; i < 12; i++)
  {
    tmp_bitmap[i + (block * 12)] = ~pixel_font[(uint8_t)character - 32][12 - i + 1];
  }

  /*uint8_t pixel_offset = x_offset % 8;
  if(pixel_offset != 0)
  {
    for(int i = 0; i < 12; i++)
    {
      tmp_bitmap[i + (block * 12)] = tmp_bitmap[i + (block * 12)] >> pixel_offset;
      tmp_bitmap[i + (block * 12)] |= (0x1 << pixel_offset);
      //invert pixel_offset bits

    }
  }*/

  /* OR bitmaps */
  for(int x = 0; x < 6; x++)
  {
    for(int y = 0; y < 12; y++)
    {
      bitmap[x * 12 + y] = ~((~bitmap[x * 12 + y]) | (~tmp_bitmap[x * 12 + y]));
    }
  }
}

/**
 * @brief Internal method : Fix checksum for a specific package
 * @param target_package_bundle Package bundle
 * @param index Package index
 */
void fix_checksum(uint8_t* target_package_bundle, uint8_t index)
{
  uint16_t checksum = 0;
  for(int i = 0; i < 61; i++)
  {
    checksum += target_package_bundle[index * 64 + i];
  }
  target_package_bundle[index * 64 + 61] = checksum >> 8;
  target_package_bundle[index * 64 + 62] = checksum & 0xff;
}

/**
 * @brief Internal method : Print all bytes of a package
 * @param target_package_bundle Package bundle
 */
void print_package_bundle(uint8_t* target_package_bundle)
{
  for(int i = 0; i < 512; i++)
  {
    printf("%02x ", target_package_bundle[i]);
    if(target_package_bundle[i] == 0xaa)
    {
      printf("\n");
    }
  }
}

/**
 * @brief Internal method : Print HID device feature report
 */
void print_feature_report()
{
  /* Device feature report returns for my device: */
  /* 00 30 03 53 00 54 00 4D 00 33 00 32 00 20 00 43 00 6F 00 6D 00 70 00 6F 00 73 00 69 00 74 00 65 00 20 00 4D 00 53 00 43 00 2B 00 48 00 49 00 44 00 */
  /* 0STM32 Composite MSC+HID */
  printf("Try to read device feature report\n");
  uint8_t buf[1024];
  int bytes_read = hid_get_feature_report(handle, buf, 1024);
  printf("Read %d bytes\n", bytes_read);
  if(bytes_read == -1)
  {
    printf("Device error: %S\n", hid_error(handle));
  }
  else
  {
    for(int i = 0; i < bytes_read; i++)
    {
      printf("%02X ", buf[i]);
    }
    printf("\n");
  }
}
