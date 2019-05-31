/*
 * *Copyright (c) 2018 Qualcomm Technologies, Inc.
 * *All Rights Reserved.
 * *Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <gst/gst.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <glib-unix.h>
#include <string.h>
#include <mm-audio/qahw_api/inc/qahw_api.h>
#include <mm-audio/qahw_api/inc/qahw_defs.h>
#include <mm-audio/qsthw_api/qsthw_api.h>
#include <mm-audio/qsthw_api/qsthw_defs.h>


struct session{
gchar *FileLoc;   //location of session
gchar *Model_location; //location of uim file
gboolean LAB; //TRUE or FALSE
gchar *Parser;    //e.g MPEGaudio
gchar *Enhancer;
gchar *Decoder;
gchar *Codec;    //eg:Mp3,flac
gchar *Layer;    //eg:HAL,OMX,FFMPEG etc
gchar *Path;     //eg:compress offload ,deep buffer,low latency
gchar *Convertor;
gchar *Encoder;
gchar *queue;
gchar *Format;    // audio encode format S16LE, S24LE
gchar *Volume;     //0-10
guint BitDepth;    //16,24 bit
gint Device;    //2- Speaker,4- Wired headset
guint Duration;   //sec
guint session_type; //1-playback, 2-record, 3-sva
guint channels;
guint sva_multimode;
guint rate;
gint audiosource; //audio-sorce, default =1-mic, 5-camcorder
guint bitrate, bandmode, dtx_enable, amrwb_enable, encode_format;
};


/* Structure to contain all our information, so we can pass it around */
typedef struct _CustomData {
  GstElement *sva_pipe, *pb_pipe, *pb_sink, *PB_source, *pb_enhancer, *pb_Decoder, *pb_Convertor, *rc_pipe, *activePipe, *sva_soundtrig;  /*declare record, playback,sva elements */
  GstBus *active_bus;
  gboolean playing, pb_playing, rec_playing, sva_playing;      /* Are we in the PLAYING state? */
  gboolean terminate, sva_terminate;    /* Should we terminate execution? */
  gboolean seek_enabled; /* Is seeking enabled for this media? */
  gboolean seek_done;    /* Have we performed the seek already? */
  gboolean is_live;
  gboolean uuid_flag;
  GMainLoop *loop;
  qahw_module_handle_t *moduleId;
  qsthw_module_handle_t *module;
  gint buffering_level;
  guint count, cmd_count,total_cmd_count, concurrency_flag,timeout_dur;
  gint64 duration;       /* How long does this media last, in nanoseconds */
  gchar *uri, *cmd,**cmd_list;
  gchar *global_Model_location; //location of uim file when the concurrency is enabled
  gchar *uuid; //value of UUID to configure 3mic or 6mic
  struct session **session_info;
}CustomData;


static void on_pad_added (GstElement *element,
              GstPad     *pad,
              gpointer    data)
{
  GstPad *sinkpad;
  GstElement *decoder = (GstElement *) data;

  /* We can now link this pad with the vorbis-decoder sink pad */
  g_print ("PVR: Dynamic pad created, linking demuxer/decoder\n");

  sinkpad = gst_element_get_static_pad (decoder, "sink");

  gst_pad_link (pad, sinkpad);

  gst_object_unref (sinkpad);
}


/* Forward definition for the message and keyboard processing functions */
static gboolean handle_keyboard (GIOChannel *source, GIOCondition cond, CustomData *data);


gint display_session_info(CustomData *data, int id){

        printf("$$$$$$$$$$$$$$$$$$$$$$$$$ session info $$$$$$$$$$$$$$$$$$$$$$$$$$\n");
        printf("Decoder params\n");
        printf("session_type  1-playback 2-record  3-SVA : %d\n",data->session_info[id]->session_type);
        printf("Fileloc  = %s\n",data->session_info[id]->FileLoc);
        printf("codec type  = %s\n",data->session_info[id]->Codec);
        printf("Layer  = %s\n",data->session_info[id]->Layer);
        printf("Path = %s\n",data->session_info[id]->Path);
        printf("BitDepth   = %d\n",data->session_info[id]->BitDepth);
        printf("parser   = %s\n",data->session_info[id]->Parser);
        printf("device   = %d\n",data->session_info[id]->Device);
        printf("volume   = %s\n",data->session_info[id]->Volume);
        printf("duration = %d\n",data->session_info[id]->Duration);
        printf("Enhancer = %s\n",data->session_info[id]->Enhancer);
        printf("Decoder = %s\n",data->session_info[id]->Decoder);
        printf("Convertor = %s\n\n",data->session_info[id]->Convertor);

        printf("Encoder params\n");
        printf("Encoder = %s\n",data->session_info[id]->Encoder);
        printf("channels   = %d\n",data->session_info[id]->channels);
        printf("Format = %s\n",data->session_info[id]->Format);
        printf("Sample Rate   = %d\n",data->session_info[id]->rate);
        printf("bitrate   = %d\n",data->session_info[id]->bitrate);
        printf("bandmode   = %d\n",data->session_info[id]->bandmode);
        printf("dtx_enable   = %d\n",data->session_info[id]->dtx_enable);
        printf("amrwb_enable   = %d\n",data->session_info[id]->amrwb_enable);
        printf("encode_format  = %d\n\n",data->session_info[id]->encode_format);

        printf("SVA params\n");
        printf("LAB   = %d\n",data->session_info[id]->LAB);

        printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
        return 1;
}


