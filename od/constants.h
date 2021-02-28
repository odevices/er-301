#pragma once

// Status Codes
#define STATUS_NONE 0
#define STATUS_PREPARED 1
#define STATUS_WORKING 2
#define STATUS_FINISHED 3
#define STATUS_CANCELED 4

#define STATUS_ERROR_OPENING_FILE -1
#define STATUS_OUT_OF_MEMORY -2
#define STATUS_ERROR_READING_FILE -3
#define STATUS_ERROR_WRITING_FILE -4
#define STATUS_SAMPLE_NOT_PREPARED -5
#define STATUS_FILE_NOT_EXIST -6

#define STATUS_STOPPED 0
#define STATUS_PLAYING 1
#define STATUS_PAUSED 2
#define STATUS_NO_SAMPLE -1

// A very small number
#define EPSILON (0.00000001f)

// Generic Choices
#define CHOICE_UNKNOWN 0
#define CHOICE_YES 1
#define CHOICE_NO 2

#define CHOICE_LEFT 1
#define CHOICE_BOTH 2
#define CHOICE_RIGHT 3

#define CHOICE_UP 1
#define CHOICE_DOWN 3

// Rate Choices
#define PER_FRAME 1
#define PER_SAMPLE 2

// How Often Choices
#define HOWOFTEN_ONCE 1
#define HOWOFTEN_LOOP 2
#define HOWOFTEN_GATE 3

// How Much Choices
#define HOWMUCH_ALL 1
#define HOWMUCH_SLICE 2
#define HOWMUCH_CUE 3

// Interpolation Choices
#define INTERPOLATION_NONE 1
#define INTERPOLATION_LINEAR 2
#define INTERPOLATION_QUADRATIC 3