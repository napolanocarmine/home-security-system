# home-security-system

The goal of this work is to design and implement a “Home Security System” based on the following requirements:

• Two different kind of alarms: AREA and BARRIER, for detecting movements and obstacles;

• Drive a buzzer for notifications and alarms (different sounds for different alarms);

• Allow the user to insert its own system configuration, that is composed by the following parameters:
a. USER PIN, used for inserting commands, composed by 4 numbers;
b. Alarms delay, each alarm can be configured with a different delay, that is the elapsed time between the signal detection and the effective emission of the alarm, a suitable value belongs to the range [0, 30] seconds;
c. Alarm duration, the system has one alarm duration that represents the amount of time during which the alarm sound must be emitted, a suitable value belongs to the range [5, 60] seconds;
d. Current date and time.

The configuration can be inserted only at boot time, and in a time window of 30 seconds. If the user can’t insert the custom configuration within 30 seconds the default configuration is loaded.
• USER PIN, [0 0 0 0];
• Sensors delay, 0 s;
• Alarm duration, 5 s;
• Current date and time;

• Allow the user to insert commands in order to:
a. Activate/Deactivate the system
b. Activate/Deactivate both or a single alarm
c. Close an active alarm

• Send, periodically (each 10 seconds), a system log message, in order to share the system state in a particular moment, the message must be composed by:
a. Current date time, in the format: [dd-mm-yyyy hh:mm:ss];
b. Alarms states, in the format: Area [STATE] – Barrier [STATE], where STATE can be {Inactive, Active, Alarmed};

• Before the sensor gets alarmed the detected signal must be stable for at least 1 second;
• The sound is emitted when the system is alarmed, and any delays are elapsed;
• The user led must be on when the system boot and when the system isn’t active;
• The user led must toggle when the system is active;
