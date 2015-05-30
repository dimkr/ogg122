/*
 * this file is part of ogg122.
 *
 * Copyright (c) 2014, 2015 Dima Krasner
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include <unistd.h>

#include <tinyalsa/asoundlib.h>
#include "stb/stb_vorbis.c"

#define SAMPLE_RATE (44100)
#define BUFFER_LEN (4096)

int main(int argc, char *argv[])
{
	struct pcm_config cfg;
	stb_vorbis *input;
	struct pcm *card;
	short *buf;
	int buflen;
	int error;
	int samples;
	int ret = EXIT_FAILURE;

	if (2 != argc) {
		(void) fprintf(stderr, "Usage: %s PATH\n", argv[0]);
		goto end;
	}

	input = stb_vorbis_open_filename(argv[1], &error, NULL);
	if (NULL == input)
		goto end;

	cfg.channels = input->channels;
	cfg.rate = SAMPLE_RATE;
	cfg.period_size = 1024;
	cfg.period_count = 4;
	cfg.format = PCM_FORMAT_S16_LE;
	cfg.start_threshold = 0;
	cfg.stop_threshold = 0;
	cfg.silence_threshold = 0;
	card = pcm_open(0, 0, PCM_OUT, &cfg);
	if (NULL == card)
		goto close_input;

	buflen = BUFFER_LEN * input->channels;
	buf = (short *) malloc(sizeof(short) * buflen);
	if (buf == NULL)
		goto close_card;

	do {
		/* read and decode samples */
		samples = stb_vorbis_get_frame_short_interleaved(input,
		                                                 input->channels,
		                                                 buf,
		                                                 buflen);
		if (0 == samples)
			break;

		/* play the decoded samples */
		if (0 != pcm_write(card, buf, pcm_frames_to_bytes(card, samples)))
			goto free_buf;
	} while (1);

	ret = EXIT_SUCCESS;

free_buf:
	free(buf);

close_card:
	(void) pcm_close(card);

close_input:
	stb_vorbis_close(input);

end:
	return ret;
}
