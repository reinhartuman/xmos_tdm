#include <platform.h>
#include <print.h>
#include <xccompat.h>
#include <string.h>
#include <xscope.h>
#include "spi.h"
#include "i2c.h"
#include "avb.h"
#include "audio_clock_CS2100CP.h"
#include "audio_clock_CS2300CP.h"
#include "audio_codec_CS4270.h"
#include "debug_print.h"
#include "media_fifo.h"
#include "ethernet_board_support.h"
#include "simple_demo_controller.h"
#include "avb_1722_1_adp.h"
#include "app_config.h"
#include "avb_ethernet.h"
#include "avb_1722.h"
#include "gptp.h"
#include "media_clock_server.h"
#include "avb_1722_1.h"
#include "avb_srp.h"
#include "aem_descriptor_types.h"

#if(AVB_AUDIO_IF_i2s)
#include "audio_i2s.h"
#endif
#if(AVB_AUDIO_IF_tdm_multi)
#include "tdm_multi.h"
#include <xports-i2s.h>
#endif

on tile[0]: otp_ports_t otp_ports0 = OTP_PORTS_INITIALIZER;
avb_ethernet_ports_t avb_ethernet_ports =
{
  on ETHERNET_DEFAULT_TILE: OTP_PORTS_INITIALIZER,
    ETHERNET_SMI_INIT,
    ETHERNET_MII_INIT_full,
    ETHERNET_DEFAULT_RESET_INTERFACE_INIT
};


on tile[0]: fl_spi_ports spi_ports = {
  PORT_SPI_MISO,
  PORT_SPI_SS,
  PORT_SPI_CLK,
  PORT_SPI_MOSI,
  XS1_CLKBLK_1
};

// Buttons on Atterotech board
enum button_mask
{
  STREAM_SEL=1, REMOTE_SEL=2, CHAN_SEL=4,
  BUTTON_TIMEOUT_PERIOD = 20000000
};

#if !AVB_XA_SK_AUDIO_PLL_SLICE
// Note that this port must be at least declared to ensure it
// drives the mute low
on tile[1]: out port p_mute_led_remote = PORT_MUTE_LED_REMOTE; // mute, led remote;
on tile[1]: out port p_chan_leds = PORT_LEDS;
on tile[1]: in port p_buttons = PORT_BUTTONS;
#else
on tile[0]: out port p_leds = XS1_PORT_4F;
#endif

//***** AVB audio ports ****
#if I2C_COMBINE_SCL_SDA
on tile[AVB_I2C_TILE]: port r_i2c = PORT_I2C;
#else
on tile[AVB_I2C_TILE]: struct r_i2c r_i2c = { PORT_I2C_SCL, PORT_I2C_SDA };
#endif

on tile[0]: out buffered port:32 p_fs[1] = { PORT_SYNC_OUT };
#if(AVB_AUDIO_IF_i2s)
on tile[0]: i2s_ports_t i2s_ports =
{
  XS1_CLKBLK_3,
  XS1_CLKBLK_4,
  PORT_MCLK,
  PORT_SCLK,
  PORT_LRCLK
};

#if AVB_DEMO_ENABLE_LISTENER
on tile[0]: out buffered port:32 p_aud_dout[AVB_DEMO_NUM_CHANNELS/2] = PORT_SDATA_OUT;
#else
  #define p_aud_dout null
#endif

#if AVB_DEMO_ENABLE_TALKER
on tile[0]: in buffered port:32 p_aud_din[AVB_DEMO_NUM_CHANNELS/2] = PORT_SDATA_IN;
#else
  #define p_aud_din null
#endif
#endif

#if(AVB_AUDIO_IF_tdm_multi)
on tile[0]: xports_i2s__context_t tdm_audio_if = {
    XS1_CLKBLK_3,
    XS1_CLKBLK_4,
    PORT_MCLK,
    PORT_SCLK,
    PORT_LRCLK,
    PORT_SDATA_OUT,
    PORT_SDATA_IN,
};
#endif

#if AVB_XA_SK_AUDIO_PLL_SLICE
on tile[0]: out port p_audio_shared = PORT_AUDIO_SHARED;
#endif

#if AVB_DEMO_ENABLE_LISTENER
media_output_fifo_data_t ofifo_data[AVB_NUM_MEDIA_OUTPUTS];
media_output_fifo_t ofifos[AVB_NUM_MEDIA_OUTPUTS];
#else
  #define ofifos null
#endif

