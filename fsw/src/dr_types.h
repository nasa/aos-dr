#ifndef DR_TYPES_H
#define DR_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/// Definitions for the maximum numbers of tests and failure modes.
/// These are used for array sizes in the code, and the d-matrix
/// table size.
#define DR_MAX_TESTS 100
#define DR_MAX_FAILURE_MODES 100

/// Definitions for the errors that can happen internally to DR.
/// These start in the -40 range in the hope they are more unique,
/// not as likely to be repeated in other parts of cFS. 
#define DR_DIAGNOSIS_ERROR                (-40)
#define DR_WATCHPOINT_TABLE_LOAD_ERROR    (-41)
#define DR_RESULTS_SAVE_ERROR             (-42)
#define DR_FILE_OPEN_ERROR                (-43)

/// Enum to define the possible values of a test result.
typedef enum
{
  /// Value for passing tests
  DR_TEST_RESULT_PASS = 0,
  /// Value for failing tests
  DR_TEST_RESULT_FAIL,
  /// Value for unknown tests
  DR_TEST_RESULT_UNKNOWN,
  /// Invalid value, may be used to terminate for-loops
  DR_TEST_RESULT_COUNT
} dr_test_result_type;

/// Enum to define the possible values of a failure mode.
typedef enum
{
  /// Value for good failure modes
  DR_FAILURE_MODE_GOOD = 0,
  /// Value for suspect failure modes
  DR_FAILURE_MODE_SUSPECT,
  /// Value for bad failure modes
  DR_FAILURE_MODE_BAD,
  /// Value for unknown failure modes
  DR_FAILURE_MODE_UNKNOWN,
  // Invalid value, may be used to terminate for-loops
  DR_FAILURE_MODE_COUNT
} dr_failure_mode_type;

/// Enum to define error values for this app.
typedef enum
{
  /// Success code, no error
  DR_ERROR_NO_ERROR = 0,
  /// The wrong number of tests was given as an argument
  DR_ERROR_WRONG_NUM_TESTS,
  /// The wrong number of failure modes was given as an argument
  DR_ERROR_WRONG_NUM_FAILURE_MODES,
  /// A test result given is not valid
  DR_ERROR_INVALID_TEST_RESULT,
  /// Unable to open, close, or write to the specified file
  DR_ERROR_FILE_ERROR,
  /// Unable to save data to the specified file
  DR_ERROR_SAVE_ERROR,
} dr_error_type;

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // DR_TYPES_H
