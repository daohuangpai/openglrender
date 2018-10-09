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
 * @brief 根据编码id找到合适的硬件加速的解码器 \n
 * 使用av_hwaccel_next遍历全部的硬件解码器
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
* @brief 打开特定流，即音频流或者视频流解码器的会话句柄，必须在mp_decode_init之后调用，mp_decode_init会初始化d->codec \n
*  @note 此时d->codec必须已经赋值，这个函数会根据解码器的编码方式d->codec初始化该流的会话结构AVCodecContext        *decoder;\n
*  如果d->codec为空就是 c = avcodec_alloc_context3(NULL); 就是创建和编码无关的一个初始化会话，但是下面avcodec_open2\n
*  就会出错
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
* @brief  初始化mp_media_t *m结构（这个结构实际上是被传递，且不断被修改着，然后作为下一帧再次输入的块） \n
* @param[in] m    要初始化的一个流结构体（如果是音频就是初始化结构中的struct mp_decode v;否则是struct mp_decode a;）
* @param[in] type  这个流要初始化为什么流（音频或者视频）
* @param[in] hw    是否使用硬件解码器
* @note 包含了mp_open _codec (struct mp_decode*d)
*/
bool mp_decode_init(AVFormatContext* avfContext, struct mp_decode* d,enum AVMediaType type, bool hw)
{
	enum AVCodecID id;
	AVStream *stream;
	int ret;
	d->available = false;
	memset(d, 0, sizeof(*d));
	d->audio = type == AVMEDIA_TYPE_AUDIO;
	//根据传进来的AVMediaType type得到默认的最好的音频或者视频的stream的ID，这里视频
	//肯定是只有一个，但是音频可能有多个音轨，这边直接忽略了音轨的选择
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
	//这个位置如果失败了并没有释放，是否会出错？还是说释放都交给mp_media_t *m做了
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
* @brief  清理掉全部的packet，在停止或者重设的时候需要这么做，因为要开始解码新的packet群
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
	//清除packet
	mp_decode_clear_packets(d);
	//把d->packets结构的内存释放并且把改结构的全部数据设为0
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
* @brief  估计当前流的播放时间
* @param[in]  d 当前流
* @param[in]  last_pts 该流上一帧播放完的时间，如果这个值为0，那么就会自动根据码率估计时间
*/
static inline int64_t get_estimated_duration(struct mp_decode *d,
		int64_t last_pts)
{
	if (last_pts)
		return d->frame_pts - last_pts;

	//如果last_pts无效的情况下，只能对其进行估计
	if (d->audio) {
		//如果是音频，那么很容易根据该帧的音频采样率和音频的基准时间估计出该帧的持续时间
		return av_rescale_q(d->frame->nb_samples,
				(AVRational){1, d->frame->sample_rate},
				(AVRational){1, 1000000000});
	} else {
		//这边都是处理视频了，没有采样率，如果有上一次的持续时间，那么就使用这个
		if (d->last_duration)
			return d->last_duration;
		//如果没有就取一个base time的时间作为该帧时间
		return av_rescale_q(d->decoder->time_base.num,
				d->decoder->time_base,
				(AVRational){1, 1000000000});
	}
}
/**
* @brief 标准的解码流程，解码一帧，数据存在d->frame，解码mp_decode中的d->pkt帧信息或者之前一帧的pkt的 \n 
*这边预先调用avcodec_receive_frame，如果失败才avcodec_send_packet然后avcodec_receive_frame，保证上一帧的数据取干净
* @return 返回 
*       -# -1 失败
*       -# 0 成功并且是上一个packet的frame，不是d->pkt解码的，如果此时*got_frame = 0代表文件已经读取结束
*       -#  大于0的数  成功并且是d->pkt解码的
*       *got_frame = 0 表示没取到帧数据
*/
static int decode_packet(struct mp_decode *d, int *got_frame)
{
	int ret;
	*got_frame = 0;

#ifdef USE_NEW_FFMPEG_DECODE_API
	//先获得一次，如果成功，说明上一次的d->pkt还没解码干净，那么先返回
	ret = avcodec_receive_frame(d->decoder, d->frame);
	if (ret != 0 && ret != AVERROR(EAGAIN)) {
		//如果文件结束了，那么ret将是0，表示成功，否则是-1失败
		if (ret == AVERROR_EOF)
			ret = 0;
		return ret;
	}

	if (ret != 0) {
		//如果上一帧解码干净了，这边就先sendpacket再解码
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
* @brief 取得下一帧的frame \n
* 同时得到设置该帧的持续时间
* 如果有变速，也在这边做时间缩放
* bug---如果在视频播放一半的时候变速，这边的持续时间会有误差，只会算出
* 变速前计算的持续时间也按比例变化了
* @rnote 该返回值都是true，没什么用
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
			if (!d->packets.size) {//如果缓冲区已经没有packet了，那么如果是文件结束则置空，否则跳过这一次等待
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
		//文件结束了，那么就直接返回退出循环
		if (!got_frame && ret == 0) {
			d->eof = true;
			return true;
		}
		if (ret < 0) {//解码失败的话就把d->packet_pending = false;停止解码
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
		//在获得帧之后d->frame->best_effort_timestamp告诉我们这个帧最好延迟多久
		//如果是空就用我们之前取得音频或者视频播放最短的那一个的延迟时间+当前的时间最为下一帧的时间，也就是这帧的持续时间
		//这边的时间是实际时间，不是相对时间
		if (d->frame->best_effort_timestamp == AV_NOPTS_VALUE)
			d->frame_pts = d->next_pts;
		else
			//如果有最好的时间参考，那么把那个时间按照1s等于1000000000纳秒也就是我们现实中的时间基准
			//将best_effort_timestamp转为实际的时间，因为best_effort_timestamp个数是以d->frame->best_effort_timestamp为基准的
			//av_rescale_q(a,b,c)是用来把时间戳从一个时基调整到另外一个时基时候用的函数。它基本的动作是计算a*b/c
			//把1的采样是d->frame->best_effort_timestamp时间，而这里有best_effort_timestamp个采样转成1秒是1000000000纳秒的基准
			d->frame_pts = av_rescale_q(
					d->frame->best_effort_timestamp,
					d->stream->time_base,
					(AVRational){1, 1000000000});
		//解码后d->frame->pkt_duration表示该帧的播放时间
		int64_t duration = d->frame->pkt_duration;
		if (!duration)//如果这个值无效则自己估计
			duration = get_estimated_duration(d, last_pts);
		else//有效则按照帧标准转成时间值
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
