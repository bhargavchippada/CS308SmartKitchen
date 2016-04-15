/*
 *
 * TeamId :     #Don't Know
 *
 * Author List:	Bhargav Chippada  (120050053)
 * 				Akhil Surya Vamshi  (120050067)
 *              Pranay Dhondi   (120050054)
 *              Jagadeesh Nelaturu (120050055)
 *
 * Filename:    main.cc
 *
 * Theme:     None
 *
 * Functions:   usage(),tagno(string),validate(),differences(string),
                calories(string),preprocess(),write_to_file(char*, char*, char*),
                spi_chip_enable(uint8_t),read_weight(),HW_init(uint32_t, uint8_t),
 *              debug_msg(int, char *),chip_reader(uint8_t),main(int, char**)
 * Global Variables: debug,weight,output
 *
*/

//Including Headers
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <syslog.h>
#include <sys/wait.h>
#include <signal.h>
#include "rfid.h"
#include "bcm2835.h"
#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <fstream>

using namespace std;

/* BAUDRATE OF USB */
#define BAUDRATE 38400

/* change this definition for the correct port */
#define MODEMDEVICE "/dev/ttyACM0"


/* POSIX compliant source */
#define _POSIX_SOURCE 1 

/* RFIDS used */
#define TAG1 "ce766604"
#define TAG2 "33b87a25"
#define TAG3 "12217b25"
#define TAG4 "8cf94578"


/* Defination of Global Variables goes here */
 uint8_t debug=0;
 char weight[255];
 char output[64];
/*End of Global Variables*/


/*

* Function Name: usage()

* Input: none

* Output: none

* Logic: Gives the instructions how to run script

* Example Call: usage();

*/

void usage(char * str) {
  printf("Usage:\n%s [options]\n\nOptions:\n -d\t Debug mode\n\n",str);
}

/*

* Function Name: tagno(string)

* Input: string of tagid

* Output: integer mapped to the given string(tag id) if nothing matches it return -1;

* Logic: Compares with all tagids available and returns its corresponding map value.

* Example Call: tagno("ce766604");

*/

int tagno(string tagid){
  if(tagid == TAG1)return 2;
  else if(tagid == TAG2)return 3;
  else if(tagid == TAG3)return 0;
  else if(tagid == TAG4)return 1;
  else return -1;
}

/*

* Function Name: validate()

* Input: none

* Output: Logs from load sensor is split each individual containers consumption

* Logic: It processes the logged values by the load sensor, compares the tagid logged
          and uses simple logic and splits the data

* Example Call: validate();

*/