#if AVB_DEMO_ENABLE_TALKER
media_input_fifo_data_t ififo_data[AVB_NUM_MEDIA_INPUTS];
media_input_fifo_t ififos[AVB_NUM_MEDIA_INPUTS];
#else
  #define ififos null
#endif

[[combinable]] void application_task(client interface avb_interface avb, server interface avb_1722_1_control_callbacks i_1722_1_entity);

[[distributable]] void audio_hardware_setup(void)
{
#if PLL_TYPE_CS2100
  audio_clock_CS2100CP_init(r_i2c, MASTER_TO_WORDCLOCK_RATIO);
#elif PLL_TYPE_CS2300
  audio_clock_CS2300CP_init(r_i2c, MASTER_TO_WORDCLOCK_RATIO);
#endif
#if AVB_XA_SK_AUDIO_PLL_SLICE
  const int codec1_addr = 0x48;
  const int codec2_addr = 0x49;
  audio_codec_CS4270_init(p_audio_shared, 0xff, codec1_addr, r_i2c);
  audio_codec_CS4270_init(p_audio_shared, 0xff, codec2_addr, r_i2c);
#endif

  while (1) {
    select {
    }
  }
}

enum mac_rx_chans {
  MAC_RX_TO_MEDIA_CLOCK = 0,
#if AVB_DEMO_ENABLE_LISTENER
  MAC_RX_TO_LISTENER,
#endif
  MAC_RX_TO_SRP,
  MAC_RX_TO_1722_1,
  NUM_MAC_RX_CHANS
};

enum mac_tx_chans {
  MAC_TX_TO_MEDIA_CLOCK = 0,
#if AVB_DEMO_ENABLE_TALKER
  MAC_TX_TO_TALKER,
#endif
  MAC_TX_TO_SRP,
  MAC_TX_TO_1722_1,
  MAC_TX_TO_AVB_MANAGER,
  NUM_MAC_TX_CHANS
};

enum avb_manager_chans {
  AVB_MANAGER_TO_SRP = 0,
  AVB_MANAGER_TO_1722_1,
  AVB_MANAGER_TO_DEMO,
  NUM_AVB_MANAGER_CHANS
};

enum ptp_chans {
  PTP_TO_AVB_MANAGER = 0,
#if AVB_DEMO_ENABLE_TALKER
  PTP_TO_TALKER,
#endif
  PTP_TO_1722_1,
  NUM_PTP_CHANS
};