int parseconfig(gchar *fp, CustomData *data)
{
    gssize length;
    gchar *content;
    gchar **tmp;
    gchar **lt;
    gchar *cmd = "";
    guint i,cmdflag=1,count=-1,iterations=0,iterflag=1,j=0;
    gint var=0;
    struct session **s=NULL;

    if (!g_file_test (fp, G_FILE_TEST_EXISTS)) {
        g_print ("PVR:File not found to open\n");
        return 0;
    }
    if (g_file_get_contents (fp, &content, &length, NULL)) {
        // Process contents
        tmp = g_strsplit_set (content, "\n", -1);
        g_print ("PVR:Length: %u\n", g_strv_length(tmp));
    }
    for(i=0;i<g_strv_length(tmp);i++)
    {
        tmp[i] = g_strstrip(tmp[i]);
    }
    for(i=0;i<g_strv_length(tmp);i++)
    {
        tmp[i] = g_strconcat(tmp[i],"\n",NULL);
    }

    cmdflag=1;
    iterflag=1;
    g_print ("PVR:processing config file\n");

    for (i= 0;i < g_strv_length(tmp); i++)
    {
                if((!(g_str_has_prefix(tmp[i],"#CMD"))) && iterflag){
                g_print ("PVR:in iteration count extract\n");
                lt=g_strsplit_set(tmp[i], ":", -1);
                lt[0]=g_strstrip(lt[0]);
                lt[1]=g_strstrip(lt[1]);
                if(g_strcmp0(lt[0],"iterations")==0){
                    var=atoi(lt[1]) ;
                    iterations=var;
                }
                i++;
                iterflag=0;
                }

                //extracting concurrency flags
                if((g_str_has_prefix(tmp[i],"concurrency"))){
                    g_print ("PVR:in concurrency count extract\n");
                    lt=g_strsplit_set(tmp[i], ":", -1);
                    lt[0]=g_strstrip(lt[0]);
                    lt[1]=g_strstrip(lt[1]);
                    if(g_strcmp0(lt[0],"concurrency")==0){
                        var=atoi(lt[1]) ;
                        data->concurrency_flag=var;
                    }
                    g_print ("PVR:in concurrency flag value:%d\n", data->concurrency_flag);
                    continue;
                }

                //extracting UUID flags
                if((g_str_has_prefix(tmp[i],"uuid"))){
                    g_print ("PVR:extracting the UUID\n");
                    lt=g_strsplit_set(tmp[i], ":", -1);
                    lt[0]=g_strstrip(lt[0]);
                    lt[1]=g_strstrip(lt[1]);
                    if(g_strcmp0(lt[0],"uuid")==0){
                        data->uuid = g_strdup(lt[1]);
                        data->uuid_flag = TRUE;
                    }
                    g_print ("PVR:in UUID value:%s\n", data->uuid);
                    continue;
                }

                //adding global sound model in concurrency scenario
                if((g_str_has_prefix(tmp[i],"global_soundmodel"))){
                    g_print ("PVR:in global_soundmodel extract\n");
                    lt=g_strsplit_set(tmp[i], ":", -1);
                    lt[0]=g_strstrip(lt[0]);
                    lt[1]=g_strstrip(lt[1]);
                    if(g_strcmp0(lt[0],"global_soundmodel")==0){
                        data->global_Model_location = g_strdup(lt[1]);
                    }
                    g_print ("PVR:in global_soundmodel location:%s\n", data->global_Model_location);
                    continue;
                }

        if((!(g_str_has_prefix(tmp[i],"#Session")))&& cmdflag)
        {
            cmd = g_strconcat(cmd,tmp[i],NULL);
        }
        else
        {   if(g_str_has_prefix(tmp[i],"#Session")){
             g_print ("structure lenght :  %d\n", sizeof(struct session ));
             count++;
             s = (struct session **)g_realloc(s, (count + 1) * sizeof(struct session *));  //add one element to the array
             s[count] = (struct session *)g_malloc(sizeof(struct session ));   //allocate memory for one `struct session`
             i++;
             data->session_info = s;
                data->session_info[count]->FileLoc = "";   //location of session
                data->session_info[count]->Model_location = ""; //location of uim file
                data->session_info[count]->LAB = 1;   //TRUE or FALSE
                data->session_info[count]->sva_multimode = 0; //sva multimode enable or disabled
                data->session_info[count]->Parser = "";    //e.g MPEGaudio
                data->session_info[count]->Codec  = "";    //e.g mp3,flac
                data->session_info[count]->Layer  = "";    //e.g HAL,OMX,ffmpeg
                data->session_info[count]->Path  = "";    //e.g compress offload,deep-buffer,low latency
                data->session_info[count]->BitDepth  = 0;    //e.g mp3,flac
                data->session_info[count]->Enhancer = "none";
                data->session_info[count]->queue = "none";
                data->session_info[count]->Decoder = "none";
                data->session_info[count]->Convertor = "none";
                data->session_info[count]->Encoder = "";
                data->session_info[count]->Format = "";    // audio encode format S16LE, S24LE
                data->session_info[count]->Device= 2;    //2- Speaker,4- Wired headset
                data->session_info[count]->Volume = "5";     //0-10

                data->session_info[count]->Duration = 10;   //sec
                data->session_info[count]->session_type = 0; //1-playback, 2-record, 3-sva
                data->session_info[count]->channels = 0;
                data->session_info[count]->rate= 0;
                data->session_info[count]->audiosource=1;
                data->session_info[count]->bitrate=0;
                data->session_info[count]->bandmode=7;
                data->session_info[count]->dtx_enable=0;
                data->session_info[count]->amrwb_enable=0;
                data->session_info[count]->encode_format=0;

               }
            cmdflag=0;

                lt=g_strsplit_set(tmp[i], ":", -1);
                lt[0]=g_strstrip(lt[0]);
                lt[1]=g_strstrip(lt[1]);

                if(g_strcmp0(lt[0],"FileLocation")==0){
                   g_print ("PVR:##### concatinating\n");
                   if(g_strv_length(lt) >2){
                     s[count]->FileLoc=g_strdup(g_strconcat(lt[1],":",lt[2], NULL));
                   }
                   else{
                       s[count]->FileLoc=g_strdup(lt[1]);
                   }
                   g_print ("PVR:##### file location %s , %d\n", tmp[i], g_strv_length(lt));
                }
                else if(g_strcmp0(lt[0],"Model_location")==0){
                   s[count]->Model_location=g_strdup(lt[1]);
                }
                else if(g_strcmp0(lt[0],"BitDepth")==0){
                   s[count]->BitDepth=atoi(lt[1]);
                }
                else if(g_strcmp0(lt[0],"LAB")==0){
                   s[count]->LAB=atoi(lt[1]);
                }
                else if(g_strcmp0(lt[0],"sva_multimode")==0){
                    s[count]->sva_multimode=atoi(lt[1]);
                }
                else if (g_strcmp0(lt[0],"session_type")==0){
                    if(g_strcmp0(lt[1],"sva")==0)
                    {
                      s[count]->session_type=3;
                      g_print ("PVR:sva case\n");
                    }
                    else if(g_strcmp0(lt[1],"record")==0)
                    {
                      s[count]->session_type=2;
                      g_print ("PVR:record case\n");
                    }
                    else if(g_strcmp0(lt[1],"streaming")==0)
                    {
                      s[count]->session_type=4;
                      g_print ("PVR:streaming case\n");
                    }
                    else
                      s[count]->session_type=1;
                }
                else if(g_strcmp0(lt[0],"Codec")==0){
                  s[count]->Codec=g_strdup(lt[1]);
                }
                else if(g_strcmp0(lt[0],"Parser")==0){
                  s[count]->Parser=g_strdup(lt[1]);
                }
                else if(g_strcmp0(lt[0],"Layer")==0){
                  s[count]->Layer=g_strdup(lt[1]);
                }
                else if(g_strcmp0(lt[0],"Path")==0){
                  s[count]->Path=g_strdup(lt[1]);
                }
                else if(g_strcmp0(lt[0],"Device")==0){
                  s[count]->Device = atoi(lt[1]);
                }
                else if(g_strcmp0(lt[0],"Volume")==0){
                  s[count]->Volume = g_strdup(lt[1]);
                }
                else if(g_strcmp0(lt[0],"Duration")==0){
                  var=atoi(lt[1]);
                  s[count]->Duration=var;
                }
                else if(g_strcmp0(lt[0],"Encoder")==0){
                  s[count]->Encoder=g_strdup(lt[1]);
                }
                else if(g_strcmp0(lt[0],"Format")==0){
                  s[count]->Format=g_strdup(lt[1]);
                }
                else if(g_strcmp0(lt[0],"rate")==0){
                  s[count]->rate=atoi(lt[1]);
                }
                else if(g_strcmp0(lt[0],"audiosource")==0){
                  s[count]->audiosource=atoi(lt[1]);
                }
                else if(g_strcmp0(lt[0],"channels")==0){
                  s[count]->channels=atoi(lt[1]);
                }
                else if(g_strcmp0(lt[0],"Decoder")==0){
                  s[count]->Decoder=g_strdup(lt[1]);
                }
                else if(g_strcmp0(lt[0],"Convertor")==0){
                  s[count]->Convertor=g_strdup(lt[1]);
                }
                else if(g_strcmp0(lt[0],"Enhancer")==0){
                  s[count]->Enhancer=g_strdup(lt[1]);
                }
                else if(g_strcmp0(lt[0],"bitrate")==0){
                  var=atoi(lt[1]);
                  s[count]->bitrate=var;
                }
                else if(g_strcmp0(lt[0],"bandmode")==0){
                  var=atoi(lt[1]);
                  s[count]->bandmode=var;
                }
                else if(g_strcmp0(lt[0],"dtx_enable")==0){
                  var=atoi(lt[1]);
                  s[count]->dtx_enable=var;
                }
                else if(g_strcmp0(lt[0],"amrwb_enable")==0){
                  var=atoi(lt[1]);
                  s[count]->amrwb_enable=var;
                }
                else if(g_strcmp0(lt[0],"queue")==0){
                  s[count]->queue=g_strdup(lt[1]);
                }
                else if(g_strcmp0(lt[0],"encode_format")==0){
                  var=atoi(lt[1]);
                  s[count]->encode_format=var;
                }
        }

    }

    g_print ("PVR: configuration file parsing completed, printining dumped values\n");
    data->cmd = cmd;
    data->count = count;
    return iterations;
}

int fillAudioCodecProperties(CustomData *data)
{
        int i =0;
        for (i=0; i <= data->count; i++){
             if (g_strcmp0(data->session_info[i]->Codec,"MP3")==0){
                 data->session_info[i]->Parser=g_strdup("mpegaudioparse");
                 if(g_strcmp0(data->session_info[i]->Layer,"OMX")==0){
                    g_print ("PVR: fillAudioCodecProperties -unsupported OMX Layer choosed for MP3\n");
                    return 0;
                 }
                 else if(g_strcmp0(data->session_info[i]->Layer,"FFMPEG")==0){
                    g_print ("PVR: fillAudioCodecProperties -unsupported FFMPEG Layer choosed for MP3\n");
                    return 0;
                 }
                 else{
                    g_print ("PVR: fillAudioCodecProperties- Default HAL Layer choosed for MP3\n");
                 }
             }
             if (g_strcmp0(data->session_info[i]->Codec,"WAV")==0){
                data->session_info[i]->Parser=g_strdup("wavparse");
                if(g_strcmp0(data->session_info[i]->Layer,"OMX")==0){
                   g_print ("PVR: fillAudioCodecProperties -unsupported OMX Layer choosed for WAV\n");
                   return 0;
                }
                else if(g_strcmp0(data->session_info[i]->Layer,"FFMPEG")==0){
                     g_print ("PVR: fillAudioCodecProperties -unsupported FFMPEG Layer choosed for WAV\n");
                     return 0;
                }
                else{
                     g_print ("PVR: fillAudioCodecProperties- Default HAL Layer choosed for WAV\n");
                }
             }
             if (g_strcmp0(data->session_info[i]->Codec,"VORBIS")==0){
                 data->session_info[i]->Parser=g_strdup("oggdemux");
                 if(g_strcmp0(data->session_info[i]->Layer,"OMX")==0){
                    g_print ("PVR: fillAudioCodecProperties -unsupported OMX Layer choosed for VORBIS\n");
                    return 0;
                 }
                 else if(g_strcmp0(data->session_info[i]->Layer,"FFMPEG")==0){
                      g_print ("PVR: fillAudioCodecProperties -unsupported FFMPEG Layer choosed for VORBIS\n");
                      return 0;
                 }
                 else{
                      g_print ("PVR: fillAudioCodecProperties- Default HAL Layer choosed for VORBIS\n");
                 }
             }
             if (g_strcmp0(data->session_info[i]->Codec,"ALAC")==0){
                 data->session_info[i]->Parser=g_strdup("qtdemux");
                 if(g_strcmp0(data->session_info[i]->Layer,"OMX")==0){
                    g_print ("PVR: fillAudioCodecProperties -unsupported OMX Layer choosed for ALAC\n");
                    return 0;
                 }
                 else if(g_strcmp0(data->session_info[i]->Layer,"FFMPEG")==0){
                      g_print ("PVR: fillAudioCodecProperties -unsupported FFMPEG Layer choosed for ALAC\n");
                      return 0;
                 }
                 else{
                     g_print ("PVR: fillAudioCodecProperties- Default HAL Layer choosed for ALAC\n");
                 }
             }
             if (g_strcmp0(data->session_info[i]->Codec,"APE")==0){
                 if(g_strcmp0(data->session_info[i]->Layer,"OMX")==0){
                    g_print ("PVR: fillAudioCodecProperties -unsupported OMX Layer choosed for APE\n");
                 return 0;
                 }
                 else if(g_strcmp0(data->session_info[i]->Layer,"FFMPEG")==0){
                      data->session_info[i]->Parser=g_strdup("avdemux_ape");
                      data->session_info[i]->Decoder = g_strdup("avdec_ape");
                      if(data->session_info[i]->BitDepth == 24){
                         data->session_info[i]->Convertor = g_strdup("audioconvert");
                      }
                      g_print ("PVR: fillAudioCodecProperties FFMPEG Layer choosed for APE\n");
                 }
                 else{
                      g_print ("PVR: fillAudioCodecProperties -unsupported HAL Layer choosed for APE\n");
                      return 0;
                 }
              }
              if (g_strcmp0(data->session_info[i]->Codec,"FLAC")==0){
                  data->session_info[i]->Parser=g_strdup("flacparse");
                  if(g_strcmp0(data->session_info[i]->Layer,"OMX")==0){
                     g_print ("PVR: fillAudioCodecProperties -unsupported OMX Layer choosed for FLAC\n");
                     return 0;
                  }
                  else if(g_strcmp0(data->session_info[i]->Layer,"FFMPEG")==0){
                       data->session_info[i]->Decoder = g_strdup("flacdec");
                       data->session_info[i]->Convertor = g_strdup("audioconvert");
                       g_print ("PVR: fillAudioCodecProperties FFMPEG Layer choosed for FLAC\n");
                  }
                  else{
                       g_print ("PVR: fillAudioCodecProperties HAL Layer choosed for FLAC\n");
                  }
             }
             if (g_strcmp0(data->session_info[i]->Codec,"AIFF")==0){
                 if(g_strcmp0(data->session_info[i]->Layer,"OMX")==0){
                    g_print ("PVR: fillAudioCodecProperties -unsupported OMX Layer choosed for AIFF\n");
                    return 0;
                 }
		 else if(g_strcmp0(data->session_info[i]->Layer,"FFMPEG")==0){
                      g_print ("PVR: fillAudioCodecProperties -unsupported FFMPEG Layer choosed for AIFF\n");
                      return 0;
                 }
                 else{
                      data->session_info[i]->Parser=g_strdup("aiffparse");
                      data->session_info[i]->Convertor = g_strdup("audioconvert");
                      if(data->session_info[i]->BitDepth == 24){
                         data->session_info[i]->Format = g_strdup("S24LE");
                      }
                      else{
                         data->session_info[i]->Format = g_strdup("S16LE");
                      }
                      g_print ("PVR: fillAudioCodecProperties HAL Layer choosed for AIFF\n");
                 }
             }
             if (g_strcmp0(data->session_info[i]->Codec,"AAC")==0){
                 data->session_info[i]->Parser=g_strdup("aacparse");
                 if(g_strcmp0(data->session_info[i]->Layer,"OMX")==0){
                    data->session_info[i]->Decoder = g_strdup("omxaacdec");
                    g_print ("PVR: fillAudioCodecProperties OMX Layer choosed for AAC\n");
                 }
                 else if(g_strcmp0(data->session_info[i]->Layer,"FFMPEG")==0){
                      data->session_info[i]->Decoder = g_strdup("avdec_aac");
                      data->session_info[i]->Convertor = g_strdup("audioconvert");
                      g_print ("PVR: fillAudioCodecProperties  FFMPEG Layer choosed for AAC\n");
                 }
                 else{
                      g_print ("PVR: fillAudioCodecProperties- Default HAL Layer choosed for AAC\n");
                 }
             }
             if (g_strcmp0(data->session_info[i]->Codec,"AMRNB")==0){
                 data->session_info[i]->Parser=g_strdup("amrparse");
                 if(g_strcmp0(data->session_info[i]->Layer,"OMX")==0){
                    data->session_info[i]->Decoder = g_strdup("omxamrnbdec");
                    g_print ("PVR: fillAudioCodecProperties -OMX Layer choosed for AMRNB\n");
                 }
                 else if(g_strcmp0(data->session_info[i]->Layer,"FFMPEG")==0){
                      data->session_info[i]->Decoder = g_strdup("avdec_amrnb");
                      data->session_info[i]->Convertor = g_strdup("audioconvert");
                      g_print ("PVR: fillAudioCodecProperties -FFMPEG Layer choosed for AMRNB\n");
                 }
                 else{
                      g_print ("PVR: fillAudioCodecProperties -unsupported HAL Layer choosed for AMRNB\n");
                      return 0;
                 }
             }
             if (g_strcmp0(data->session_info[i]->Codec,"AMRWB")==0){
                 data->session_info[i]->Parser=g_strdup("amrparse");
                 if(g_strcmp0(data->session_info[i]->Layer,"OMX")==0){
                    data->session_info[i]->Decoder = g_strdup("omxamrwbdec");
                    g_print ("PVR: fillAudioCodecProperties -OMX Layer choosed for AMRWB\n");
                 }
                 else if(g_strcmp0(data->session_info[i]->Layer,"FFMPEG")==0){
                      data->session_info[i]->Decoder = g_strdup("avdec_amrwb");
                      data->session_info[i]->Convertor = g_strdup("audioconvert");
                      g_print ("PVR: fillAudioCodecProperties -FFMPEG Layer choosed for AMRWB\n");
                 }
                 else{
                      g_print ("PVR: fillAudioCodecProperties -unsupported HAL Layer choosed for AMRWB\n");
                      return 0;
                 }
             }
        }
}


