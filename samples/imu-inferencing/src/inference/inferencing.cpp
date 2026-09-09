/* The Clear BSD License
 *
 * Copyright (c) 2025 EdgeImpulse Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#
#include "inference/inferencing.h"
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/classifier/ei_print_results.h"
#include "edge-impulse-sdk/dsp/numpy.hpp"
#include "sensors/ei_accelerometer.h"
#include "sensors/ei_inertial.h"

static bool ei_run_inference(void);
static bool ei_start_impulse(void);
static bool ei_stop_impulse(void);

typedef enum {
    INFERENCE_STATE_RUNNING,
    INFERENCE_STATE_SAMPLING,
    INFERENCE_STATE_DATA_READY,
    INFERENCE_STATE_STOP
} inference_state_t;

static uint16_t samples_per_inference;
static float samples_circ_buff[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE]  __ALIGNED(4);
static int samples_wr_index = 0;
static inference_state_t state = INFERENCE_STATE_SAMPLING;

/**
 * @brief Callback to be called when new samples are available
 */
bool ei_samples_callback(const void *raw_sample, uint32_t raw_sample_size)
{
    float *sample = (float *)raw_sample;
    bool ret = false;

    for (int i = 0; i < (int)(raw_sample_size / sizeof(float)); i++) {
        if (samples_wr_index >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
            /* start from beginning of the circular buffer */
            samples_wr_index = 0;
        }
        samples_circ_buff[samples_wr_index++] = sample[i];

        if (samples_wr_index >= samples_per_inference) {
            state = INFERENCE_STATE_DATA_READY;
            ret =  true;
        }
    }

    return ret;
}

/**
 * @brief Inference state machine
 * @return true
 */
bool ei_inference_sm(void)
{
    ei_start_impulse();
    state = INFERENCE_STATE_SAMPLING;

    while(INFERENCE_STATE_STOP != state) {
        switch(state){
            case INFERENCE_STATE_SAMPLING:
                ei_fusion_accelerometer_read_data(3);
                ei_sleep(EI_CLASSIFIER_INTERVAL_MS);
                // wait for data
                break;
            case INFERENCE_STATE_DATA_READY:
                ei_printf("Data ready\n");
                // run inference, not much to do in this example
                state = INFERENCE_STATE_RUNNING;
                break;
            case INFERENCE_STATE_RUNNING:
                ei_printf("run inference\n");
                if (ei_run_inference() == false) {
                    ei_printf("ERR: Inference failed\n");
                }
                state = INFERENCE_STATE_SAMPLING;   // and back sampling
                break;
            case INFERENCE_STATE_STOP:  // in this example we never reach this state
                                        // but could be useful for your application
                break;
        }
    }

    ei_stop_impulse();

    return state;
}

/**
 * @brief Run inference process
 * @return true if successful
 */
static bool ei_run_inference(void)
{
    bool ret = true;
    ei_impulse_result_t result = {nullptr};

    signal_t features_signal;

    // shift circular buffer, so the newest data will be the first
    // if samples_wr_index is 0, then roll is immediately returning
    numpy::roll(samples_circ_buff, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, (-samples_wr_index));
    /* reset wr index, the oldest data will be overwritten */
    samples_wr_index = 0;

    // Create a data structure to represent this window of data
    int err = numpy::signal_from_buffer(samples_circ_buff, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &features_signal);
    if (err != 0) {
        ei_printf("ERR: Failed to create signal from buffer (%d)\n", err);
        return false;
    }

    // invoke the impulse
    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false);

    if (res != 0) {
        ei_printf("ERR: Failed to run classifier\n");
        ei_printf("ERR: %d\n", res);
        ret = false;
    }
    else {
        ei_print_results(&ei_default_impulse, &result);
    }

    return ret;
}

/**
 * @brief Start inference process
 * @return true if successful
 */
static bool ei_start_impulse(void)
{
    ei_printf("Edge Impulse start inferencing on Zephyr\n");

    ei_printf("Inferencing settings:\n");
    ei_printf("\tClassifier interval: %.2f ms.\n", (float)EI_CLASSIFIER_INTERVAL_MS);
    ei_printf("\tInput frame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    ei_printf("\tRaw sample count: %d samples.\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT);
    ei_printf("\tRaw samples per frame: %d samples.\n", EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME);
    ei_printf("\tNumber of output classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));

    // let's start, we will continuously run inference
    run_classifier_init();

    samples_wr_index = 0;
    samples_per_inference = EI_CLASSIFIER_RAW_SAMPLE_COUNT * EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME;

    return true;
}

/**
 * @brief Stop inference process
 * @return true if successful
 */
static bool ei_stop_impulse(void)
{
    ei_printf("Stopping inferencing\n");
    state = INFERENCE_STATE_STOP;

    return true;
}
