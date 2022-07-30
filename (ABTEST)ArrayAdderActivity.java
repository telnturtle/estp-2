쓰레드를 해보려고 한 버전인데 그냥 포기함 ww 일단 백업만 해둔다 그리고 .java에서 쓰레드 추가한거 다 지워야함

package ac.kr.kgu.esproject;

import java.util.Random;

//import com.hanback.segment.SegmentActivity;
//import com.hanback.segment.SegmentActivity.BackThread;

import android.app.Activity;
import android.app.Dialog;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.text.format.Time;
import android.widget.TextView;

public class ArrayAdderActivity extends Activity {
    /** Called when the activity is first created. */
	Random r = new Random();
	
	BackSegThread thread_segment = new BackSegThread(); // 세그먼트 출력에 사용할 스레드
	BackBuzThread thread_buzzer = new BackBuzThread(); // 부저 출력에 사용할 스레드
	protected static final int THREAD_FLAGS_PRINT = 0; // 기존 카운터
	protected static final int THREAD_FLAGS_CLOCK = 1; // 시스템 시간
	protected static final int THREAD_FLAGS_WAIT = 2;
	protected static final int THREAD_FLAGS_BEEP = 1; // 부저 온
	protected static final int DIALOG_SIMPLE_MESSAGE = 1;
	int flag_segment = -1; // 스레드 기능의 시작을 알리는 플래그
	int flag_buzzer = -1; // 스레드 기능의 시작을 알리는 플래그
	boolean stop = false; // 스레드 기능의 종료를 알리는 플래그
	int count = 0; // 사용자 입력을 받는 변수
	
    static int BuzData = 0;
    static int result = 0;
    int arr[];
    static int arrlength = 0;
    static int sum = 0;
    static int CompareResult;
    static int buzdata;
    static final int FLAG_PRINT = 0;
    static final int FLAG_WAIT = 1;
    Button On, Off, createArr;
    Spinner spinner;
    TextView showArr, showResult;
    EditText UserInput;
    String str = "";
	
    public native void NativeSum(int arr[], int value);
    public native void Clear(int cmd);
	   
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        System.loadLibrary("buzzer");
        
		// 스레드 시작
		thread_segment.setDaemon(true);
		thread_segment.start();
		thread_buzzer.setDaemon(true);
		thread_buzzer.start();
        
        spinner = (Spinner) findViewById(R.id.Spinner);
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource	(this, R.array.number, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinner.setAdapter(adapter);
        createArr = (Button) findViewById(R.id.ArrayCreate);
        showArr = (TextView) findViewById(R.id.ArrayValue);
        showArr.setScroller(null);
        UserInput = (EditText) findViewById(R.id.input);        
        On = (Button) findViewById(R.id.OK);
        Off = (Button) findViewById(R.id.Clear);
        showResult = (TextView) findViewById(R.id.ResultValue);
        // 해당 버튼 클릭하면 어레이를 생성함
        createArr.setOnClickListener(new Button.OnClickListener(){
            public void onClick(View v){
            	// 배열을 만드어서 배열 요소 출력하기
               str = "";
               showArr.setText(str);
               sum = 0;
               Object user_obj = spinner.getSelectedItem();
               String user_str = (String) user_obj;
               int user = Integer.parseInt(user_str);
               arr = new int[user];
               
               for(int i = 0; i < user; i++){
                  arr[i] = r.nextInt(9);
                  sum += arr[i];
                  str += "배열 요소#" + (i+1) + " : " + arr[i] + "\n";
               }
               showArr.setText(str);
            }
         });
        // 해당 OK 버튼 클릭하면 JNI 함수를 호출함 
        On.setOnClickListener(new Button.OnClickListener(){
           public void onClick(View v){
        	   // 
        	  CompareResult = Integer.parseInt(UserInput.getText().toString());
              if(CompareResult == sum){
                 showResult.setText("0");
//                 ArrayAdderBuzzerControl(arr,CompareResult);
//                 ArrayAdderSegmentIOControl(FLAG_PRINT);
//                 ArrayAdderSegmentControl(arr,CompareResult);
//                 ArrayAdderSegmentIOControl(FLAG_WAIT);
//                 ArrayAdderSegmentControl(arr, CompareResult);
                 NativeSum(arr, CompareResult);
              }
              else{
                 showResult.setText("X");
//                 ArrayAdderBuzzerControl(arr,CompareResult);
//                 ArrayAdderSegmentIOControl(FLAG_PRINT);
//                 ArrayAdderSegmentControl(arr,CompareResult);
//                 ArrayAdderSegmentIOControl(FLAG_WAIT);
//                 ArrayAdderSegmentControl(arr, CompareResult);
                 NativeSum(arr, CompareResult);
              }   
           
           }
        });
        // 해당 클리어 버튼을 클릭하면 클리어함
        Off.setOnClickListener(new Button.OnClickListener(){
           public void onClick(View V){
              for(int i = 0; i<arr.length; i++)
                 arr[i]=0;
              sum = 0;
              showArr.setText(" ");
              showResult.setText(" ");
//              ArrayAdderSegmentIOControl(FLAG_WAIT);  
//              ArrayAdderSegmentControl(arr,CompareResult);
              SegmentIOControl(THREAD_FLAGS_WAIT);
              Clear(0);
              }
           });
     }
    
