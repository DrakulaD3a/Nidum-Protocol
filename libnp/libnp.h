/* The Nidum Protocol v0.1.0
*
* Lukáš Strnad <strnad.lukas@proton.me>
*
*/

#include <stdint.h>

typedef uint8_t np_type;
typedef uint8_t np_option;

struct np_packet {
    np_type type;
    np_option option;
    uint16_t size;

    char *payload;
};

#define NP_TYPE_INFO 0x00
#define NP_TYPE_REPLY 0x01
#define NP_TYPE_UNSET 0xff

#define NP_INFO_TEXT 0x00

#define NP_REPLY_VALID 0x00
#define NP_REPLY_BAD_TYPE 0x01
#define NP_REPLY_BAD_OPTION 0x02
#define NP_REPLY_BAD_SIZE 0x03
#define NP_REPLY_INVALID_DATA 0x04
#define NP_REPLY_UNSET 0xff

#define NP_UNSET_UNSET 0xff

#define NP_MINIMUM_PKT_SIZE 4
#define NP_MAXIMUM_PKT_SIZE 4096

struct serialize_result {
    np_option reply;
    int size;
};

void print_packet(struct np_packet *pkt);

void serialize(uint8_t buffer[NP_MAXIMUM_PKT_SIZE], struct np_packet *pkt,
               struct serialize_result *result);
void deserialize(uint8_t buffer[NP_MAXIMUM_PKT_SIZE], struct np_packet *pkt,
                 struct serialize_result *result);
