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

#include "decode.h"

#define USE_NEW_FFMPEG_DECODE_API

//#include "media.h"
 /**
 * @brief ���ݱ���id�ҵ����ʵ�Ӳ�����ٵĽ����� \n
 * ʹ��av_hwaccel_next����ȫ����Ӳ��������
 */
static AVCodec *find_hardware_decoder(enum AVCodecID id)
{
	AVHWAccel *hwa = av_hwaccel_next(NULL);
	AVCodec *c = NULL;

	while (hwa) {
		if (hwa->id == id) {
			if (hwa->pix_fmt == AV_PIX_FMT_VDTOOL ||
				hwa->pix_fmt == AV_PIX_FMT_DXVA2_VLD ||
			    hwa->pix_fmt == AV_PIX_FMT_VAAPI_VLD) {
				c = avcodec_find_decoder_by_name(hwa->name);
				if (c)
					break;
			}
		}

		hwa = av_hwaccel_next(hwa);
	}

	return c;
}
/**
* @brief ���ض���������Ƶ��������Ƶ���������ĻỰ�����������mp_decode_init֮����ã�mp_decode_init���ʼ��d->codec \n
*  @note ��ʱd->codec�����Ѿ���ֵ�������������ݽ������ı��뷽ʽd->codec��ʼ�������ĻỰ�ṹAVCodecContext        *decoder;\n
*  ���d->codecΪ�վ��� c = avcodec_alloc_context3(NULL); ���Ǵ����ͱ����޹ص�һ����ʼ���Ự����������avcodec_open2\n
*  �ͻ����
*/
static int mp_open_codec(struct mp_decode *d)
{
	AVCodecContext *c;
	int ret;

#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 40, 101)
	c = avcodec_alloc_context3(d->codec);
	if (!c) {
		blog(LOG_WARNING, "MP: Failed to allocate context");
		return -1;
	}

	ret = avcodec_parameters_to_context(c, d->stream->codecpar);
	if (ret < 0)
		goto fail;
#else
	c = d->stream->codec;
#endif

	if (c->thread_count == 1 &&
	    c->codec_id != AV_CODEC_ID_PNG &&
	    c->codec_id != AV_CODEC_ID_TIFF &&
	    c->codec_id != AV_CODEC_ID_JPEG2000 &&
	    c->codec_id != AV_CODEC_ID_MPEG4 &&
	    c->codec_id != AV_CODEC_ID_WEBP)
		c->thread_count = 0;

	ret = avcodec_open2(c, d->codec, NULL);
	if (ret < 0)
		goto fail;

	d->decoder = c;
	return ret;

fail:
	avcodec_close(c);
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 40, 101)
	av_free(d->decoder);
