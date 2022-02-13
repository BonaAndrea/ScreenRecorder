#include "ScreenRecorder.h"
#include <cassert>



using namespace std;
static int64_t last_pts = AV_NOPTS_VALUE;

ScreenRecorder::ScreenRecorder() : pauseCapture(false), stopCapture(false), started(false), activeMenu(true) {
    avdevice_register_all();
}

ScreenRecorder::ScreenRecorder(std::string RecPath) : pauseCapture(false), stopCapture(false), started(false), activeMenu(true) {
    avdevice_register_all();
    RecordingPath = RecPath;
}


ScreenRecorder::ScreenRecorder(const ScreenRecorder& p1) : pauseCapture(p1.pauseCapture), stopCapture(p1.stopCapture), started(p1.started), activeMenu(p1.activeMenu)
{
    
}


ScreenRecorder::~ScreenRecorder() {

    if (started) {
        value = av_write_trailer(outAVFormatContext);
        if (value < 0) {
            cerr << "Error in writing av trailer" << endl;
            exit(-1);
        }

        avformat_close_input(&inAudioFormatContext);
        if (inAudioFormatContext == nullptr) {
            cout << "inAudioFormatContext close successfully" << endl;
        }
        else {
            cerr << "Error: unable to close the inAudioFormatContext" << endl;
            exit(-1);
            //throw "Error: unable to close the file";
        }
        avformat_free_context(inAudioFormatContext);
        if (inAudioFormatContext == nullptr) {
            cout << "AudioFormat freed successfully" << endl;
        }
        else {
            cerr << "Error: unable to free AudioFormatContext" << endl;
            exit(-1);
        }

        avformat_close_input(&pAVFormatContext);
        if (pAVFormatContext == nullptr) {
            cout << "File close successfully" << endl;
        }
        else {
            cerr << "Error: unable to close the file" << endl;
            exit(-1);
            //throw "Error: unable to close the file";
        }

        avformat_free_context(pAVFormatContext);
        if (pAVFormatContext == nullptr) {
            cout << "VideoFormat freed successfully" << endl;
        }
        else {
            cerr << "Error: unable to free VideoFormatContext" << endl;
            exit(-1);
        }
    }
}

/*==================================== VIDEO ==============================*/