static gboolean only_effect (CustomData *data) {
    static gboolean i =TRUE;
    /*Move the SVA pipe to PAUSE again to be ready to receive new KWD + Command */

      if(i){
      g_object_set (data->pb_enhancer, "strength", 1, NULL);
      g_print ("PVR: only_effect- setting to 1 \n");
      }
      else
      {
          g_object_set (data->pb_enhancer, "strength", 999, NULL);
          g_print ("PVR: only_effect- setting to 999 \n");
      }
      i = !i;
      return 1;
}


static gboolean sva_timeout (CustomData *data) {
      /*Move the SVA pipe to PAUSE again to be ready to receive new KWD + Command */
      g_print ("PVR: sva_timeout- setting SVA pipeline to pause\n");
      gst_element_set_state (data->sva_pipe, GST_STATE_PAUSED);
      g_print ("PVR: sva_timeout- setting SVA pipeline to NULL\n");
      gst_element_set_state (data->sva_pipe, GST_STATE_NULL);
      g_print ("PVR: sva_timeout- copy the SVA recorded file or else will overwrite in 10 secs\n");
      g_print ("PVR: sva_timeout- setting SVA pipeline to Playstate\n");
      gst_element_set_state (data->sva_pipe, GST_STATE_PLAYING);
      return 0;
}


static gboolean
cb_print_position (GstElement *pipeline)
{
  gint64 pos, len;
  g_print ("PVR: cb_print_position -querying the seek position\n");

  if (gst_element_query_position (pipeline, GST_FORMAT_TIME, &pos)
    && gst_element_query_duration (pipeline, GST_FORMAT_TIME, &len)) {
    g_print ("PVR: seek Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\n",
         GST_TIME_ARGS (pos), GST_TIME_ARGS (len));
  }
  return TRUE;
}


static gboolean action_section (CustomData *data) {
    gchar **action;

    g_print ("PVR:  $$$$$$$$$$$$  perfroming action $$$$$$$$$$$$$$$$$$$$$$: %s\n", data->cmd_list[data->cmd_count]);

    if(!(data->terminate) && !(g_str_has_prefix(data->cmd_list[data->cmd_count],"#CMD")) && (data->cmd_count < (data->total_cmd_count-1))){
        action=g_strsplit_set(data->cmd_list[data->cmd_count], ":", -1);
        action[0]=g_strstrip(action[0]);
        action[1]=g_strstrip(action[1]);

        g_print ("PVR: checking dynamic timeout values\n");
        if(g_strv_length(action) >2){
            action[2]=g_strstrip(action[2]);
            g_print ("PVR: setting Dynamic timeout values :%d\n", atoi(action[2]));
            data->timeout_dur = atoi(action[2]);;
        }
        else{
            data->timeout_dur = 10;
        }
        if((g_str_has_prefix(data->cmd_list[data->cmd_count],"Trick"))){
              if(data->pb_playing && g_str_has_prefix(action[1],"pause")){
              g_print ("PVR: action_section -Pausing the pipeline\n");
              gst_element_set_state (data->activePipe, GST_STATE_PAUSED);
              }
              else if(!data->pb_playing && g_str_has_prefix(action[1],"play")){
                  g_print ("PVR: action_section -playing the pipeline\n");
                  gst_element_set_state (data->activePipe, GST_STATE_PLAYING);
              }
              else if(data->pb_playing && g_str_has_prefix(action[1],"eos")){
                  g_print ("PVR: action_section -pipeline will be played till EOS\n");
                  return 0;
              }
              else if(!data->pb_playing && g_str_has_prefix(action[1],"stop")){
                  g_print ("PVR: action_section -stoping the pipeline\n");
                  gst_element_set_state (data->activePipe, GST_STATE_NULL);
              }
              else
                  g_print ("PVR: action_section -unknown state,cann't perform trick mode\n");
        }
        else if((g_str_has_prefix(data->cmd_list[data->cmd_count],"Seek")) && data->pb_playing && data->seek_enabled){
              /* seeking the playback */
              g_print ("PVR: action_section -seeking the playback\n");
              gst_element_seek_simple (data->activePipe, GST_FORMAT_TIME,
                            GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT, (atoi(action[1])) * GST_SECOND);
        }
        else if((g_str_has_prefix(data->cmd_list[data->cmd_count],"strength")) && data->pb_playing){
              /* setting the strength */
              g_print ("PVR: action_section -seting the Strength of the Dynamic filter\n");
              g_object_set (data->pb_enhancer, "strength", (atoi(action[1])), NULL);

        }
        else if((g_str_has_prefix(data->cmd_list[data->cmd_count],"Session")) && (data->pb_playing || data->sva_playing || data->rec_playing)){
            /* Displaying the current active session*/
            g_print ("PVR: action_section -Current Active session %s\n", data->cmd_list[data->cmd_count]);
            display_session_info(data, (atoi(action[1])));
        }
        else if((g_str_has_prefix(data->cmd_list[data->cmd_count],"Volume")) && data->pb_playing){
            /* setting volume */
            g_print ("PVR: action_section -setting volume on current playback session %s\n", data->cmd_list[data->cmd_count]);
            g_object_set (data->pb_sink, "volume", (atof(action[1])), NULL);
        }
        else if((g_str_has_prefix(data->cmd_list[data->cmd_count],"Left-volume")) && data->pb_playing){
            /* setting left-Volume  */
            g_print ("PVR: action_section -setting Left- volume on current playback session %s\n", data->cmd_list[data->cmd_count]);
            g_object_set (data->pb_sink, "left-volume", (atof(action[1])), NULL);
        }
        else if((g_str_has_prefix(data->cmd_list[data->cmd_count],"Right-volume")) && data->pb_playing){
            /* setting right-Volume  */
            g_print ("PVR: action_section -setting Right- volume on current playback session %s\n", data->cmd_list[data->cmd_count]);
            g_object_set (data->pb_sink, "right-volume", (atof(action[1])), NULL);
        }
        else if((g_str_has_prefix(data->cmd_list[data->cmd_count],"mute")) && data->pb_playing){
            /* setting mute flag on playback  */
            g_print ("PVR: action_section - setting mute flag on playback session %s\n", data->cmd_list[data->cmd_count]);
            g_object_set (data->pb_sink, "mute", (atoi(action[1]) == 1), NULL);
        }
        else if((g_str_has_prefix(data->cmd_list[data->cmd_count],"preset")) && data->pb_playing){
              /* setting the strength */
              g_print ("PVR: action_section -seting the Strength of the Dynamic filter\n");
              g_object_set (data->pb_enhancer, "preset", (atoi(action[1])), NULL);
        }
        else
            g_print ("PVR: action_section -Please pass valid command or pipeline is not active\n");
    }
    else{
              g_print ("PVR:  $$$$$$$$$$$$  End of CMD's or EOS or terminated $$$$$$$$$$$$$$$$$$$$$$ \n");
              g_print ("PVR:  changing active pipe line state to Pause\n");
              gst_element_set_state (data->activePipe, GST_STATE_PAUSED);
              g_print ("PVR:  changing active pipe line state to Ready\n");
              gst_element_set_state (data->activePipe, GST_STATE_READY);
              g_print ("PVR:  changing active pipe line state to NULL\n");
              gst_element_set_state (data->activePipe, GST_STATE_NULL);
              g_print ("PVR: action_section -Quiting the current pipeline loop\n");
              g_main_loop_quit (data->loop);
              g_print ("PVR: action_section -cleaned the current running pipeline \n");
              return 0;
    }
    data->cmd_count++;
    //data->timeout_dur = data->timeout_dur + 10;
    if (data->concurrency_flag  == 0){
        g_print ("PVR: action_section -setting dynamic  time out for callback function \n");
        g_timeout_add_seconds_full (G_PRIORITY_HIGH, data->timeout_dur, (GSourceFunc)action_section, data, NULL);
        return 0;
    }
    return 1;
}


