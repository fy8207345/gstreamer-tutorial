#include <stdio.h>
#include <gst/gst.h>

int main01(int argc, char *argv[]) {
    GstElement *pipepline;
    GstBus *bus;
    GstMessage *msg;

    //init
    gst_init(&argc, &argv);

    //build pipeline
    pipepline = gst_parse_launch(
            "playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm",
            NULL
            );
    //start playing
    gst_element_set_state(pipepline, GST_STATE_PLAYING);

    //wait until error or EOS
    bus = gst_element_get_bus(pipepline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                     GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    //release resources
    if(msg != NULL){
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
    gst_element_set_state(pipepline, GST_STATE_NULL);
    gst_object_unref(pipepline);
    return 0;
}