int ScreenRecorder::openVideoDevice() {
    value = 0;
    options = nullptr;
    pAVFormatContext = nullptr;

    pAVFormatContext = avformat_alloc_context();

    string dimension = to_string(width) + "x" + to_string(height);
    //av_dict_set(&options, "video_size", dimension.c_str(), 0);   //option to set the dimension of the screen section to record
    //av_dict_set(&options, "video_size", "1920x1080", 0);   //option to set the dimension of the screen section to record

    value = av_dict_set(&options, "probesize", "60M", 0);
    if (value < 0) {
        cerr << "Error in setting probesize value" << endl;
        exit(-1);
    }
    value = av_dict_set(&options, "rtbufsize", "2048M", 0);
    if (value < 0) {
        cerr << "Error in setting probesize value" << endl;
        exit(-1);
    }
    value = av_dict_set(&options, "offset_x", "0", 0);
    if (value < 0) {
        cerr << "Error in setting offset x value" << endl;
        exit(-1);
    }
    value = av_dict_set(&options, "offset_y", "0", 0);
    if (value < 0) {
        cerr << "Error in setting offset y value" << endl;
        exit(-1);
    }



#ifdef _WIN32
    HKEY hKey;
    char hexString[20];
    _itoa_s(cropX, hexString, 16);
    DWORD value = strtoul(hexString, NULL, 16);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\screen-capture-recorder\\"), 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
        if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,
            TEXT("SOFTWARE\\screen-capture-recorder\\"),
            0, NULL, 0,
            KEY_WRITE, NULL,
            &hKey, &value) != ERROR_SUCCESS) cout << "errore registro" << endl;
    RegSetValueEx(hKey, TEXT("start_x"), 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
    RegCloseKey(hKey);
    _itoa_s(cropY, hexString, 16);
    value = strtoul(hexString, NULL, 16);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\screen-capture-recorder\\"), 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
        if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,
            TEXT("SOFTWARE\\screen-capture-recorder\\"),
            0, NULL, 0,
            KEY_WRITE, NULL,
            &hKey, &value) != ERROR_SUCCESS) cout << "errore registro" << endl;
    RegSetValueEx(hKey, TEXT("start_y"), 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
    RegCloseKey(hKey);
    _itoa_s(cropW, hexString, 16);
    value = strtoul(hexString, NULL, 16);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\screen-capture-recorder\\"), 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
        if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,
            TEXT("SOFTWARE\\screen-capture-recorder\\"),
            0, NULL, 0,
            KEY_WRITE, NULL,
            &hKey, &value) != ERROR_SUCCESS) cout << "errore registro" << endl;
    RegSetValueEx(hKey, TEXT("capture_width"), 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
    RegCloseKey(hKey);
    _itoa_s(cropH, hexString, 16);
    value = strtoul(hexString, NULL, 16);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\screen-capture-recorder\\"), 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
        if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,
            TEXT("SOFTWARE\\screen-capture-recorder\\"),
            0, NULL, 0,
            KEY_WRITE, NULL,
            &hKey, &value) != ERROR_SUCCESS) cout << "errore registro" << endl;
    RegSetValueEx(hKey, TEXT("capture_height"), 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
    RegCloseKey(hKey);


    pAVInputFormat = av_find_input_format("dshow");
    if (avformat_open_input(&pAVFormatContext, "video=screen-capture-recorder", pAVInputFormat, &options) != 0) {
        cerr << "Couldn't open input stream" << endl;
        exit(-1);
    }

#elif defined linux

    int offset_x = 0, offset_y = 0;
    url = ":0.0+" + to_string(offset_x) + "," + to_string(offset_y);  //custom string to set the start point of the screen section
    pAVInputFormat = const_cast<AVInputFormat*> (av_find_input_format("x11grab"));
    value = avformat_open_input(&pAVFormatContext, url.c_str(), pAVInputFormat, &options);

    if (value != 0) {
        cerr << "Error in opening input device (video)" << endl;
        exit(-1);
    }
#else

    value = av_dict_set(&options, "pixel_format", "0rgb", 0);
    if (value < 0) {
        cerr << "Error in setting pixel format" << endl;
        exit(-1);
    }

    value = av_dict_set(&options, "video_device_index", "1", 0);

    if (value < 0) {
        cerr << "Error in setting video device index" << endl;
        exit(-1);
    }

    pAVInputFormat = av_find_input_format("avfoundation");

    if (avformat_open_input(&pAVFormatContext, "Capture screen 0:none", pAVInputFormat, &options) != 0) {  //TODO trovare un modo per selezionare sempre lo schermo (forse "Capture screen 0")
        cerr << "Error in opening input device" << endl;
        exit(-1);
    }



#endif
    //set frame per second

    value = av_dict_set(&options, "framerate", "30", 0);
    if (value < 0) {
        cerr << "Error in setting dictionary value (setting framerate)" << endl;
        exit(-1);
    }

    value = av_dict_set(&options, "preset", "medium", 0);
    if (value < 0) {
        cerr << "Error in setting dictionary value (setting preset value)" << endl;
        exit(-1);
    }
    /*
    value = av_dict_set(&options, "vsync", "1", 0);
    if(value < 0){
        cerr << "Error in setting dictionary value (setting vsync value)" << endl;
        exit(-1);
    }
    */


    //get video stream infos from context
    value = avformat_find_stream_info(pAVFormatContext, &options);
    if (value < 0) {
        cerr << "Error in retrieving the stream info" << endl;
        exit(-1);
    }

    VideoStreamIndx = -1;
    for (int i = 0; i < pAVFormatContext->nb_streams; i++) {
        if (pAVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            VideoStreamIndx = i;
            break;
        }
    }
    if (VideoStreamIndx == -1) {
        cerr << "Error: unable to find video stream index" << endl;
        exit(-2);
    }

    //pAVCodecContext = pAVFormatContext->streams[VideoStreamIndx]->codec;
    pCodecParameters = pAVFormatContext->streams[VideoStreamIndx]->codecpar;
    pAVCodec = const_cast<AVCodec*>(avcodec_find_decoder(pAVFormatContext->streams[VideoStreamIndx]->codecpar->codec_id/*params->codec_id*/));
    pAVCodecContext = avcodec_alloc_context3(pAVCodec);
    value = avcodec_parameters_to_context(pAVCodecContext, pCodecParameters);
    cout << "pAVCodec is: " << pAVCodec << endl;
    if (pAVCodec == nullptr) {
        cerr << "Error: unable to find decoder video" << endl;
        exit(-1);
    }
    if (value < 0)
    {
        cout << "\nUnable to set the parameter of the codec";
        exit(1);
    }

    //cout << "Insert height and width [h w]: ";   //custom screen dimension to record
    //cin >> h >> w;


    return 0;
}

/*==========================================  AUDIO  ============================*/

