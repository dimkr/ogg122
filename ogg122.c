#include <stdlib.h>
#include <unistd.h>
#include <tinyalsa/asoundlib.h>
#include "stb_vorbis.c"

/* the sample rate */
#	define SAMPLE_RATE (44100)

/* the maximum number of chunks read at once */
#	define SAMPLE_CHUNK_SIZE (4096)

/* the usage message */
#	define USAGE "Usage: ogg012 PATH\n"

static void _show_help() {
	(void) write(STDOUT_FILENO, USAGE, sizeof(USAGE) - sizeof(char));
}

int main(int argc, char *argv[]) {
	/* playback settings */
	struct pcm_config config = {0};

	/* the decoding chunk size */
	int chunk_size = 0;

	/* an error code */
	int error = 0;

	/* the number of samples read at once */
	int samples = 0;

	/* the exit code */
	int exit_code = EXIT_FAILURE;

	/* the input file */
	stb_vorbis *input = NULL;

	/* a sound card */
	struct pcm *card = NULL;

	/* the decoded data */
	short *data = NULL;

	/* make sure only one argument was passed */
	if (2 != argc) {
		_show_help();
		goto end;
	}

	/* open the input file */
	input = stb_vorbis_open_filename(argv[1], &error, NULL);
	if (NULL == input) {
		if (VORBIS_file_open_failure == error) {
			_show_help();
		}
		goto end;
	}

	/* open the sound card */
	config.channels = input->channels;
	config.rate = SAMPLE_RATE;
	config.period_size = 1024;
	config.period_count = 4;
	config.format = PCM_FORMAT_S16_LE;
	config.start_threshold = 0;
	config.stop_threshold = 0;
	config.silence_threshold = 0;
	card = pcm_open(0, 0, PCM_OUT, &config);
	if (NULL == card) {
		goto close_input;
	}

	/* allocate memory for decoded samples */
	chunk_size = SAMPLE_CHUNK_SIZE * input->channels;
	data = (short *) malloc(sizeof(short) * chunk_size);
	if (data == NULL) {
		goto close_card;
	}

	do {
		/* read and decode samples */
		samples = stb_vorbis_get_frame_short_interleaved(input,
		                                                 input->channels,
		                                                 data,
		                                                 chunk_size);
		if (0 == samples) {
			break;
		}

		/* play the decoded samples */
		if (0 != pcm_write(card, data, pcm_frames_to_bytes(card, samples))) {
			goto free_samples;
		}
	} while (1);

	/* report success */
	exit_code = EXIT_SUCCESS;

free_samples:
	/* free the decoded samples */
	free(data);

close_card:
	/* close the sound card */
	(void) pcm_close(card);

close_input:
	/* close the input file */
	stb_vorbis_close(input);

end:
	return exit_code;
}
