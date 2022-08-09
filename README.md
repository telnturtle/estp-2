# Embedded Systems Term Project #2

2017 fall semester Kyonggi University DD751 Embedded Systems term project #2

- Android application - other
- JNI and device driver - me

## logs

### 2017-11

로직을 D/D에 삽입해야겠다. 

세그먼트와 부저가 시간에 따라 각각 동작을 수행하는데, 동시에 이루어져야 한다.

### 2017-12-04
현재 부저 로직은 jni에 코드되어있다. 이걸 부저 D/D 드라이버로 함수 형태로 옮기자.

ioctl 또는 unlocked_ioctl? -> 리눅스 커널 버전을 알아야 하는데 지금은 알 수가 없네. -> ioctl로 먼저 작성하고 나중에 필요하면 바꾼다


```
buzzer D/D build log
root@hanback:/Android/drivers/248.buzzer_driver# make
make -C /Android/linux-2.6.32-hanback SUBDIRS=/Android/drivers/248.buzzer_driver modules
make[1]: Entering directory `/Android/linux-2.6.32-hanback'
  CC [M]  /Android/drivers/248.buzzer_driver/buzzer.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC      /Android/drivers/248.buzzer_driver/buzzer.mod.o
  LD [M]  /Android/drivers/248.buzzer_driver/buzzer.ko
make[1]: Leaving directory `/Android/linux-2.6.32-hanback'
rm -f default
root@hanback:/Android/drivers/248.buzzer_driver# 
```

아 설마 `segment_data`에다가 쓰면 `segment_grid`의 sel에 만 쓰이는건가 그래서 띄워놓기가 어렵다고 한 건가

```
buzzer의 file write의 값에다가...
  int의 포인터 넣고 length 1 <- 레퍼런스
  char의 포인터 넣고 length 1 
  위에걸로 하자
```


```
JNIEXPORT void Java_ac_kr_kgu_esproject_ArrayAdderActivity_Clear (JNIEnv* env,
    jobject thiz, jint cmd)
```
    에서 cmd는 아무 데도 안 쓰지만 다시 빌드하기 싫어서 냅둠.

### 2017-12-18

#### 14:38
```
JNIEXPORT void JNICALL Java_ac_kr_kgu_esproject_ArrayAdderActivity_NativeSum (JNIEnv* env,
                                    jobject thiz, jintArray jarray, jint jsum)
```

```
JNIEXPORT void Java_ac_kr_kgu_esproject_ArrayAdderActivity_NativeSum (JNIEnv* env,
                                    jobject thiz, jintArray jarray, jint jsum)
```

`JNICALL` 유무에 따라 달라질까? 한백 예제 코드에는 없었고 JNI 레퍼런스엔 있었음

#### 15:59
`eomhy_estp_20171130_1.c` 와 `jni_nosegment.c`는 A,B 버전인데, `jni_nosegment`의 수정을 `eomhy`가 따라가야 한다

#### 16:41
부저 D/D는 단순 전달;    세그먼트 D/D는 write로 테스트 후 ioctl로 변경

JNI에 부저와 세그먼트 컨트롤 로직 담음

안드로이드엔 2 스레드와 플래그를 담고, 플래그에 따라서 주기마다 JNI 함수 호출; 주기 1.5초

...

안됨 왜냐하면 JNI 함수 하나만 부르기 때문

## cf.

Android 프로젝트 생성 시 사용값
- ① Project 이름: ESTermProject
- ② Application 이름: ArrayAdder
- ③ Package 이름: ac.kr.kgu.esproject
- ④ Activity 이름: ArrayAdderActivity

jni의 함수 이름은 결정되어있음

안드로이드의 함수 이름은 미정

jni의 함수 이름 작명법
```
jint Java_com_hanback_segment_SegmentActivity_SegmentControl (JNIEnv* env, jobject thiz, jint data )
ret_type Java_package_Activity_funcOfAndroid (fixed arg, fixed arg, real arg)

```

오리지날은 세그먼트만 이용하지만 텀프는 세그먼트에 추가로 부저도
사용한다. 파일 오픈에 추가

함수 jint Java_ac_kr_kgu_esproject_ArrayAdderActivity_fn1의 리턴 타입은
jint와 void가 될 수 있다. 일단 void로 ㄱ

### references
Java Native Interface Specification Contents    https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/jniTOC.html

http://rtos.kyonggi.ac.kr/rtos/lecture/2017-2/es/lecture_notes/JNI.pdf

JNI programming - (5) 배열 다루기 2    http://forum.falinux.com/zbxe/index.php?document_srl=571214&mid=lecture_tip

JNI, java code와 native code 간에 배열을 주고받고 싶습니다.    https://www.androidpub.com/794987

[JNI/NDK] JNI에서 기본형 배열을 다루는 예    http://rockdrumy.tistory.com/1008

