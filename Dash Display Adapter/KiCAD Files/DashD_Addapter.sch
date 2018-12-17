EESchema Schematic File Version 4
LIBS:DashD_Addapter-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:RJ45 J1
U 1 1 5A74B889
P 7625 3225
F 0 "J1" H 7525 2775 50  0000 C CNN
F 1 "CAN" V 7250 3000 50  0000 C CNN
F 2 "Connector_RJ:RJ45_Amphenol_54602-x08_Horizontal" H 7625 3225 50  0001 C CNN
F 3 "" H 7625 3225 50  0001 C CNN
F 4 "523-RJE081880110 or 649-54602-908LF" H 7625 3225 50  0001 C CNN "Part Number"
	1    7625 3225
	-1   0    0    1   
$EndComp
Wire Wire Line
	7225 2925 6500 2925
Wire Wire Line
	6500 4200 7200 4200
Text Label 5500 4200 2    60   ~ 0
CAN-H
Text Label 5500 4300 2    60   ~ 0
CAN-L
Wire Wire Line
	7225 3025 6600 3025
Wire Wire Line
	6600 4300 7200 4300
Wire Wire Line
	7225 3125 6700 3125
Wire Wire Line
	6700 4800 7200 4800
Wire Wire Line
	6700 4400 7200 4400
Connection ~ 6700 4400
Wire Wire Line
	7225 3525 6700 3525
Connection ~ 6700 3525
Wire Wire Line
	7225 3625 7100 3625
Wire Wire Line
	7100 3625 7100 4900
Wire Wire Line
	7100 4900 7200 4900
Wire Wire Line
	7225 3425 7000 3425
Wire Wire Line
	7000 3425 7000 4700
Wire Wire Line
	7000 4700 7200 4700
Wire Wire Line
	7200 4600 6900 4600
Wire Wire Line
	6900 3325 6900 4600
Wire Wire Line
	6900 3325 7225 3325
Wire Wire Line
	7225 3225 6800 3225
Wire Wire Line
	6800 3225 6800 4500
Wire Wire Line
	6800 4500 7200 4500
Text Label 5550 4900 2    60   ~ 0
CAN-V+
Wire Wire Line
	6700 3125 6700 3525
Wire Wire Line
	6600 3025 6600 4300
Wire Wire Line
	6700 4400 6700 4800
Wire Wire Line
	6700 3525 6700 4400
Wire Wire Line
	6500 4200 6500 2925
$Comp
L Connector:RJ45 J4
U 1 1 5BE9B607
P 7600 4500
F 0 "J4" H 7500 4050 50  0000 C CNN
F 1 "CAN" V 7225 4275 50  0000 C CNN
F 2 "Connector_RJ:RJ45_Amphenol_54602-x08_Horizontal" H 7600 4500 50  0001 C CNN
F 3 "" H 7600 4500 50  0001 C CNN
F 4 "523-RJE081880110 or 649-54602-908LF" H 7600 4500 50  0001 C CNN "Part Number"
	1    7600 4500
	-1   0    0    1   
$EndComp
Text Label 5550 4800 2    60   ~ 0
CAN-V-
Wire Wire Line
	6700 4800 6050 4800
Wire Wire Line
	7100 4900 5900 4900
$Comp
L Device:Varistor VAR1
U 1 1 59A95FFE
P 4850 5300
F 0 "VAR1" V 4725 5275 50  0000 C CNN
F 1 "350mA" V 4940 5300 50  0000 C CNN
F 2 "Fuse:Fuse_1812_4532Metric" H 4850 5300 60  0001 C CNN
F 3 "" H 4850 5300 60  0000 C CNN
F 4 "Bell Fuse" H 4850 5300 50  0001 C CNN "Manufacture"
F 5 "0ZCG0035AF2C " V 4850 5300 60  0001 C CNN "Part Number"
	1    4850 5300
	0    1    -1   0   
$EndComp
Wire Wire Line
	4575 5300 4700 5300
Wire Wire Line
	5675 5300 5900 5300
Wire Wire Line
	5000 5300 5375 5300
Text Label 5125 5300 2    20   ~ 0
XE
$Comp
L Device:D_Schottky D2
U 1 1 5BA7ADDA
P 5525 5575
AR Path="/5BA7ADDA" Ref="D2"  Part="1" 
AR Path="/59A30B88/5BA7ADDA" Ref="D?"  Part="1" 
F 0 "D2" H 5375 5525 40  0000 C CNN
F 1 "MRA4003T3G " H 5550 5675 40  0000 C CNN
F 2 "Diode_SMD:D_SMA" H 5525 5575 60  0001 C CNN
F 3 "~" H 5525 5575 60  0000 C CNN
F 4 "On Semi" H 5525 5575 50  0001 C CNN "Manufacture"
F 5 "MRA4003T3G " H 5525 5575 50  0001 C CNN "Part Number"
	1    5525 5575
	1    0    0    1   
