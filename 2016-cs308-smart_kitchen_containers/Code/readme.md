2016 - CS308  Group X : Project README 
================================================ 
 
Group Info: 
------------ 
+   Akhil Surya Vamshi (120050067) 
+   Pranay Dhondi (120050054)
+   Bhargav Chippada (120050053)
+   Jagadeesh Nelaturu (120050055) 
 
Extension Of 
------------ 
No our project is not an extension
 
Project Description 
------------------- 
        Over the last decade, the concept of food has gone a radical transformation. In addition to their nutritional and sensory properties, they play an important role in maintenance of health, on psycho-physical well-being and prevention of certain diseases. Today foods are not only intended to satisfy hunger and to provide necessary nutrients for humans. Health consciousness of humans made people to take selective food in prescribed quantities.
        Our Smart Kitchen containers help users in tracking their usage of kitchen commodities, thereby alerting the user if it needs to be refilled and also monitoring calorie consumption thereby their health everyday.
        Our System logs down the weights of containers and shows user in aesthetically pleasing manner, thereby every one can understand the consumption(both weight,calorie) trends that have observed, They can also share the data with doctors. If the amount of a commodity goes below a certain level, the user is to be alerted. 
         
Technologies Used 
------------------- 
    Various Technologies are used to for the system. They are :-

+   Embedded C 
+   Python 
+   Specialized Hardware

Installation Instructions 
========================= 
+ Calibrating and using HX711 to record weights requires code written for processing to be uploaded to HX711 using energia. It can be obtained from here: http://energia.nu/download/
+ R-PI code requires bcm2835 library, It can be obtained from: http://www.airspayce.com/mikem/bcm2835/index.html  
Compressed version of the library is also available in R-PI folder.  
To run R-PI Code, follow the below steps -   
a) Install C library for Broadcom BCM 2835 as used in Raspberry Pi  
Follow the following instructions to Install:  
    1. tar -zxf bcm2835-1.50.tar.gz
    2. cd cd bcm2835-1.50
    3. ./configure
    4. sudo make install

    b) Run this command from inside R-PI/code  
    **g++ --std=c++0x config.c rfid.c rc522.c main.cc -o rc522_reader -lbcm2835**  
    rc522_reader executable will be created which can be run either as:
    1. normal mode:  
    ./rc522_reader
    2. debug mdode:  
    ./rc522_reader -d  

    c) Run the UI server  
    The UI Code path: UI Code/smart_kitchen  
    cmd: **python -m SimpleHTTPServer**   
    Open browser and type url **<rpi server ip address>:port**  
    port is generally 8000
+  Inside Raspberry pi Copy the Micro-controller and UI Code as follows,  
Place the smart_kitchen folder (found inside UI Code) inside R-PI code (found inside Microcontroller Code/R-PI/code). Run ./rc522_reader and python server.  
The tiva code should be loaded into tiva-board ofcourse. 

Demonstration Video 
=========================  
Add the youtube link of the screencast of your project demo.


References 
=========== 
 
Please give references to importance resources.  
 
+ https://www.raspberrypi.org/documentation/remote-access/ip-address.md
+ Visualization of data (https://d3js.org/)
+ Website Design (https://jquery.com/)(http://getbootstrap.com/)
+ Usb serial Programming(http://www.tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html)
+ r-pi bcm2835 library (http://bsd.ee/~hadara/blog/?p=1017) (http://www.airspayce.com/mikem/bcm2835/index.html)