int main(void)
{
  // Ethernet channels
  chan c_mac_tx[NUM_MAC_TX_CHANS];
  chan c_mac_rx[NUM_MAC_RX_CHANS];

  // PTP channels
  chan c_ptp[NUM_PTP_CHANS];

  // AVB unit control
#if AVB_DEMO_ENABLE_TALKER
  chan c_talker_ctl[AVB_NUM_TALKER_UNITS];
#else
  #define c_talker_ctl null
#endif

#if AVB_DEMO_ENABLE_LISTENER
  chan c_listener_ctl[AVB_NUM_LISTENER_UNITS];
  chan c_buf_ctl[AVB_NUM_LISTENER_UNITS];
#else
  #define c_listener_ctl null
  #define c_buf_ctl null
#endif

  // Media control
  chan c_media_ctl[AVB_NUM_MEDIA_UNITS];
  interface media_clock_if i_media_clock_ctl;

  interface avb_interface i_avb[NUM_AVB_MANAGER_CHANS];
  interface srp_interface i_srp;
  interface avb_1722_1_control_callbacks i_1722_1_entity;
  interface spi_interface i_spi;

  par
  {
    on ETHERNET_DEFAULT_TILE: avb_ethernet_server(avb_ethernet_ports,
                                        c_mac_rx, NUM_MAC_RX_CHANS,
                                        c_mac_tx, NUM_MAC_TX_CHANS);

    on tile[0]: media_clock_server(i_media_clock_ctl,
                                   null,
                                   c_buf_ctl,
                                   AVB_NUM_LISTENER_UNITS,
                                   p_fs,
                                   c_mac_rx[MAC_RX_TO_MEDIA_CLOCK],
                                   c_mac_tx[MAC_TX_TO_MEDIA_CLOCK],
                                   c_ptp, NUM_PTP_CHANS,
                                   PTP_GRANDMASTER_CAPABLE);

    on tile[AVB_I2C_TILE]: [[distribute]] audio_hardware_setup();

    // AVB - Audio
    on tile[0]:
    {
#if AVB_DEMO_ENABLE_TALKER
      init_media_input_fifos(ififos, ififo_data, AVB_NUM_MEDIA_INPUTS);
#endif

#if AVB_DEMO_ENABLE_LISTENER
      init_media_output_fifos(ofifos, ofifo_data, AVB_NUM_MEDIA_OUTPUTS);
#endif

#if(AVB_AUDIO_IF_i2s)
      i2s_master(i2s_ports,
                 p_aud_din, AVB_NUM_MEDIA_INPUTS,
                 p_aud_dout, AVB_NUM_MEDIA_OUTPUTS,
                 MASTER_TO_WORDCLOCK_RATIO,
                 ififos,
                 ofifos,
                 c_media_ctl[0],
                 0);
#endif

#if(AVB_AUDIO_IF_tdm_multi)
        tdm_master_multi(
                tdm_audio_if,
                AVB_NUM_MEDIA_INPUTS,
                AVB_NUM_MEDIA_OUTPUTS,
                MASTER_TO_WORDCLOCK_RATIO,
#if AVB_DEMO_ENABLE_TALKER
                ififos,
#else
                null,
#endif

#if AVB_DEMO_ENABLE_LISTENER
                ofifos,
#else
                null,
#endif
                c_media_ctl[0],
                0
        );
#endif
    }

#if AVB_DEMO_ENABLE_TALKER
    // AVB Talker - must be on the same tile as the audio interface
    on tile[0]: avb_1722_talker(c_ptp[PTP_TO_TALKER],
                                c_mac_tx[MAC_TX_TO_TALKER],
                                c_talker_ctl[0],
                                AVB_NUM_SOURCES);
#endif

#if AVB_DEMO_ENABLE_LISTENER
    // AVB Listener
    on tile[0]: avb_1722_listener(c_mac_rx[MAC_RX_TO_LISTENER],
                                  c_buf_ctl[0],
                                  null,
                                  c_listener_ctl[0],
                                  AVB_NUM_SINKS);
#endif

    on tile[1]: [[combine]] par {
      avb_manager(i_avb, NUM_AVB_MANAGER_CHANS,
                  i_srp,
                  c_media_ctl,
                  c_listener_ctl,
                  c_talker_ctl,
                  c_mac_tx[MAC_TX_TO_AVB_MANAGER],
                  i_media_clock_ctl,
                  c_ptp[PTP_TO_AVB_MANAGER]);
      avb_srp_task(i_avb[AVB_MANAGER_TO_SRP],
                   i_srp,
                   c_mac_rx[MAC_RX_TO_SRP],
                   c_mac_tx[MAC_TX_TO_SRP]);
    }

    on tile[1]: application_task(i_avb[AVB_MANAGER_TO_DEMO], i_1722_1_entity);

    on tile[0].core[0]: avb_1722_1_maap_task(otp_ports0,
                                    i_avb[AVB_MANAGER_TO_1722_1],
                                    i_1722_1_entity,
                                    i_spi,
                                    c_mac_rx[MAC_RX_TO_1722_1],
                                    c_mac_tx[MAC_TX_TO_1722_1],
                                    c_ptp[PTP_TO_1722_1]);
    on tile[0].core[0]: spi_task(i_spi, spi_ports);
  }

    return 0;
}

