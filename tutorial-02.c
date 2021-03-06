#include <gst/gst.h>

//
// Created by Administrator on 2021/8/5.
//

int main02(int argc, char *argv[]) {
    GstElement *pipeline, *source, *sink;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    //init
    gst_init(&argc, &argv);

    //create elements
    source = gst_element_factory_make("videotestsrc", "source");
    sink = gst_element_factory_make("autovideosink", "sink");

    //create an empty pipeline
    pipeline = gst_pipeline_new("test-pipeline");
    if(!pipeline || !source || !sink){
        g_printerr("Not All elements are ready\n");
        if(pipeline){
            gst_object_unref(pipeline);
        }
        return -1;
    }

    //build pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, sink, NULL);
    if(gst_element_link(source, sink) != TRUE){
        g_printerr("elements could not be linked\n");
        gst_object_unref(pipeline);
        return -2;
    }

    //modify source's properties
    g_object_set(source, "pattern", 0, NULL);

    //start playing
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if(ret == GST_STATE_CHANGE_FAILURE){
        g_printerr("unable to set the pipeline to the playing state\n");
        gst_object_unref(pipeline);
        return -3;
    }

    //wait until error or EOS
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
            GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
    if(msg != NULL){
        GError *err;
        gchar *debug_info;
        switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debug_info);
                g_printerr("error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
                g_printerr("debugging info: %s\n", debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("End of Stream\n");
                break;
            default:
                g_printerr("unexpected message received\n");
                break;
        }
        gst_message_unref(msg);
    }

    //free resources
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

}