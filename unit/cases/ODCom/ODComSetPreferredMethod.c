#define UT_CALL(handle, value) utt_ODComSetPreferredMethod((handle), (value))
#define UT_VALUE kComMethodSocket
#define UT_FIELD(port) ((port)->Method)
#include "simple_setter_fixture.h"
