#include <gst/gst.h>
#include <string.h>

typedef struct _CustomData12 {
    gboolean is_live;
    GstElement *pipeline;
    GMainLoop *mainLoop;
} CustomData12;

static void cb_message(GstBus *bus, GstMessage *msg, CustomData12 *data) {
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR: {
            GError *err;
            gchar *debug;

            gst_message_parse_error(msg, &err, &debug);
            g_print("Error: %s\n", err->message);
            g_error_free(err);
            g_free(debug);

            gst_element_set_state(data->pipeline, GST_STATE_READY);
            g_main_loop_quit(data->mainLoop);
            break;
        }
        case GST_MESSAGE_EOS:
            /* end-of-stream */
            gst_element_set_state(data->pipeline, GST_STATE_READY);
            g_main_loop_quit(data->mainLoop);
            break;
        case GST_MESSAGE_BUFFERING:
            gint percent = 0;
            if (data->is_live)
                break;
            gst_message_parse_buffering(msg, &percent);
            g_print("Buffering (%3d%%)\r", percent);
            if (percent < 50) {
                g_print("start buffer\n");
                gst_element_set_state(data->pipeline, GST_STATE_PAUSED);
            } else {
                g_print("start play\n");
                gst_element_set_state(data->pipeline, GST_STATE_PLAYING);
            }
            break;
        case GST_MESSAGE_CLOCK_LOST:
            /* Get a new clock */
            gst_element_set_state(data->pipeline, GST_STATE_PAUSED);
            gst_element_set_state(data->pipeline, GST_STATE_PLAYING);
            break;
        default:
            /* Unhandled message */
            break;
    }
}

int main12(int argc, char *argv[]) {
    GstElement *pipeline;
    GstBus *bus;
    GstStateChangeReturn ret;
    GMainLoop *mainLoop;
    CustomData12 data;

    gst_init(&argc, &argv);

    memset(&data, 0, sizeof (data));

    pipeline = gst_parse_launch("playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm", NULL);
    bus = gst_element_get_bus(pipeline);

    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (pipeline);
        return -1;
    } else if (ret == GST_STATE_CHANGE_NO_PREROLL) {
        data.is_live = TRUE;
    }

    mainLoop = g_main_loop_new (NULL, FALSE);
    data.mainLoop = mainLoop;
    data.pipeline = pipeline;

    gst_bus_add_signal_watch(bus);
    g_signal_connect(bus, "message", G_CALLBACK(cb_message), &data);
    g_main_loop_run(mainLoop);

    /* Free resources */
    g_main_loop_unref (mainLoop);
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
    return 0;
}
