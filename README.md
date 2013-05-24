ATSC_Waveform_Generator
=======================

Take a JPG or PNG, then generate an ATSC waveform

ffmpeg format for image to transport stream: ffmpeg -i <ImageInput> -vcodec mpeg2video -f mpegts <outputName>
	-NOTE: 	1) The file extension of .jpg or .png must be present. 
		2) MPEG2 cannot handle any transparencies.
