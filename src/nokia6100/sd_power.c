
#include "sd_power.h"
#include "libmaple.h"
#include "gpio.h"

/* Use maple pin 23 == PC15 */
#define SD_PWR_GPIO              GPIOC_BASE
#define SD_PWR_PIN               15
#define SD_PWR_MODE              GPIO_MODE_OUTPUT_OD


void sd_init( void )
{
  gpio_set_mode( SD_PWR_GPIO, SD_PWR_PIN, SD_PWR_MODE );
  sd_power( false );
}

void sd_power( bool on )
{
  /*!!AMM GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; */

  if (on) {
    gpio_write_bit( SD_PWR_GPIO, SD_PWR_PIN, 0 );
  } else {
    /*!AMM Chip select internal pull-down (to avoid parasite powering) 
     *     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CS;
     *         GPIO_Init(GPIO_CS, &GPIO_InitStructure); */

    gpio_write_bit( SD_PWR_GPIO, SD_PWR_PIN, 1 );
  }
}

void sd_toggle_power( void )
{
static bool power_off = true;
  sd_power( power_off );

  power_off = power_off ? false : true;
}

bool sd_chk_power( void )
{
  if ( gpio_read_bit( SD_PWR_GPIO, SD_PWR_PIN ) == 1 ) {
    return false;
  } else {
    return true;
  }

}
