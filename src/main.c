/*******************************************************
 LEDTool
 Alternative tool to upload messages to the VADIV LED Nametag
 Device 12x48 (VID:0x483, PID:0x5750)
 STMicroelectronics 0STM32 Composite MSC+HID

 Philipp Kutsch
********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <inttypes.h>
#include <errno.h>

#include "led.h"
#include "hidapi.h"

/* Argument handling with getopt */
int getopt_long(int argc, char * const argv[], const char *optstring,
    const struct option *longopts, int *longindex);
extern char *optarg;
extern int optind, opterr, optopt;

static const struct option long_options[] = {
  { "help",      no_argument,       0, 'h' },
  { "time",      required_argument, 0, 't' },
  { "blink",     no_argument,       0, 'b' },
  { "animation", required_argument, 0, 'a' },
  { "aspeed",    required_argument, 0, 's' },
  { "line",      required_argument, 0, 'l' },
  { "ospeed",    required_argument, 0, 'o' },
  { "verbose",   no_argument,       0, 'v' },
  {0}
};


void print_usage();

/* Display all debug messages */
bool verbose = false;

int main(int argc, char **argv)
{
  /* Default LED tag options. struct is updated
     after all arguments are read */
  struct led_options options;
  options.display_time = 100;
  options.animation_type = 0;
  options.animation_speed = 5;
  options.border_type = 0;
  options.border_speed = 5;
  options.blinking = false;

  /* Parse arguments using getopt */
  while (1)
  {
    int index = -1;
    int result = getopt_long(argc, argv, "ht:ba:s:l:o:v", long_options, &index);
    if (result == -1) break;
    switch (result)
    {
      /* Parse help argument */
      case 'h':
      {
        print_usage();
        return 0;
      }
      case 't':
      {
        /* Parse time argument */
        uintmax_t value = strtoumax(optarg, NULL, 10);
        if((value == UINTMAX_MAX && errno == ERANGE) || value > 255 || value == 0)
        {
          printf("Invalid argument: -t TIME, --time TIME  Display time (Range 1...255)\n");
          return 0;
        }
        options.display_time = (uint8_t)value;
      }break;
      case 'b':
      {
        /* Parse blink argument */
        options.blinking = true;
      }break;
      case 'a':
      {
        /* Parse animation argument */
        uintmax_t value = strtoumax(optarg, NULL, 10);
        if((value == UINTMAX_MAX && errno == ERANGE) || value > 11)
        {
          printf("Invalid argument: -a ANIM, --animation ANIM  Set building text animation\n");
          return 0;
        }

        options.animation_type = (uint8_t)value;
      }break;
      case 's':
      {
        /* Parse animation speed argument */
        uintmax_t value = strtoumax(optarg, NULL, 10);
        if((value == UINTMAX_MAX && errno == ERANGE) || value > 9 || value == 0)
        {
          printf("Invalid argument: -s ASPEED, --aspeed ASPEED  Animation speed (Range 1...9)\n");
          return 0;
        }

        options.animation_speed = (uint8_t)value;
      }break;
      case 'l':
      {
        /* Parse outline argument */
        uintmax_t value = strtoumax(optarg, NULL, 10);
        if((value == UINTMAX_MAX && errno == ERANGE) || value > 14)
        {
          printf("Invalid argument: -l LINE, --line LINE  Set building outline\n");
          return 0;
        }

        options.border_type = (uint8_t)value;
      }break;

      case 'o':
      {
        /* Parse outline speed argument */
        uintmax_t value = strtoumax(optarg, NULL, 10);
        if((value == UINTMAX_MAX && errno == ERANGE) || value > 9 || value == 0)
        {
          printf("Invalid argument: -o LSPEED, --ospeed LSPEED  Outline animation speed (Range 1...9)");
          return 0;
        }

        options.border_speed = (uint8_t)value;
      }break;

      case 'v':
      {
        /* Parse verbose argument */
        verbose = true;
        led_tag_enable_verbose_output();
      }break;
    }
  }

  /* @TODO read message from argument list */
  /* @TODO maybe add other options later like bitmaps, build in images, etc.*/
  char *output_string;
  while (optind < argc)
  {
    output_string = argv[optind++];
  }

  /* Initialise LED tag */
  uint8_t res = led_tag_init();
  if(res != 0)
  {
    fprintf(stderr, "Failed to initialise led tag.\n");
    return -1;
  }

  /* Connect to LED tag */
  res = led_tag_connect();
  if(res != 0)
  {
    fprintf(stderr, "Failed to connect led tag.\n");
    led_tag_exit();
    return -1;
  }

  /* Send text to LED tag */
  res = led_tag_display_text(output_string, &options);
  if(res != 0)
  {
    fprintf(stderr, "Failed to send data to led tag.\n");
    led_tag_disconnect();
    led_tag_exit();
    return -1;
  }

  /* Disconnect LED tag and finialize */
  led_tag_disconnect();
  led_tag_exit();

  return 0;
}

/* Print usage message */
void print_usage()
{
  printf("Usage: ledtool [-h] [-t TIME] [-b] [-a ANIM] [-s ASPEED]\n");
  printf("               [-l LINE] [-o LSPEED] [-d] MESSAGE\n\n");
  printf("Alternative tool to upload messages to the VADIV LED Nametag\n");
  printf("Device 12x48 (VID:0x483, PID:0x5750) STMicroelectronics 0STM32 Composite MSC+HID\n\n");
  printf("Arguments:\n");
  printf("-h, --help\t\t\t Display this message\n");
  printf("-t TIME, --time TIME\t\t Display time (Range 1...255)\n");
  printf("-b, --blink\t\t\t Toggle text blinking\n");
  printf("-a ANIM, --animation ANIM\t Set building text animation\n");
  printf("\t\t\t\t\t 0:  No animation (DEFAULT)\n");
  printf("\t\t\t\t\t 1, 2, 3, 4:  Move up/down/left/right\n");
  printf("\t\t\t\t\t 5:  Snow\n");
  printf("\t\t\t\t\t 6:  Horizontal louvre\n");
  printf("\t\t\t\t\t 7:  Vertical louvre\n");
  printf("\t\t\t\t\t 8, 9, 10, 11:  Pull up/down/left/right\n");
  printf("-s ASPEED, --aspeed ASPEED\t Animation speed (Range 1...9)\n");
  printf("-l LINE, --line LINE\t\t Set building outline\n");
  printf("\t\t\t\t\t 0:  No outline (DEFAULT)\n");
  printf("\t\t\t\t\t 1:  Random outline\n");
  printf("\t\t\t\t\t 2:  Static outline\n");
  printf("\t\t\t\t\t 3:  Static single dotted outline\n");
  printf("\t\t\t\t\t 4:  Static double dotted outline\n");
  printf("\t\t\t\t\t 5:  Blinking outline\n");
  printf("\t\t\t\t\t 6:  Blinking single dotted outline\n");
  printf("\t\t\t\t\t 7:  Blinking double dotted outline\n");
  printf("\t\t\t\t\t 8, 9, 10, 11:  Rotating single/double/four/eight dotted outline\n");
  printf("\t\t\t\t\t 12:  Two static outlines\n");
  printf("\t\t\t\t\t 13:  One static outline\n");
  printf("\t\t\t\t\t 14:  One dotted outline\n");
  printf("-o LSPEED, --ospeed LSPEED\t Outline animation speed (Range 1...9)\n");
  printf("-v, --verbose\t\t\t Enable verbose output\n");
}
