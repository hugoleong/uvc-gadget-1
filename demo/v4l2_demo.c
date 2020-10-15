/*
 * V4L2 usage demo
 *
 *
 * Contact: Rocky Ching <zdh1021680772@163.com>
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/videodev2.h>


#define log  printf

struct video_buffer {
	void* start;
	unsigned int length;
};


static void v4l2_dump_cap(const struct v4l2_capability *cap)
{
	log("Video capabilities:\n");
	if (cap) {
		log("\tdriver: %s\n", cap->driver);
		log("\tcard: %s\n", cap->card);
		log("\tbus_info: %s\n", cap->bus_info);
		log("\tversion: %u.%u.%u\n", (cap->version>>16)&0xFF, (cap->version>>8)&0xFF, cap->version&0xFF);
		log("\tcapabilities: %x\n", cap->capabilities);
		log("\tdevice_caps: %x\n", cap->device_caps);
	}
	log("\n");
}

static void v4l2_dump_input(const struct v4l2_input *input)
{
	log("Video input:\n");
	if (input) {
		log("\tindex: %u\n", input->index);
		log("\tname: %s\n", input->name);
		log("\ttype: %u\n", input->type);
		log("\tstd: %llu\n", input->std);
		log("\tstatus: %u\n", input->status);
	}
	log("\n");
}

static void v4l2_dump_buffer(const struct v4l2_buffer *buff)
{
	log("Video buffer:\n");
	if (buff) {
		log("\tindex: %u\n", buff->index);
		log("\ttype: %u\n", buff->type);
		log("\tbytesused: %u\n", buff->bytesused);
		log("\tflags: %x\n", buff->flags);
		log("\tfield: %u\n", buff->field);
		log("\toffset: %u\n", buff->m.offset);
		log("\tlength: %u\n", buff->length);
		log("\ttimecode: %d:%d:%d\n", buff->timecode.hours, buff->timecode.minutes, buff->timecode.seconds);
	}
}

static int g_count = 0;


/**
 * videa_stream_process - Save video stream data as raw pictures
 * start - frame data start addredd
 * size - frame size
 */
static void videa_stream_process(void *start, unsigned int size)
{
	char filename[64] = { 0, };
	log("start: %p, size: %u\n", start, size);

	sprintf(filename, "/mnt/hgfs/F/%05d.raw", g_count++);
	int fd = open(filename, O_WRONLY | O_CREAT, 00766);//保存图像数据
	if (fd >= 0) {
		write(fd, start, size);
		close(fd);
		log("Video frame saved as %s\n", filename);
	} else {
		log("open %s failed: %d(%s)\n", filename, errno, strerror(errno));
	}
	log("\n");
}

