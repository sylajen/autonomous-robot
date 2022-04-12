/*code used in the final demo*/

#include <EV3_LCDDisplay.h>
#include <EV3_Sensor_Ultrasonic.h>
#include "EV3_Sensor_Color.h"
#include <EV3_Sensor_Gyro.h>
#include <EV3_BrickUI.h> // Code used to check whether a button is pressed
#include <EV3_Thread.h> // Code used for sleep function
#include <EV3_Motor.h>

using namespace ev3_c_api;

/*Declaration and Initialization of Constants*/
const E_Brick_Layer layer = E_Layer_1; //Always 1 for our project
const E_Sensor_Port portUltrasonic = E_Port_3; //Ultrasonic sensor is connected to port #3
const E_Sensor_Port portColorSensor = E_Port_4; //Color/reflectivity sensor is in port #4
const E_Sensor_Port portGyroSensor = E_Port_1; //Gyro sensor is in port #1
const E_Distance_Units units = E_Dist_Cm; //Measurement units for the ultrasonic sensor (can also use inches)
const E_Motor_Port motorLeft = E_Port_B; //Left motor is connected to port B
const E_Motor_Port motorRight = E_Port_C; //Right motor is connected to port C

//Declaration and Initialization of Variables
float dist = 0;  //Store distance data from the ultrasonic sensor
int colour = 0; //Colour data from the colour sensor
int angle = 0; //Store angular data from the gyro sensor
signed char speed = 25; //Motor speed (-100 -> 100)
float stopDist = 15; //Distance from an object to stop
int stopAngle = 84; //techincally 90, but set to 80 to allow for input delay
int aboutTurn = 174; //technically 180, but set to 170 to allow for input delay
int boundary = 5; //colour that the boundary will be; 0 = No Colour, 1 = Black, 2 = Blue, 3 = Green, 4 = Yellow, 5 = Red, 6 = White, 7 = Brown

void stopRobot(int time)
{
    SpeedTwoMotors(motorLeft, 0, motorRight, 0, layer);
    EV3_Sleep(time);
    return;
}

//Goes straight
float goStraight(float dis)
{
    while (dis > stopDist && colour != boundary)
    {
        //Move forward until there's an object ahead of the robot
        SpeedTwoMotors(motorLeft, speed, motorRight, 0.95*speed, layer); //right motor is 95% of speed to account for drift/lag
        dis = GetDistance(portUltrasonic, units, layer);
        colour = GetColor(portColorSensor, layer);
    }
    return dis;
}

//Turns 90 degrees to the right
void turnRight(int ang)
{
    ResetHard(portGyroSensor, layer);
    ResetAngle(portGyroSensor, layer);
    ang = 0;
    //Turn 90 degrees right
    while (ang < stopAngle)
    {
        //Left motor spins forward, right motor spins backwards
        SpeedTwoMotors(motorLeft, speed, motorRight, -1 * speed, layer);
        ang = GetAngle(portGyroSensor, layer); //Read in the angle from the gyro sensor
    }
}

//Turns 90 degrees to the left
void turnLeft(int ang)
{
    ResetHard(portGyroSensor, layer);
    ResetAngle(portGyroSensor, layer);
    ang = 0;
    //Turn -90 degrees
    while (ang > (-1 * stopAngle))
    {
        //Right motor spins forward, left motor spins backwards
        SpeedTwoMotors(motorLeft, -1 * speed, motorRight, speed, layer);
        ang = GetAngle(portGyroSensor, layer); //Read in the angle from the gyro sensor
    }
}

//Turns 180 degrees to the left
void turnAround(int ang)
{
    ResetHard(portGyroSensor, layer);
    ResetAngle(portGyroSensor, layer);
    ang = 0;
    //Turn -180 degrees
    while (ang > (-1 * aboutTurn))
    {
        //Right motor spins forward, left motor spins backwards
        SpeedTwoMotors(motorLeft, -1 * speed, motorRight, speed, layer);
        ang = GetAngle(portGyroSensor, layer); //Read in the angle from the gyro sensor
    }
}

//Goes backwards for 2 seconds
void goBack()
{
    //Pause for 1 second
    stopRobot(500);

    //Move back a little bit
    SpeedTwoMotors(motorLeft, -5, motorRight, -5, layer);
    EV3_Sleep(2000);

    //Pause for 1 second
    stopRobot(500);
    return;
}

int main()
{
    //Calibrate the gyro sensor to begin
    ResetHard(portGyroSensor, layer);
    ResetAngle(portGyroSensor, layer);

    /***********************EV3 MOVEMENT**************************/
    while (1)
    {
        dist = GetDistance(portUltrasonic, units, layer);
        colour = GetColor(portColorSensor, layer);
        angle = GetAngle(portGyroSensor, layer);

        // if there's no object, then go straight
        if (dist >= stopDist && colour != boundary)
        {
            goStraight(dist); //this loop goes straight while there's no object/boundary
        }

        // else, perform object detection maneuvers
        else
        {
            // stop the robot and move it back a step to allow for space to rotate
            stopRobot(1000);
            goBack();

            // turn 90 to the right
            if (dist <= stopDist || colour == boundary)
            {
                //Calibrate the gyro sensor to begin
                ResetHard(portGyroSensor, layer);
                ResetAngle(portGyroSensor, layer);
                angle = 0;

                turnRight(angle);
            }

            stopRobot(500);
            //Get dist and colour to get current distance and colour values
            dist = GetDistance(portUltrasonic, units, layer);
            colour = GetColor(portColorSensor, layer);

            // turn 180 to the left
            if (dist <= stopDist || colour == boundary)
            {
                //Calibrate the gyro sensor to begin
                ResetHard(portGyroSensor, layer);
                ResetAngle(portGyroSensor, layer);
                angle = 0;

                turnAround(angle);
            }

            stopRobot(500);
            //Get dist and colour to get current distance and colour values
            dist = GetDistance(portUltrasonic, units, layer);
            colour = GetColor(portColorSensor, layer);

            // turn 90 to the left
            if (dist <= stopDist || colour == boundary)
            {
                //Calibrate the gyro sensor to begin
                ResetHard(portGyroSensor, layer);
                ResetAngle(portGyroSensor, layer);
                angle = 0;

                turnLeft(angle);
            }

            stopRobot(500);
            //Get dist and colour to get current distance and colour values
            dist = GetDistance(portUltrasonic, units, layer);
            colour = GetColor(portColorSensor, layer);


            // dead end, stop robot and end
            if (dist <= stopDist || colour == boundary)
            {
                stopRobot(1000);
                break;
            }

            //Stop robot if button is pressed
            if (isBrickButtonPressed(E_BTN_ENTER) || isBrickButtonPressed(E_BTN_ESC) || isBrickButtonPressed(E_BTN_DOWN))
            {
                stopRobot(1000);
                break;
            }
        }
    }
    return 0;
}
