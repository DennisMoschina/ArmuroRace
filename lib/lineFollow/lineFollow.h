#ifndef LINE_FOLLOW_H
#define LINE_FOLLOW_H

/**
 * @brief Follows the line until the end of the line is reached.
 * 
 * @param speed the speed at which the robot should follow the line (0-100)
 */
void followLine(int speed);

/**
 * @brief Follows the line until the end of the line is reached.
 * 
 * @return 0 if currently following the line, -1 if line is lost
 */
int followLineTask();

#endif