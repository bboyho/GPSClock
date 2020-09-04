Arduino GPS Clock Project

========================================

The current time is displayed using an Arduino Uno, SparkFun GPS Shield,[EM406A](https://www.sparkfun.com/products/retired/465), and [7-Segment Serial Display - Red]( https://www.sparkfun.com/products/11441). This code parses the GPS data when there is a satellite lock and outputs the data to a 7-segment serial display using software serial pins. The clock is adjust for Mountain Daylight Time (MDT) with an optional mode for daylight savings with the press of a latching button, jumper wire, or switch. There is a status LED indicator for the MDT to indicate whether it is AM or PM.

[![GPS Clock with 7-Segment Serial Display](https://raw.githubusercontent.com/bboyho/GPSClock/master/Documentation/Arduino_GPS_Clock.jpg)](https://raw.githubusercontent.com/bboyho/GPSClock/master/Documentation/Arduino_GPS_Clock.jpg)

Documentation
--------------

* **[Tiny GPS Plus Library](https://github.com/mikalhart/TinyGPSPlus)** - Arduino library that parses GPS serial data
* **[Using the Serial 7-Segment Display](https://learn.sparkfun.com/tutorials/using-the-serial-7-segment-display/)** - Basic hookup guide for 7-segment  serial display
* **[GPS Shield Hookup Guide](https://learn.sparkfun.com/tutorials/gps-shield-hookup-guide)** - GPS Shield Hookup Guide



License Information
-------------------

This project is _**open source**_! 

Please review the [LICENSE.md](https://github.com/bboyho/GPSClock/blob/master/LICENSE.md) file for license information. 

Distributed as-is; no warranty is given.

- Your friends at SparkFun.
