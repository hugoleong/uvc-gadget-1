#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/videodev2.h>


#define BRIGHTNESS_ID V4L2_CID_BRIGHTNESS
#define CONTRAST_ID V4L2_CID_CONTRAST
#define SATURATION_ID V4L2_CID_SATURATION
#define HUE_ID V4L2_CID_HUE
#define WHITE_BALANCE_TEMP_AUTO_ID V4L2_CID_AUTO_WHITE_BALANCE
#define GAMMA_ID V4L2_CID_GAMMA
#define POWER_LINE_FREQUENCY_ID V4L2_CID_POWER_LINE_FREQUENCY
#define WHITE_BALANCE_TEMP_ID V4L2_CID_WHITE_BALANCE_TEMPERATURE
#define SHARPNESS_ID V4L2_CID_SHARPNESS
#define BACKLIGHT_COMPENSATION_ID V4L2_CID_BACKLIGHT_COMPENSATION
#define EXPOSURE_AUTO_ID V4L2_CID_EXPOSURE_AUTO
#define EXPOSURE_ABSOLUTE_ID V4L2_CID_EXPOSURE_ABSOLUTE
#define EXPOSURE_AUTO_PRIORITY_ID V4L2_CID_EXPOSURE_AUTO_PRIORITY


static struct v4l2_capability cap;
static struct v4l2_queryctrl qctrl;
static struct v4l2_control ctrl;
static struct v4l2_format Format;
static struct v4l2_streamparm Stream_Parm;


static void get_v4l2_info(int fd)
{
	printf("************************Get format info********************************\n");
	Format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_G_FMT, &Format) < 0) {
		perror("VIDIOC_G_FMT");
		exit(EXIT_FAILURE);
	}

	printf(">>>:[width: %d]\t[pixelformat: %x]\n", Format.fmt.pix.width, Format.fmt.pix.height);
	printf(">>>:[format: %d]\t[field: %d]\n", Format.fmt.pix.pixelformat, Format.fmt.pix.field);
	printf(">>>:[bytesperline: %d]\t[sizeimage: %d]\n", Format.fmt.pix.bytesperline, Format.fmt.pix.sizeimage);
	printf(">>>:[colorspace: %d]\n", Format.fmt.pix.colorspace);
	printf("***********************************************************************\n");
	printf("\n");

	printf("************************Get Stream_Parm********************************\n");
	Stream_Parm.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(fd, VIDIOC_G_PARM, &Stream_Parm) < 0) {
		perror("VIDIOC_G_PARM");
		exit(EXIT_FAILURE);
	}

	printf(">>>:[Frame rate: %u] [%u]\n",Stream_Parm.parm.capture.timeperframe.numerator, Stream_Parm.parm.capture.timeperframe.denominator);
	printf(">>>:[capability: %d] [capturemode: %d]\n", Stream_Parm.parm.capture.capability, Stream_Parm.parm.capture.capturemode);
	printf(">>>:[extendemode: %d] [readbuffers: %d]\n", Stream_Parm.parm.capture.extendedmode, Stream_Parm.parm.capture.readbuffers);
	printf("***********************************************************************\n");
	printf("\n");
}

void set_v4l2_para(int fd, int value)
{
	printf("Set format.........\n");
	memset(&Format, 0, sizeof(struct v4l2_format));
	Format.type= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	Format.fmt.pix.width = 320;
	Format.fmt.pix.height = 240;
	Format.fmt.pix.pixelformat= V4L2_PIX_FMT_YUYV;
	Format.fmt.pix.field = (enum v4l2_field)1;
	if (ioctl(fd, VIDIOC_S_FMT, &Format) < 0) {
		perror("VIDIOC_S_FMT");
		exit(EXIT_FAILURE);
	}
	sleep(1);
	printf("\n");

	printf("Set Stream_Parm.........\n");
	Stream_Parm.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	Stream_Parm.parm.capture.timeperframe.denominator =30;
	Stream_Parm.parm.capture.timeperframe.numerator =1;
	if (ioctl(fd, VIDIOC_S_PARM, &Stream_Parm) < 0) {
		perror("VIDIOC_S_PARM");
		exit(EXIT_FAILURE);
	}
	sleep(1);
	printf("\n");

#if 0
	printf("Set Exposure Auto Type.........\n");
	ctrl.id = 0x009a0901;
	ctrl.value = 3;
	if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0) {
		perror("VIDIOC_S_CTRL");
		exit(EXIT_FAILURE);
	}
	sleep(1);
	printf("\n");
#endif
#if 0
	printf("Set Exposure value.........\n");
	ctrl.id = 0x009a0902;
	ctrl.value = 312;
	if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0) {
		perror("VIDIOC_S_CTRL");
		exit(EXIT_FAILURE);
	}
	sleep(1);
	printf("\n");
#endif
#if 1
	printf("Set Exposure Auto Priority.........\n");
	ctrl.id = 0x009a0903;
	ctrl.value = 2;
	if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0) {
		perror("VIDIOC_S_CTRL");
		exit(EXIT_FAILURE);
	}
	sleep(1);
	printf("\n");