$EndComp
$Comp
L Device:D_Schottky D1
U 1 1 5BB38710
P 5525 5300
AR Path="/5BB38710" Ref="D1"  Part="1" 
AR Path="/59A30B88/5BB38710" Ref="D?"  Part="1" 
F 0 "D1" H 5675 5350 40  0000 C CNN
F 1 "MRA4003T3G " H 5550 5200 40  0000 C CNN
F 2 "Diode_SMD:D_SMA" H 5525 5300 60  0001 C CNN
F 3 "~" H 5525 5300 60  0000 C CNN
F 4 "On Semi" H 5525 5300 50  0001 C CNN "Manufacture"
F 5 "MRA4003T3G " H 5525 5300 50  0001 C CNN "Part Number"
	1    5525 5300
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5900 5300 5900 4900
Connection ~ 5900 4900
Wire Wire Line
	5900 4900 5125 4900
Wire Wire Line
	5675 5575 6050 5575
Wire Wire Line
	6050 5575 6050 4800
Connection ~ 6050 4800
Wire Wire Line
	6050 4800 5125 4800
$Comp
L Connector:Conn_01x01_Female J8
U 1 1 5BEF7A8B
P 4375 5575
F 0 "J8" H 4650 5575 50  0000 C CNN
F 1 "-" H 4500 5575 100 0000 C CNB
F 2 "MountingHole:MountingHole_5.3mm_M5_Pad_Via" H 4375 5575 50  0001 C CNN
F 3 "~" H 4375 5575 50  0001 C CNN
	1    4375 5575
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x01_Female J7
U 1 1 5BEF7B7E
P 4375 5300
F 0 "J7" H 4650 5300 50  0000 C CNN
F 1 "+" H 4500 5300 100 0000 C CNB
F 2 "MountingHole:MountingHole_5.3mm_M5_Pad_Via" H 4375 5300 50  0001 C CNN
F 3 "~" H 4375 5300 50  0001 C CNN
	1    4375 5300
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x01_Female J2
U 1 1 5BEF8721
P 4900 4200
F 0 "J2" H 5425 4200 50  0000 C CNN
F 1 "GREEN" H 5100 4200 50  0000 C CNB
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 4900 4200 50  0001 C CNN
F 3 "~" H 4900 4200 50  0001 C CNN
	1    4900 4200
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x01_Female J3
U 1 1 5BEF887E
P 4900 4300
F 0 "J3" H 5425 4300 50  0000 C CNN
F 1 "YELLOW" H 5100 4300 50  0000 C CNB
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 4900 4300 50  0001 C CNN
F 3 "~" H 4900 4300 50  0001 C CNN
	1    4900 4300
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x01_Female J5
U 1 1 5BEF8986
P 4925 4900
F 0 "J5" H 5450 4900 50  0000 C CNN
F 1 "RED" H 5125 4900 50  0000 C CNB
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 4925 4900 50  0001 C CNN
F 3 "~" H 4925 4900 50  0001 C CNN
	1    4925 4900
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x01_Female J6
U 1 1 5BEF89D6
P 4925 4800
F 0 "J6" H 5450 4800 50  0000 C CNN
F 1 "BLACK" H 5125 4800 50  0000 C CNB
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 4925 4800 50  0001 C CNN
F 3 "~" H 4925 4800 50  0001 C CNN
	1    4925 4800
	-1   0    0    1   
$EndComp
Connection ~ 6700 4800
Connection ~ 7100 4900
Connection ~ 6500 4200
Connection ~ 6600 4300
Wire Wire Line
	4575 5575 5375 5575
Text Label 4625 5575 2    20   ~ 0
-
Text Label 4625 5300 2    20   ~ 0
+
$Comp
L Connector_Generic:Conn_01x02 J9
U 1 1 5C1C1AE3
P 5800 3400
F 0 "J9" V 5700 3250 40  0000 C CNN
F 1 "TERM" V 5900 3375 40  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" V 5950 3400 40  0001 C CNN
F 3 "" H 5800 3400 60  0001 C CNN
	1    5800 3400
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R1
U 1 1 5C1C1AEB
P 5800 3825
F 0 "R1" V 5880 3825 40  0000 C CNN
F 1 "120" V 5807 3826 40  0000 C CNN
F 2 "Resistor_SMD:R_1210_3225Metric" V 5730 3825 30  0001 C CNN
F 3 "~" H 5800 3825 30  0000 C CNN
F 4 "RC1210FR-07120RL" V 5800 3825 60  0001 C CNN "Part Number"
	1    5800 3825
	-1   0    0    1   
$EndComp
Wire Wire Line
	5800 3600 5800 3675
Wire Wire Line
	5100 4200 5900 4200
Wire Wire Line
	5100 4300 5800 4300
Wire Wire Line
	5900 3600 5900 4200
Connection ~ 5900 4200
Wire Wire Line
	5900 4200 6500 4200
Wire Wire Line
	5800 3975 5800 4300
Connection ~ 5800 4300
Wire Wire Line
	5800 4300 6600 4300
$EndSCHEMATC
