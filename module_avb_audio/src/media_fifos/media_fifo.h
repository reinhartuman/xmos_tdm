#ifndef __media_fifo_h__
#define __media_fifo_h__
#include "media_output_fifo.h"
#include "media_input_fifo.h"

#ifdef __XC__
void media_ctl_register(chanend media_ctl,
                        media_input_fifo_t (&?input_fifos)[num_in],
                        unsigned num_in,
                        media_output_fifo_t (&?output_fifos)[num_out],
                        unsigned num_out,
                        int clk_ctl_index);
/**
 * Single core implementation of ififo/ofifo support channel interface
 */
void media_input_output_fifo_support_upto_16ch(streaming chanend samples_out,
            streaming chanend c_samples_from_adc,
            media_output_fifo_t ?output_fifos[],
            media_input_fifo_t ?input_fifos[]
            );
#endif

#endif // __media_fifo_h__
