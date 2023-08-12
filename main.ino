
#include<SoftwareSerial.h>

SoftwareSerial uhf(6, 5);
SoftwareSerial myGsm(2,3); //(10, 11); //(2, 3); //sim tx rx

int u, c, counter = 1, tagID_print_delay = 300;
String string, n, power_string, final_string, multi_read_stop_string, pc, epc;
char INFO[50];

void setup() {
  Serial.begin(115200);
  uhf.begin(115200);
  myGsm.begin(9600);
 // pinMode(9, OUTPUT);
 // delay(100);
 Serial.println("shuru");
  delay(10000);
  myGsm.listen();
  sim_Init();
  delay(100);
  u = 'B';
}

void loop() {
  //delay(2000);

  uhf.listen();
  // get_Location();
  // data_Send(Lat, Long);
  // delay(5000);

  set_mode();
  read_data();
  string_compare();
  if (pc == "3000")
  {
   myGsm.listen();
    Serial.println("mil gya");
    analogWrite(9,255);
    delay(1000);
    Serial.println(epc);
    analogWrite(9,0);
    delay(500);
    data_Send(epc);
    delay(4000);
    pc = "";
    // while(1);
  }
 // analogWrite(9,0);
   // delay(500);
  //while(1);
}

void read_data()
{
  while (uhf.available())
  {
    c = uhf.read();
    n = String(c, HEX);
    n.toUpperCase();
    //Serial.print(c);
    //Serial.print(c,HEX);
    //Serial.print((char)c);

    if (n.length() < 2)
    {
      n = ("0" + n);
    }
    string += n;
    //Serial.print(string);
    //string= "";
  }
  while (Serial.available())
  {
    u = Serial.read();
  }
}

void set_mode()
{
  if (u == 'A')
  {
    hardware_info();
  }
  if (u == 'B')
  {
    multi_polling();
  }
  if (u == 'C')
  {
    stop_multi_polling();
  }
}

void hardware_info()
{
  Serial.println("");
  Serial.print("Harware Info  --  ");
  uint8_t message[] = {0xBB, 0x00, 0x03, 0x00, 0x01, 0x00, 0x04, 0x7E};
  uhf.write(message, sizeof(message));
  uhf.flush();
  u = 0;
  //delay(10);
}

void multi_polling()
{
  Serial.println("");
  Serial.println("Multi-Read started");
  uint8_t message[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x27, 0x10, 0x83, 0x7E};
  uhf.write(message, sizeof(message));
  uhf.flush();
  u = 0;
  delay(200);
}

void stop_multi_polling()
{
  //Serial.println("");
  //Serial.println("Stopping-Multi-Read");
  uint8_t message[] = {0xBB, 0x00, 0x28, 0x00, 0x00, 0x28, 0x7E};
  for (int i = 0; i <= 600; i++)
  { uhf.write(message, sizeof(message));
    uhf.flush();
  }
  u = 0;
  delay(1000);
}

void string_compare()
{

  power_string = string.substring(0, 20);
  multi_read_stop_string = string.substring(0, 16);

  if ((string.length() > 48) && (power_string == "FFFFFFFFFFFFFFFFFFFF"))
  {
    Serial.println("");
    Serial.println("Please power the UHF");
    delay(500);
  }
  if ((multi_read_stop_string == "BB01280001002A7E") || (multi_read_stop_string == "BB01280001002B7E") || (multi_read_stop_string == "BB01FF00115167E") || (multi_read_stop_string == "BB01FF000115167EBB01280001002A7E") || (multi_read_stop_string == "BB01FF000115167EBB01280001002A7F") || (multi_read_stop_string == "BB012400C0002A7E") || (multi_read_stop_string == "BB01FF000114167EBB01280001002A7E") || (multi_read_stop_string == "BB0128000100357E") || (multi_read_stop_string == "BB012800C000297E"))
  {
    Serial.println("");
    Serial.println("Multi-Read stopped");
    delay(1000);
    string = "";
    string.length() < 0;
  }

  if ((string.length() > 48) && (power_string != "FFFFFFFFFFFFFFFFFFFF"))
  {

    final_string = string.substring(0, 48);
    if (final_string.length() == 48)
    {
      epc = final_string.substring(16, 40);
      pc = final_string.substring(12, 16);

      if (pc == "3000")
      {
        Serial.println("");
        Serial.println("Assset found -- ");
        Serial.println("Asset ID = " + epc);
       // analogWrite(9, 255);
       // delay(200);
      }
      //+=[;pppppp.lp-analogWrite(9, 0);
      delay(tagID_print_delay);
      string = "";
      string.length() < 0;
    }
  }

  if (string == "BB01030010004D31303020323664426D2056312E30927E")
  {
    //d = readString,HEX;
    //Serial.println("");
    string = string.substring(12, 42);
    string.toCharArray(INFO , 50);
    //INFO = ((char)INFO);
    Serial.print(INFO);
    Serial.println("");
    // Serial.println("Hardware is");
    delay(100);
    string = "";
    string.length() < 0;
  }


}

void data_Send(String x)
{
  myGsm.println("AT+HTTPINIT");
  delay(2000);
  PrintSerialData();

  // Set value
  // https://dweet.io/dweet/for/godfather?testvar=69
  
  String url = "AT+HTTPPARA=\"URL\",\"api key";
  url += x;
  url += "/detect/";
  // url+="&longitude=";
  // url+=y;
  url += "\"";
  myGsm.println(url);
  //https://api.robogyan.tech/assets/864764033769049/update_location/?latitude=34.0&longitude=69.0322
  //http://dweet.io/dweet/for/godfather?testvar=77
  //  myGsm.println("AT+HTTPPARA=\"URL\",\"http://dweet.io/dweet/for/godfather?testvar=E20000196504014225906E8B\"");

  delay(4000);
  PrintSerialData();

  // set http action type 0 = GET
  myGsm.println("AT+HTTPACTION=0");
  delay(6000);
  PrintSerialData();

  // read server response
  myGsm.println("AT+HTTPREAD");
  delay(1000);
  PrintSerialData();

  //myGsm.println("");
  myGsm.println("AT+HTTPTERM");
  PrintSerialData();
  delay(300);

  myGsm.println("");


}

void sim_Init()
{
  Serial.println("Connection");
  delay(2000);
  Serial.println("Complete!...");
  myGsm.flush();
  Serial.flush();


  myGsm.println("AT+SAPBR =0,1");
  delay(1000);
  PrintSerialData();

  myGsm.println("AT+CIPSHUT");
  delay(1000);
  PrintSerialData();
  // Cheking to see if gprs is attached
  myGsm.println("AT+CGATT?");
  delay(1000);
  PrintSerialData();

  // Check signal strength
  myGsm.println("AT+CSQ ");
  delay(1000);
  PrintSerialData();

  // Set connection type to GPRS
  myGsm.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  delay(2000);
  PrintSerialData();

  // Set the APN
  myGsm.println("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\"");
  delay(2000);
  PrintSerialData();

  // Enable GPRS
  myGsm.println("AT+SAPBR=1,1");
  delay(3000);
  PrintSerialData();

  // Check to see if connection is correct and get your IP address
  myGsm.println("AT+SAPBR=2,1");
  delay(2000);
  PrintSerialData();
}

void PrintSerialData()
{
  while (myGsm.available() != 0)
  {
    Serial.write(myGsm.read());
  }
}



