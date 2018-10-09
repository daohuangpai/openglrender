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

//����mp_decode _init (mp_media _t*m, enum AVMediaType type, bool hw)����ܵ���mp_decode _next (struct mp_decode*d)
//�õ�ÿһ֡�Ľ�������frame��
struct mp_decode {       ///<һ��mp_decode��ʾĳһ���ض�������Ƶ��������Ƶ���Ľ������ṹ
	//struct mp_media       *m;
	AVStream              *stream;
	bool                  audio; ///<��ʾ�ǲ�����Ƶ����������Ƶ��������Ƶ���
	bool                  available;///<��ʾ�Ƿ��ʼ���������ɹ�������ʹ��

	AVCodecContext        *decoder;
	AVCodec               *codec;

	int64_t               last_duration;///<��֡�ĳ���ʱ��
	int64_t               frame_pts;///<��֡�Ĳ���ʱ��,��mp_decode_next�б���ֵ
	int64_t               next_pts;///<��һ֡�Ĳ���ʱ��
	AVFrame               *frame;
	bool                  got_first_keyframe;
	bool                  frame_ready;///<��ʾ��ǰ��frame�Ƿ�׼���õ���Ч���Խ���
	bool                  eof;

	AVPacket              orig_pkt;///<ֻ�ǻ�����packets pop�������ݵ�һ�������ַ����ԭ����ֵ�������̰����ݿ�����pkt
	AVPacket              pkt;///<����decode_packet��������ṹ�е�frame��������packet
	bool                  packet_pending;
	struct circlebuf      packets;///<ֻ��һ��AVPacket���ݵĿռ���仺�棬��������ȫ���Ĵ������AVPacket
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