int ScreenRecorder::openAudioDevice() {
    audioOptions = nullptr;
    inAudioFormatContext = nullptr;

    inAudioFormatContext = avformat_alloc_context();
    value = av_dict_set(&audioOptions, "sample_rate", "44100", 0);
    if (value < 0) {
        cerr << "Error: cannot set audio sample rate" << endl;
        exit(-1);
    }
    value = av_dict_set(&audioOptions, "async", "1", 0);
    if (value < 0) {
        cerr << "Error: cannot set audio sample rate" << endl;
        exit(-1);
    }

#if defined linux
    audioInputFormat = const_cast<AVInputFormat*>(av_find_input_format("alsa"));
    value = avformat_open_input(&inAudioFormatContext, "hw:0", audioInputFormat, &audioOptions);
    if (value != 0) {
        cerr << "Error in opening input device (audio)" << endl;
        exit(-1);
    }
#endif

#if defined _WIN32
    audioInputFormat = av_find_input_format("dshow");
    //value = avformat_open_input(&inAudioFormatContext, "audio=Microfono (Realtek(R) Audio)", audioInputFormat, &audioOptions);
    value = avformat_open_input(&inAudioFormatContext, "audio=Microphone Array (Realtek(R) Audio)", audioInputFormat, &audioOptions);
    if (value != 0) {
        cerr << "Error in opening input device (audio)" << endl;
        exit(-1);
    }
#endif

    value = avformat_find_stream_info(inAudioFormatContext, nullptr);
    if (value != 0) {
        cerr << "Error: cannot find the audio stream information" << endl;
        exit(-1);
    }

    audioStreamIndx = -1;
    for (int i = 0; i < inAudioFormatContext->nb_streams; i++) {
        if (inAudioFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndx = i;
            break;
        }
    }
    if (audioStreamIndx == -1) {
        cerr << "Error: unable to find audio stream index" << endl;
        exit(-2);
    }
}

int ScreenRecorder::initOutputFile() {
    value = 0;

    outAVFormatContext = nullptr;
    outputAVFormat = const_cast<AVOutputFormat*>(av_guess_format(nullptr, RecordingPath.data(), nullptr));
    if (outputAVFormat == nullptr) {
        cerr << "Error in guessing the video format, try with correct format" << endl;
        exit(-5);
    }
#if WIN32
    avformat_alloc_output_context2(&outAVFormatContext, outputAVFormat, outputAVFormat->name, RecordingPath.data());
    if (outAVFormatContext == nullptr) {
        cerr << "Error in allocating outAVFormatContext" << endl;
        exit(-4);
    }
#elif linux
    avformat_alloc_output_context2(&outAVFormatContext, outputAVFormat, outputAVFormat->name, RecordingPath.data());
    if (outAVFormatContext == nullptr) {
        cerr << "Error in allocating outAVFormatContext" << endl;
        exit(-4);
    }
#endif


    /*===========================================================================*/
    this->generateVideoStream();
if(recordAudio)
    this->generateAudioStream();
    //create an empty video file
#if WIN32
    if (!(outAVFormatContext->flags & AVFMT_NOFILE)) {
        if (avio_open2(&outAVFormatContext->pb, RecordingPath.data(), AVIO_FLAG_WRITE, nullptr, nullptr) < 0) {
            cerr << "Error in creating the video file" << endl;
            exit(-10);
        }
    }
#elif linux
    if (!(outAVFormatContext->flags & AVFMT_NOFILE)) {
        if (avio_open2(&outAVFormatContext->pb, RecordingPath.data(), AVIO_FLAG_WRITE, nullptr, nullptr) < 0) {
            cerr << "Error in creating the video file" << endl;
            exit(-10);
        }
    }
#endif

    if (outAVFormatContext->nb_streams == 0) {
        cerr << "Output file does not contain any stream" << endl;
        exit(-11);
    }
    value = avformat_write_header(outAVFormatContext, &options);
    if (value < 0) {
        cerr << "Error in writing the header context" << endl;
        exit(-12);
    }
    return 0;
}

/*===================================  VIDEO  ==================================*/

