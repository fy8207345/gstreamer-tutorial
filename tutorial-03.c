#include <gst/gst.h>

typedef struct _CustomData {
    GstElement *pipeline;
    GstElement *source;
    GstElement *convert;
    GstElement *resample;
    GstElement *sink;
    GstElement *videoConvert;
    GstElement *videoSink;
} CustomData;
// handler for the pad-added signal
static void pad_added_handler(GstElement *src, GstPad *pad, CustomData *data);

int main03(int argc, char *argv[]) {
    CustomData data;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;
    gboolean terminate = FALSE;

    gst_init(&argc, &argv);

    //create elements
    data.source = gst_element_factory_make("uridecodebin", "source");
    data.convert = gst_element_factory_make("audioconvert", "aconvert");
    data.resample = gst_element_factory_make("audioresample", "aresample");
    data.sink = gst_element_factory_make("autoaudiosink", "asink");

    data.videoConvert = gst_element_factory_make("videoconvert", "vconvert");
    data.videoSink = gst_element_factory_make("autovideosink", "vsink");

    //create the empty pipeline
    data.pipeline = gst_pipeline_new("test-pipeline");
    if(!data.pipeline || !data.source || !data.convert || !data.resample || !data.sink){
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }

    //build pipeline
    gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.convert, data.resample, data.sink, data.videoConvert, data.videoSink, NULL);
    if(!gst_element_link_many(data.convert, data.resample, data.sink, NULL)){
        g_printerr ("Elements could not be linked.\n");
        gst_object_unref (data.pipeline);
        return -2;
    }

    if(!gst_element_link_many(data.videoConvert, data.videoSink, NULL)){
        g_printerr ("Elements could not be linked.\n");
        gst_object_unref (data.pipeline);
        return -3;
    }

    //set the uri to play
    g_object_set(data.source, "uri", "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm", NULL);

    // connect to the pad-added signal
    g_signal_connect(data.source, "pad_added", G_CALLBACK(pad_added_handler), &data);

    //start playing
    ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (data.pipeline);
        return -4;
    }

    bus = gst_element_get_bus(data.pipeline);
    do{
        msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
                                          GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
        /* Parse message */
        if (msg != NULL) {
            GError *err;
            gchar *debug_info;

            switch (GST_MESSAGE_TYPE (msg)) {
                case GST_MESSAGE_ERROR:
                    gst_message_parse_error (msg, &err, &debug_info);
                    g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
                    g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
                    g_clear_error (&err);
                    g_free (debug_info);
                    terminate = TRUE;
                    break;
                case GST_MESSAGE_EOS:
                    g_print ("End-Of-Stream reached.\n");
                    terminate = TRUE;
                    break;
                case GST_MESSAGE_STATE_CHANGED:
                    /* We are only interested in state-changed messages from the pipeline */
                    if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data.pipeline)) {
                        GstState old_state, new_state, pending_state;
                        gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                        g_print ("Pipeline state changed from %s to %s:\n",
                                 gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
                    }
                    break;
                default:
                    /* We should not reach here */
                    g_printerr ("Unexpected message received.\n");
                    break;
            }
            gst_message_unref (msg);
        }
    }while (!terminate);

    /* Free resources */
    gst_object_unref (bus);
    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_object_unref (data.pipeline);
    return 0;
}

static void pad_added_handler(GstElement *src, GstPad *new_pad, CustomData *data){
    GstPad *sink_pad = NULL;
    GstPadLinkReturn ret;
    GstCaps *new_pad_capds = NULL;
    GstStructure *new_pad_struct = NULL;
    const gchar *new_pad_type = NULL;
    const gchar *new_pad_name = GST_PAD_NAME(new_pad);

    new_pad_capds = gst_pad_get_current_caps(new_pad);
    new_pad_struct = gst_caps_get_structure(new_pad_capds, 0);
    new_pad_type = gst_structure_get_name(new_pad_struct);

    if(g_str_has_prefix(new_pad_type, "audio/x-raw")){
        sink_pad = gst_element_get_static_pad(data->convert, "sink");
    }else if(g_str_has_prefix(new_pad_type, "video/x-raw")){
        sink_pad = gst_element_get_static_pad(data->videoConvert, "sink");
    }else{
        g_printerr("unknown pad type %s\n", new_pad_type);
        goto exit;
    }

    g_print("received new pad '%s' : '%s' from '%s'\n", new_pad_name, new_pad_type, GST_ELEMENT_NAME(src));

    /* If our converter is already linked, we have nothing to do here */
    if (gst_pad_is_linked (sink_pad)) {
        g_print ("We are already linked. Ignoring.\n");
        goto exit;
    }

    /* Attempt the link */
    ret = gst_pad_link (new_pad, sink_pad);
    if (GST_PAD_LINK_FAILED (ret)) {
        g_print ("Type is '%s' but link failed.\n", new_pad_type);
    } else {
        g_print ("Link succeeded (type '%s' - '%s').\n", new_pad_type, new_pad_name);
    }

    exit:
        if (new_pad_capds != NULL)
            gst_caps_unref (new_pad_capds);

        /* Unreference the sink pad */
        gst_object_unref (sink_pad);
}