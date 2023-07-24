#ifndef LINE_FOLLOW_H
#define LINE_FOLLOW_H

/**
 * @defgroup lineFollow Line Follow
 * @brief Let the robot follow the line.
 */

/**
 * @brief The result of the search line task.
 * @ingroup lineFollow
 */
typedef enum SearchLineResult {
    /// @brief The robot is currently searching the line.
    SEARCHING = 0,
    /// @brief The robot found the line.
    FOUND = 1,
    /// @brief The robot lost the line.
    LOST = -1,
    /// @brief The robot reached the end of the line.
    END_OF_LINE = 2
} SearchLineResult;

/**
 * @brief The result of performing a line check.
 * @ingroup lineFollow
 */
typedef enum CheckLineResult {
    /// @brief The robot is partly on the line.
    ON_LINE = 0,
    /// @brief The robot is not on the line.
    OFF_LINE = -1,
    /// @brief The robot is completely on the line.
    ALL_BLACK = 1
} CheckLineResult;

/**
 * @brief The result of following the line.
 * @ingroup lineFollow
 */
typedef enum FollowLineResult {
    /// @brief The robot is currently following the line.
    FOLLOWING = 0,
    /// @brief The robot lost the line.
    LOST_LINE = -1,
    /// @brief The robot is fully on the line.
    ALL_LINE = 1
} FollowLineResult;

/**
 * @brief Follows the line until the end of the line is reached.
 * @details A @ref pidController is used to follow the line. The error is the difference between the left and right line sensor, which should be corrected to 0.
 * @ingroup lineFollow
 * 
 * @param speed the speed at which the robot should follow the line (0-100)
 */
void followLine(int speed);

/**
 * @brief Follows the line until the end of the line is reached.
 * @ingroup lineFollow
 * @note This method should be called in the main loop of the program as frequent as possible to assure the correct timing.
 *
 * @return 0 if currently following the line, -1 if line is lost, 1 if all is black
 */
FollowLineResult followLineTask();

/**
 * @brief Checks if the line is lost.
 * @ingroup lineFollow
 * 
 * @return 0 if partly on line, -1 if line is lost, 1 if all is black
 */
CheckLineResult checkForLine();

/**
 * @brief Searches for the line.
 * @ingroup lineFollow
 * 
 */
void searchLine();

/**
 * @brief Searches for the line.
 * @details Turn to each side for 90 degrees and check if the line is found.
 *          This behaviour is implemented in a state machine in @ref SearchLineState.
 * @ingroup lineFollow
 * @note This method should be called in the main loop of the program as frequent as possible to assure the correct timing.
 *
 * @return 0 if currently searching for the line, 1 if line is found, -1 if line is lost, 2 if end of line is reached
 */
SearchLineResult searchLineTask();

#endif