static void pb_cb_message (GstBus *bus, GstMessage *msg, CustomData *data) {

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_ERROR: {
      GError *err;
      gchar *debug;
      gst_message_parse_error (msg, &err, &debug);
      g_print ("PVR Error: pb_cb_message -%s\n", err->message);
      g_error_free (err);
      g_free (debug);
      data->terminate = TRUE;
      break;
    }
    case GST_MESSAGE_BUFFERING: {
        gint percent = 0;
        gst_message_parse_buffering (msg, &percent);
        g_print ("PVR: Buffering (%u percent done)\n", percent);
        break;
      }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      g_print ("PVR: pb_cb_message -EOS is reached.\n");
      g_print ("PVR:  changing active pipe line state to Ready\n");
      gst_element_set_state (data->activePipe, GST_STATE_READY);
      g_print ("PVR:  changing active pipe line state to NULL\n");
      gst_element_set_state (data->activePipe, GST_STATE_NULL);
      g_print ("PVR: pb_cb_message -Quiting the current pipeline loop\n");
      data->terminate = TRUE;
      g_main_loop_quit (data->loop);
      break;
    case GST_MESSAGE_CLOCK_LOST:
      /* Get a new clock */
      gst_element_set_state (data->pb_pipe, GST_STATE_PAUSED);
      gst_element_set_state (data->pb_pipe, GST_STATE_PLAYING);
      break;
    case GST_MESSAGE_STATE_CHANGED: {
          GstState old_state, new_state, pending_state;
          gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
          if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data->pb_pipe)) {
            g_print ("PVR: pb_cb_message -Pipeline state changed from %s to %s:\n",
                gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
            /* Remember whether we are in the PLAYING state or not */
            data->pb_playing = (new_state == GST_STATE_PLAYING);
            if (data->pb_playing) {
              /* We just moved to PLAYING. Check if seeking is possible */
              GstQuery *query;
              gint64 start, end;
              query = gst_query_new_seeking (GST_FORMAT_TIME);
              if (gst_element_query (data->pb_pipe, query)) {
                gst_query_parse_seeking (query, NULL, &data->seek_enabled, &start, &end);
                if (data->seek_enabled) {
                  g_print ("PVR: pb_cb_message -Seeking is ENABLED from %" GST_TIME_FORMAT " to %" GST_TIME_FORMAT "\n",
                      GST_TIME_ARGS (start), GST_TIME_ARGS (end));
                  g_print ("PVR: action_section -calling seek print position function\n");
                  cb_print_position(data->pb_pipe);
                } else {
                  g_print ("PVR: pb_cb_message -Seeking is DISABLED for this stream.\n");
                }
              }
              else {
                g_printerr ("PVR: pb_cb_message -Seeking query failed.");
              }
              gst_query_unref (query);
            }
          }
        } break;
    default:
      /* Unhandled message */
      break;
    }
}


static void rec_cb_message (GstBus *bus, GstMessage *msg, CustomData *data) {

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_ERROR: {
      GError *err;
      gchar *debug;

      gst_message_parse_error (msg, &err, &debug);
      g_print ("PVR Error: rec_cb_message -%s\n", err->message);
      g_error_free (err);
      g_free (debug);
      data->terminate = TRUE;
      break;
    }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      g_print ("PVR: rec_cb_message -EOS is reached.\n");
      data->terminate = TRUE;
      break;
    case GST_MESSAGE_CLOCK_LOST:
      /* Get a new clock */
      gst_element_set_state (data->pb_pipe, GST_STATE_PAUSED);
      gst_element_set_state (data->pb_pipe, GST_STATE_PLAYING);
      break;
    case GST_MESSAGE_STATE_CHANGED: {
          GstState old_state, new_state, pending_state;
          gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
          if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data->rc_pipe)) {
            g_print ("PVR: rec_cb_message -Pipeline state changed from %s to %s:\n",
                gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
            /* Remember whether we are in the PLAYING state or not */
            data->rec_playing = (new_state == GST_STATE_PLAYING);
            if (data->rec_playing) {
                g_print ("PVR: rec_cb_message -$$$$ recording started ......\n");
            }
          }
        } break;
    default:
      /* Unhandled message */
      break;
    }
}


static void sva_cb_message (GstBus *bus, GstMessage *msg, CustomData *data) {
  gboolean r = 0;
  if (msg){
    GstObject *src_obj;
    const GstStructure *s;
    guint32 seqnum;

    seqnum = gst_message_get_seqnum (msg);

    s = gst_message_get_structure (msg);

    src_obj = GST_MESSAGE_SRC (msg);

    if (GST_IS_ELEMENT (src_obj)){
      g_print("PVR:Got message #%u from element \"%s\" (%s): ",
          (guint) seqnum, GST_ELEMENT_NAME (src_obj),
          GST_MESSAGE_TYPE_NAME (msg));
    } else if (GST_IS_PAD (src_obj)) {
      g_print("PVR:Got message #%u from pad \"%s:%s\" (%s): ",
          (guint) seqnum, GST_DEBUG_PAD_NAME (src_obj),
          GST_MESSAGE_TYPE_NAME (msg));
    } else if (GST_IS_OBJECT (src_obj)) {
      g_print("PVR:Got message #%u from object \"%s\" (%s): ",
          (guint) seqnum, GST_OBJECT_NAME (src_obj),
          GST_MESSAGE_TYPE_NAME (msg));
    } else {
      g_print("PVR:Got message #%u (%s): ", (guint) seqnum,
          GST_MESSAGE_TYPE_NAME (msg));
    }

    if (s){
      gchar *sstr;

      sstr = gst_structure_to_string (s);
      g_print("PVR:%s\n", sstr);
      g_free (sstr);
    } else {
      g_print("PVR:no message details\n");
    }
  }

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_ERROR: {
      GError *err;
      gchar *debug;

      gst_message_parse_error (msg, &err, &debug);
      g_print ("PVR Error: sva_cb_message -%s\n", err->message);
      g_error_free (err);
      g_free (debug);

      gst_element_set_state (data->sva_pipe, GST_STATE_READY);
      data->sva_terminate = TRUE;
      g_main_loop_quit (data->loop);
      break;
    }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      g_print ("PVR:sva_cb_message-  EOS is reached.\n");
      gst_element_set_state (data->sva_pipe, GST_STATE_READY);
      data->sva_terminate = TRUE;
      g_main_loop_quit (data->loop);
      break;
    case GST_MESSAGE_CLOCK_LOST:
      /* Get a new clock */
      gst_element_set_state (data->sva_pipe, GST_STATE_PAUSED);
      gst_element_set_state (data->sva_pipe, GST_STATE_PLAYING);
      break;
    case GST_MESSAGE_STATE_CHANGED: {
          GstState old_state, new_state, pending_state;
          gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
          if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data->sva_pipe)) {
            g_print ("PVR: sva_cb_message- SVA Pipeline state changed from %s to %s:\n",
                gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
            /* Remember whether we are in the PLAYING state or not */
            data->sva_playing = (new_state == GST_STATE_PLAYING);
              if (data->sva_playing) {
                  g_print ("PVR: sva_cb_message-calling action function from SVA call back function \n");
                  //gst_element_set_state (data->pb_pipe, GST_STATE_PLAYING);
                  if (data->concurrency_flag  != 0){
                      r = action_section(data);
                  }

                  if(r && data->concurrency_flag != 0)
                      g_timeout_add_seconds_full (G_PRIORITY_HIGH, 10, (GSourceFunc)sva_timeout, data, NULL);

              }
              if(new_state == GST_STATE_PAUSED){
                  g_print ("**********************************************************************************************\n");
                  g_print ("                                                                                              \n");
                  g_print ("       SMART ASSIATANCE in listening Mode: Utter keywork to perform action                    \n");
                  g_print ("                                                                                              \n");
                  g_print ("**********************************************************************************************\n");
              }
          }
        } break;
    default:
      /* Unhandled message */
      break;
    }
}

