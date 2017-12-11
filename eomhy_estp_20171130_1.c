#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/mman.h>
#include <android/log.h>
#include <errno.h> // buzzer.c 오리지널 jni 프로그램에서 가져온 것임
#include <unistd.h> // sleep()

// term_project_2.pdf 에서,
// Android 프로젝트 생성 시 사용값
// ① Project 이름: ESTermProject
// ② Application 이름: ArrayAdder
// ③ Package 이름: ac.kr.kgu.esproject
// ④ Activity 이름: ArrayAdderActivity
// jni의 함수 이름은 결정되어있음
// 안드로이드의 함수 이름은 안정해져있다

// jni의 함수 이름 작명법
// jint Java_com_hanback_segment_SegmentActivity_SegmentControl (JNIEnv* env, jobject thiz, jint data )
// ret_type Java_package_Activity_funcOfAndroid (fixed arg, fixed arg, real arg)

// 오리지날은 세그먼트만 이용하지만 텀프는 세그먼트에 추가로 부저도
// 사용한다. 파일 오픈에 추가

// 함수 jint Java_ac_kr_kgu_esproject_ArrayAdderActivity_fn1의 리턴 타입은
// jint와 void가 될 수 있다. 일단 void로 ㄱ

// 코딩할때 참조하는 사이트
// Java Native Interface Specification Contents    https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/jniTOC.html
//     http://rtos.kyonggi.ac.kr/rtos/lecture/2017-2/es/lecture_notes/JNI.pdf
// JNI programming - (5) 배열 다루기 2    http://forum.falinux.com/zbxe/index.php?document_srl=571214&mid=lecture_tip
// JNI, java code와 native code 간에 배열을 주고받고 싶습니다.    https://www.androidpub.com/794987
// [JNI/NDK] JNI에서 기본형 배열을 다루는 예    http://rockdrumy.tistory.com/1008


// 
// eomhy
// 
JNIEXPORT void Java_ac_kr_kgu_esproject_ArrayAdderActivity_NativeSum (JNIEnv* env,
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
    // 덧셈이 정확한 경우
    if (is_correct)
    {
        // 7-Segment에 덧셈 결과값 출력
        // 처리 루틴은 나중에 작성하고 일단 buzzer부터 만듦
        
        // Buzzer 활성화
        // Buzzer의 경우 “삐”음이 주기적으로 On/Off되어야 함
        // 두 번 켜졌다 꺼지고 Buzzer 작동 끝난다고 임의로 설정
        // 1초 on, 1초 off, 1초 on, 1초 off, 1초 on, 계속 off
        
        // ioctl 인자 순서
        // 파일 디스크립터, 커맨드, 보조 인자, ...
        // buzzer cmd 0: 맞음, 1: 틀림
        // buzzer ioctl에서 보조 인자는 아무 값이나 (0) 넣어
        ioctl(dev_buzzer, 0, 0); 
        // ioctl(dev_segment, <cmd>, <잘 가공한 정수값>);
    }

    // 덧셈이 틀린 경우
    else 
    {
        // 7-segment에 사용자 입력 값, 덧셈 결과값 출력
        // 
        // (처리 루틴은 나중에 작성하고 일단 buzzer부터 만듦)
        
        // Buzzer 활성화
        // Buzzer의 경우 “삐”음이 주기적으로 On/Off되어야 함
        // 3초 on 후 계속 off
        ioctl(dev_buzzer, 1, 0); 
    }

    // 디바이스 닫기
    close(dev_segment);
    close(dev_buzzer);
}




// D/D 테스트하는 jni 함수
JNIEXPORT void Java_ac_kr_kgu_esproject_ArrayAdderActivity_DDTest (JNIEnv* env,
                                    jobject thiz, jint cmd)
{
    // Buzzer의 레지스터에 쓰는 값
    int BUZZER_ON = 1;
    int BUZZER_OFF = 0;

    // 디바이스 열기
    int dev_buzzer, dev_segment, ret ;
    dev_buzzer = open("/dev/buzzer",O_WRONLY);

    // 
    if (dev_buzzer < 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "ArrayAdderActivity", "Buzzer Device Open ERROR!\n");
        close(dev_buzzer);
        exit(1);
    }

    ioctl(dev_buzzer, 0, 0); 
    sleep(7);
    ioctl(dev_buzzer, 1, 0)

    // 디바이스 닫기
    close(dev_buzzer);
}














}
/* // 부저 테스트 중엔 사용하지 않는다
jint
Java_com_hanback_segment_SegmentActivity_SegmentIOControl (JNIEnv* env,
                                                    jobject thiz, jint data )
{
    int dev, ret ;
    dev = open("/dev/segment",O_RDWR | O_SYNC);

    if(dev != -1) {
        ret = ioctl(dev, data, NULL, NULL);
        close(dev);
    } else {
        __android_log_print(ANDROID_LOG_ERROR, "SegmentActivity", "Device Open ERROR!\n");
        exit(1);
    }
    return 0;
}
*/





// 
// orignal 7segment.c 함수 내용
// 
/*
jint
Java_com_hanback_segment_SegmentActivity_SegmentControl (JNIEnv* env,
                                                    jobject thiz, jint data )
{	
    int dev, ret ;
    dev = open("/dev/segment",O_RDWR | O_SYNC);

    if(dev != -1) {
        ret = write(dev,&data,4);
        close(dev);
    } else {
        // 디바이스를 열지 못하면 로그를 출력하고 프로그램 종료
        __android_log_print(ANDROID_LOG_ERROR, "SegmentActivity", "Device Open ERROR!\n");
        exit(1);
    }
    return 0;
}

jint
Java_com_hanback_segment_SegmentActivity_SegmentIOControl (JNIEnv* env,
                                                    jobject thiz, jint data )
{
    int dev, ret ;
    dev = open("/dev/segment",O_RDWR | O_SYNC);

    if(dev != -1) {
        ret = ioctl(dev, data, NULL, NULL);
        close(dev);
    } else {
        __android_log_print(ANDROID_LOG_ERROR, "SegmentActivity", "Device Open ERROR!\n");
        exit(1);
    }
    return 0;
}
*/
