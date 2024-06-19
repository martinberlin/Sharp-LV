// Just a quick I2C Address scan demo + MXT commands
// SOURCED FROM: https://github.com/atmel-maxtouch/mxt-app/blob/master/src/libmaxtouch/libmaxtouch.h
#include "mxt.h"

#define TOUCH_RST  GPIO_NUM_3
#define TOUCH_INT  GPIO_NUM_2

#define RESET_TOUCH 1

#define SDA_GPIO 0//21 for ESP32 std. SDA
#define SCL_GPIO 1//22 for ESP32


#define I2C_MASTER_FREQ_HZ 100000                     /*!< I2C master clock frequency */
#define I2C_SCLK_SRC_FLAG_FOR_NOMAL       (0)         /*!< Any one clock source that is available for the specified frequency may be choosen*/

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
  #define portTICK_RATE_MS portTICK_PERIOD_MS
#endif
// Enable on HIGH 5V boost converter
//#define GPIO_ENABLE_5V GPIO_NUM_38

static const char *TAG = "i2c test";

// setup i2c master
static esp_err_t i2c_master_init()
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = SDA_GPIO;
    conf.scl_io_num = SCL_GPIO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

    i2c_param_config(I2C_NUM_0, &conf);
    return i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
}

static esp_err_t i2c_master_read_reg(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t i2c_reg_lsb, uint8_t i2c_reg, uint8_t* data_rd, size_t size) {
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    // first, send device address (indicating write) & register to be read
    i2c_master_write_byte(cmd, ( i2c_addr << 1 ), ACK_CHECK_EN);
    // send register we want
    i2c_master_write_byte(cmd, i2c_reg_lsb, ACK_CHECK_DIS);
    i2c_master_write_byte(cmd, i2c_reg, ACK_CHECK_EN);
    // Send repeated start
    i2c_master_start(cmd);
    // now send device address (indicating read) & read data
    i2c_master_write_byte(cmd, ( i2c_addr << 1 ) | READ_BIT, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

static esp_err_t i2c_master_read_slave_reg(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t i2c_reg, uint8_t* data_rd, size_t size) {
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    // first, send device address (indicating write) & register to be read
    i2c_master_write_byte(cmd, ( i2c_addr << 1 ), ACK_CHECK_EN);
    // send register we want
    i2c_master_write_byte(cmd, i2c_reg, ACK_CHECK_EN);
    // Send repeated start
    i2c_master_start(cmd);
    // now send device address (indicating read) & read data
    i2c_master_write_byte(cmd, ( i2c_addr << 1 ) | READ_BIT, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

static esp_err_t i2c_master_write_reg(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t i2c_reg_lsb, uint8_t i2c_reg, uint8_t* data_wr, size_t size) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    // first, send device address (indicating write) & register to be written
    i2c_master_write_byte(cmd, ( i2c_addr << 1 ), ACK_CHECK_EN);
    // send 2 bytes register we want
    i2c_master_write_byte(cmd, i2c_reg_lsb, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, i2c_reg, ACK_CHECK_EN);
  
    // write the data
    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

static esp_err_t i2c_master_write_slave_reg(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t i2c_reg, uint8_t* data_wr, size_t size) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    // first, send device address (indicating write) & register to be written
    i2c_master_write_byte(cmd, ( i2c_addr << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    // send register we want
    i2c_master_write_byte(cmd, i2c_reg, ACK_CHECK_EN);
    // write the data
    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/* Read contents of a register
---------------------------------------------------------------------------*/
esp_err_t i2c_read_reg( uint8_t reg, uint8_t *pdata, uint8_t count ) {
	return( i2c_master_read_slave_reg( I2C_NUM_0, TOUCH_ADDR,  reg, pdata, count ) );
}

/* Write value to specified register
---------------------------------------------------------------------------*/
esp_err_t i2c_write_reg( uint8_t reg, uint8_t *pdata, uint8_t count ) {
	return( i2c_master_write_slave_reg( I2C_NUM_0, TOUCH_ADDR,  reg, pdata, count ) );
}

esp_err_t mxt_seq_read(const uint16_t addr, void *buf, const uint8_t len) {
    //const uint16_t addr_lsb = sys_cpu_to_le16(addr);
    uint8_t byte[2];
    byte[0] = addr & 0xff;
    byte[1] = (addr >> 8);
    printf("read_lsb[0]:%d addr[1]:%d LEN:%d\n", byte[0], byte[1], len);

    return( i2c_master_read_reg( I2C_NUM_0, TOUCH_ADDR,  byte[0], byte[1], buf, len ) );
}
esp_err_t mxt_seq_write(const uint16_t addr, void *buf, const uint8_t len) {
    uint8_t byte[2];
    byte[0] = addr & 0xff;
    byte[1] = (addr >> 8);
    printf("write_lsb[0]:%d addr[1]:%d LEN:%d\n", byte[0], byte[1], len);

    return( i2c_master_write_reg( I2C_NUM_0, TOUCH_ADDR,  byte[0], byte[1], buf, len ) );
}

/**
 * @deprecated not used for now, though is recommended in MXTAN0213 PDF guidelines
 * 
 * @return uint8_t 
 */
uint8_t id_num_objects() {
    uint8_t readval[7];
    esp_err_t rst = mxt_seq_read(0x00, &readval, 7);
    if (rst < 0) {
    printf("ERROR %d reading num_objects\n", rst);
    }
    /* for (int p = 0; p<sizeof(readval); p++)  {
        printf("id %d:%d ",p,readval[p]);
    } */
    ESP_LOG_BUFFER_HEX(__func__, readval, sizeof(readval));
    printf("\n");
    printf("id_num_objects[%d]=%d ",6,readval[6]);
    return readval[6];
}

/**
 * @brief The driver code can read the whole Information Block from chip 
 * to the host memory, including the 3-byte information block checksum.
 */
int read_info_block() {

    struct mxt_information_block  info = {};
    int retval = mxt_seq_read(0x00, (uint8_t*) &info, sizeof(struct mxt_information_block));
    if (retval < 0) {
        printf("%s: Error, fail to read device on host interface bus\n", __func__);
    }

    printf("Found a maXTouch: family %d, variant %d, version %d. Matrix size: "
            "%d/%d and num of objects %d\n\n",
            info.family_id, info.variant_id, info.version, info.matrix_x_size,
            info.matrix_y_size, info.num_objects);
    
    uint8_t report_id = 1;
    uint16_t object_addr = sizeof(struct mxt_information_block); // Object table starts after the info block
    struct mxt_data data;

    for (int i = 0; i < info.num_objects; i++) {
        struct mxt_object_table_element obj_table;

        int ret = mxt_seq_read(object_addr, &obj_table, sizeof(obj_table));
        if (ret < 0) {
            ESP_LOGE(__func__, "Failed to load object table %d: %d", i, ret);
            return ret;
        }
        uint16_t addr = sys_le16_to_cpu(obj_table.position);
        switch (obj_table.type) {
        case 2:
            data.t2_encryption_status_address = addr;
            break;
        case 5:
            data.t5_message_processor_address = addr;
            // We won't request a checksum, so subtract one
            data.t5_max_message_size = obj_table.size_minus_one - 1;
            break;
        case 6:
            data.t6_command_processor_address = addr;
            break;
        case 7:
            data.t7_powerconfig_address = addr;
            break;
        case 8:
            data.t8_acquisitionconfig_address = addr;
            break;
        case 44:
            data.t44_message_count_address = addr;
            break;
        case 46:
            data.t46_cte_config_address = addr;
            break;
        case 100:
            data.t100_multiple_touch_touchscreen_address = addr;
            data.t100_first_report_id = report_id;
            break;
        }

        object_addr += sizeof(obj_table);
        report_id += obj_table.report_ids_per_instance * (obj_table.instances_minus_one + 1);
    }
    printf("t2_encryption_status_address:%d\nt5_message_processor_address:%d\nt5_max_message_size:%d\nt6_command_processor_address:%d\nt7_powerconfig_address:%d\nt8_acquisitionconfig_address:%d\n\n", 
    data.t2_encryption_status_address,data.t5_message_processor_address, 
    data.t5_max_message_size,data.t6_command_processor_address,
    data.t7_powerconfig_address,data.t8_acquisitionconfig_address);
    
    if (data.t7_powerconfig_address) {
        struct mxt_gen_powerconfig_t7 t7_conf = {0};
        t7_conf.idleacqint = 10;
        t7_conf.actacqint = 10;
        t7_conf.actv2idleto = 50;

        retval = mxt_seq_write(data.t7_powerconfig_address, &t7_conf, sizeof(t7_conf));
        if (retval < 0) {
            printf("%s: Error, fail to mxt_seq_write powerconfig\n", __func__);
        }
    }
    
    return 0;
}

void app_main()
{
    // Set INT (IRQ) pin mode
    gpio_set_direction(TOUCH_INT, GPIO_MODE_INPUT);
    gpio_set_pull_mode(TOUCH_INT, GPIO_PULLUP_ONLY);

    ESP_LOGI(TAG, "SCL_GPIO = %d", SCL_GPIO);
    ESP_LOGI(TAG, "SDA_GPIO = %d", SDA_GPIO);

    char * device;
    // Let's toogle Reset
    #ifdef RESET_TOUCH
    gpio_set_direction(TOUCH_RST, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(TOUCH_RST, GPIO_PULLDOWN_ONLY);
    // TOGGLE Reset
    gpio_set_level(TOUCH_RST, 0);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    
    gpio_set_level(TOUCH_RST, 1);
    #endif

    // i2c init & scan
    if (i2c_master_init() != ESP_OK)
        ESP_LOGE(TAG, "i2c init failed\n");

     printf("I2C scan: \n");
     //while (touch != 0x24) {
     for (uint8_t i = 1; i < 127; i++)
     {
        int ret;
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, 1);
        i2c_master_stop(cmd);
        ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_RATE_MS);
        i2c_cmd_link_delete(cmd);
        int touch = 0;
        if (ret == ESP_OK)
        {
            switch (i)
            {
            case 0x20:
                device = (char *)"PCA9535";
                break;
            case 0x24:
            touch = 0x24;
                device = (char *)"TT21100 or some Touch";
                break;
            case 0x4a:
                device = (char *)"Atmel MX Touch";
                break;
            case 0x51:
                device = (char *)"PCF8563 RTC";
                break;
            case 0x68:
                device = (char *)"DS3231 /TPS PMIC";
                break;
            default:
                device = (char *)"unknown";
                break;
            }
            printf("Found device at: 0x%2x %s\n", i, device);
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    // According to MXTAN0213 PDF
    /* uint8_t num_declared_objects = id_num_objects();
    uint16_t no_of_bytes = NUM_ID_BYTES + CRC_LENGTH + num_declared_objects;
    printf("NUM_OBJECTS: %d NUM_BYTES:%d\n", num_declared_objects, no_of_bytes); */
    read_info_block(); //no_of_bytes
    vTaskDelay(pdMS_TO_TICKS(2000));


    int c = 0;
    uint8_t interrupt = 0;
    while (c<40) {
        c++;
        interrupt = gpio_get_level(TOUCH_INT);
        if (interrupt == 0) {
            printf("INT low\n");
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