void ScreenRecorder::generateVideoStream() {

    outVideoCodec = const_cast<AVCodec*>(avcodec_find_encoder(AV_CODEC_ID_MPEG4));  //AV_CODEC_ID_MPEG4
    if (outVideoCodec == nullptr) {
        cerr << "Error in finding the AVCodec, try again with the correct codec" << endl;
        exit(-8);
    }
    //outAVCodecContext = avcodec_alloc_context3(outAVCodec);
    outVideoCodecContext = avcodec_alloc_context3(outVideoCodec);
    if (outVideoCodecContext == nullptr) {
        cerr << "Error in allocating the codec context" << endl;
        exit(-7);
    }
    //Generate video stream

    videoSt = avformat_new_stream(outAVFormatContext, outVideoCodec);
    if (videoSt == nullptr) {
        cerr << "Error in creating AVFormatStream" << endl;
        exit(-6);
    }

    //set properties of the video file (stream)
    avcodec_parameters_to_context(outVideoCodecContext, videoSt->codecpar);
    outVideoCodecContext->codec_id = AV_CODEC_ID_MPEG4;
    outVideoCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    outVideoCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    outVideoCodecContext->bit_rate = 10000000;
    outVideoCodecContext->width = 1920; //setto larghezza
    outVideoCodecContext->height = 1080; // setto altezza video
    outVideoCodecContext->gop_size = 10;
    outVideoCodecContext->global_quality = 500;
    outVideoCodecContext->max_b_frames = 2;
    outVideoCodecContext->time_base.num = 1;
    outVideoCodecContext->time_base.den = 30;
    outVideoCodecContext->bit_rate_tolerance = 400000;

    if (outVideoCodecContext->codec_id == AV_CODEC_ID_H264) {
        av_opt_set(outVideoCodecContext->priv_data, "preset", "slow", 0);
    }

    if (outAVFormatContext->oformat->flags & AVFMT_GLOBALHEADER) {
        outVideoCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    value = avcodec_open2(outVideoCodecContext, outVideoCodec, nullptr);
    if (value < 0) {
        cerr << "Error in opening the AVCodec" << endl;
        exit(-9);
    }

    outVideoStreamIndex = -1;
    for (int i = 0; i < outAVFormatContext->nb_streams; i++) {
        if (outAVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_UNKNOWN) {
            outVideoStreamIndex = i;
        }
    }
    if (outVideoStreamIndex < 0) {
        cerr << "Error: cannot find a free stream index for video output" << endl;
        exit(-1);
    }
    avcodec_parameters_from_context(outAVFormatContext->streams[outVideoStreamIndex]->codecpar, outVideoCodecContext);
}

/*===============================  AUDIO  ==================================*/

void ScreenRecorder::generateAudioStream() {
    AVCodecParameters* params = inAudioFormatContext->streams[audioStreamIndx]->codecpar;
    inAudioCodec = const_cast<AVCodec*>(avcodec_find_decoder(params->codec_id));
    if (inAudioCodec == nullptr) {
        cerr << "Error: cannot find the audio decoder" << endl;
        exit(-1);
    }

    inAudioCodecContext = avcodec_alloc_context3(inAudioCodec);
    if (avcodec_parameters_to_context(inAudioCodecContext, params) < 0) {
        cout << "Cannot create codec context for audio input" << endl;
    }

    value = avcodec_open2(inAudioCodecContext, inAudioCodec, nullptr);
    if (value < 0) {
        cerr << "Error: cannot open the input audio codec" << endl;
        exit(-1);
    }

    //Generate audio stream
    outAudioCodecContext = nullptr;
    outAudioCodec = nullptr;
    int i;

    AVStream* audio_st = avformat_new_stream(outAVFormatContext, nullptr);
    if (audio_st == nullptr) {
        cerr << "Error: cannot create audio stream" << endl;
        exit(1);
    }

    outAudioCodec = const_cast<AVCodec*>(avcodec_find_encoder(AV_CODEC_ID_AAC));
    if (outAudioCodec == nullptr) {
        cerr << "Error: cannot find requested encoder" << endl;
        exit(1);
    }

    outAudioCodecContext = avcodec_alloc_context3(outAudioCodec);
    if (outAudioCodecContext == nullptr) {
        cerr << "Error: cannot create related VideoCodecContext" << endl;
        exit(1);
    }

    if ((outAudioCodec)->supported_samplerates) {
        outAudioCodecContext->sample_rate = (outAudioCodec)->supported_samplerates[0];
        for (i = 0; (outAudioCodec)->supported_samplerates[i]; i++) {
            if ((outAudioCodec)->supported_samplerates[i] == inAudioCodecContext->sample_rate)
                outAudioCodecContext->sample_rate = inAudioCodecContext->sample_rate;
        }
    }
    outAudioCodecContext->codec_id = AV_CODEC_ID_AAC;
    outAudioCodecContext->sample_fmt = (outAudioCodec)->sample_fmts ? (outAudioCodec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
    outAudioCodecContext->channels = inAudioCodecContext->channels;
    outAudioCodecContext->channel_layout = av_get_default_channel_layout(outAudioCodecContext->channels);
    outAudioCodecContext->bit_rate = 96000;
    outAudioCodecContext->time_base = { 1, inAudioCodecContext->sample_rate };

    outAudioCodecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

    if ((outAVFormatContext)->oformat->flags & AVFMT_GLOBALHEADER) {
        outAudioCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    if (avcodec_open2(outAudioCodecContext, outAudioCodec, nullptr) < 0) {
        cerr << "error in opening the avcodec" << endl;
        exit(1);
    }

    //find a free stream index
    outAudioStreamIndex = -1;
    for (i = 0; i < outAVFormatContext->nb_streams; i++) {
        if (outAVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_UNKNOWN) {
            outAudioStreamIndex = i;
        }
    }
    if (outAudioStreamIndex < 0) {
        cerr << "Error: cannot find a free stream for audio on the output" << endl;
        exit(1);
    }

    avcodec_parameters_from_context(outAVFormatContext->streams[outAudioStreamIndex]->codecpar, outAudioCodecContext);
}

int ScreenRecorder::init_fifo()
{
    /* Create the FIFO buffer based on the specified output sample format. */
    if (!(fifo = av_audio_fifo_alloc(outAudioCodecContext->sample_fmt,
        outAudioCodecContext->channels, outAudioCodecContext->sample_rate))) {
        fprintf(stderr, "Could not allocate FIFO\n");
        return AVERROR(ENOMEM);
    }
    return 0;
}

int ScreenRecorder::add_samples_to_fifo(uint8_t** converted_input_samples, const int frame_size) {
    int error;
    /* Make the FIFO as large as it needs to be to hold both,
     * the old and the new samples. */
    if ((error = av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + frame_size)) < 0) {
        fprintf(stderr, "Could not reallocate FIFO\n");
        return error;
    }
    /* Store the new samples in the FIFO buffer. */
    if (av_audio_fifo_write(fifo, (void**)converted_input_samples, frame_size) < frame_size) {
        fprintf(stderr, "Could not write data to FIFO\n");
        return AVERROR_EXIT;
    }
    return 0;
}

int ScreenRecorder::initConvertedSamples(uint8_t*** converted_input_samples,
    AVCodecContext* output_codec_context,
    int frame_size) {
    int error;
    /* Allocate as many pointers as there are audio channels.
     * Each pointer will later point to the audio samples of the corresponding
     * channels (although it may be NULL for interleaved formats).
     */
    if (!(*converted_input_samples = (uint8_t**)calloc(output_codec_context->channels,
        sizeof(**converted_input_samples)))) {
        fprintf(stderr, "Could not allocate converted input sample pointers\n");
        return AVERROR(ENOMEM);
    }
    /* Allocate memory for the samples of all channels in one consecutive
     * block for convenience. */
    if (av_samples_alloc(*converted_input_samples, nullptr,
        output_codec_context->channels,
        frame_size,
        output_codec_context->sample_fmt, 0) < 0) {

        exit(1);
    }
    return 0;
}

static int64_t pts = 0;
void ScreenRecorder::captureAudio() {
    int ret;
    AVPacket* inPacket, * outPacket;
    AVFrame* rawFrame, * scaledFrame;
    uint8_t** resampledData;
    init_fifo();

    //allocate space for a packet
    //inPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
    inPacket = av_packet_alloc();
    if (!inPacket) {
        cerr << "Cannot allocate an AVPacket for encoded video" << endl;
        exit(1);
    }
    //av_init_packet(inPacket);

    //allocate space for a packet
    rawFrame = av_frame_alloc();
    if (!rawFrame) {
        cerr << "Cannot allocate an AVPacket for encoded video" << endl;
        exit(1);
    }

    scaledFrame = av_frame_alloc();
    if (!scaledFrame) {
        cerr << "Cannot allocate an AVPacket for encoded video" << endl;
        exit(1);
    }

    outPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
    if (!outPacket) {
        cerr << "Cannot allocate an AVPacket for encoded video" << endl;
        exit(1);
    }

    //init the resampler
    SwrContext* resampleContext = nullptr;
    resampleContext = swr_alloc_set_opts(resampleContext,
        av_get_default_channel_layout(outAudioCodecContext->channels),
        outAudioCodecContext->sample_fmt,
        outAudioCodecContext->sample_rate,
        av_get_default_channel_layout(inAudioCodecContext->channels),
        inAudioCodecContext->sample_fmt,
        inAudioCodecContext->sample_rate,
        0,
        nullptr);
    if (!resampleContext) {
        cerr << "Cannot allocate the resample context" << endl;
        exit(1);
    }
    if ((swr_init(resampleContext)) < 0) {
        fprintf(stderr, "Could not open resample context\n");
        swr_free(&resampleContext);
        exit(1);
    }

    //while (true) {
    while (pAVCodecContext->frame_number < maxFrameNumber) {
        if (pauseCapture) {
            cout << "Pause audio" << endl;
            avformat_close_input(&inAudioFormatContext); //serve per il sync dell'audio???
            closedAudioRecording = true;
        }

        std::unique_lock<std::mutex> ul(mu);
        
        cv.wait(ul, [this]() { return !pauseCapture; });
        if (stopCapture) {
            break;
        }
        if (closedAudioRecording) {
#if WIN32
            avformat_open_input(&inAudioFormatContext, "audio=Microphone Array (Realtek(R) Audio)", audioInputFormat, &audioOptions); //per il sync?
#elif linux
            avformat_open_input(&inAudioFormatContext, "hw:0", audioInputFormat, &audioOptions);
#endif
            closedAudioRecording = false;
        }
        ul.unlock();
        if (av_read_frame(inAudioFormatContext, inPacket) >= 0 && inPacket->stream_index == audioStreamIndx) {
            //decode audio routing
            av_packet_rescale_ts(outPacket, inAudioFormatContext->streams[audioStreamIndx]->time_base, inAudioCodecContext->time_base);
            if ((ret = avcodec_send_packet(inAudioCodecContext, inPacket)) < 0) {
                cout << "Cannot decode current audio packet " << ret << endl;
                continue;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(inAudioCodecContext, rawFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    break;
                else if (ret < 0) {
                    cerr << "Error during decoding" << endl;
                    exit(1);
                }
                if (outAVFormatContext->streams[outAudioStreamIndex]->start_time <= 0) {
                    outAVFormatContext->streams[outAudioStreamIndex]->start_time = rawFrame->pts;
                }
                initConvertedSamples(&resampledData, outAudioCodecContext, rawFrame->nb_samples);

                swr_convert(resampleContext,
                    resampledData, rawFrame->nb_samples,
                    (const uint8_t**)rawFrame->extended_data, rawFrame->nb_samples);

                add_samples_to_fifo(resampledData, rawFrame->nb_samples);

                //raw frame ready
                //av_init_packet(outPacket);
                outPacket = av_packet_alloc();
                outPacket->data = nullptr;
                outPacket->size = 0;

                const int frame_size = FFMAX(av_audio_fifo_size(fifo), outAudioCodecContext->frame_size);

                scaledFrame = av_frame_alloc();
                if (!scaledFrame) {
                    cerr << "Cannot allocate an AVPacket for encoded video" << endl;
                    exit(1);
                }

                scaledFrame->nb_samples = outAudioCodecContext->frame_size;
                scaledFrame->channel_layout = outAudioCodecContext->channel_layout;
                scaledFrame->format = outAudioCodecContext->sample_fmt;
                scaledFrame->sample_rate = outAudioCodecContext->sample_rate;
                av_frame_get_buffer(scaledFrame, 0);

                while (av_audio_fifo_size(fifo) >= outAudioCodecContext->frame_size) {

                    ret = av_audio_fifo_read(fifo, (void**)(scaledFrame->data), outAudioCodecContext->frame_size);
                    scaledFrame->pts = pts;
                    pts += scaledFrame->nb_samples;
                    if (avcodec_send_frame(outAudioCodecContext, scaledFrame) < 0) {
                        cout << "Cannot encode current audio packet " << endl;
                        exit(1);
                    }
                    while (ret >= 0) {
                        ret = avcodec_receive_packet(outAudioCodecContext, outPacket);
                        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                            break;
                        else if (ret < 0) {
                            cerr << "Error during encoding" << endl;
                            exit(1);
                        }
                        av_packet_rescale_ts(outPacket, outAudioCodecContext->time_base, outAVFormatContext->streams[outAudioStreamIndex]->time_base);

                        outPacket->stream_index = outAudioStreamIndex;

                        write_lock.lock();

                        cout << outAVFormatContext << " " << outPacket << endl;
                        if (av_interleaved_write_frame(outAVFormatContext, outPacket) != 0)
                            //if (av_write_frame(outAVFormatContext, outPacket) != 0)
                        {
                            cerr << "Error in writing audio frame" << endl;
                        }
                        write_lock.unlock();
                        av_packet_unref(outPacket);
                    }
                    ret = 0;
                }
                av_frame_free(&scaledFrame);
                av_packet_unref(outPacket);
            }
        }
    }
}

int ScreenRecorder::captureVideoFrames() {
    int64_t pts = 0;
    int flag;
    int frameFinished = 0;
    bool endPause = false;
    int numPause = 0;
    AVFrame* croppedFrame;
#if linux
    string screenRes;
#endif


    ofstream outFile{ "..\\media\\log.txt", ios::out };

    int frameIndex = 0;
    value = 0;

    pAVPacket = av_packet_alloc();
    if (pAVPacket == nullptr) {
        cerr << "Error in allocating AVPacket" << endl;
        exit(-1);
    }

    pAVFrame = av_frame_alloc();
    if (pAVFrame == nullptr) {
        cerr << "Error: unable to alloc the AVFrame resources" << endl;
        exit(-1);
    }

    outFrame = av_frame_alloc();
    if (outFrame == nullptr) {
        cerr << "Error: unable to alloc the AVFrame resources for out frame" << endl;
        exit(-1);
    }

    int videoOutBuffSize;
    int nBytes = av_image_get_buffer_size(outVideoCodecContext->pix_fmt, outVideoCodecContext->width, outVideoCodecContext->height, 32);
    uint8_t* videoOutBuff = (uint8_t*)av_malloc(nBytes);

    if (videoOutBuff == nullptr) {
        cerr << "Error: unable to allocate memory" << endl;
        exit(-1);
    }

    value = av_image_fill_arrays(outFrame->data, outFrame->linesize, videoOutBuff, AV_PIX_FMT_YUV420P, outVideoCodecContext->width, outVideoCodecContext->height, 1);
    if (value < 0) {
        cerr << "Error in filling image array" << endl;
    }

    SwsContext* swsCtx_;

    if (!(swsCtx_ = sws_alloc_context()))
    {
        cout << "\nError nell'allocazione del SwsContext";
        exit(1);
    }
    value = sws_init_context(swsCtx_, NULL, NULL);
    if (value < 0)
    {
        cout << "\nErrore nell'inizializzazione del SwsContext";
        exit(1);
    }


    swsCtx_ = sws_getCachedContext(swsCtx_,
        pAVCodecContext->width,
        pAVCodecContext->height,
        pAVCodecContext->pix_fmt,
        outVideoCodecContext->width,
        outVideoCodecContext->height,
        outVideoCodecContext->pix_fmt,
        SWS_BILINEAR, NULL, NULL, NULL);


    if (avcodec_open2(pAVCodecContext, pAVCodec, &options) < 0) {
        cerr << "Could not open codec" << endl;
        exit(-1);
    }


    AVPacket* outPacket;
    int gotPicture;

    time_t startTime;
    time(&startTime);

    //cout << "Framerate is: " << outAVFormatContext. << endl;//av_dict_get, "framerate", NULL, AV_DICT_IGNORE_SUFFIX)->value << endl;


    //while (true) {
    while (pAVCodecContext->frame_number < maxFrameNumber) {
        //if (GetAsyncKeyState(VK_CONTROL)) pauseCapture = !pauseCapture;
        if (pauseCapture) {
            cout << "Pause" << endl;
            outFile << "///////////////////   Pause  ///////////////////" << endl;
            cout << "outVideoCodecContext->time_base: " << outVideoCodecContext->time_base.num << ", " << outVideoCodecContext->time_base.den << endl;
            avformat_close_input(&pAVFormatContext);
            closedVideoRecording = true;
        }
        std::unique_lock<std::mutex> ul(mu);

        cv.wait(ul, [this]() { return !pauseCapture; });   //pause capture (not busy waiting)
        if (endPause) {
            endPause = false;
        }

        if (stopCapture)  //check if the capture has to stop
            break;

        if (closedVideoRecording) {
#if WIN32
            avformat_open_input(&pAVFormatContext, "video=screen-capture-recorder", pAVInputFormat, &options);
#elif linux
            avformat_open_input(&pAVFormatContext, url.c_str(), pAVInputFormat, &options);
#endif
            closedVideoRecording = false;
        }

        ul.unlock();

        if (!pauseCapture&&av_read_frame(pAVFormatContext, pAVPacket) >= 0 && pAVPacket->stream_index == VideoStreamIndx) {
            av_packet_rescale_ts(pAVPacket, pAVFormatContext->streams[VideoStreamIndx]->time_base, pAVCodecContext->time_base);

                value = avcodec_send_packet(pAVCodecContext, pAVPacket);
                if (value < 0) {
                    cout << AVERROR(value);
                    cout << "Unable to decode video" << endl;
                }

                value = avcodec_receive_frame(pAVCodecContext, pAVFrame);
                //pAVFrame = crop_frame(pAVFrame, cropW-cropX, cropH-cropY, cropX, cropY);
                //pAVFrame = crop_frame(pAVFrame, cropX, cropY, cropW-cropX, cropH-cropY);
                cout << "\nFrame: " << pAVCodecContext->frame_number << "\n";
                if (value == AVERROR(EAGAIN) || value == AVERROR_EOF) {
                    cout << "\nOutput not available in this state.  Try to send new input. ";
                    //break;
                    //exit(1);
                }
                else if (value < 0)
                {
                    cout << "\nError during decoding";
                    exit(1);
                }

                value = sws_scale(swsCtx_, pAVFrame->data, pAVFrame->linesize, 0, pAVFrame->height, outFrame->data, outFrame->linesize);


                if (value < 0) {
                    cout << "\nProblem with sws_scale ";
                    //break;
                    exit(1);
                }
                //frame successfully decoded
                //sws_scale(swsCtx_, pAVFrame->data, pAVFrame->linesize, 0, pAVCodecContext->height, outFrame->data, outFrame->linesize);
                //av_init_packet(&outPacket);
                outPacket = av_packet_alloc();
                outPacket->data = nullptr;
                outPacket->size = 0;



                //if (outAVFormatContext->streams[outVideoStreamIndex]->start_time <= 0) {
                  //  outAVFormatContext->streams[outVideoStreamIndex]->start_time = pAVFrame->pts;
                //cout << "Width: " << outVideoCodecContext->width << ", height: " << outVideoCodecContext->height << endl;
                outFrame->width = outVideoCodecContext->width;
                outFrame->height = outVideoCodecContext->height;
                outFrame->format = outVideoCodecContext->pix_fmt;
                //cout << "Width: " << outFrame->width << ", height: " << outFrame->height << endl;

                value = avcodec_send_frame(outVideoCodecContext, outFrame);
                if (value < 0)
                {
                    cout << "\nError sending a frame for encoding. ERROR CODE: " << value;
                    continue;
                    //exit(1);
                }
                //disable warning on the console

                //avcodec_encode_video2(outVideoCodecContext, outPacket, outFrame, &gotPicture);

                //cout << "Write frame " << j++ << " (size = " << outPacket.size / 1000 << ")" << endl;
                //cout << "(size = " << outPacket.size << ")" << endl;

                //av_packet_rescale_ts(&outPacket, outVideoCodecContext->time_base, outAVFormatContext->streams[outVideoStreamIndex]->time_base);
                //outPacket.stream_index = outVideoStreamIndex;

                value = avcodec_receive_packet(outVideoCodecContext, outPacket); //Legge i dati codificati dall'encoder.
                if (value == AVERROR(EAGAIN))
                {
                    cout << "\nOutput not available in this state.  Try to send new input";
                    continue;
                    //exit(1);
                }
                else if (value < 0 && value != AVERROR_EOF)
                {
                    //cout << "\nAVERROR_EOF: " << AVERROR_EOF;
                    //cout << "\nAVERROR(EAGAIN): " << AVERROR(EAGAIN);
                    cout << "\nError during encoding";
                    //continue;
                    exit(1);
                }

                if (value >= 0) {
                    if (outPacket->pts != AV_NOPTS_VALUE) {
                        outPacket->pts = av_rescale_q(outPacket->pts, outVideoCodecContext->time_base, videoSt->time_base);
                    }
                    if (outPacket->dts != AV_NOPTS_VALUE) {
                        outPacket->dts = av_rescale_q(outPacket->dts, outVideoCodecContext->time_base, videoSt->time_base);
                    }



                    outFile << "outPacket->duration: " << outPacket->duration << ", " << "pAVPacket->duration: " << pAVPacket->duration << endl;
                    outFile << "outPacket->pts: " << outPacket->pts << ", " << "pAVPacket->pts: " << pAVPacket->pts << endl;
                    outFile << "outPacket.dts: " << outPacket->dts << ", " << "pAVPacket->dts: " << pAVPacket->dts << endl;
                    time_t timer;
                    double seconds;

                    mu.lock();
                    if (!activeMenu) {
                        time(&timer);
                        seconds = difftime(timer, startTime);
                        int h = (int)(seconds / 3600);
                        int m = (int)(seconds / 60) % 60;
                        int s = (int)(seconds) % 60;

                        std::cout << std::flush << "\r" << std::setw(2) << std::setfill('0') << h << ':'
                            << std::setw(2) << std::setfill('0') << m << ':'
                            << std::setw(2) << std::setfill('0') << s << std::flush;
                    }
                    mu.unlock();

                    write_lock.lock();
                    if (av_write_frame(outAVFormatContext, outPacket) != 0) {
                        cerr << "Error in writing video frame" << endl;
                    }
                    write_lock.unlock();
                    av_packet_free(&outPacket);
                }
                av_packet_free(&outPacket);
                av_packet_free(&pAVPacket);
                pAVPacket = av_packet_alloc();
                if (!pAVPacket)
                    exit(1);

                av_frame_free(&pAVFrame);
                pAVFrame = av_frame_alloc();
                if (!pAVFrame) // Verifichiamo che l'operazione svolta da "av_frame_alloc()" abbia avuto successo
                {
                    cout << "\nUnable to release the avframe resources";
                    exit(1);
                }

                av_frame_free(&outFrame);
                outFrame = av_frame_alloc();
                if (!outFrame)
                {
                    cout << "\nUnable to release the avframe resources for outframe";
                    exit(1);
                }
                value = av_image_fill_arrays(outFrame->data, outFrame->linesize, videoOutBuff, AV_PIX_FMT_YUV420P, outVideoCodecContext->width, outVideoCodecContext->height, 1);
                if (value < 0) // Verifico che non ci siano errori
                {
                    cout << "\nError in filling image array";
                    exit(value);
                }
            }
    }

    stopCapture = true;

    av_packet_free(&outPacket);

    value = av_write_trailer(outAVFormatContext);
    /*
     * Scrive il trailer dello stream in un file multimediale di output e
     * libera i dati privati ​​del file. Se non ci sono stati errori, ritorna 0.
     */
    if (value < 0)
    {
        cout << "\nError in writing av trailer";
        exit(1);
    }

    outFile.close();

    av_packet_free(&pAVPacket);
    sws_freeContext(swsCtx_);
    av_frame_free(&pAVFrame);
    av_frame_free(&outFrame);

    av_free(videoOutBuff);


    return 0;
}

void ScreenRecorder::CreateThreads() {
    std::thread t2(&ScreenRecorder::captureVideoFrames, this);
    //thread t2(&ScreenRecorder::captureVideoFrames, mu);
    if (recordAudio){
        std::thread t1(&ScreenRecorder::captureAudio, this);
    t1.join();
}
    t2.join();
}

void ScreenRecorder::SetUpScreenRecorder() {
    InnerSetup();
}

void ScreenRecorder::StopRecording() {
    stopCapture = true;
    if (pauseCapture) {
        pauseCapture = false;
        cv.notify_all();
    }
}

void ScreenRecorder::InnerSetup() {
    openVideoDevice();
    openAudioDevice();
    initOutputFile();
    CreateThreads();
}

void ScreenRecorder::PauseRecording()
{
    pauseCapture = !pauseCapture;
    if (!pauseCapture) {
        cv.notify_all();
    }
}


