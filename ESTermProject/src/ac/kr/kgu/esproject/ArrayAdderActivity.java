package ac.kr.kgu.esproject;

import java.util.Random;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;

public class ArrayAdderActivity extends Activity {
    /** Called when the activity is first created. */
	static{ System.loadLibrary("buzzer"); }
    public native void NativeSum(int Jarray[], int value);
	public native void Clear(int cmd);
	   
	   int Jarray[];
	   static int Jarraylength = 0;
	   static int sum = 0;
	   static int result = 0;
	   static int ArrResult;
	   String str = "";
	   Button On, Off, createJarray;
	   Spinner spinner;
	   TextView showJarray, showResult;
	   EditText Input;
	 
	   
	   Random r = new Random();
    @Override
    
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        
        spinner = (Spinner) findViewById(R.id.Spinner);
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource	(this, R.array.number, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinner.setAdapter(adapter);
        
        showResult = (TextView) findViewById(R.id.ResultValue);
        createJarray = (Button) findViewById(R.id.ArrCreate);
        showJarray = (TextView) findViewById(R.id.ArrayValue);
        showJarray.setScroller(null);
       
        
        createJarray.setOnClickListener(new Button.OnClickListener(){
            public void onClick(View v){
               str = "";
               showJarray.setText(str);
               sum = 0;
               Object obj = spinner.getSelectedItem();
               String uStr = (String) obj;
               int inputvalue = Integer.parseInt(uStr);
               Jarray = new int[inputvalue];
               
               for(int i = 0; i < inputvalue; i++){
                  Jarray[i] = r.nextInt(10)-1;
                  sum += Jarray[i];
                  str += "배열 요소#" + (i+1) + " : " + Jarray[i] + "\n";
               }
               showJarray.setText(str);
            }
         });
        Input = (EditText) findViewById(R.id.input);
        
        On = (Button) findViewById(R.id.OK);
        On.setOnClickListener(new Button.OnClickListener(){
           public void onClick(View v){
              ArrResult = Integer.parseInt(Input.getText().toString());
              
              if(ArrResult == sum){
                 showResult.setText("right");
          
//                 JarrayayAdderBuzzerControl(Jarray,ArrResult);
//                 JarrayayAdderSegmentIOControl(FLAG_PRINT);
//                 JarrayayAdderSegmentControl(Jarray,ArrResult);
//                 JarrayayAdderSegmentIOControl(FLAG_WAIT);
//                 JarrayayAdderSegmentControl(Jarray, ArrResult);
                 NativeSum(Jarray, ArrResult);
              }
              else{
                 showResult.setText("wrong");
              
//                 JarrayayAdderBuzzerControl(Jarray,ArrResult);
//                 JarrayayAdderSegmentIOControl(FLAG_PRINT);
//                 JarrayayAdderSegmentControl(Jarray,ArrResult);
//                 JarrayayAdderSegmentIOControl(FLAG_WAIT);
//                 JarrayayAdderSegmentControl(Jarray, ArrResult);
                 NativeSum(Jarray, ArrResult);
              }   
              
           }
        });
        Off = (Button) findViewById(R.id.Clear);
        Off.setOnClickListener(new Button.OnClickListener(){
           
           public void onClick(View V){
              for(int i = 0; i<Jarray.length; i++)
                 Jarray[i]=0;
              sum = 0;
              showJarray.setText(" ");
              showResult.setText(" ");
//              JarrayayAdderSegmentIOControl(FLAG_WAIT);  
//              JarrayayAdderSegmentControl(Jarray,ArrResult);
              Clear(0);
              }
           });
     }
  
    
}