static gint create_streaming_pipeline(CustomData *data, gint session_id){
        display_session_info(data, session_id);
        GstElement *PB_source, *PB_parser;
        GstStateChangeReturn ret;
        data->terminate = FALSE;

        /* playback create elements */
        data->pb_pipe = gst_element_factory_make ("playbin", "play-bin");
        data->pb_sink = gst_element_factory_make ("qahwsink", "PB_sink");
        g_object_set(G_OBJECT(data->pb_sink), "module-handle", data->moduleId, "sync", false, NULL);

        /*checking for the playback elements created or not */
        if (!data->pb_pipe || !data->pb_sink ) {
            g_printerr ("PVR: create_playback_pipeline- Not all Playback elements could be created.\n");
	    return -1;
        }
        g_printerr ("PVR:setting url link\n");
        //data->session_info[session_id]->FileLoc = g_strconcat("uri://",data->session_info[session_id]->FileLoc,NULL);
        g_object_set (G_OBJECT (data->pb_pipe), "uri", data->session_info[session_id]->FileLoc, NULL);
        g_object_set (data->pb_pipe, "flags", "audio", NULL);
        g_object_set (data->pb_pipe, "audio-sink", data->pb_sink, NULL);

        // g_object_set (PB_source, "location", data->session_info[session_id]->FileLoc, NULL);

        g_print ("PVR: -setting volume on current playback session pipeline %s\n", data->session_info[session_id]->Volume);
        g_object_set (data->pb_sink, "volume", (atof(data->session_info[session_id]->Volume)), NULL);

        g_object_set (data->pb_sink, "output-device", data->session_info[session_id]->Device, NULL);
        g_print ("PVR: create_playback_pipeline -setting the pipeline to play state\n");
        ret = gst_element_set_state (data->pb_pipe, GST_STATE_PLAYING);
        if (ret == GST_STATE_CHANGE_FAILURE) {
           g_printerr ("PVR: create_playback_pipeline -Unable to set the playback_pipeline to the playing state.\n");
           gst_object_unref (data->pb_pipe);
           return -1;
        }
        data->activePipe = data->pb_pipe;
        g_print("PVR: ret successful\n");

        return 1;
}


