/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once

#include "zos.h"

/** 
 * @addtogroup lib_displays_led_matrix8x8
 * @{
 */
/**
 * @brief Operation
*/ 
typedef enum
{
    LED_MATRIX_NO_OPERATION             = 0, //!< No operation
    LED_MATRIX_ROTATE90_CLOCKWISE       = 1, //!< Rotate 90 degrees clockwise
    LED_MATRIX_ROTATE90_ANTICLOCKWISE   = 2, //!< Rotate 90 degrees anticlockwise
} led_matrix8x8_operation_t;

#pragma pack(1)
/**
 * @brief Frame
 *
 * A frame is an 8x8 animation bitmap.  
 *
 * Each frame of animation contains exactly 8 bytes (unsigned 8 bit integer) of graphics data.
 * 
 * There is no error checking for length.  
 * 
 * A bit value of 0 represents an off pixel. A bit value of 1 represents an on pixel.
 * 
 * The final value in a frame is the time to display this frame in 1/100ths of a second. 
 * 
 * Range is 0 (no delay) to 255 (2.55 seconds). 
 * 
 * If longer delays are needed, make duplicate frames. 
 * 
 */
typedef struct
{
    uint8_t data[8];       //!< data
    uint32_t display_time; //!< display time (1/100ths of a second)
} led_matrix8x8_frame_t;   
#pragma pack()

/**
 * Animate context
 */
typedef struct
{
    zos_queue_t frame_queue;                    //!< frame queue
    led_matrix8x8_frame_t current_frame;        //!< current frame
    void (*populate_event_handler)(void *arg);  //!< event handler
} led_matrix8x8_animate_context_t;

/**
 * Context
 */

typedef struct
{
    zos_i2c_device_t device;            //!< device
    uint8_t brightness;                 //!< brightness
    uint8_t blink_rate;                 //!< blink rate
    led_matrix8x8_operation_t operation;//!< operation
    char* text;                         //!< text
    uint8_t *text_buffer;               //!< text buffer
    uint16_t *display_buffer;           //!< display buffer

    struct
    {
        uint16_t position;              //!< position
        uint16_t end;                   //!< end
        uint16_t column;                //!< column
        uint16_t rate;                  //!< rate
    } scroll;                           //!< scroll

    led_matrix8x8_animate_context_t *animate_context;
} led_matrix8x8_context_t;

/**
 * Initialize
 */
zos_result_t led_matrix8x8_init(zos_i2c_t i2c_port);

/**
 * De-initialize
 */
void led_matrix8x8_deinit(void);

/**
 * Get context
 */
const led_matrix8x8_context_t* led_matrix8x8_get_context(void);

/**
 * Set brightness
 * 
 * Ht16K33_BRIGHTNESS_MAX      15
 */
zos_result_t led_matrix8x8_set_brightness(uint8_t brightness);

/**
 * Set blink rate
 *
 * - HT16K33_BLINKRATE_OFF       0x00
 * - HT16K33_BLINKRATE_2HZ       0x01
 * - HT16K33_BLINKRATE_1HZ       0x02
 * - HT16K33_BLINKRATE_HALFHZ    0x03
 */
zos_result_t led_matrix8x8_set_blink_rate(uint8_t rate);

/**
 * Set operation
 */
zos_result_t led_matrix8x8_set_operation(led_matrix8x8_operation_t operation);

/**
 * Set text
 *
 * Dynamically allocates memory depending on the length of the text text_len:
 * DISPLAY_BUFFER_LEN + TEXT_BUFFER_LEN + text_len
 * 
 * where
 * - DISPLAY_BUFFER_LEN = (sizeof(uint16_t)*8)
 * - TEXT_BUFFER_LEN = text_len * 8
 */
zos_result_t led_matrix8x8_set_text(const char *text);

/**
 * Get text
 */
zos_result_t led_matrix8x8_get_text(const char **text);

/**
 * Update display
 */
zos_result_t led_matrix8x8_update_display(const uint8_t *display_buffer);

/**
 * Clear display
 */
zos_result_t led_matrix8x8_clear_display(void);

/**
 * Set scroll rate
 *
 * @param rate_ms The period in ms after which the display is scrolled a single pixel to the left.
 *
 * Range 0 to 65635, however setting below 10 may result in errors. Smaller is faster. 
 *
 */
zos_result_t led_matrix8x8_set_scroll_rate(uint16_t rate_ms);

/**
 * Animate
 */
zos_result_t led_matrix8x8_animate(led_matrix8x8_animate_context_t *context, void (*populate_event_handler)(void *arg), uint8_t queue_size);

/**
 * Flush frames
 */
zos_result_t led_matrix8x8_flush_frames(void);

/**
 * Push frame
 */
zos_result_t led_matrix8x8_push_frame(const led_matrix8x8_frame_t *frame, uint32_t timeout_ms);

/**
 * Get current frame
 */
zos_result_t led_matrix8x8_get_current_frame(led_matrix8x8_frame_t *frame_buffer);

/**
 * @}
*/ 