int main(int argc, char *argv[])
{
	int cap_fd = -1;
	const char *CAMERA_FILE = argv[1];
	int res, i;

	if (argc != 2) {
		log("no input\n");
		return -1;
	}

	cap_fd = open(CAMERA_FILE, O_RDWR);
	if (cap_fd < 0) {
		perror("open video device");
		return -1;
	}

	struct v4l2_capability cap;
	memset(&cap, 0, sizeof(cap));
	res = ioctl(cap_fd, VIDIOC_QUERYCAP, &cap);
	if (res < 0) {
		perror("open video device");
		goto exit_close_fd;
	}
	v4l2_dump_cap(&cap);
	
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		log("%s does not support streaming i/o\n", CAMERA_FILE);
		goto exit_close_fd;
	}
	
	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		log("%s does not support streaming i/o\n", CAMERA_FILE);
		goto exit_close_fd;
	}

	v4l2_std_id std = 0;
	res = ioctl(cap_fd, VIDIOC_QUERYSTD, &std);
	if (res < 0) {
		perror("VIDIOC_QUERYSTD");
		//goto exit_close_fd;
	} else {
		log("video std_id: %llu\n", std);
	}

	struct v4l2_fmtdesc fmtdesc;
	fmtdesc.index = 0;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	log("Support format:\n");
	while (ioctl(cap_fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1) {
		printf("\t%d.%s\n", fmtdesc.index+1, fmtdesc.description);
		fmtdesc.index ++;
	}

	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	res = ioctl(cap_fd, VIDIOC_G_FMT, &fmt);
	if (res < 0) {
		perror("VIDIOC_G_FMT");
		//goto exit_close_fd;
	} else {
		log("Current data format information:\n");
		log("\twidth: %d\n\theight: %d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);
		log("\tpixelformat: %d\n\tfield: %d\n", fmt.fmt.pix.pixelformat, fmt.fmt.pix.field);
		log("\tsizeimage: %d\n\tcolorspace: %d\n", fmt.fmt.pix.sizeimage, fmt.fmt.pix.colorspace);
	}
	
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32;
	if (ioctl(cap_fd, VIDIOC_TRY_FMT, &fmt) == -1) {
		if (errno == EINVAL)
			log("%s not support format RGB32!\n\n", CAMERA_FILE);
	} else {
		log("%s support format RGB32!\n\n", CAMERA_FILE);
	}

	struct v4l2_input input;
	memset(&input, 0, sizeof(input));
	res = ioctl(cap_fd, VIDIOC_G_INPUT, &input.index);
	if (res < 0) {
		perror("VIDIOC_G_INPUT");
		goto exit_close_fd;
	}
	log("video input index: %u\n", input.index);

	res = ioctl(cap_fd, VIDIOC_ENUMINPUT, &input);
	if (res < 0) {
		perror("VIDIOC_ENUMINPUT");
		goto exit_close_fd;
	}
	v4l2_dump_input(&input);

	struct v4l2_standard standard;
	memset(&standard, 0, sizeof(standard));
	standard.index = 0;
	while (0 == ioctl(cap_fd, VIDIOC_ENUMSTD, &standard)) {
		if (standard.id & input.std) {
			log("got std: %s\n", standard.name);
			standard.index++;
		}

		/* EINVAL indicates the end of the enumeration, which cannot be empty unless this device falls under the USB exception. */
		if (errno != EINVAL || standard.index == 0) {
			perror("VIDIOC_ENUMSTD");
			break;
		}
    }

	struct v4l2_requestbuffers req; 
	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
	req.memory = V4L2_MEMORY_MMAP; 
	res = ioctl(cap_fd, VIDIOC_REQBUFS, &req);
	if (res < 0) {
		perror("VIDIOC_REQBUFS");
		goto exit_close_fd;
	}

	struct v4l2_buffer buf;
	memset(&buf, 0, sizeof(buf));
	buf.index = 0;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	res = ioctl(cap_fd, VIDIOC_QUERYBUF, &buf);
	if (res < 0) {
		perror("VIDIOC_QUERYBUF");
		goto exit_close_fd;
	}
	v4l2_dump_buffer(&buf);
	
	struct video_buffer buffers[4];
	//buffers = (struct video_buffer **) calloc(4, sizeof (struct video_buffer));
	///if (NULL == buffers) {
	//	log("Out of memory/n");
	//	goto exit_close_fd;
	//}

	for (i = 0; i < 4; i++) {
		memset(&buf,0,sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == ioctl(cap_fd, VIDIOC_QUERYBUF, &buf)) {
			perror("VIDIOC_QUERYBUF");
			goto exit_close_fd;
		}

		buffers[i].length = buf.length;
		buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, cap_fd, buf.m.offset);
		if (MAP_FAILED == buffers[i].start) {
			perror("VIDIOC_QUERYBUF");
			goto exit_close_fd;
		}
	}

	enum v4l2_buf_type type;
	for (i = 0; i < 4; ++i) {
		memset(&buf,0,sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		res = ioctl(cap_fd, VIDIOC_QBUF, &buf);
		if (res < 0) {
			perror("VIDIOC_QBUF");
			goto exit_close_fd;
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	res = ioctl(cap_fd, VIDIOC_STREAMON, &type);
	if (res < 0) {
		perror("VIDIOC_QBUF");
		goto exit_close_fd;
	}

	while (1) {
		memset(&buf,0,sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		res = ioctl(cap_fd, VIDIOC_DQBUF, &buf); // get a frame from buffer
		if (res < 0) {
			perror("VIDIOC_DQBUF");
			break;
		}
	
		videa_stream_process(buffers[buf.index].start, buf.bytesused);
		res = ioctl(cap_fd, VIDIOC_QBUF, &buf);
		if (res < 0) {
			perror("VIDIOC_DQBUF");
			break;
		}

		usleep(1000000);
	}

	for (i = 0; i < 4; i++) {
		munmap(buffers[i].start, buffers[i].length);
	}

exit_close_fd:
	if (cap_fd > 0) {
		close(cap_fd);
	}

	log("demo exit.\n");
	return 0;
}