#ifndef _CONFIG_H_
#define _CONFIG_H_

static const uint16_t MESSAGE_HEADER_SIZE = 4; //bytes
static const uint16_t MESSAGE_MAX_SIZE = 512; //bytes

static const uint8_t MAX_MESSAGE_BUFFER_SIZE = 32; //messages

static const uint16_t DEFAULT_LOOP_DURATION = 1000; //msec

static const uint16_t MULTICAST_PORT = 12345;
static const char* MULTICAST_IP = "ff02::1";

#endif /* _CONFIG_H_ */