static gint create_playback_pipeline(CustomData *data, gint session_id){

    display_session_info(data, session_id);
    GstElement *PB_source, *PB_parser, *PB_capFilter;
    GstStateChangeReturn ret;
    data->terminate = FALSE;
    GstCaps *caps;
    GError *error = NULL;
    gchar *pipe_str;

      /*Validating the URI of the file location */
      g_print ("PVR: create_playback_pipeline -Validating the URI of the file location %s\n", data->session_info[session_id]->FileLoc);
      if (g_file_test (data->session_info[session_id]->FileLoc, G_FILE_TEST_EXISTS)){
          g_print ("PVR: create_playback_pipeline- file location is valid: %s\n", data->session_info[session_id]->FileLoc);
      }
      else
      {
          g_print ("PVR: create_playback_pipeline- file location is NOT valid: %s\n", data->session_info[session_id]->FileLoc);
          return -1;
      }

      g_print ("PVR: create_playback_pipeline -new pipeline instance created\n");
      data->pb_pipe = gst_pipeline_new ("playback-pipeline");

      /* playback create elements */
      g_print ("PVR: create_playback_pipeline -new Element instance creating\n");
      PB_source = gst_element_factory_make ("filesrc", "PB_source");
      PB_parser = gst_element_factory_make (data->session_info[session_id]->Parser, "Parser");
      data->pb_sink = gst_element_factory_make ("qahwsink", "PB_sink");
      g_object_set(G_OBJECT(data->pb_sink), "module-handle", data->moduleId, "sync", false, NULL);

      data->PB_source = PB_source;

      if(g_strcmp0(data->session_info[session_id]->queue,"1")==0){

          pipe_str = g_strdup_printf ("filesrc location=%s ! qtdemux name=demux demux.audio_0 ! queue ! aacparse ! qahwsink volume=%f", data->session_info[session_id]->FileLoc, (atof(data->session_info[session_id]->Volume)));
          data->pb_pipe = gst_parse_launch (pipe_str, &error);
          if (data->pb_pipe != NULL, "Error parsing pipeline: %s",
                error ? error->message : "(invalid error)");
          g_free (pipe_str);
          ret = gst_element_set_state (data->pb_pipe, GST_STATE_PLAYING);
          if (ret == GST_STATE_CHANGE_FAILURE) {
            g_printerr ("PVR: create_playback_pipeline -Unable to set the playback_pipeline to the playing state.\n");
            gst_object_unref (data->pb_pipe);
            return -1;
          }
          data->activePipe = data->pb_pipe;
          g_print("PVR: ret successful\n");
          return 1;

      }
      else if(g_strcmp0(data->session_info[session_id]->Parser,"aiffparse")==0){
             g_print ("PVR: AIFF Parser found ....pipeline will create Aiff parser-->audioconvert --> caps -->sink ...\n");
             data->pb_Convertor = gst_element_factory_make (data->session_info[session_id]->Convertor, "Convertor");
             PB_capFilter = gst_element_factory_make ("capsfilter", "PB_capFilter");
             /*checking for the playback elements created or not */
             g_print ("PVR: create_playback_pipeline -checking new Element instance creating\n");
             if (!data->pb_pipe || !PB_source || !PB_parser || !data->pb_Convertor || !PB_capFilter || !data->pb_sink) {
                 g_printerr ("PVR: create_playback_pipeline- Not all Playback elements could be created.\n");
                 return -1;
             }

             /* must add elements to playback pipeline before linking them */
             gst_bin_add_many (GST_BIN (data->pb_pipe), PB_source, PB_parser, data->pb_Convertor, PB_capFilter, data->pb_sink, NULL);

             /* link Playback Elements*/
             if (!gst_element_link_many (PB_source, PB_parser, data->pb_Convertor, PB_capFilter, data->pb_sink, NULL)) {
                 g_warning ("PVR: create_playback_pipeline -Failed to link playback elements!");
                 gst_object_unref (data->pb_pipe);
                 return -1;
             }

              //creating the cap filter element properties with format
              g_print("PVR: create_playback_pipeline- setting caps with format \n");
              caps = gst_caps_new_simple ("audio/x-raw",
                      "format", G_TYPE_STRING, data->session_info[session_id]->Format,NULL);

              //Assigning caps to capfilter elements with channel & rate
              g_print("PVR: create_playback_pipeline- Assigning caps to capfilter elements with format\n");
              g_object_set (PB_capFilter, "caps",caps, NULL);

      }
      else if(g_strcmp0(data->session_info[session_id]->Parser,"oggdemux")==0){
              g_print ("PVR: OGG Parser found ....pipeline will create OGG parser--> caps -->sink ...\n");
              PB_capFilter = gst_element_factory_make ("capsfilter", "PB_capFilter");
              /*checking for the playback elements created or not */
              g_print ("PVR: create_playback_pipeline -checking new Element instance creating\n");
              if (!data->pb_pipe || !PB_source || !PB_parser || !PB_capFilter || !data->pb_sink) {
                  g_printerr ("PVR: create_playback_pipeline- Not all Playback elements could be created.\n");
                  return -1;
              }

              /* must add elements to playback pipeline before linking them */
              gst_bin_add_many (GST_BIN (data->pb_pipe), PB_source, PB_parser, PB_capFilter, data->pb_sink, NULL);

              /* link Playback Elements*/
              if (!(gst_element_link (PB_source, PB_parser) && gst_element_link_many (PB_capFilter, data->pb_sink, NULL))) {
                  g_warning ("PVR: create_playback_pipeline -Failed to link playback elements!");
                  gst_object_unref (data->pb_pipe);
                  return -1;
              }

               //creating the cap filter element properties with format
               g_print("PVR: create_playback_pipeline- setting caps \n");
               caps = gst_caps_new_simple ("audio/x-vorbis",
                       "framed", G_TYPE_BOOLEAN, "true",NULL);

               //Assigning caps to capfilter elements with channel & rate
               g_print("PVR: create_playback_pipeline- Assigning caps to capfilter elements \n");
               g_object_set (PB_capFilter, "caps",caps, NULL);
               g_signal_connect (PB_parser, "pad-added", G_CALLBACK (on_pad_added), PB_capFilter);
       }
      else if((g_strrstr(data->session_info[session_id]->Parser,"demux")!=NULL) && (g_strcmp0(data->session_info[session_id]->Decoder,"none")!=0) && (g_strcmp0(data->session_info[session_id]->Convertor,"none")!=0)){
          g_print ("PVR: Demuxer found ....pipeline will create demux-->dec-->convert --> sink ...\n");
          data->pb_Decoder = gst_element_factory_make (data->session_info[session_id]->Decoder, "Decoder");
          data->pb_Convertor = gst_element_factory_make (data->session_info[session_id]->Convertor, "Convertor");
          /*checking for the playback elements created or not */
          g_print ("PVR: create_playback_pipeline -checking new Element instance creating using demuxer\n");
          if (!data->pb_pipe || !PB_source || !PB_parser || !data->pb_Decoder || !data->pb_Convertor || !data->pb_sink) {
                g_printerr ("PVR: create_playback_pipeline- Not all Playback elements could be created.\n");
                return -1;
          }
          /* must add elements to playback pipeline before linking them */
          gst_bin_add_many (GST_BIN (data->pb_pipe), PB_source, PB_parser, data->pb_Decoder, data->pb_Convertor, data->pb_sink, NULL);

          /* link Playback Elements*/
         if(!(gst_element_link (PB_source, PB_parser) && gst_element_link_many (data->pb_Decoder, data->pb_Convertor, data->pb_sink, NULL))){
             g_warning ("PVR: create_playback_pipeline -Failed to link playback elements using muxers!");
             gst_object_unref (data->pb_pipe);
             return -1;
         }
         g_signal_connect (PB_parser, "pad-added", G_CALLBACK (on_pad_added), data->pb_Decoder);

      }
      else if((g_strrstr(data->session_info[session_id]->Parser,"demux")!=NULL) && (g_strcmp0(data->session_info[session_id]->Decoder,"none")!=0) ){
          g_print ("PVR: Demuxer found ....pipeline will create demux-->dec-->sink ...\n");
          data->pb_Decoder = gst_element_factory_make (data->session_info[session_id]->Decoder, "Decoder");
          /*checking for the playback elements created or not */
          g_print ("PVR: create_playback_pipeline -checking new Element instance creating using demuxer\n");
          if (!data->pb_pipe || !PB_source || !PB_parser || !data->pb_Decoder || !data->pb_sink) {
                g_printerr ("PVR: create_playback_pipeline- Not all Playback elements could be created.\n");
                return -1;
          }
          /* must add elements to playback pipeline before linking them */
          gst_bin_add_many (GST_BIN (data->pb_pipe), PB_source, PB_parser, data->pb_Decoder, data->pb_sink, NULL);

          /* link Playback Elements*/
         if(!(gst_element_link (PB_source, PB_parser) && gst_element_link_many (data->pb_Decoder, data->pb_sink, NULL))){
             g_warning ("PVR: create_playback_pipeline -Failed to link playback elements using muxers!");
             gst_object_unref (data->pb_pipe);
             return -1;
         }
         g_signal_connect (PB_parser, "pad-added", G_CALLBACK (on_pad_added), data->pb_Decoder);

      }
      else if((g_strrstr(data->session_info[session_id]->Parser,"demux")!=NULL)){
          if(g_strcmp0(data->session_info[session_id]->Enhancer,"none")!=0){
                  g_print ("PVR: Demuxer and Enhancer found ....pipeline will create demux-->Enhancer-->sink ...\n");
                  data->pb_enhancer = gst_element_factory_make (data->session_info[session_id]->Enhancer, "Enhancer");
              /*checking for the playback elements created or not */
              g_print ("PVR: create_playback_pipeline -checking new Element instance creating using demuxer,enahncer,sink\n");
              if (!data->pb_pipe || !PB_source || !PB_parser || !data->pb_enhancer || !data->pb_sink) {
                    g_printerr ("PVR: create_playback_pipeline- Not all Playback elements could be created.\n");
                    return -1;
              }
              /* must add elements to playback pipeline before linking them */
              gst_bin_add_many (GST_BIN (data->pb_pipe), PB_source, PB_parser, data->pb_enhancer, data->pb_sink, NULL);

              /* link source and parser Elements*/
              if(!(gst_element_link (PB_source, PB_parser))){
                 g_warning ("PVR: create_playback_pipeline -Failed to link playback elements using muxers!");
                 gst_object_unref (data->pb_pipe);
                 return -1;
             }
              /* link enhancer and sik Elements*/
              if(!(gst_element_link (data->pb_enhancer, data->pb_sink))){
                 g_warning ("PVR: create_playback_pipeline -Failed to link playback elements using muxers!");
                 gst_object_unref (data->pb_pipe);
                 return -1;
             }
              g_object_set (data->pb_enhancer, "enable", TRUE, NULL);
              g_signal_connect (PB_parser, "pad-added", G_CALLBACK (on_pad_added), data->pb_enhancer);
      }
          else{
              g_print ("PVR: Demuxer found ....pipeline will create demux-->sink ...\n");
                  /*checking for the playback elements created or not */
                  g_print ("PVR: create_playback_pipeline -checking new Element instance creating using demuxer\n");
                  if (!data->pb_pipe || !PB_source || !PB_parser || !data->pb_sink) {
                       g_printerr ("PVR: create_playback_pipeline- Not all Playback elements could be created.\n");
                       return -1;
                  }
                  /* must add elements to playback pipeline before linking them */
                  gst_bin_add_many (GST_BIN (data->pb_pipe), PB_source, PB_parser, data->pb_sink, NULL);
                  /* link Playback Elements*/
                  if(!(gst_element_link (PB_source, PB_parser))){
                       g_warning ("PVR: create_playback_pipeline -Failed to link playback elements using muxers!");
                       gst_object_unref (data->pb_pipe);
                       return -1;
                  }
                  g_signal_connect (PB_parser, "pad-added", G_CALLBACK (on_pad_added), data->pb_sink);
         }
      }
      else if((g_strcmp0(data->session_info[session_id]->Decoder,"none")!=0) && (g_strcmp0(data->session_info[session_id]->Convertor,"none")!=0)){
         g_print ("PVR: Parser found ....pipeline will create parser-->decode-->convert-->sink ...\n");

         data->pb_Decoder = gst_element_factory_make (data->session_info[session_id]->Decoder, "Decoder");
         data->pb_Convertor = gst_element_factory_make (data->session_info[session_id]->Convertor, "Convertor");
         /*checking for the playback elements created or not */
         g_print ("PVR: create_playback_pipeline -checking new Element instance creating\n");
         if (!data->pb_pipe || !PB_source || !PB_parser || !data->pb_Decoder || !data->pb_Convertor || !data->pb_sink) {
             g_printerr ("PVR: create_playback_pipeline- Not all Playback elements could be created.\n");
             return -1;
         }

         /* must add elements to playback pipeline before linking them */
         gst_bin_add_many (GST_BIN (data->pb_pipe), PB_source, PB_parser, data->pb_Decoder, data->pb_Convertor, data->pb_sink, NULL);

         /* link Playback Elements*/
         if (!gst_element_link_many (PB_source, PB_parser, data->pb_Decoder, data->pb_Convertor, data->pb_sink, NULL)) {
             g_warning ("PVR: create_playback_pipeline -Failed to link playback elements!");
             gst_object_unref (data->pb_pipe);
             return -1;
         }

     }
      else if((g_strcmp0(data->session_info[session_id]->Decoder,"none")!=0) ){
         g_print ("PVR: Parser found ....pipeline will create parser-->decode-->sink ...\n");

         data->pb_Decoder = gst_element_factory_make (data->session_info[session_id]->Decoder, "Decoder");
         /*checking for the playback elements created or not */
         g_print ("PVR: create_playback_pipeline -checking new Element instance creating\n");
         if (!data->pb_pipe || !PB_source || !PB_parser || !data->pb_Decoder || !data->pb_sink) {
             g_printerr ("PVR: create_playback_pipeline- Not all Playback elements could be created.\n");
             return -1;
         }

         /* must add elements to playback pipeline before linking them */
         gst_bin_add_many (GST_BIN (data->pb_pipe), PB_source, PB_parser, data->pb_Decoder, data->pb_sink, NULL);

         /* link Playback Elements*/
         if (!gst_element_link_many (PB_source, PB_parser, data->pb_Decoder, data->pb_sink, NULL)) {
             g_warning ("PVR: create_playback_pipeline -Failed to link playback elements!");
             gst_object_unref (data->pb_pipe);
             return -1;
         }

     }
     else if(g_strcmp0(data->session_info[session_id]->Enhancer,"none")!=0){
         g_print ("PVR: Parser found ....pipeline will create parser-->enhancer-->sink ...\n");
         data->pb_enhancer = gst_element_factory_make (data->session_info[session_id]->Enhancer, "Enhancer");
         /*checking for the playback elements created or not */
         g_print ("PVR: create_playback_pipeline -checking new Element instance creating\n");
         if (!data->pb_pipe || !PB_source || !PB_parser || !data->pb_enhancer || !data->pb_sink) {
             g_printerr ("PVR: create_playback_pipeline- Not all Playback elements could be created.\n");
             return -1;
         }

         /* must add elements to playback pipeline before linking them */
         gst_bin_add_many (GST_BIN (data->pb_pipe), PB_source, PB_parser, data->pb_enhancer, data->pb_sink, NULL);

         /* link Playback Elements*/
         if (!gst_element_link_many (PB_source, PB_parser, data->pb_enhancer, data->pb_sink, NULL)) {
             g_warning ("PVR: create_playback_pipeline -Failed to link playback elements!");
             gst_object_unref (data->pb_pipe);
             return -1;
         }
         g_object_set (data->pb_enhancer, "enable", TRUE, NULL);
     }
     else{
         /*checking for the playback elements created or not */
         g_print ("PVR: Parser found ....pipeline will create parser-->sink ...\n");
         if (!data->pb_pipe || !PB_source || !PB_parser || !data->pb_sink) {
             g_printerr ("PVR: create_playback_pipeline- Not all Playback elements could be created using normal method\n");
             return -1;
         }

         /* must add elements to playback pipeline before linking them */
         gst_bin_add_many (GST_BIN (data->pb_pipe), PB_source, PB_parser, data->pb_sink, NULL);

         /*  link Playback Elements */
         if (!gst_element_link_many (PB_source, PB_parser, data->pb_sink, NULL)) {
             g_warning ("PVR: create_playback_pipeline -Failed to link playback elements!");
             gst_object_unref (data->pb_pipe);
         return -1;
         }
     }

      g_object_set (PB_source, "location", data->session_info[session_id]->FileLoc, NULL);
      g_print ("PVR: -setting volume on current playback session pipeline %s\n", data->session_info[session_id]->Volume);
      g_object_set (data->pb_sink, "volume", (atof(data->session_info[session_id]->Volume)), NULL);
      g_print ("PVR: -setting output-flags path on current playback session pipeline:  %s\n", data->session_info[session_id]->Path);
      if(g_strcmp0(data->session_info[session_id]->Path,"Deepbuffer")==0){
          g_print("PVR: seting Deepbuffer path \n");
          g_object_set (data->pb_sink, "output-flags", 0x00000008, NULL);
      }
      else if(g_strcmp0(data->session_info[session_id]->Path,"LowLatency")==0){
          g_print("PVR: seting LowLatency path\n");
          g_object_set (data->pb_sink, "output-flags", 0x00000004, NULL);
      }
      else if (g_strcmp0(data->session_info[session_id]->Path,"Offload")==0){
          g_print("PVR: seting compress offload path\n");
          g_object_set (data->pb_sink, "output-flags", 0x00000010, NULL);
      }
      else{
          g_print("PVR: seting default non-blocking+compress-offload+direct path\n");
      }

      g_object_set (data->pb_sink, "output-device", data->session_info[session_id]->Device, NULL);
      g_print ("PVR: create_playback_pipeline -setting the pipeline to play state\n");
      ret = gst_element_set_state (data->pb_pipe, GST_STATE_PLAYING);
      if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("PVR: create_playback_pipeline -Unable to set the playback_pipeline to the playing state.\n");
        gst_object_unref (data->pb_pipe);
        return -1;
      }
      data->activePipe = data->pb_pipe;
      g_print("PVR: ret successful\n");
      return 1;
}