	// 세그 출력을 담당하는 스레드 클래스                                                                                                                                                                      
	class BackSegThread extends Thread {
		public void run() {
			while (!stop) {
				switch (flag_segment) {
				default:
					// 아무일도 하지 않음
					break;
				case THREAD_FLAGS_PRINT:
				   // 카운터 출력
					SegmentIOControl(THREAD_FLAGS_PRINT);
					while (count > 0 && flag == THREAD_FLAGS_PRINT) {
						for (int i = 0; i < 14 && flag == THREAD_FLAGS_PRINT; i++) {
							SegmentControl(count);
						}
						count--;
					}
					// flag = 0;
					break;

				case THREAD_FLAGS_CLOCK:
					// 안드로이드 시간 출력 (시,분,초)
					SegmentIOControl(THREAD_FLAGS_CLOCK);					int result = 0;
					// 시간 받아오기
					Time t = new Time();
					t.set(System.currentTimeMillis());
					// 포맷 결정
					result = t.hour * 10000 + t.minute * 100 + t.second;
					// 백만의 자릿수를 플래그로 사용
					result += 1000000;
					for (int i = 0; i < 20; i++)
						SegmentControl(result);
					break;
				}
			}
		}
	}
	
	// 부저 출력을 담당하는 스레드 클래스                                                                                                                                                                      
	class BackBuzThread extends Thread {
		public void run() {
			while (!stop) {
				switch (flag) {
				default:
					// 아무일도 하지 않음
					break;
				case THREAD_FLAGS_PRINT:
				   // 카운터 출력
					SegmentIOControl(THREAD_FLAGS_PRINT);
					while (count > 0 && flag == THREAD_FLAGS_PRINT) {
						for (int i = 0; i < 14 && flag == THREAD_FLAGS_PRINT; i++) {
							SegmentControl(count);
						}
						count--;
					}
					// flag = 0;
					break;

				case THREAD_FLAGS_CLOCK:
					// 안드로이드 시간 출력 (시,분,초)
					SegmentIOControl(THREAD_FLAGS_CLOCK);					int result = 0;
					// 시간 받아오기
					Time t = new Time();
					t.set(System.currentTimeMillis());
					// 포맷 결정
					result = t.hour * 10000 + t.minute * 100 + t.second;
					// 백만의 자릿수를 플래그로 사용
					result += 1000000;
					for (int i = 0; i < 20; i++)
						SegmentControl(result);
					break;
				}
			}
		}
	}

	// 키 제어. BACK 버튼 눌려 프로그램 종료 시 스레드도 종료 시킴
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			flag = -1;
			stop = true;
			thread.interrupt();
		}
		return super.onKeyDown(keyCode, event);
	}

	// 문자 입력 예외 메시지 처리
	@Override
	protected Dialog onCreateDialog(int id) {
		// TODO Auto-generated method stub
		Dialog d = new Dialog(SegmentActivity.this);
		Window window = d.getWindow();
		window.setFlags(WindowManager.LayoutParams.FIRST_APPLICATION_WINDOW,
				WindowManager.LayoutParams.FIRST_APPLICATION_WINDOW);

		switch (id) {
		case DIALOG_SIMPLE_MESSAGE:
			d.setTitle("입력 가능한 문자의 길이는 6자까지 입니다.");
			d.show();
			return d;
		}
		return super.onCreateDialog(id);
	}
    
    

}