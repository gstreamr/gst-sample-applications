/*
 * Copyright (c) 2025 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#include <gst/gst.h>

static void on_pad_added(GstElement *src, GstPad *new_pad, gpointer data) {
    GstElement *parser = (GstElement *)data;
    GstPad *sink_pad = gst_element_get_static_pad(parser, "sink");

    if (gst_pad_is_linked(sink_pad)) {
        g_object_unref(sink_pad);
        return;
    }

    if (gst_pad_link(new_pad, sink_pad) != GST_PAD_LINK_OK) {
        g_printerr("Failed to link dynamic pad.\n");
    } else {
        g_print("Dynamic pad linked.\n");
    }

    g_object_unref(sink_pad);
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    GstElement *pipeline = gst_pipeline_new("composite-pipeline");

    // Elements for first video
    GstElement *src1 = gst_element_factory_make("filesrc", "src1");
    GstElement *demux1 = gst_element_factory_make("qtdemux", "demux1");
    GstElement *parse1 = gst_element_factory_make("h264parse", "parse1");
    GstElement *dec1 = gst_element_factory_make("v4l2h264dec", "dec1");
    GstElement *conv1 = gst_element_factory_make("videoconvert", "conv1");

    // Elements for second video
    GstElement *src2 = gst_element_factory_make("filesrc", "src2");
    GstElement *demux2 = gst_element_factory_make("qtdemux", "demux2");
    GstElement *parse2 = gst_element_factory_make("h264parse", "parse2");
    GstElement *dec2 = gst_element_factory_make("v4l2h264dec", "dec2");
    GstElement *conv2 = gst_element_factory_make("videoconvert", "conv2");

    // Compositor and sink
    GstElement *compositor = gst_element_factory_make("compositor", "compositor");
    GstElement *sink = gst_element_factory_make("waylandsink", "sink");

    if (!pipeline || !src1 || !demux1 || !parse1 || !dec1 || !conv1 ||
        !src2 || !demux2 || !parse2 || !dec2 || !conv2 ||
        !compositor || !sink) {
        g_printerr("Failed to create elements.\n");
        return -1;
    }

    // Set file locations
    g_object_set(src1, "location", argv[1], NULL);
    g_object_set(src2, "location", argv[2], NULL);

    // Add all elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline),
        src1, demux1, parse1, dec1, conv1,
        src2, demux2, parse2, dec2, conv2,
        compositor, sink, NULL);

    // Link static parts
    gst_element_link(src1, demux1);
    gst_element_link(src2, demux2);

    gst_element_link_many(parse1, dec1, conv1, NULL);
    gst_element_link_many(parse2, dec2, conv2, NULL);
    gst_element_link(compositor, sink);

    // Connect dynamic pads
    g_signal_connect(demux1, "pad-added", G_CALLBACK(on_pad_added), parse1);
    g_signal_connect(demux2, "pad-added", G_CALLBACK(on_pad_added), parse2);

    // Link converters to compositor with positioning
    GstPad *sinkpad1 = gst_element_request_pad_simple(compositor, "sink_%u");
    GstPad *srcpad1 = gst_element_get_static_pad(conv1, "src");
    gst_pad_link(srcpad1, sinkpad1);
    g_object_set(sinkpad1, "xpos", 0, "ypos", 0, NULL);

    GstPad *sinkpad2 = gst_element_request_pad_simple(compositor, "sink_%u");
    GstPad *srcpad2 = gst_element_get_static_pad(conv2, "src");
    gst_pad_link(srcpad2, sinkpad2);
    g_object_set(sinkpad2, "xpos", 640, "ypos", 0, NULL); // Adjust based on video width

    gst_object_unref(sinkpad1);
    gst_object_unref(sinkpad2);
    gst_object_unref(srcpad1);
    gst_object_unref(srcpad2);

    // Start playing
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run main loop
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    // Cleanup
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}

