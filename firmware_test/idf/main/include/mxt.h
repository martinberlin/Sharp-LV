#include <stdint.h>
// I2C Address
#define TOUCH_ADDR 0x4a

// From MxT MXTAN0213_Interfacing_with_maXTouch
const uint8_t NUM_ID_BYTES = 7;
const uint8_t CRC_LENGTH = 3;
const uint8_t ID_OBJECTS_OFFSET = 6; /* Offset to Object Table */

// Byte swapping (Comes from Zephyr)
#define 	BSWAP_16(x)   ((uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))
// 	Convert 16-bit integer from host endianness to little-endian
#define 	sys_cpu_to_le16(val)   BSWAP_16(val)
#define 	sys_le16_to_cpu(val)   BSWAP_16(val)

struct mxt_information_block {
    uint8_t family_id;   // 0
    uint8_t variant_id;
    uint8_t version;
    uint8_t build;
    uint8_t matrix_x_size;
    uint8_t matrix_y_size;
    uint8_t num_objects; // 6
} __packed;

struct mxt_data {
    /* const struct device *dev;
    struct gpio_callback gpio_cb;
    struct k_work work; */

    uint16_t t2_encryption_status_address;
    uint16_t t5_message_processor_address;
    uint16_t t5_max_message_size;
    uint16_t t6_command_processor_address;
    uint16_t t7_powerconfig_address;
    uint16_t t8_acquisitionconfig_address;
    uint16_t t44_message_count_address;
    uint16_t t46_cte_config_address;
    uint16_t t100_multiple_touch_touchscreen_address;

    uint16_t t100_first_report_id;
};
struct mxt_object_table_element {
    uint8_t type;
    uint16_t position;
    uint8_t size_minus_one;
    uint8_t instances_minus_one;
    uint8_t report_ids_per_instance;
};
struct mxt_gen_powerconfig_t7 {
    uint8_t idleacqint;
    uint8_t actacqint;
    uint8_t actv2idleto;
    uint8_t cfg;
    uint8_t cfg2;
    uint8_t idleacqintfine;
    uint8_t actvaqintfine;
};
struct gen_commandprocessor_t6_config_t {
    uint8_t reset;
    /* Force chip reset
    */
    uint8_t backupnv;
    /* Force backup to NVM */
    uint8_t calibrate;
    /* Force recalibration */
    /* the rest of the bytes are reserved */
    uint8_t reserved1;
    uint8_t reserved2;
};

struct mxt_message {
    uint8_t report_id;
    uint8_t data[5];
};

/* Values to write to the command processor fields */
#define RESET_COMMAND       0x01
#define BOOTLOADER_COMMAND  0xA5
#define BACKUPNV_COMMAND    0x55
#define CALIBRATE_COMMAND   0x01

/* GEN_COMMANDPROCESSOR_T6 Register offsets from T6 base address */
#define MXT_T6_RESET_OFFSET      0x00
#define MXT_T6_BACKUPNV_OFFSET   0x01
#define MXT_T6_CALIBRATE_OFFSET  0x02
#define MXT_T6_REPORTALL_OFFSET  0x03
#define MXT_T6_RESERVED_OFFSET   0x04
#define MXT_T6_DIAGNOSTIC_OFFSET 0x05

// I2C common protocol defines
#define WRITE_BIT                          I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                           I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                       0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                      0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                            0x0              /*!< I2C ack value */
#define NACK_VAL                           0x1              /*!< I2C nack value */

/* Prefix for T5 messages */
#define MSG_PREFIX "MXT MSG:"

/* Polling delay for continually polling messages */
#define MXT_MSG_POLL_DELAY_MS 10

/* Calibrate timeout */
#define MXT_CALIBRATE_TIMEOUT 10

/* Soft reset time, no i2c activity */
#define MXT_SOFT_RESET_TIME  1000

