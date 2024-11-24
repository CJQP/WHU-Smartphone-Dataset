# WHU-Smartphone Dataset
## A large-scale multi-traffic scene GNSS dataset of Android smartphones and a low-cost GNSS module collected in China

To conduct a comprehensive analysis of the performance and accuracy of smartphone positioning, we released the open-source WHU-Smartphone dataset , which is a large-scale, multi-scene, and multi-mode GNSS dataset collected by four Android smartphones from different manufactures and a low-cost GNSS module with high-precision ground truth. Our dataset contains several hundred driving routes with more than 2500 km from three large cities, including open-sky or shaded urban roads and highways. Multifrequency observation space representation (OSR) corrections of real-time kinematic (RTK) positioning with multiple GNSS systems, particularly those that supported the BeiDou Navigation Satellite System (BDS)-2 and BDS-3 systems, were implemented. On the basis of WHU-Smartphone dataset, we proposed some tools for converting data format and evaluating position accuracy with self-developed RTK positioning algorithm. We hope that the open-source GNSS dataset can help researchers verify the effectiveness of proposed algorithms and compare accuracy between algorithms with a large amount of dataset in various environments. We also expect more extensive progress in robust positioning, integrity monitoring, and cooperative positioning, and more advanced studies in challenging environments.

If you have any question for our dataset, please contact me via email: yixin9264@whu.edu.cn

Related Papers:
- Wang, Yixin, Chuang Qian, and Hui Liu. "WHU-Smartphone: A Large-Scale Multi-Traffic Scene GNSS Dataset of Android Smartphones and a Low-Cost GNSS Module." IEEE Transactions on Intelligent Transportation Systems (2024).

*if you use our dataset for your academic research, please cite our related papers*

