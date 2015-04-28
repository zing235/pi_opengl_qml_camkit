#ifndef CAMERAKIT_H
#define CAMERAKIT_H



#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <linux/videodev2.h>
#include <sys/time.h>
#include <pthread.h>
#include <assert.h>
#include <stdint.h>
#include "camkit.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_camera();
void camera_cce();
void camera_release();

#ifdef __cplusplus
}
#endif






#endif // CAMERAKIT_H