//******************************************************************************
/// \brief Return codes
enum mxt_rc {
  MXT_SUCCESS = 0,                           /*!< Success */
  MXT_INTERNAL_ERROR = 1,                    /*!< Internal error/assert */
  MXT_ERROR_IO = 2,                          /*!< Input/output error */
  MXT_ERROR_NO_MEM = 3,                      /*!< Memory allocation failure */
  MXT_ERROR_TIMEOUT = 4,                     /*!< Timeout */
  MXT_ERROR_NO_DEVICE = 5,                   /*!< Could not find a device or device went away */
  MXT_ERROR_ACCESS = 6,                      /*!< Permission denied */
  MXT_ERROR_NOT_SUPPORTED = 7,               /*!< Operation not allowed for this device type */
  MXT_ERROR_INTERRUPTED = 8,                 /*!< Interrupt function call */
  MXT_ERROR_OBJECT_NOT_FOUND = 9,            /*!< Object not available on device */
  MXT_ERROR_NO_MESSAGE = 10,                 /*!< Received unexpected invalid message from message processor */
  MXT_ERROR_SELF_TEST_INVALID = 11,          /*!< Self test invalid test command */
  MXT_ERROR_SELF_TEST_ANALOG = 12,           /*!< Self test AVdd Analog power is not present */
  MXT_ERROR_SELF_TEST_PIN_FAULT = 13,        /*!< Self test Pin fault */
  MXT_ERROR_SELF_TEST_AND_GATE = 14,         /*!< Self test AND Gate Fault */
  MXT_ERROR_SELF_TEST_SIGNAL_LIMIT = 15,     /*!< Self test Signal limit fault */
  MXT_ERROR_SELF_TEST_GAIN = 16,             /*!< Self test Gain error */
  MXT_ERROR_CHECKSUM_MISMATCH = 17,          /*!< Information block or config checksum error */
  MXT_ERROR_BOOTLOADER_UNLOCKED = 18,        /*!< Bootloader already unlocked */
  MXT_ERROR_BOOTLOADER_FRAME_CRC_FAIL = 19,  /*!< Bootloader CRC failure (transmission failure) */
  MXT_ERROR_FILE_FORMAT = 20,                /*!< File format error */
  MXT_FIRMWARE_UPDATE_NOT_REQUIRED = 21,     /*!< Device firmware already required version */
  MXT_ERROR_BOOTLOADER_NO_ADDRESS = 22,      /*!< Could not identify bootloader address */
  MXT_ERROR_FIRMWARE_UPDATE_FAILED = 23,     /*!< Version on device did not match version given after bootloading operation */
  MXT_ERROR_RESET_FAILURE = 24,              /*!< Device did not reset */
  MXT_ERROR_UNEXPECTED_DEVICE_STATE = 25,    /*!< Device in unexpected state */
  MXT_ERROR_BAD_INPUT = 26,                  /*!< Incorrect command line parameters or menu input given */
  MXT_ERROR_PROTOCOL_FAULT = 27,             /*!< Bridge TCP protocol parse error */
  MXT_ERROR_CONNECTION_FAILURE = 28,         /*!< Bridge connection error */
  MXT_ERROR_SERIAL_DATA_FAILURE = 29,        /*!< Serial data download failed */
  MXT_ERROR_NOENT = 30,                      /*!< No such file or directory */
  MXT_ERROR_SELFCAP_TUNE = 31,               /*!< Error processing self cap command */
  MXT_MSG_CONTINUE = 32,                     /*!< Continue processing messages */
  MXT_BROKEN_LINE_DETECTED = 33,             /*!< Broken Line Detected */
  MXT_DEVICE_IN_BOOTLOADER = 34,             /*!< Device is in bootloader mode */
  MXT_ERROR_OBJECT_IS_VOLATILE = 35,         /*!< Object is volatile */
  MXT_SENSOR_VARIANT_DETECTED = 36,          /*!< Sensor variant issue detected */
  MXT_CTE_TEST_FAILURE = 37,                 /*!< Capacitive Touch Engine (CTE) failed */
  MXT_MEM_RAM_FAILURE = 38,                  /*!< RAM memory test failure */
  MXT_FLASH_MEM_FAILURE = 39,                /*!< Flash memory test failure */
  MXT_CLOCK_FAILURE = 40,                    /*!< Clock related test failure */
  MXT_POWER_FAILURE = 41,                    /*!< Power related test failure */
  MXT_BIST_OVERRUN = 42,                     /*!< BIST test cycle overrun */
  MXT_OND_TEST_FAILURE = 43,                 /*!< On-Demand test falure */
};