## 1 Platform Equipment
The platform is equipped with the following sensors:
- Four Android smartphones [Xiaomi MI8; Huawei P20; Huawei P30; and VIVO X30]
- low-cost GNSS module (MXT906A,BDS/GPS,L1)
- GNSS/IMU reference benchmark (Trimble BD982 geodetic receiver and Honeywell Hg4930 IMU)
![image](https://github.com/user-attachments/assets/a99def4a-5616-46d5-937d-b00f8dfe40dd)

## 2 Device Information

The specific information of the smartphones and the GNSS module is provided in the table.
| Device (Abb.)   | Xiaomi 8 (XIM8)         | Huawei P20 (HP20)                             | Huawei P30 (HP30)                             | VIVO X30 (VX30)                              | MXT module (MXT)       |
|-----------------|-------------------------|-----------------------------------------------|-----------------------------------------------|-----------------------------------------------|------------------------|
| GNSS chips      | Broadcom 47755          | Kirin 970                                     | Kirin 980                                     | Exynos 980                                    | MXT906A               |
| L5              | YES                     | NO                                            | YES                                          | NO                                            | NO                    |
| ADR             | YES                     | YES                                           | YES                                          | NO                                            | YES                   |
| Duty Cycle      | OFF                     | ON                                            | OFF                                          | \                                             | \                     |
| GNSS Signals    | GPS: L1+L5; GLO: L1; GAL: E1; BDS: B1 | GPS: L1; GLO: L1; BDS: B1                    | GPS: L1+L5; GLO: L1; BDS: B1                 | GPS: L1; GLO: L1; GAL: E1; BDS: B1           | GPS: L1; BDS: B1      |
| BDS-3           | YES                     | YES                                           | YES                                          | YES                                          | NO                    |
| Other           | \                       | Duty cycle occurs mainly in static mode       | Abnormal carrier observation noise (0.2m-0.5m)| Unavailable mAccumulatedDelta-RangeState     | \                    |

- Base Station for RTK
  - base station with fixed coordinates:  GPS L1/L2/L5, GLO G1/G2, GAL E1/E5, BDS-2&BDS-3 B1/B2/B3, 1Hz
  - virtual reference station (VRS) service：GPS L1/L2, GLO G1/G2, BDS B1/B2, QXFZ, provided by Qianxun Spatial Intelligence, Inc
  - RTCM3 format

  *Notes: For data security reasons，the observation files of base station are available upon request by contacting the author(yixin9264@whu.edu.cn).*
  

## 3 Driving Routes
The total length of the driving routes in the three cities was about 2500 km, with large coverage areas and rich test scenes, including open-sky/shaded urban roads and highways.

- **Test Environments**
<div align="center">
 <img src=https://github.com/user-attachments/assets/00cf5f9f-36e8-43c3-8b37-c93e07369742>
 <img src=https://github.com/user-attachments/assets/36fc4f47-bb7c-4ad2-8a3a-aa4764e2f9dd>
</div>



- **Beijing (39°26′N ~ 41°03′N)**
  
  The driving test in Beijing was from October 14, 2020 to October 22, 2020, and the total length of the driving routes was 978.1 km.
<div align="center">
 <img src=https://github.com/user-attachments/assets/c1b77680-f22b-4815-b678-b73a696fbb30>
</div>

  
- **Wuhan (29°58′N–31°22′N)**

  The driving test in Wuhan was from July 3, 2020 to September 2, 2020, the total length of driving routes was 713.3 km.
<div align="center">
      <img src=https://github.com/user-attachments/assets/db437627-0ef7-4d00-908b-dfb86c7411fa>
</div>

- **Shenzhen (22°24′N–22°52′N)**

  The driving test in Shenzhen was from September 17, 2020 to September 25, 2020, and the total length of the driving routes was 795.5 km.
<div align="center">
      <img src=https://github.com/user-attachments/assets/2d1dee9d-b38b-4150-8b7c-f2e7332112ee>
</div>    

## 4 Data Format
We provide the raw GNSS observation data in two formats:
- MXT GNSS module(YYYY_MM_DD_hh_mm_ss_bluetooth_data_log.txt):
  - RTCM3 format,you can use [RTKLIB](https://www.rtklib.com/) to convert RTCM file to RINEX file.
    
- Smartphones(YYYY_MM_DD_hh_mm_ss.sss_spare_log.txt):
  -  JSON Format
  -  Spare file 
  ![image](https://github.com/user-attachments/assets/44fb0271-66a4-40fa-a666-05876dc47c36)
  
    | **Data Type**            | **Description**                                                                                   |
    |---------------------------|---------------------------------------------------------------------------------------------------|
    | **current_Start_Timestamp** | Observation data timestamp of smartphones (local time)                                          |
    | **gga**                   | Positions calculated by GNSS chipset (NMEA-0183 format[^1])                                      |
    | **imei**                  | International mobile equipment identity of smartphone                                            |
    | **obsd_t**                | Raw GNSS observations at current epoch                                                          |
    | **d**                     | Doppler observation                                                                             |
    | **l**                     | Carrier-phase observation                                                                       |
    | **lli**                   | Loss-of-lock indicator                                                                          |
    | **p**                     | Pseudorange observation                                                                         |
    | **prn**                   | Satellite index                                                                                 |
    | **snr**                   | Signal-to-noise ratio                                                                           |
    | **type**                  | Observation code (RINEX Version 3)                                                              |
    | **vd**                    | Variance of Doppler observation                                                                 |
    | **vl**                    | Variance of carrier-phase observation                                                          |
    | **vp**                    | Variance of pseudorange observation                                                            |
    | **satnum**                | Number of visible satellites at current epoch                                                   |
    | **t**                     | Observation data timestamp of smartphones (GPST)                                               |

    [^1]: A voluntary industry standard developed for GNSS receivers, which contains the position, velocity, and time message.(https://en.wikipedia.org/wiki/NMEA_0183)



## 1.5 Data Conversion Tools

- ConvSpare.bat:  Extract the position results in NMEA-0183 format calculated by the GNSS chipset (chipset solution) from the spare file and then convert them into files with “pos”(RTKLIB format) and KML files(driving tracks).

   Notes:
  - 1.Put "ConvSpare.bat" in a folder with the spare files.
  - 2.You can only extract all the files of the same day at one time, the Year/Month/Day[2020/08/15] in "ConvSpare.bat" needs to be the same as that in the spare file.

- spare2rinex: The function converts available raw GNSS information extracted from the spare file into RINEX file (RINEX 3.x format), which can be used in RTKLIB.
  - Supporting Visual Studio 2017/2019/2021.
  - The functions are written in C language, and they can be extended and modified in accordance with one’s needs.
  