int validate() {
  std::string timestamp, weight, tag1, tag2; 
  std::string timestamp_prev = "", weight_prev, tag1_prev, tag2_prev;
  std::ifstream fin("out.txt"); // file descripter is created for the load cell logged files
  std::ofstream container1("smart_kitchen/container1.tsv", std::ofstream::out), container2("smart_kitchen/container2.tsv", std::ofstream::out),
  container3("smart_kitchen/container3.tsv", std::ofstream::out), container4("smart_kitchen/container4.tsv", std::ofstream::out);
  // file descripter is created for each of individual containers 
  container1 << "date\tclose\n";
  container2 << "date\tclose\n";
  container3 << "date\tclose\n";
  container4 << "date\tclose\n";
  int count = 0;
  std::string temp;
  while(fin >> temp) {
  //std::cout << count << '\n';
  //std::cout << temp << '\n';
    switch(count) { // reading each word for a particual life if word 0 then it is timestamp, 1 for weight ,2 for tag1, 3 for tag2
      case 0:
      timestamp = temp;
      case 1:
      weight = temp;
      case 2:
      tag1 = temp;
      case 3:
      tag2 = temp;
    }
    ++count;
    if(count == 4) {
      //finally when line is read
      /*
        Logic here is :-
          if weight of both container is W and if one of container is w1 then the weight of taken container is W-w1
          if weight of one container is w1 and other container is placed and it weighs W then the weight of added container is
          W-w1
      */
        std::cout << timestamp << " $ " << weight << " $ " << tag1 << " $ " << tag2 << '\n';
        if(timestamp_prev == "") { 
          if(tag1 == "NOTAG") {
            switch(tagno(tag2)) {
              case 0 :
              container1 << timestamp+"\t"+weight+'\n';
              cout << "Writing to file\n";
              break;
              case 1 :
              container2 << timestamp+"\t"+weight+'\n';
              cout << "Writing to file\n";
              break;
              case 2 :
              container3 << timestamp+"\t"+weight+'\n';
              cout << "Writing to file\n";
              break;
              case 3 :
              container4 << timestamp+"\t"+weight+'\n';
              cout << "Writing to file\n";
              break;
              default :
              cout<<"Error:Unknown RFID Found(-32)\n";
              break;
            }
          }
        }
        else if(tag2 == "NOTAG") {
          switch(tagno(tag1)){
            case 0 :
            container1 << timestamp+"\t"+weight+'\n';
            cout << "Writing to file\n";
            break;
            case 1 :
            container2 << timestamp+"\t"+weight+'\n';
            cout << "Writing to file\n";
            break;
            case 2 :
            container3 << timestamp+"\t"+weight+'\n';
            cout << "Writing to file\n";
            break;
            case 3 :
            container4 << timestamp+"\t"+weight+'\n';
            cout << "Writing to file\n";
            break;
            default :
            cout<<"Error:Unknown RFID Found(-32)\n";
            cout << "Writing to file\n";
            break;
          }
        }

        if(tag1 == tag1_prev) {

          if(tag2 == "NOTAG") {
    // weight of tag2_prev is weight_prev - weight
            double diffWeight = atof(weight_prev.c_str()) - atof(weight.c_str());
            switch(tagno(tag2_prev)) { 
              case 0 :
              container1 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              case 1 :
              container2 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              case 2 :
              container3 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              case 3 :
              container4 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              default :
              cout<<"Error:Unknown RFID Found(-32)\n";
              break;
            }
            switch(tagno(tag1)){
              case 0 :
              container1 << timestamp+"\t"+weight+'\n';
              break;
              case 1 :
              container2 << timestamp+"\t"+weight+'\n';
              break;
              case 2 :
              container3 << timestamp+"\t"+weight+'\n';
              break;
              case 3 :
              container4 << timestamp+"\t"+weight+'\n';
              break;
              default :
              cout<<"Error:Unknown RFID Found(-32)\n";
              break;
            }
          }
          else if(tag2_prev == "NOTAG") {
    // weight of tag2 is weight - weight_prev
            double diffWeight = atof(weight.c_str()) - atof(weight_prev.c_str());
            switch(tagno(tag2)){
              case 0 :
              container1 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              case 1 :
              container2 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              case 2 :
              container3 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              case 3 :
              container4 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              default :
              cout<<"Error:Unknown RFID Found(-32)\n";
              break;
            }
          }
        }
        else if(tag2 == tag2_prev) {
          if(tag1 == "NOTAG") {
    // weight of tag1_prev is weight_prev - weight
            double diffWeight = atof(weight_prev.c_str()) - atof(weight.c_str());
            switch(tagno(tag1_prev)){
              case 0 :
              container1 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              case 1 :
              container2 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              case 2 :
              container3 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              case 3 :
              container4 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              default :
              cout<<"Error:Unknown RFID Found(-32)\n";
              break;
            }
            switch(tagno(tag2)){
              case 0 :
              container1 << timestamp+"\t"+weight+'\n';
              break;
              case 1 :
              container2 << timestamp+"\t"+weight+'\n';
              break;
              case 2 :
              container3 << timestamp+"\t"+weight+'\n';
              break;
              case 3 :
              container4 << timestamp+"\t"+weight+'\n';
              break;
              default :
              cout<<"Error:Unknown RFID Found(-32)\n";
              break;
            }
          }
          else if(tag1_prev == "NOTAG") {
    // weight of tag1 is weight - weight_prev
            double diffWeight = atof(weight.c_str()) - atof(weight_prev.c_str());
            switch(tagno(tag1)){
              case 0 :
              container1 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              case 1 :
              container2 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              case 2 :
              container3 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              case 3 :
              container4 << timestamp+"\t"+to_string(diffWeight)+'\n';
              break;
              default :
              cout<<"Error:Unknown RFID Found(-32)\n";
              break;
            }
          }
        }
      /*
        Here the values are loaded into previous values so that helps in finding next values and count to 0
      */
        timestamp_prev = timestamp;
        weight_prev = weight;
        tag1_prev = tag1;
        tag2_prev = tag2;
        count = 0;
      }
    }
  // closing file descriptors
    fin.close();
    container1.close();
    container2.close();
    container3.close();
    container4.close();
  }