/** The main application control task **/
[[combinable]]
void application_task(client interface avb_interface avb, server interface avb_1722_1_control_callbacks i_1722_1_entity)
{
  int button_val;
  int buttons_active = 1;
  unsigned buttons_timeout;
  int selected_chan = 0;
  timer button_tmr;

  p_mute_led_remote <: ~0;
  p_chan_leds <: ~(1 << selected_chan);
  p_buttons :> button_val;

#if AVB_DEMO_ENABLE_TALKER
  const int channels_per_stream = AVB_NUM_MEDIA_INPUTS/AVB_NUM_SOURCES;
  int map[AVB_NUM_MEDIA_INPUTS/AVB_NUM_SOURCES];
#endif
  const unsigned default_sample_rate = 48000;
  unsigned char aem_identify_control_value = 0;

  // Initialize the media clock
  avb.set_device_media_clock_type(0, DEVICE_MEDIA_CLOCK_INPUT_STREAM_DERIVED);
  avb.set_device_media_clock_rate(0, default_sample_rate);
  avb.set_device_media_clock_state(0, DEVICE_MEDIA_CLOCK_STATE_ENABLED);

#if AVB_DEMO_ENABLE_TALKER
  for (int j=0; j < AVB_NUM_SOURCES; j++)
  {
    avb.set_source_channels(j, channels_per_stream);
    for (int i = 0; i < channels_per_stream; i++)
      map[i] = j ? j*(channels_per_stream)+i  : j+i;
    avb.set_source_map(j, map, channels_per_stream);
    avb.set_source_format(j, AVB_SOURCE_FORMAT_MBLA_24BIT, default_sample_rate);
    avb.set_source_sync(j, 0); // use the media_clock defined above
  }
#endif

  avb.set_sink_format(0, AVB_SOURCE_FORMAT_MBLA_24BIT, default_sample_rate);

  while (1)
  {
    select
    {
      case buttons_active => p_buttons when pinsneq(button_val) :> unsigned new_button_val:
      {
        if ((button_val & CHAN_SEL) == CHAN_SEL && (new_button_val & CHAN_SEL) == 0)
        {
          selected_chan++;
          if (selected_chan > ((AVB_NUM_MEDIA_OUTPUTS>>1)-1))
          {
            selected_chan = 0;
          }
          p_chan_leds <: ~(1 << selected_chan);
          if (AVB_NUM_MEDIA_OUTPUTS > 2)
          {
            int map[AVB_NUM_MEDIA_OUTPUTS/AVB_NUM_SINKS];
            int len;
            enum avb_sink_state_t cur_state[AVB_NUM_SINKS];

            for (int i=0; i < AVB_NUM_SINKS; i++)
            {
              avb.get_sink_state(i, cur_state[i]);
              if (cur_state[i] != AVB_SINK_STATE_DISABLED)
                avb.set_sink_state(i, AVB_SINK_STATE_DISABLED);
            }

            for (int i=0; i < AVB_NUM_SINKS; i++)
            {
              avb.get_sink_map(i, map, len);
              for (int j=0;j<len;j++)
              {
                if (map[j] != -1)
                {
                  map[j] += 2;

                  if (map[j] > AVB_NUM_MEDIA_OUTPUTS-1)
                  {
                    map[j] = map[j]%AVB_NUM_MEDIA_OUTPUTS;
                  }
                }
              }
              avb.set_sink_map(i, map, len);
            }

            for (int i=0; i < AVB_NUM_SINKS; i++)
            {
              if (cur_state[i] != AVB_SINK_STATE_DISABLED)
                avb.set_sink_state(i, AVB_SINK_STATE_POTENTIAL);
            }
          }
          buttons_active = 0;
        }
        if (!buttons_active)
        {
          button_tmr :> buttons_timeout;
          buttons_timeout += BUTTON_TIMEOUT_PERIOD;
        }
        button_val = new_button_val;
        break;
      }
      case !buttons_active => button_tmr when timerafter(buttons_timeout) :> void:
      {
        buttons_active = 1;
        p_buttons :> button_val;
        break;
      }
      case i_1722_1_entity.get_control_value(unsigned short control_index,
                                            unsigned short &values_length,
                                            unsigned char values[AEM_MAX_CONTROL_VALUES_LENGTH_BYTES]) -> unsigned char return_status:
      {
        return_status = AECP_AEM_STATUS_NO_SUCH_DESCRIPTOR;

        switch (control_index)
        {
          case DESCRIPTOR_INDEX_CONTROL_IDENTIFY:
              values[0] = aem_identify_control_value;
              values_length = 1;
              return_status = AECP_AEM_STATUS_SUCCESS;
            break;
        }

        break;
      }

      case i_1722_1_entity.set_control_value(unsigned short control_index,
                                            unsigned short values_length,
                                            unsigned char values[AEM_MAX_CONTROL_VALUES_LENGTH_BYTES]) -> unsigned char return_status:
      {
        return_status = AECP_AEM_STATUS_NO_SUCH_DESCRIPTOR;

        switch (control_index) {
          case DESCRIPTOR_INDEX_CONTROL_IDENTIFY: {
            if (values_length == 1) {
              aem_identify_control_value = values[0];
              p_mute_led_remote <: (~0) & ~((int)aem_identify_control_value<<1);
              if (aem_identify_control_value) {
                debug_printf("IDENTIFY Ping\n");
              }
              return_status = AECP_AEM_STATUS_SUCCESS;
            }
            else
            {
              return_status = AECP_AEM_STATUS_BAD_ARGUMENTS;
            }
            break;
          }
        }


        break;
      }
    }
  }
}
