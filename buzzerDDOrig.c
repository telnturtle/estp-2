/*
 * Copyright (C) 2009 Hanback Electronics Inc.
 *
 */
#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/mman.h>
#include <errno.h>


jint
Java_com_hanback_buzzer_BuzzerActivity_BuzzerControl( JNIEnv* env,
                                                  jobject thiz, jint value )
{
    int fd,ret;
    int data = value;

    fd = open("/dev/buzzer",O_WRONLY);
  
    if(fd < 0) return -errno;
  
    ret = write(fd, &data, 1);
    close(fd);
  
    if(ret == 1) return 0;
      
    return -1;
}


