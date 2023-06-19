#ifndef LINE_FOLLOW_H
#define LINE_FOLLOW_H

typedef enum SearchLineResult {
    SEARCHING = 0,
    FOUND = 1,
    LOST = -1,
    END_OF_LINE = 2
} SearchLineResult;

typedef enum CheckLineResult {
    ON_LINE = 0,
    OFF_LINE = -1,
    ALL_BLACK = 1
} CheckLineResult;

typedef enum FollowLineResult {
    FOLLOWING = 0,
    LOST_LINE = -1,
    ALL_LINE = 1
} FollowLineResult;

/**
 * @brief Follows the line until the end of the line is reached.
 * 
 * @param speed the speed at which the robot should follow the line (0-100)
 */
void followLine(int speed);

/**
 * @brief Follows the line until the end of the line is reached.
 * 
 * @return 0 if currently following the line, -1 if line is lost, 1 if all is black
 */
FollowLineResult followLineTask();

/**
 * @brief Checks if the line is lost.
 * 
 * @return 0 if partly on line, -1 if line is lost, 1 if all is black
 */
CheckLineResult checkForLine();

/**
 * @brief Searches for the line.
 * 
 */
void searchLine();

/**
 * @brief Searches for the line.
 * 
 * @return 0 if currently searching for the line, 1 if line is found, -1 if line is lost, 2 if end of line is reached
 */
SearchLineResult searchLineTask();

#endif