static gint create_recorder_pipeline(CustomData *data, gint session_id){

    display_session_info(data, session_id);
    GstElement *RC_source, *RC_parser, *RC_sink, *RC_capFilter;
    GstStateChangeReturn ret;
    GstCaps *caps;
    data->terminate = FALSE;

    data->rc_pipe = gst_pipeline_new ("recorder-pipeline");

      /* recorder create elements */
      g_print("PVR: creating recorder elements\n");
      RC_source = gst_element_factory_make ("qahwsrc", "RC_source");

      g_print("PVR: creting encoder element %s \n",data->session_info[session_id]->Encoder);
      RC_parser = gst_element_factory_make (data->session_info[session_id]->Encoder, "encoder");

      g_print("PVR: creating filesink elements\n");
      RC_sink = gst_element_factory_make ("filesink", "RC_sink");

      g_print("PVR: creating capfilter elements\n");
      RC_capFilter = gst_element_factory_make ("capsfilter", "RC_capFilter");

      /*checking for the recorder elements created or not */
      if (!data->rc_pipe || !RC_source || !RC_parser || !RC_sink || !RC_capFilter) {
        g_printerr ("PVR: create_recorder_pipeline- Not all Recorder elements could be created.\n");
        return -1;
      }

      /* must add elements to recorder pipeline before linking them */
      gst_bin_add_many (GST_BIN (data->rc_pipe), RC_source, RC_capFilter, RC_parser, RC_sink, NULL);

      /* link recorder Elements*/
      if (!gst_element_link_many (RC_source, RC_capFilter, RC_parser, RC_sink, NULL)) {
        g_warning ("PVR: create_recorder_pipeline -Failed to link recorder elements!");
        gst_object_unref (data->rc_pipe);
        return -1;
      }

      g_object_set (RC_source, "audio-source",data->session_info[session_id]->audiosource , NULL);

      //creating the cap filter element properties with channels ,format,sample rate
      g_print("PVR: setting caps with channel & rate\n");
      caps = gst_caps_new_simple ("audio/x-raw",
              "format", G_TYPE_STRING, data->session_info[session_id]->Format,
              "rate", G_TYPE_INT, data->session_info[session_id]->rate,
              "channels", G_TYPE_INT, data->session_info[session_id]->channels,
              NULL);

      //Assigning caps to capfilter elements with channel & rate
      g_print("PVR: Assigning caps to capfilter elements with channel & rate\n");
      g_object_set (RC_capFilter, "caps",caps, NULL);
      g_object_set (RC_sink, "location",data->session_info[session_id]->FileLoc , NULL);

      //adding encoder parameter to the encoder/parser element

      if(data->session_info[session_id]->bitrate!=0){
          g_print ("PVR: setting bitrate to encoder element\n");
          g_object_set (RC_parser, "bitrate",data->session_info[session_id]->bitrate , NULL);
      }
      if(data->session_info[session_id]->bandmode!=7){
          g_print ("PVR: setting bandmode to encoder element\n");
          g_object_set (RC_parser, "bandmode",data->session_info[session_id]->bandmode , NULL);
      }
      if(data->session_info[session_id]->dtx_enable!=0){
          g_print ("PVR: setting dtx_enable to encoder element\n");
          g_object_set (RC_parser, "dtx-enable",data->session_info[session_id]->dtx_enable , NULL);
      }
      if(data->session_info[session_id]->amrwb_enable!=0){
          g_print ("PVR: setting amrwb_enable to encoder element\n");
          g_object_set (RC_parser, "amrwb-enable",data->session_info[session_id]->amrwb_enable , NULL);
      }
      if(data->session_info[session_id]->encode_format!=0){
          g_print ("PVR: setting encode_format to encoder element\n");
          g_object_set (RC_parser, "encode-format",data->session_info[session_id]->encode_format , NULL);
      }

      g_print ("PVR: create_recorder_pipeline -setting the pipeline to record state\n");
      ret = gst_element_set_state (data->rc_pipe, GST_STATE_PLAYING);
      if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("PVR: create_recorder_pipeline -Unable to set the sva_pipeline to the recording state.\n");
        gst_object_unref (data->rc_pipe);
        return -1;
      }
      data->activePipe = data->rc_pipe;
      g_print("PVR: succesfully created recorder pipeline\n");
      return 1;
}


static gint create_sva_pipeline(CustomData *data, gint session_id){
display_session_info(data, session_id);
GstElement *sva_source, *sva_filter, *sva_sink;
GstStateChangeReturn ret;
data->terminate = FALSE;

 /* create pipeline and bus for playback*/
  data->sva_pipe = gst_pipeline_new ("sva-pipe");

  /* SVA create elements */
  g_print("PVR: creating sva elements\n");
  sva_source = gst_element_factory_make ("qsthwstrig", "sva_source");

  g_print("PVR: creting encoder element %s \n",data->session_info[session_id]->Encoder);
  sva_filter = gst_element_factory_make (data->session_info[session_id]->Encoder, "wavencode");
  // sva_filter = gst_element_factory_make ("wavenc", "wavencode");
  g_print("PVR: creating filesink elements\n");
  sva_sink = gst_element_factory_make ("filesink", "sva_sink");

  /*checking for the SVA elements created or not */
  if(data->session_info[session_id]->LAB)
  {
   if (!data->sva_pipe || !sva_source || !sva_filter || !sva_sink ) {
    g_printerr ("PVR: with LAB =1, Not all SVA elements could be created.\n");
    return -1;
  }
  }
  else{
       if (!data->sva_pipe || !sva_source ) {
        g_printerr ("PVR: with LAB = 0, Not all SVA elements could be created.\n");
        return -1;
      }
  }

  data->sva_soundtrig = sva_source;

  /* must add elements to sva_pipeline before linking them */
  gst_bin_add_many (GST_BIN (data->sva_pipe), sva_source, sva_filter, sva_sink, NULL);

     if (!gst_element_link_many (sva_source, sva_filter, sva_sink, NULL)) {
        g_warning ("PVR: create_sva_pipeline -Failed to link recorder elements!");
        gst_object_unref (data->sva_pipe);
        return -1;
      }

      g_object_set (sva_source, "lab", data->session_info[session_id]->LAB, NULL);
      g_object_set (sva_source, "model-location", data->session_info[session_id]->Model_location, NULL);
      g_object_set (sva_sink, "location", data->session_info[session_id]->FileLoc, NULL);
      g_object_set (sva_source, "module-handle", data->module, NULL);

      if(data->uuid_flag){
          g_print("PVR: setting UUID %s in  sva pipeline\n", data->uuid);
          g_object_set (sva_source, "vendor-uuid", data->uuid, NULL);
      }

      g_print ("PVR: create_sva_pipeline -setting the pipeline to play state\n");
      ret = gst_element_set_state (data->sva_pipe, GST_STATE_PLAYING);
      if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("PVR: create_sva_pipeline -Unable to set the sva_pipeline to the playing state.\n");
        gst_object_unref (data->sva_pipe);
        return -1;
      }
      data->activePipe = data->sva_pipe;
      g_print("PVR: succesfully created sva pipeline\n");
      return 1;
  }

static gint create_async_sva_pipeline(CustomData *data, gint session_id){
display_session_info(data, session_id);
GstElement *sva_source, *sva_filter, *sva_sink;
GstStateChangeReturn ret;
data->terminate = FALSE;
GError *error = NULL;
char pipe_str[1000];

 /* create pipeline and bus for playback*/
  data->sva_pipe = gst_pipeline_new ("sva-pipe");

  sprintf(pipe_str, "qsthwstrig module-handle=%u lab=1 model-location=%s ! wavenc ! filesink location=/data/sva_record.wav async=0 qsthwstrig lab=1 module-handle=%u model-location=%s ! wavenc ! filesink location=%s async=0", data->module, data->global_Model_location, data->module, data->session_info[session_id]->Model_location, data->session_info[session_id]->FileLoc);

   printf("\nPVR: Pipeline is :  %s",pipe_str );

   data->sva_pipe = gst_parse_launch (pipe_str, &error);

  if (data->sva_pipe != NULL, "Error parsing pipeline: %s",
        error ? error->message : "(invalid error)");

  ret = gst_element_set_state (data->sva_pipe, GST_STATE_PLAYING);

  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("PVR: create_playback_pipeline -Unable to set the playback_pipeline to the playing state.\n");
    gst_object_unref (data->sva_pipe);
    return -1;
  }

  data->activePipe = data->sva_pipe;
  g_print("PVR: ret successful\n");
  return 1;
}


/* Process keyboard input */
static gboolean handle_keyboard (GIOChannel *source, GIOCondition cond, CustomData *data) {
  gchar *str = NULL;

  if (g_io_channel_read_line (source, &str, NULL, NULL, NULL) == G_IO_STATUS_NORMAL) {
      g_print ("PVR: handle_keyboard: Keystroke detected: %s\n", str);
  }
  g_free (str);
  return TRUE;
}