#endif
	return ret;
}
/**
* @brief  ��ʼ��mp_media_t *m�ṹ������ṹʵ�����Ǳ����ݣ��Ҳ��ϱ��޸��ţ�Ȼ����Ϊ��һ֡�ٴ�����Ŀ飩 \n
* @param[in] m    Ҫ��ʼ����һ�����ṹ�壨�������Ƶ���ǳ�ʼ���ṹ�е�struct mp_decode v;������struct mp_decode a;��
* @param[in] type  �����Ҫ��ʼ��Ϊʲô������Ƶ������Ƶ��
* @param[in] hw    �Ƿ�ʹ��Ӳ��������
* @note ������mp_open _codec (struct mp_decode*d)
*/
bool mp_decode_init(AVFormatContext* avfContext, struct mp_decode* d,enum AVMediaType type, bool hw)
{
	enum AVCodecID id;
	AVStream *stream;
	int ret;
	d->available = false;
	memset(d, 0, sizeof(*d));
	d->audio = type == AVMEDIA_TYPE_AUDIO;
	//���ݴ�������AVMediaType type�õ�Ĭ�ϵ���õ���Ƶ������Ƶ��stream��ID��������Ƶ
	//�϶���ֻ��һ����������Ƶ�����ж�����죬���ֱ�Ӻ����������ѡ��
	ret = av_find_best_stream(avfContext, type, -1, -1, NULL, 0);
	if (ret < 0)
		return false;
	stream = d->stream = avfContext->streams[ret];

#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 40, 101)
	id = stream->codecpar->codec_id;
#else
	id = stream->codec->codec_id;
#endif

	if (hw) {
		d->codec = find_hardware_decoder(id);
		if (d->codec) {
			ret = mp_open_codec(d);
			if (ret < 0)
				d->codec = NULL;
		}
	}

	if (!d->codec) {
		if (id == AV_CODEC_ID_VP8)
			d->codec = avcodec_find_decoder_by_name("libvpx");
		else if (id == AV_CODEC_ID_VP9)
			d->codec = avcodec_find_decoder_by_name("libvpx-vp9");

		if (!d->codec)
			d->codec = avcodec_find_decoder(id);
		if (!d->codec) {
			blog(LOG_WARNING, "MP: Failed to find %s codec",
					av_get_media_type_string(type));
			return false;
		}

		ret = mp_open_codec(d);
		if (ret < 0) {
			blog(LOG_WARNING, "MP: Failed to open %s decoder: %s",
					av_get_media_type_string(type),
					av_err2str(ret));
			return false;
		}
	}
	//���λ�����ʧ���˲�û���ͷţ��Ƿ���������˵�ͷŶ�����mp_media_t *m����
	d->frame = av_frame_alloc();
	if (!d->frame) {
		blog(LOG_WARNING, "MP: Failed to allocate %s frame",
				av_get_media_type_string(type));
		return false;
	}

	if (d->codec->capabilities & CODEC_CAP_TRUNC)
		d->decoder->flags |= CODEC_FLAG_TRUNC;

	d->available = true;
	return true;
}
/**
* @brief  �����ȫ����packet����ֹͣ���������ʱ����Ҫ��ô������ΪҪ��ʼ�����µ�packetȺ
*/
void mp_decode_clear_packets(struct mp_decode *d)
{
	if (d->packet_pending) {
		av_packet_unref(&d->orig_pkt);
		d->packet_pending = false;
	}

	while (d->packets.size) {
		AVPacket pkt;
		circlebuf_pop_front(&d->packets, &pkt, sizeof(pkt));
		av_packet_unref(&pkt);
	}
}

void mp_decode_free(struct mp_decode *d)
{
	//���packet
	mp_decode_clear_packets(d);
	//��d->packets�ṹ���ڴ��ͷŲ��ҰѸĽṹ��ȫ��������Ϊ0
	circlebuf_free(&d->packets);

	if (d->decoder) {
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 40, 101)
		avcodec_free_context(&d->decoder);
#else
		avcodec_close(d->decoder);
#endif
	}
	if (d->frame) {
		av_frame_unref(d->frame);
		av_free(d->frame);
	}

	memset(d, 0, sizeof(*d));
}

