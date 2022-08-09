#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/mman.h>
#include <android/log.h>
#include <errno.h> // buzzer.c 오리지널 jni 프로그램에서 가져온 것임
#include <unistd.h> // sleep()
#include <android/log.h>

#include <linux/kernel.h> // printk()


JNIEXPORT void JNICALL Java_ac_kr_kgu_esproject_ArrayAdderActivity_NativeSum (JNIEnv* env,
                                    jobject thiz, jintArray jarray, jint jsum)
{
    // Buzzer의 레지스터에 쓰는 값
    int BUZZER_ON = 1;
    int BUZZER_OFF = 0;

    // 배열의 합계를 구한다
    int i, sum = 0;
    jsize len = (*env)->GetArrayLength(env, jarray);
    jint * array = (*env)->GetIntArrayElements(env, jarray, 0);
    for (i = 0; i < len; i++)
        sum += array[i];
    (*env)->ReleaseIntArrayElements(env, jarray, array, 0);
    
    // sum과 비교
    int is_correct = 0; // 덧셈이 맞았으면 1, 틀리면 0 값을 가진다
    if (sum == jsum)    is_correct = 1;
    else                is_correct = 0;

    // 디바이스 열기
    int dev_buzzer, dev_segment, ret ;
    dev_buzzer = open("/dev/buzzer",O_WRONLY);
    dev_segment = open("/dev/segment",O_RDWR | O_SYNC);
    
    // 디바이스를 열지 못했을 경우
    if ( dev_segment == -1 || dev_buzzer < 0 )
    {
        if (dev_segment == -1)
        {
            // 세그먼트 디바이스를 열지 못하면 로그를 출력
            printk("Segment Device Open Error\n");
            __android_log_print(ANDROID_LOG_ERROR, "ArrayAdderActivity", "Segment Device Open ERROR!\n");
        }
        if (dev_buzzer < 0)
        {
            __android_log_print(ANDROID_LOG_ERROR, "ArrayAdderActivity", "Buzzer Device Open ERROR!\n");
            printk("Buzzer Device Open ERROR!\n");
        }
        close(dev_segment);
        close(dev_buzzer);
        // 프로그램 종료
        exit(1);
    }

    // 디바이스를 성공적으로 열었을 경우
    // 덧셈이 정확한 경우
    if (is_correct)
    {
        // Buzzer 활성화
        // Buzzer의 경우 “삐”음이 주기적으로 On/Off되어야 함
        // 두 번 켜졌다 꺼지고 Buzzer 작동 끝난다고 임의로 설정
        // 1초 on, 1초 off, 1초 on, 1초 off, 1초 on, 계속 off
        
        // ioctl(dev_segment, <cmd>, <잘 가공한 정수값>);

        ioctl(dev_segment, 2, sum);
        // ioctl 인자 순서: 파일 디스크립터, 커맨드, 보조 인자, ...
        // buzzer cmd 0: 맞음, 1: 틀림
        // buzzer ioctl에서 보조 인자는 아무 값이나 (0 넣음)
        ioctl(dev_buzzer, 0, 0); 
    }
    // 덧셈이 틀린 경우
    else 
    {
        // Buzzer 활성화
        // Buzzer의 경우 “삐”음이 주기적으로 On/Off되어야 함
        // 3초 on 후 계속 off
        ioctl(dev_buzzer, 1, 0); 
        ioctl(dev_segment, 3, jsum*100+sum);
    }

    // 디바이스 닫기
    close(dev_segment);
    close(dev_buzzer);
}


// clear 버튼 누르면 부르는 함수
JNIEXPORT void Java_ac_kr_kgu_esproject_ArrayAdderActivity_Clear (JNIEnv* env,
    jobject thiz, jint cmd)
{
    // 디바이스 열기
    int dev_buzzer=0, dev_segment=0, ret=0;
    dev_buzzer = open("/dev/buzzer",O_WRONLY);
    dev_segment = open("/dev/segment",O_RDWR | O_SYNC);
    int buzzer_off = 0;
    
    // 디바이스를 열지 못했을 경우
    if ( dev_segment == -1 || dev_buzzer < 0 )
    {
        if (dev_segment == -1)
        {
            // 세그먼트 디바이스를 열지 못하면 로그를 출력
            __android_log_print(ANDROID_LOG_ERROR, "ArrayAdderActivity", "Segment Device Open ERROR!\n");
        }
        if (dev_buzzer < 0)
        {
            __android_log_print(ANDROID_LOG_ERROR, "ArrayAdderActivity", "Buzzer Device Open ERROR!\n");
        }
        close(dev_segment);
        close(dev_buzzer);
        // 프로그램 종료
        exit(1);
    }

    // 디바이스를 성공적으로 열었을 경우

    // Buzzer가 활성화된 경우 비활성화 시킴
    ret = write(dev_buzzer, &buzzer_off, 1);
    // ③ 7-Segment의 내용을 움직이는 직사각형으로 표시
    ioctl(dev_segment, 4, 0); 

    // 디바이스 닫기
    close(dev_segment);
    close(dev_buzzer);
}


/*JNIEXPORT */void /*JNICALL*/ Java_ac_kr_kgu_esproject_ArrayAdderActivity_TestBuzzerControl (JNIEnv* env,
    jobject thiz, jint value)
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


/*JNIEXPORT */void /*JNICALL*/ Java_ac_kr_kgu_esproject_ArrayAdderActivity_TestSegControl (JNIEnv* env,
    jobject thiz, jint data )
{
    __android_log_print(ANDROID_LOG_ERROR, "SegmentActivity", "Device Open ERROR!\n");
    int dev, ret ;
    dev = open("/dev/segment",O_RDWR | O_SYNC);

    if(dev != -1) {
        ret = write(dev,&data,4);
        close(dev);
    } else {
        // 디바이스를 열지 못하면 로그를 출력하고 프로그램 종료
        // __android_log_print(ANDROID_LOG_ERROR, "SegmentActivity", "Device Open ERROR!\n");
        exit(1);
    }
    return 0;
}
