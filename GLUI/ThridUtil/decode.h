/*
 * Copyright (c) 2017 Hugh Bailey <obs.jim@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "circlebuf.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4204)
#endif

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

#include "base.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#if LIBAVCODEC_VERSION_MAJOR >= 58
#define CODEC_CAP_TRUNC AV_CODEC_CAP_TRUNCATED
#define CODEC_FLAG_TRUNC AV_CODEC_FLAG_TRUNCATED
#else
#define CODEC_CAP_TRUNC CODEC_CAP_TRUNCATED
#define CODEC_FLAG_TRUNC CODEC_FLAG_TRUNCATED
#endif

#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(54, 31, 100)
#define AV_PIX_FMT_VDTOOL AV_PIX_FMT_VIDEOTOOLBOX
#else
#define AV_PIX_FMT_VDTOOL AV_PIX_FMT_VDA_VLD
#endif

typedef void(*mp_video_cb)(void *opaque, struct obs_source_frame *frame);
typedef void(*mp_audio_cb)(void *opaque, struct obs_source_audio *audio);
typedef void(*mp_stop_cb)(void *opaque);

//调用mp_decode _init (mp_media _t*m, enum AVMediaType type, bool hw)后就能调用mp_decode _next (struct mp_decode*d)
//得到每一帧的解码数据frame了
struct mp_decode {       ///<一个mp_decode表示某一种特定流，音频流或者视频流的解码器结构
	//struct mp_media       *m;
	AVStream              *stream;
	bool                  audio; ///<表示是不是音频流，不是音频流就是视频流喽
	bool                  available;///<表示是否初始化解码器成功，可以使用

	AVCodecContext        *decoder;
	AVCodec               *codec;

	int64_t               last_duration;///<该帧的持续时间
	int64_t               frame_pts;///<该帧的播放时间,在mp_decode_next中被赋值
	int64_t               next_pts;///<下一帧的播放时间
	AVFrame               *frame;
	bool                  got_first_keyframe;
	bool                  frame_ready;///<表示当前的frame是否准备好的有效可以解码
	bool                  eof;

	AVPacket              orig_pkt;///<只是缓存区packets pop出来数据的一个缓冲地址，即原来的值，会立刻把数据拷贝到pkt
	AVPacket              pkt;///<调用decode_packet将把这个结构中的frame解码成这个packet
	bool                  packet_pending;
	struct circlebuf      packets;///<只是一个AVPacket数据的空间分配缓存，里面存放着全部的待解码的AVPacket
};

extern bool mp_decode_init(AVFormatContext* avfContext, struct mp_decode* d, enum AVMediaType type, bool hw);
extern void mp_decode_free(struct mp_decode *decode);

extern void mp_decode_clear_packets(struct mp_decode *decode);

extern void mp_decode_push_packet(struct mp_decode *decode, AVPacket *pkt);
extern bool mp_decode_next(struct mp_decode *decode, bool eof);
extern void mp_decode_flush(struct mp_decode *decode);

#ifdef __cplusplus
}
#endif
