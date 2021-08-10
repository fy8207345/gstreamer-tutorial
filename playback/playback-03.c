#include <gst/gst.h>
#include <gst/audio/audio.h>
#include <string.h>

#define CHUNK_SIZE 1024
#define SAMPLE_RATE 44100

typedef struct _CustomData03 {
    GstElement *pipeline;
    GstElement *app_source;

    guint64 num_samples;
    gfloat a, b, c, d;
    guint sourceid;

    GMainLoop *mainLoop;
} CustomData03;

static gboolean push_data(CustomData03 *data){
    GstBuffer *buffer;
    GstFlowReturn ret;
    int i;
    GstMapInfo map;
    gint16 *raw;
    gint num_samples = CHUNK_SIZE / 2;
    gfloat freq;

    buffer = gst_buffer_new_and_alloc(CHUNK_SIZE);

    GST_BUFFER_TIMESTAMP(buffer) = gst_util_uint64_scale(data->num_samples, GST_SECOND, SAMPLE_RATE);
    GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale(num_samples, GST_SECOND, SAMPLE_RATE);

    gst_buffer_map(buffer, &map, GST_MAP_WRITE);
    raw = (gint16 *)map.data;
    data->c += data->d;
    data->d -= data->c / 1000;
    freq = 1100 + 1000 * data->d;
    for(i=0;i <num_samples; i++){
        data->a += data->b;
        data->b -= data->a / freq;
        raw[i] = (gint16) (500 * data->a);
    }
    gst_buffer_unmap(buffer, &map);
    data->num_samples += num_samples;

    //push the buffer into appsrc
    g_signal_emit_by_name(data->app_source, "push-buffer", buffer, &ret);

    if(ret != GST_FLOW_OK){
        return FALSE;
    }
    return TRUE;
}

static void start_feed(GstElement *source, guint size, CustomData03 *data){
    if(data->sourceid == 0){
        g_print("start feeding\n");
        data->sourceid = g_idle_add((GSourceFunc) push_data, data);
    }
}

static void stop_feed(GstElement *source, CustomData03 *data){
    if(data->sourceid != 0){
        g_print("stop feeding\n");
        g_source_remove(data->sourceid);
        data->sourceid = 0;
    }
}

static void error_cb(GstBus *bus, GstMessage *msg, CustomData03 *data){
    GError *err;
    gchar *debug_info;

    /* Print error details on the screen */
    gst_message_parse_error (msg, &err, &debug_info);
    g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
    g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
    g_clear_error (&err);
    g_free (debug_info);

    g_main_loop_quit (data->mainLoop);
}

static void source_setup(GstElement *pipeline, GstElement *source, CustomData03 *data){
    GstAudioInfo info;
    GstCaps *audio_caps;

    g_print("source has been created. Configuring.\n");
    data->app_source = source;

    gst_audio_info_set_format(&info, GST_AUDIO_FORMAT_S16, SAMPLE_RATE, 1, NULL);
    audio_caps = gst_audio_info_to_caps(&info);
    g_object_set(source, "caps", audio_caps, "format", GST_FORMAT_TIME, NULL);
    g_signal_connect(source, "need-data", G_CALLBACK(start_feed), data);
    g_signal_connect(source, "enough-data", G_CALLBACK(stop_feed), data);
    gst_caps_unref(audio_caps);
}

int main03(int argc, char *argv[]) {
    CustomData03 data;
    GstBus *bus;

    /* Initialize cumstom data structure */
    memset (&data, 0, sizeof (data));
    data.b = 1; /* For waveform generation */
    data.d = 1;

    gst_init(&argc, &argv);

    /* Create the playbin element */
    data.pipeline = gst_parse_launch("playbin uri=appsrc://", NULL);
    g_signal_connect(data.pipeline, "source-setup", G_CALLBACK(source_setup), &data);

    /* Instruct the bus to emit signals for each received message, and connect to the interesting signals */
    bus = gst_element_get_bus (data.pipeline);
    gst_bus_add_signal_watch (bus);
    g_signal_connect (G_OBJECT (bus), "message::error", (GCallback)error_cb, &data);
    gst_object_unref (bus);

    /* Start playing the pipeline */
    gst_element_set_state (data.pipeline, GST_STATE_PLAYING);

    /* Create a GLib Main Loop and set it to run */
    data.mainLoop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (data.mainLoop);

    /* Free resources */
    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_object_unref (data.pipeline);
    return 0;
}