/*

* Function Name: differences(string)

* Input: contaniner number

* Output: Comptutes the differnce/consumption in between time for given container

* Logic: It reads the file of a container and amount added/consumed is difference between previous
          and current value.

* Example Call: differences("1");

*/

int differences(string number) {
  // opening files of container and putting it in corresponding differnce file
  ifstream istr("smart_kitchen/container" + number + ".tsv");
  ofstream ostr("smart_kitchen/differences" + number + ".tsv");
  string temp;
  istr >> temp; // "date"
  istr >> temp; // "close"
  ostr << "date\tclose\n";
  string prev_weight;
  istr >> temp >>  prev_weight; // first data-point
  int count = 0;
  while(istr >> temp) {
    if(count == 0) {
      ostr << temp << '\t';
      count = 1;
    }
    else if(count == 1) {
      ostr << to_string(atof(temp.c_str()) - atof(prev_weight.c_str())) << '\n';
      prev_weight = temp;
      count = 0;
    }
  }
  istr.close();
  ostr.close();
}

/*

* Function Name: calories(string)

* Input: contaniner number

* Output: Comptutes the calories consumed in between time for given container

* Logic: It reads the file of a container and amount added/consumed is difference between previous
          and current value and multipies with  corresponding calorific value.

* Example Call: calories("1");

*/

int calories(string number) {
  ifstream istr("smart_kitchen/container" + number + ".tsv");
  ofstream ostr("smart_kitchen/calories" + number + ".tsv");
  string temp;
  istr >> temp; // "date"
  istr >> temp; // "close"
  ostr << "date\tclose\n";
  string prev_weight;
  istr >> temp >>  prev_weight; // first data-point
  int count = 0;
  while(istr >> temp) {
    if(count == 0) {
      ostr << temp << '\t';
      count = 1;
    }
    else if(count == 1) {
      ostr << to_string((atof(temp.c_str()) - atof(prev_weight.c_str()))*30) << '\n';
      prev_weight = temp;
      count = 0;
    }
  }
  istr.close();
  ostr.close();
}
/*

* Function Name: preprocess()

* Input: none

* Output: It separates files corresponding to each container values and computers the
          amount consumed and calories consumed

* Logic: First separate the values in different files and for each file computer the
         amount of calories and weight consumed

* Example Call: calories("1");

*/

void preprocess(){
  validate();
  for(int i = 1; i <= 4; ++i) {
    differences(to_string(i));
    calories(to_string(i));
  }
}

/*

* Function Name: write_to_file()

* Input: char* total weight, rfid-1, rfid-2

* Output: 0

* Logic: Write the total weight, rfid-1, rfid-2 to a file along with timestamp and call
* 		 preprocess function.

* Example Call: write_to_file("0.67","12217b25","8cf94578")

*/


