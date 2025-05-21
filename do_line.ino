float kp=60;   // Hệ số tỉ lệ
float ki=0;     // Hệ số tích phân (đặt bằng 0 vì không sử dụng I)
float kd=6;     // Hệ số vi phân
float ronaldo=0; // Giá trị lệch hiện tại (tên biến độc đáo)
float ronaldo_2008=0; // Giá trị lệch trước đó
unsigned long thoi_gian=0; // Thời gian hiện tại
unsigned long thoi_gian_cu=0; // Thời gian trước đó
float tong=0;   // Tổng giá trị PID
float tich_phan=0; // Tích phân
float dao_ham=0; // Đạo hàm
float P=0;      // Thành phần P
float I=0;      // Thành phần I
float D=0;      // Thành phần D
float dt=0;     // Khoảng thời gian delta

#define InA 5
#define InB 6
#define InC 7
#define InD 8
#define ENA 3
#define ENB 9

#define cam_bien_1 A0
#define cam_bien_2 A1
#define cam_bien_3 A2
#define cam_bien_4 A3
#define cam_bien_5 A4

void tinh_pid(){
  thoi_gian=millis();
  dt=(thoi_gian-thoi_gian_cu)/1000.0; // Tính delta time (giây)

  P=kp*ronaldo; // Thành phần tỉ lệ

  tich_phan+=ronaldo*dt; // Tích phân sai số
  I=ki*tich_phan; // Thành phần tích phân

  dao_ham=(ronaldo-ronaldo_2008)/dt; // Đạo hàm sai số
  D=kd*dao_ham; // Thành phần vi phân

  tong=P+I+D; // Tổng hợp 3 thành phần

  ronaldo_2008=ronaldo; // Lưu giá trị lệch hiện tại cho lần sau
  thoi_gian_cu=thoi_gian; // Cập nhật thời gian
}

void dieu_khien_motor(){
  int toc_do_co_ban=255; // Tốc độ cơ bản
  int toc_do_trai=toc_do_co_ban+tong; // Motor trái = tốc độ cơ bản + giá trị PID
  int toc_do_phai=toc_do_co_ban-tong; // Motor phải = tốc độ cơ bản - giá trị PID

  // Giới hạn tốc độ trong khoảng 0-255
  toc_do_trai=constrain(toc_do_trai,0, 255);
  toc_do_phai=constrain(toc_do_phai,0, 255);

  // Điều khiển motor
  analogWrite(ENA, toc_do_trai);
  analogWrite(ENB, toc_do_phai);

  // Đặt chiều quay (tiến)
digitalWrite(5, HIGH);  // InA
digitalWrite(6, LOW);   // InB (motor trái quay tiến)
digitalWrite(7, HIGH);  // InC
digitalWrite(8, LOW);   // InD (motor phải quay tiến)
}

void do_line(){
  int S1= analogRead(cam_bien_1);
  int S2= analogRead(cam_bien_2);
  int S3= analogRead(cam_bien_3);
  int S4= analogRead(cam_bien_4);
  int S5= analogRead(cam_bien_5);

if (S1 > 500 && S2 > 500 && S3 > 500 && S4 < 500 && S5 < 500) {
  ronaldo = 5; //lệch phải nhiều
}
else if (S1 > 500 && S2 > 500 && S3 > 500 && S4 > 500 && S5 < 500) {
  ronaldo = 4; //lệch phải vừa
}
else if (S1 > 500 && S2 > 500 && S3 > 500 && S4 > 500 && S5 < 500) {
  ronaldo = 3;
}
else if (S1 > 500 && S2 > 500 && S3 > 500 && S4 < 500 && S5 < 500) {
  ronaldo = 2;
}
else if (S1 > 500 && S2 > 500 && S3 < 500 && S4 < 500 && S5 < 500) {
  ronaldo = 1;
}
else if (S1 > 500 && S2 < 500 && S3 < 500 && S4 < 500 && S5 < 500) {
  ronaldo = 0;
}
else if (S1 < 500 && S2 < 500 && S3 < 500 && S4 < 500 && S5 > 500) {
  ronaldo = -1;
}
else if (S1 < 500 && S2 < 500 && S3 < 500 && S4 > 500 && S5 > 500) {
  ronaldo = -2;
}
else if (S1 < 500 && S2 < 500 && S3 > 500 && S4 > 500 && S5 > 500) {
  ronaldo = -3;
}
else if (S1 < 500 && S2 > 500 && S3 > 500 && S4 > 500 && S5 > 500) {
  ronaldo = -4; //lệch trái vừa 
}
else if (S1 < 500 && S2 < 500 && S3 < 500 && S4 > 500 && S5 > 500) {
  ronaldo = -5; //lệch trái nhiều 
}
else {
  ronaldo = ronaldo_2008; //giữ nguyên giá trị nếu không phát hiện line
}

}
void setup() {
  pinMode(InA, OUTPUT);  // Chân điều khiển motor A1
  pinMode(InB, OUTPUT);  // Chân điều khiển motor A2  
  pinMode(InC, OUTPUT);  // Chân điều khiển motor B1 (Lưu ý: Inc nên là InC)
  pinMode(InD, OUTPUT);  // Chân điều khiển motor B2
  pinMode(ENA, OUTPUT);  // PWM điều khiển tốc độ motor A
  pinMode(ENB, OUTPUT);  // PWM điều khiển tốc độ motor B

  pinMode(cam_bien_1, INPUT);  // Cảm biến line 1 (trái nhất)
  pinMode(cam_bien_2, INPUT);  // Cảm biến line 2
  pinMode(cam_bien_3, INPUT);  // Cảm biến line 3 (giữa)
  pinMode(cam_bien_4, INPUT);  // Cảm biến line 4
  pinMode(cam_bien_5, INPUT);  // Cảm biến line 5 (phải nhất)
  
  Serial.begin(9600);

  di_thang();
  re_trai();
  re_phai();
  dung();
  do_line();
  tinh_pid();
  dieu_khien_motor();
}

void loop() {
do_line();
tinh_pid();
dieu_khien_motor();
}