int main (int   argc,
      char *argv[])
{
 GstElement *sva_pipe;
 GstElement *sva_source, *sva_filter, *sva_sink;
  GMainLoop *main_loop;
  GstStateChangeReturn ret;
  gint i, r, session_count=0, iteration_count,dur=10,AudiopropertyFlag=0;
  guint iterations=0,j=0;
  GIOChannel *io_stdin;

  CustomData data;
  GstBus *sva_bus;

  /* Initialize our data structure */
  memset (&data, 0, sizeof (data));
  data.uri = g_strdup (argv[1]);
  data.terminate = FALSE;
  data.moduleId = NULL;
  data.module = NULL;
  data.concurrency_flag = 0;
  data.uuid_flag = FALSE;
  data.global_Model_location = g_strdup("/data/HeySnapdragon.uim");

  /*Parsing config file */
  iterations=parseconfig(g_strdup (argv[1]), &data);
  data.cmd_list = g_strsplit_set (data.cmd, "\n", -1);
  g_print ("commands Length to perform on pipeline: %u\n", g_strv_length(data.cmd_list));
  data.total_cmd_count = g_strv_length(data.cmd_list);
  for(i=0;i<g_strv_length(data.cmd_list);i++)
    {
        data.cmd_list[i] = g_strconcat(data.cmd_list[i],"\n",NULL);
        data.cmd_list[i] = g_strstrip(data.cmd_list[i]);
    }
  AudiopropertyFlag = fillAudioCodecProperties(&data);
  if (AudiopropertyFlag == 0){
	  goto label;
  }
  /* Gstreamer init */
  gst_init (&argc, &argv);

  /* Loading primary module init */
  data.moduleId = qahw_load_module("audio.primary");

  /* Loading soundtrigger module init */
  data.module = qsthw_load_module ("soundtrigger.primary");

  if(data.concurrency_flag != 0){
          /* create pipeline and bus for SVA pipeline*/
          sva_pipe = gst_pipeline_new ("sva-pipeline");

          /* SVA create elements */
          sva_source = gst_element_factory_make ("qsthwstrig", "sva_source");
          sva_filter = gst_element_factory_make ("wavenc", "wavencode");
          sva_sink = gst_element_factory_make ("filesink", "sva_sink");

          /*checking for the SVA elements created or not */
          if (!sva_pipe || !sva_source || !sva_filter || !sva_sink) {
            g_printerr ("PVR: Not all SVA elements could be created.\n");
            return -1;
          }

          /* must add elements to sva_pipeline before linking them */
          gst_bin_add_many (GST_BIN (sva_pipe), sva_source, sva_filter, sva_sink, NULL);

          /* link SVA Elements*/
          if (!gst_element_link_many (sva_source, sva_filter, sva_sink, NULL)) {
            g_warning ("PVR: Failed to link sva elements!");
            gst_object_unref (sva_pipe);
            return -1;
          }

          /*creating bus */
          sva_bus = gst_element_get_bus (sva_pipe);

          /* Setting sva params */
          g_object_set (sva_source, "lab", TRUE, NULL);

          g_print ("PVR:sound model location:%s\n", data.global_Model_location);
          g_object_set (sva_source, "model-location", data.global_Model_location, NULL);

          g_print ("PVR:sva recorderlocation location: /data/sva_record.wav");
          g_object_set (sva_sink, "location", "/data/sva_record.wav", NULL);

          g_print ("PVR:sound model handle in concurrency scenario :%s\n", data.global_Model_location);
          g_object_set (sva_source, "module-handle", data.module, NULL);

          g_print ("PVR:sva UUID flag :%d",data.uuid_flag);
          if(data.uuid_flag){
              g_print("PVR: setting UUID %s in  sva concurrency pipeline\n", data.uuid);
              g_object_set (sva_source, "vendor-uuid", data.uuid, NULL);
          }
          data.sva_pipe = sva_pipe;

          /*Adding signal to watch SVA bus messages */
          gst_bus_add_signal_watch (sva_bus);
          g_signal_connect (sva_bus, "message", G_CALLBACK (sva_cb_message), &data);

  }
  /* Creating main loop instance and assigning the playbin and pipeline detials to global DATA structure */
  main_loop = g_main_loop_new (NULL, FALSE);
  data.loop = main_loop;

  iteration_count = 0;

  data.timeout_dur=data.session_info[session_count]->Duration;

  for(j=iterations;j>0;j--){
      g_print ("PVR: $$$$$$$$$$$$$$$ Iteration count : %d $$$$$$$$$$$$$$$$$$$\n", (iteration_count+1));
      data.cmd_count = 0;
      for (session_count =0; session_count <= data.count; session_count ++){
         g_print ("PVR: start of session : %d\n", session_count);
         /*setting the duration period to perform cmd section */
         dur=data.session_info[session_count]->Duration;

         /* Start playing the SVA pipeline before session if concurrency_flag == 1*/
         if(data.concurrency_flag == 1){
           g_print ("PVR: setting the SVA-BIN in playstate before session\n");
           ret = gst_element_set_state (sva_pipe, GST_STATE_PLAYING);
          if (ret == GST_STATE_CHANGE_FAILURE) {
              g_printerr ("PVR: Unable to set the sva_pipeline to the playing state before session\n");
             gst_object_unref (sva_pipe);
              return -1;
              }
         }

       if(data.session_info[session_count]->session_type == 1){
          r = create_playback_pipeline(&data, session_count);
           /*creating active session(playback) bus */
          if(r==1){
           g_print ("PVR: create the bus for playback pipeline\n");
           data.active_bus = gst_element_get_bus (data.activePipe);
           /*Adding signal to watch current running pipeline bus messages */
           g_print ("PVR: Adding signal watch for playback pipeline\n");
           gst_bus_add_signal_watch (data.active_bus);
           g_print ("PVR: connecting the bus to watch messages for playback pipeline\n");
           g_signal_connect (data.active_bus, "message", G_CALLBACK (pb_cb_message), &data);
          }
       }
       else if(data.session_info[session_count]->session_type == 2){
          r = create_recorder_pipeline(&data, session_count);
           /*creating active session(record) bus */
          if(r==1){
           g_print ("PVR: create the bus for Recorder pipeline\n");
           data.active_bus = gst_element_get_bus (data.activePipe);
           /*Adding signal to watch current running pipeline bus messages */
           g_print ("PVR: Adding signal watch for recorder pipeline\n");
           gst_bus_add_signal_watch (data.active_bus);
           g_print ("PVR: connecting the bus to watch messages for recorder pipeline\n");
           g_signal_connect (data.active_bus, "message", G_CALLBACK (rec_cb_message), &data);
          }
       }
       else if(data.session_info[session_count]->session_type == 3){
          if (data.session_info[session_count]->sva_multimode == 1){
              g_print ("PVR: calling create_async_sva_pipeline function\n");
              r = create_async_sva_pipeline(&data, session_count);
          }
          else{
              g_print ("PVR: calling create_sva_pipeline function\n");
              r = create_sva_pipeline(&data, session_count);
          }

           /*creating active session(sva) bus */
          if(r==1){
           g_print ("PVR: create the bus for SVA pipeline\n");
           data.active_bus = gst_element_get_bus (data.activePipe);
           /*Adding signal to watch current running pipeline bus messages */
           g_print ("PVR: Adding signal watch for sva pipeline\n");
           gst_bus_add_signal_watch (data.active_bus);
           g_print ("PVR: connecting the bus to watch messages for sva pipeline\n");
           g_signal_connect (data.active_bus, "message", G_CALLBACK (sva_cb_message), &data);
          }
       }
       else if(data.session_info[session_count]->session_type == 4){
          r = create_streaming_pipeline(&data, session_count);
           /*creating active session(playback) bus */
          if(r==1){
           g_print ("PVR: create the bus for playback pipeline\n");
           data.active_bus = gst_element_get_bus (data.activePipe);
           /*Adding signal to watch current running pipeline bus messages */
           g_print ("PVR: Adding signal watch for playback pipeline\n");
           gst_bus_add_signal_watch (data.active_bus);
           g_print ("PVR: connecting the bus to watch messages for playback pipeline\n");
           g_signal_connect (data.active_bus, "message", G_CALLBACK (pb_cb_message), &data);
          }
       }

       if(!(r ==1)){
           g_printerr ("PVR: some thing wrong in creating the Dynamic pipeline.\n");
           //gst_object_unref (data.active_bus);
       }
       else{

           if(g_str_has_prefix(data.cmd_list[data.cmd_count],"#CMD")){
               g_print ("PVR: processing the CMD's as per the config file on %s\n", data.cmd_list[data.cmd_count+1]);
           }
           else{
               g_print ("PVR: seeking to next cmd list\n");
               while(!(g_str_has_prefix(data.cmd_list[data.cmd_count],"#CMD"))){
                   g_print ("PVR: seeking ......\n");
                   data.cmd_count= data.cmd_count + 1;
               }
           g_print ("PVR: seek Completed\n");
           }
           data.cmd_count= data.cmd_count + 1;
       }

        /*Start playing the SVA pipeline after session if concurrency_flag == 2 */
       if(data.concurrency_flag == 2){
           g_print ("PVR: setting the SVA-BIN in play state after creating the session\n");
           ret = gst_element_set_state (sva_pipe, GST_STATE_PLAYING);
           if (ret == GST_STATE_CHANGE_FAILURE) {
              g_printerr ("PVR: Unable to set the sva_pipeline to the playing state after creating the session\n");
              gst_object_unref (sva_pipe);
              return -1;
           }
       }

       if(data.concurrency_flag == 0){
           g_timeout_add_seconds_full (G_PRIORITY_HIGH, data.timeout_dur, (GSourceFunc)action_section, &data, NULL);
       }
       g_main_loop_run (main_loop);
       g_print ("PVR: releasing resources\n");
      /* Free resources */

       if(data.concurrency_flag != 0){
           sleep(10);
           g_print ("PVR: Setting SVA pipeline to NUll State\n");
           gst_element_set_state (sva_pipe, GST_STATE_NULL);
       }
      if(r==1){
          g_print ("PVR: releasing Gstreamer bus resources\n");
          gst_object_unref (data.active_bus);
          g_print ("PVR: releasing Gstreamer active pipeline resources\n");
          gst_object_unref (GST_OBJECT (data.activePipe));
      }

      g_print ("PVR: End of session : %d\n", session_count);
      r=0;
      }

      g_print("PVR: End of iteration : %d\n",iteration_count);
      sleep(10);
      iteration_count++;
  }

  g_print ("PVR: Free/cleanup resources\n");

  /* Freeing up allocated resources */
      /* Free gtsreamer main loop */
      g_print ("PVR: releasing Gstreamer mainloop \n");
      g_main_loop_unref (main_loop);

      /* Free SVA pipeline and bus in concurrency scenario*/
      if(data.concurrency_flag != 0){
          g_print ("PVR: freeing SVA bus in concurrency scenario \n");
          gst_object_unref (sva_bus);
          g_print ("PVR: freeing SVA pipeline in concurrency scenario \n");
          gst_object_unref (sva_pipe);
      }

      if(data.moduleId){
          int err=0;
          printf("###########PVR: Unloading qahw module\n");
          err = qahw_unload_module(data.moduleId);
            if (err)
                printf("###########PVR: Failed to Unload qahw Module\n");
      }

      if(data.module){
          int err=0;
          printf("###########PVR: Unloading ST module\n");
          err = qsthw_unload_module(data.module);
            if (err)
                printf("###########PVR: Failed to Unload ST Module\n");
      }
      label:
      g_print ("PVR: releasing config file resources\n");
      for(i=0;i<data.count;i++){

        //free particular session at end
        g_free(data.session_info[i]);
      }

  g_free(data.session_info);
  g_print("PVR: Cleaning up DONE");
  g_print ("\n");
  return 0;
}