int write_to_file(char *W, char *Tag1, char *Tag2){
  FILE *outfp;
  outfp = fopen("out.txt", "a");
  time_t now = time(0);
  tm *ltm = localtime(&now);
  fprintf(outfp,"%d-%d-%d-%d-%d-%d\t%s\t%s\t%s\n",ltm->tm_mday,1+ltm->tm_mon,1900+ltm->tm_year,ltm->tm_hour,ltm->tm_min,ltm->tm_sec,W,Tag1,Tag2);
  fclose(outfp);
  preprocess();
  return 0;
}

/*

* Function Name: read_weight()

* Input: None

* Output: 0 if successful else 1

* Logic: Read the total weight transmitted by tivaboard using usb on the port /dev/ttyACM0
* 		 generally.

* Example Call: read_weight()

*/

int read_weight(){
  int fd,c, res;
  struct termios oldtio,newtio;
  char buf[255];

  fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
  if (fd <0) {perror(MODEMDEVICE); return -1; }
  tcgetattr(fd,&oldtio); /* save current serial port settings */
  bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

  newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR | ICRNL;
  newtio.c_oflag = 0;
  newtio.c_lflag = ICANON;
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&newtio);
  res = read(fd,buf,255);
  buf[res]=0;             /* set end of string, so we can printf */

/* restore the old              port settings */
  tcsetattr(fd,TCSANOW,&oldtio);

  if(res>1){
    buf[res-1] = 0;
    sprintf(weight,"%s",buf);
    return 0;
  }else return 1;
}

/*

* Function Name: spi_chip_enable(uint8_t)

* Input: id of the chip enable pin

* Output: 0 if successful else 1

* Logic: selets the chip enable pin and sets the polarity low so that the required rfid
* 		 reader is enabled

* Example Call: spi_chip_enable(0)

*/

uint8_t spi_chip_enable(uint8_t chip_select){
  uint8_t dummy_data = 0x02;
  uint8_t data;
  switch(chip_select){
    case 0: // The Default
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    data = bcm2835_spi_transfer(dummy_data);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
    break;
    case 1:
    bcm2835_spi_chipSelect(BCM2835_SPI_CS1);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, LOW);
    data = bcm2835_spi_transfer(dummy_data);
    break;
    default:
    if(debug) cout<<"Invalid Chip Select Option!"<<endl;
    else syslog(LOG_DAEMON|LOG_ERR,"Invalid Chip Select Option!\n");
    return 1;
    break;
  }
  return 0;
}

/*

* Function Name: HW_init(uint32_t, uint8_t)

* Input: spi_speed and chip_select value

* Output: 0 if successful else 1

* Logic: initializes spi interface and the required rfid reader

* Example Call: HW_init(125000L,0)

*/

uint8_t HW_init(uint32_t spi_speed, uint8_t chip_select) {
  uint16_t sp;

  sp=(uint16_t)(250000L/spi_speed);
  if (!bcm2835_init()) {
    syslog(LOG_DAEMON|LOG_ERR,"Can't init bcm2835!\n");
    return 1;
  }

  bcm2835_spi_begin();
  bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
  bcm2835_spi_setClockDivider(sp); // The default
  spi_chip_enable(chip_select);
  return 0;
}

/*

* Function Name: debug_msg(int, char *)

* Input: number indicating rfid reader, msg to print

* Output: None

* Logic: Prints debug messages

* Example Call: debug_msg(0, "Random Error")

*/

