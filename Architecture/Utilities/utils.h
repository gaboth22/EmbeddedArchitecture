#ifndef UTILS_H
#define UTILS_H

#define BETWEEN(min, val, max) ((val >= min) && (val <= max))
#define TRUNCATE_UP(val, max) (val > max ? max : val)
#define TRUNCATE(min, val, max) TRUNCATE_UP(val < min ? min : val, max)
#define TRUNCATE_U16_SUBTRACTION(val1, val2) (val1 > val2 ? val1 - val2 : UINT16_MAX - (val2 - val1))
#define RECAST(new_name, cast_me, new_type) new_type new_name = (new_type) cast_me
#define IGNORE(something) (void)something
#define CONCAT2(a, b) a ## b
#define CONCAT3(a, b, c) a ## b ## c

#endif