void mp_decode_push_packet(struct mp_decode *decode, AVPacket *packet)
{
	circlebuf_push_back(&decode->packets, packet, sizeof(*packet));
}
/**
* @brief  ���Ƶ�ǰ���Ĳ���ʱ��
* @param[in]  d ��ǰ��
* @param[in]  last_pts ������һ֡�������ʱ�䣬������ֵΪ0����ô�ͻ��Զ��������ʹ���ʱ��
*/
static inline int64_t get_estimated_duration(struct mp_decode *d,
		int64_t last_pts)
{
	if (last_pts)
		return d->frame_pts - last_pts;

	//���last_pts��Ч������£�ֻ�ܶ�����й���
	if (d->audio) {
		//�������Ƶ����ô�����׸��ݸ�֡����Ƶ�����ʺ���Ƶ�Ļ�׼ʱ����Ƴ���֡�ĳ���ʱ��
		return av_rescale_q(d->frame->nb_samples,
				(AVRational){1, d->frame->sample_rate},
				(AVRational){1, 1000000000});
	} else {
		//��߶��Ǵ�����Ƶ�ˣ�û�в����ʣ��������һ�εĳ���ʱ�䣬��ô��ʹ�����
		if (d->last_duration)
			return d->last_duration;
		//���û�о�ȡһ��base time��ʱ����Ϊ��֡ʱ��
		return av_rescale_q(d->decoder->time_base.num,
				d->decoder->time_base,
				(AVRational){1, 1000000000});
	}
}
/**
* @brief ��׼�Ľ������̣�����һ֡�����ݴ���d->frame������mp_decode�е�d->pkt֡��Ϣ����֮ǰһ֡��pkt�� \n 
*���Ԥ�ȵ���avcodec_receive_frame�����ʧ�ܲ�avcodec_send_packetȻ��avcodec_receive_frame����֤��һ֡������ȡ�ɾ�
* @return ���� 
*       -# -1 ʧ��
*       -# 0 �ɹ���������һ��packet��frame������d->pkt����ģ������ʱ*got_frame = 0�����ļ��Ѿ���ȡ����
*       -#  ����0����  �ɹ�������d->pkt�����
*       *got_frame = 0 ��ʾûȡ��֡����
*/
static int decode_packet(struct mp_decode *d, int *got_frame)
{
	int ret;
	*got_frame = 0;

#ifdef USE_NEW_FFMPEG_DECODE_API
	//�Ȼ��һ�Σ�����ɹ���˵����һ�ε�d->pkt��û����ɾ�����ô�ȷ���
	ret = avcodec_receive_frame(d->decoder, d->frame);
	if (ret != 0 && ret != AVERROR(EAGAIN)) {
		//����ļ������ˣ���ôret����0����ʾ�ɹ���������-1ʧ��
		if (ret == AVERROR_EOF)
			ret = 0;
		return ret;
	}

	if (ret != 0) {
		//�����һ֡����ɾ��ˣ���߾���sendpacket�ٽ���
		ret = avcodec_send_packet(d->decoder, &d->pkt);
		if (ret != 0 && ret != AVERROR(EAGAIN)) {
			if (ret == AVERROR_EOF)
				ret = 0;
			return ret;
		}

		ret = avcodec_receive_frame(d->decoder, d->frame);
		if (ret != 0 && ret != AVERROR(EAGAIN)) {
			if (ret == AVERROR_EOF)
				ret = 0;
			return ret;
		}

		*got_frame = (ret == 0);
		ret = d->pkt.size;
	} else {
		ret = 0;
		*got_frame = 1;
	}

#else
	if (d->audio) {
		ret = avcodec_decode_audio4(d->decoder,
				d->frame, got_frame, &d->pkt);
	} else {
		ret = avcodec_decode_video2(d->decoder,
				d->frame, got_frame, &d->pkt);
	}
#endif
	return ret;
}
/**
* @brief ȡ����һ֡��frame \n
* ͬʱ�õ����ø�֡�ĳ���ʱ��
* ����б��٣�Ҳ�������ʱ������
* bug---�������Ƶ����һ���ʱ����٣���ߵĳ���ʱ�������ֻ�����
* ����ǰ����ĳ���ʱ��Ҳ�������仯��
* @rnote �÷���ֵ����true��ûʲô��
*/