void debug_msg(int num, char *msg){
  if (debug) {fprintf(stderr,"%d:%s\n",num, msg);}
  else{syslog(LOG_DAEMON|LOG_INFO,"%d:%s\n",num, msg);}
}
/*

* Function Name: chip_reader(uint8_t)

* Input: chip select value to indicate which rfid reader to enable

* Output: 0 if successful else 1

* Logic: Tries reading the rfid tag placed over the enabled rfid reader and assign 
* 		 char* output with rfid tag number or "NOTAG"

* Example Call: chip_reader(0)

*/
uint8_t chip_reader(uint8_t chip_select){

  sprintf(output,"NOTAG");

  if(spi_chip_enable(chip_select)) return 1;

  PcdReset();
  uint16_t CType=0;
  char status= find_tag(&CType);
  if (status==TAG_NOTAG) {debug_msg(chip_select, "NOTAG STATUS"); return 1;}
  else if ((status!=TAG_OK)&&(status!=TAG_COLLISION)) {debug_msg(chip_select, "!TAG_OK and !TAG_COLLISION STATUS"); return 1;}

  uint8_t SN_len=0;
  uint8_t SN[10];
  if (select_tag_sn(SN,&SN_len)!=TAG_OK) {debug_msg(chip_select, "NOT TAG_OK STATUS"); return 1;}

  char *p;
  char sn_str[21];
  p=sn_str;
  for (int tmp=0;tmp<SN_len;tmp++) {
    sprintf(p,"%02x",SN[tmp]);
    p+=2;
  }
  *p=0;

  if (debug) {fprintf(stderr,"Tag: type=%04x SNlen=%d SN=%s\n",CType,SN_len,sn_str);}
  else{syslog(LOG_DAEMON|LOG_INFO,"Tag: type=%04x SNlen=%d SN=%s\n",CType,SN_len,sn_str);}

  sprintf(output,"%s",&sn_str);

  PcdHalt();
  return 0;
}

/*
 * Function Name:- main(int, char**)
 *
 * Input: command line arguments
 *
 * Ouptut: 0
 *
 * Logic: Reads values from config file. Initializes spi hardware and rc522 reader.
 * 		  In the for loop it reads the weight transmitted by tiva board (load sensor is 
 * 		  connected to tiva), then reads the rfid tag placed over rfid reader-1, then 
 * 		  reads the rfid tag placed over rfid reader-2. Writes the values to a file if 
 * 		  there was any change in the rfid readings.
 *
 * Example Call:  called automatically by the OS
 *
*/


 int main(int argc, char *argv[]) {

  if (argc>1) {
    if (strcmp(argv[1],"-d")==0) {
      debug=1;
      cout<<"Debug mode:"<<endl;
    }else{
      usage(argv[0]);
      exit(1);
    }
  }

  if (open_config_file(config_file)!=0) {
    if (debug) {fprintf(stderr,"Can't open config file!");}
    else{syslog(LOG_DAEMON|LOG_ERR,"Can't open config file!");}
    return 1;
  }

  char str[255];
  uint32_t spi_speed=10000000L;
  if (find_config_param("SPI_SPEED=",str,sizeof(str)-1,1)==1) {
    spi_speed=(uint32_t)strtoul(str,NULL,10);
    if (spi_speed>125000L) spi_speed=125000L;
    if (spi_speed<4) spi_speed=4;
  }

  if (HW_init(spi_speed,0)) return 1;
  uid_t uid;
//if (read_conf_uid(&uid)!=0) return 1;
//setuid(0);
  InitRc522();

  char oldtag1[64], oldtag2[64];
  sprintf(oldtag1,"NOTAG");
  sprintf(oldtag2,"NOTAG");

  for (;;) {
    if(read_weight()) {continue;}
    char data1[64],data2[64],data[256];

    if(chip_reader(0)) usleep(200000);
    sprintf(data1,"%s",output);

    if(chip_reader(1)) usleep(200000);
    sprintf(data2,"%s",output);
    if(strcmp(oldtag1,data1)!=0 || strcmp(oldtag2,data2)!=0){
      write_to_file(weight, data1, data2);
      sprintf(oldtag1,"%s",data1);
      sprintf(oldtag2,"%s",data2);
    }

    sprintf(data,"%s %s %s",weight, data1,data2);
    printf("%s\n",data);
    usleep(200000);
  }

  bcm2835_spi_end();
  bcm2835_close();
  close_config_file();
  return 0;
}