#endif
#ifdef XXXX_150
	printf("Set Gain value.........\n");
	ctrl.id = 0x00980913;
	if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0) {
		perror("VIDIOC_S_CTRL");
		exit(EXIT_FAILURE);
	}
	sleep(1);
	printf("\n");
#endif
#if 1
	printf("Set white balance Temperature Auto.........\n");
	ctrl.id = WHITE_BALANCE_TEMP_AUTO_ID;
	ctrl.value = value;
	if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0) {
		perror("VIDIOC_S_CTRL");    
		exit(EXIT_FAILURE);
	}
	sleep(1);
	printf("\n");
#endif
	printf("Set white balance Temperature.........\n");
	ctrl.id = WHITE_BALANCE_TEMP_ID;
	ctrl.value = value;
	if (ioctl(fd, VIDIOC_G_CTRL, &ctrl) < 0) {
		perror("VIDIOC_S_CTRL");
		exit(EXIT_FAILURE);
	}
	sleep(1);
	printf("\n");

	printf("Set brightness value.........\n");
	ctrl.id = BRIGHTNESS_ID;
	ctrl.value = value;
	if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0) {
		perror("VIDIOC_S_CTRL");
		exit(EXIT_FAILURE);
	}
	sleep(1);
	printf("\n");

	printf("Set contrast value.........\n");
	ctrl.id = CONTRAST_ID;
	ctrl.value = value;
	if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0) {
		perror("VIDIOC_S_CTRL");
		exit(EXIT_FAILURE);
	}
	sleep(1);
	printf("\n");

	printf("Set saturation value.........\n");
	ctrl.id = SATURATION_ID;
	ctrl.value = value;
	if(ioctl(fd,VIDIOC_G_CTRL,&ctrl) < 0) {
		perror("VIDIOC_S_CTRL");
		exit(EXIT_FAILURE);
	}
	sleep(1);
	printf("\n");
	printf("**********************All Set Success*********************\n");
}

int main(int argc, char *argv[])
{
	int fd = -1;
	int res = -1;
	int i;

	fd = open("/dev/video0",O_RDWR,0);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	res = ioctl(fd,VIDIOC_QUERYCAP, &cap);
	if (res == -1) {
		perror("VIDIOC_QUERYCAP");
		exit(EXIT_FAILURE);
	}

	printf("*********************Querycap Success**************************\n");
	if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
		printf("*****************capture is on*****************************\n");
	if(cap.capabilities & V4L2_CAP_STREAMING)
		printf("*****************Stream is on******************************\n");
	printf("\n\n");

	printf("*************************Capabilities**************************\n");
	printf(">>>:[driver: %s]\n>>>:[card: %s]\n>>>:[bus_info: %s]\n>>>:[version: %u.%u.%u]\n",
			cap.driver, cap.card, cap.bus_info, (cap.version>>16)&0xFF, (cap.version>>8)&0xFF, cap.version&0xFF);
	printf("**************************************************************\n");
	printf("\n\n");

	printf("UVC CLASS_USER: \n");
	for (i = V4L2_CID_BASE; i < V4L2_CID_LASTP1; i++) {
		qctrl.id = i;
		if (ioctl(fd, VIDIOC_QUERYCTRL, &qctrl) == 0) {
			ctrl.id = qctrl.id;
			if (ioctl(fd, VIDIOC_G_CTRL, &ctrl) < 0) {
				perror("VIDIOC_G_CTRL");
				continue;
			}

			printf(">>>:[name: %s]\t[id: %08x]\n", qctrl.name, qctrl.id);
			printf(">>>:[type: %d]\n",qctrl.type);
			printf(">>>:[min: %d]\t[max: %d]\n", qctrl.minimum, qctrl.maximum);
			printf(">>>:[value: %d]\t[step: %d]\n", ctrl.value, qctrl.step);
			printf(">>>:[default_value: %d]\n", qctrl.default_value);
			printf("\n");
		}
	}
	printf("UVC CLASS_USER end\n");
	printf("------------------------------------------------------------------\n\n");

	printf("UVC CLASS_CAMERA: \n");
	for (i = V4L2_CID_CAMERA_CLASS_BASE; i < V4L2_CID_TILT_SPEED + 1; i++) {
		qctrl.id = i;
		if (ioctl(fd, VIDIOC_QUERYCTRL, &qctrl) == 0) {
			ctrl.id = qctrl.id;
			if(ioctl(fd, VIDIOC_G_CTRL, &ctrl)<0) {
				perror("VIDIOC_G_CTRL\n");
				continue;
			}

			printf(">>>:[name: %s]\t[id: %08x]\n",qctrl.name,qctrl.id);
			printf(">>>:[type: %d]\n",qctrl.type);
			printf(">>>:[min: %d]\t[max: %d]\n",qctrl.minimum,qctrl.maximum);
			printf(">>>:[value: %d]\t[step: %d]\n",ctrl.value,qctrl.step);
			printf(">>>:[default_value: %d]\n",qctrl.default_value);
			printf("\n");
		}
	}
	printf("UVC CLASS_CAMERA end\n");

	get_v4l2_info(fd);

	set_v4l2_para(fd, 30);
}