bool mp_decode_next(struct mp_decode *d, bool eof)
{
	//bool eof = d->m->eof;
	int got_frame;
	int ret;

	d->frame_ready = false;

	if (!eof && !d->packets.size)
		return true;

	while (!d->frame_ready) {
		if (!d->packet_pending) {
			if (!d->packets.size) {//����������Ѿ�û��packet�ˣ���ô������ļ��������ÿգ�����������һ�εȴ�
				if (eof) {
					d->pkt.data = NULL;
					d->pkt.size = 0;
				} else {
					return true;
				}
			} else {
				circlebuf_pop_front(&d->packets, &d->orig_pkt,
						sizeof(d->orig_pkt));
				d->pkt = d->orig_pkt;
				d->packet_pending = true;
			}
		}

		ret = decode_packet(d, &got_frame);
		//�ļ������ˣ���ô��ֱ�ӷ����˳�ѭ��
		if (!got_frame && ret == 0) {
			d->eof = true;
			return true;
		}
		if (ret < 0) {//����ʧ�ܵĻ��Ͱ�d->packet_pending = false;ֹͣ����
#ifdef DETAILED_DEBUG_INFO
			blog(LOG_DEBUG, "MP: decode failed: %s",
					av_err2str(ret));
#endif

			if (d->packet_pending) {
				av_packet_unref(&d->orig_pkt);
				av_init_packet(&d->orig_pkt);
				av_init_packet(&d->pkt);
				d->packet_pending = false;
			}
			return true;
		}

		d->frame_ready = !!got_frame;

		if (d->packet_pending) {
			if (d->pkt.size) {
				d->pkt.data += ret;
				d->pkt.size -= ret;
			}

			if (d->pkt.size <= 0) {
				av_packet_unref(&d->orig_pkt);
				av_init_packet(&d->orig_pkt);
				av_init_packet(&d->pkt);
				d->packet_pending = false;
			}
		}
	}

	if (d->frame_ready) {
		int64_t last_pts = d->frame_pts;
		//�ڻ��֮֡��d->frame->best_effort_timestamp�����������֡����ӳٶ��
		//����ǿվ�������֮ǰȡ����Ƶ������Ƶ������̵���һ�����ӳ�ʱ��+��ǰ��ʱ����Ϊ��һ֡��ʱ�䣬Ҳ������֡�ĳ���ʱ��
		//��ߵ�ʱ����ʵ��ʱ�䣬�������ʱ��
		if (d->frame->best_effort_timestamp == AV_NOPTS_VALUE)
			d->frame_pts = d->next_pts;
		else
			//�������õ�ʱ��ο�����ô���Ǹ�ʱ�䰴��1s����1000000000����Ҳ����������ʵ�е�ʱ���׼
			//��best_effort_timestampתΪʵ�ʵ�ʱ�䣬��Ϊbest_effort_timestamp��������d->frame->best_effort_timestampΪ��׼��
			//av_rescale_q(a,b,c)��������ʱ�����һ��ʱ������������һ��ʱ��ʱ���õĺ������������Ķ����Ǽ���a*b/c
			//��1�Ĳ�����d->frame->best_effort_timestampʱ�䣬��������best_effort_timestamp������ת��1����1000000000����Ļ�׼
			d->frame_pts = av_rescale_q(
					d->frame->best_effort_timestamp,
					d->stream->time_base,
					(AVRational){1, 1000000000});
		//�����d->frame->pkt_duration��ʾ��֡�Ĳ���ʱ��
		int64_t duration = d->frame->pkt_duration;
		if (!duration)//������ֵ��Ч���Լ�����
			duration = get_estimated_duration(d, last_pts);
		else//��Ч����֡��׼ת��ʱ��ֵ
			duration = av_rescale_q(duration,
					d->stream->time_base,
					(AVRational){1, 1000000000});

		/*if (d->m->speed != 100) {
			d->frame_pts = av_rescale_q(d->frame_pts,
					(AVRational){1, d->m->speed},
					(AVRational){1, 100});
			duration = av_rescale_q(duration,
					(AVRational){1, d->m->speed},
					(AVRational){1, 100});
		}
*/
		d->last_duration = duration;
		d->next_pts = d->frame_pts + duration;
	}

	return true;
}

void mp_decode_flush(struct mp_decode *d)
{
	avcodec_flush_buffers(d->decoder);
	mp_decode_clear_packets(d);
	d->eof = false;
	d->frame_pts = 0;
	d->frame_ready = false;
}
