#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

// Test code to demonstrate traversal of the incremental search space in John the Ripper.
//
// Author: taviso@...xchg8b.com, Jun 2012.

struct header {
    uint32_t    version;
    uint32_t    check[6];
    uint8_t     min;
    uint8_t     max;
    uint8_t     length;
    uint8_t     count;
    uint32_t    offsets[8];
    struct {
        uint8_t length;
        uint8_t fixed;
        uint8_t count;
    } order[3420];
} __attribute__((packed));

static const uint32_t kMinLength     = 8;
static const uint32_t kMaxLength     = 8;
static const uint32_t kCharsetLen    = 95;
static const uint32_t kTotalShards   = 1 << 21;

// static const uint64_t kTotalKeySpace = pow(kCharsetLen, kMaxLength);
// $ bc <<< 'obase=16;95^8'
// 1791C60F6FED01
static const uint64_t kTotalKeySpace = 0x1791C60F6FED01ULL;

// $ bc <<< 'obase=16;(95^8)/(2^21)'
// BC8E307B
static const uint64_t kWorkUnit      = 0xBC8E307BULL;

// Convert an array of numdigits integers of a specified base into an integer.
static uint64_t convert_from_base(uint8_t *digits,
                                  uint8_t  numdigits,
                                  uint8_t  base)
{
    uint64_t result;
    uint32_t i;

    for (result = i = 0; numdigits--; i++) {
        result += digits[numdigits] * pow(base, i);
    }

    return result;
}

// Convert an integer into an array of digits.
static bool convert_to_base(uint8_t *digits,
                            uint8_t  numdigits,
                            uint8_t  base,
                            uint64_t value)
{
    int32_t i;

    // Not possible to represent numbers in an unsigned integer base less than
    // one, return failure.
    if (base == 0) return false;

    // Distribute value digits appropriately.
    for (i = numdigits - 1; i >= 0; i--) {
        digits[i] = value % base;
        value     = value / base;
    }

    return true;
}

// Given a { length, fixed, count } triplet, calculate the maximum amount of
// work expected in inc_key_loop().
static uint64_t calculate_maximum_attempts(uint32_t length,
                                           uint32_t fixed,
                                           uint32_t count)
{
    int64_t result;
    int32_t pos;

    // A table of coefficients required to compensate for fixed characters in
    // JtR character files.
    static const int64_t kOrderCoefficients[][8] = {
        { 0,   0,   0,   0,   0,   0,   0,   0 },
        { 0,  -1,   0,   0,   0,   0,   0,   0 },
        { 0,  -2,   1,   0,   0,   0,   0,   0 },
        { 0,  -3,   3,  -1,   0,   0,   0,   0 },
        { 0,  -4,   6,  -4,   1,   0,   0,   0 },
        { 0,  -5,  10, -10,   5,  -1,   0,   0 },
        { 0,  -6,  15, -20,  15,  -6,   1,   0 },
        { 0,  -7,  21, -35,  35, -21,   7,  -1 },
    };

    // Calculate base complexity of this rule.
    result = pow(count + 1, length);

    // Compensate for fixed characters.
    for (pos = 0; pos < kMaxLength; pos++) {
        result += kOrderCoefficients[fixed][pos] * pow(count + 1, length - pos);
    }

    return result;
}

// Given a { length, fixed, count } triplet, calculate the internal state of
// inc_key_loop() after the specified number of crypt operations.
//
// Note that the state produced may be a few crypts early due to the
// number_cache behaviour. This is unavoidable, john does the same thing with
// REC files internally.
static bool calculate_number_state(uint8_t  *numbers,
                                   uint32_t  length,
                                   uint32_t  fixed,
                                   uint32_t  count,
                                   uint64_t  crypts)
{
    // Compensate for the number_cache in inc_key_loop() skipping certain
    // increments of numbers[].
    if (fixed != length)
        crypts = crypts - (crypts % (count + 1));

    // Convert number of crypts into a k-digit base-n representation
    // of crypts. This closely matches how John stores it's internal state,
    // with the exception of fixed digits.
    convert_to_base(numbers, kMaxLength, count + 1, crypts);

    // Move the digits above the fixed digit down one position, because the
    // fixed digit will displace them. This is the same as partial
    // multiplication of these digits by base, but moving them is more
    // efficient than having to convert them to integers.
    memmove(&numbers[0], &numbers[1], fixed);

    // Now we can force the fixed digit into the correct position.
    numbers[fixed] = count;

    // Finally, we need to adjust the digits above the fixed position
    // independently. This is because John will never set a digit in those
    // positions to count, however other arithmetic rules still apply.
    //
    // We can interpret this algorithm behaviour as setting these digits to one
    // base lower than the rest of the digits. Yes, this is confusing.
    convert_to_base(numbers,
                    fixed,
                    count,
                    convert_from_base(numbers, fixed, count + 1));

    // Complete.
    return true;
}

// A simplified version of the inc_key_loop() algorithm from John the Ripper
// used for verifying prediction results.
static uint64_t inc_key_loop(uint8_t  *numbers,
                             uint32_t  length,
                             uint32_t  fixed,
                             uint32_t  count)
{
    int32_t numbers_cache;
    int32_t pos = 0;
    uint64_t crypts = 0;

    numbers[fixed]  = count;

update_ending:

    numbers_cache   = numbers[length];

update_last:

    // A crypt() operation would happen here.
    crypts++;

    pos = length;

    if (fixed < length) {
        if (++numbers_cache <= count) {
            if (length >= 2) goto update_last;
            numbers[length] = numbers_cache;
            goto update_ending;
        }
        numbers[pos--] = 0;
        while (pos > fixed) {
            if (++numbers[pos] <= count) goto update_ending;
            numbers[pos--] = 0;
        }
    }
    while (pos-- > 0) {
        if (++numbers[pos] < count) goto update_ending;
        numbers[pos] = 0;
    }

    return crypts;
}


int main(int argc, char **argv)
{
    struct header header;   // Header of CHR file.
    uint64_t crypts;        // Total number of crypt() operations expected.
    uint64_t estimated;     // Estimated number of crypts() required to complete this order entry.
    uint32_t entry;         // Current order index (a table stored in the CHR files).
    uint64_t result;
    uint32_t i;

    // There are two important components of JtR internal state for incremental
    // cracking. First, the entry number (essentially an index into the order
    // table of the CHR files). Second is the numbers array, for which the bulk
    // of the algorithm is in inc_key_loop() in John's inc.c.
    uint8_t john_numbers_state[kMaxLength];

    // Read in CHR file from stdin.
    fread(&header, sizeof header, 1, stdin);

    fprintf(stderr, "Version:   %08X\n", header.version);
    fprintf(stderr, "Check:     %08X\n", header.check[0]);
    fprintf(stderr, "Min:       %u\n", header.min);
    fprintf(stderr, "Max:       %u\n", header.max);
    fprintf(stderr, "Length:    %u\n", header.length);
    fprintf(stderr, "Count:     %u\n", header.count);
    fprintf(stderr, "Offsets (ignored):\n");
    fprintf(stderr, "\t%08X %08X %08X %08X %08X\n\t...\n",
            header.offsets[0],
            header.offsets[1],
            header.offsets[2],
            header.offsets[3],
            header.offsets[4]);

    for (entry = 0, crypts = 0;
         entry < (sizeof header.order / sizeof header.order[0]);
         entry++) {

        // This logic duplicated from do_incremental_crack()
        if (header.order[entry].fixed && !header.order[entry].count)
            continue;
        if (header.order[entry].length + 1 < kMinLength)
            continue;
        if (header.order[entry].length >= kMaxLength)
            continue;
        if (header.order[entry].count >= kCharsetLen)
            continue;

        // Calculate worst case requirements for crypt() operations this entry
        // requires. We still need to adjust for 'Fixed' characters, which is
        // done with the table below.
        estimated = calculate_maximum_attempts(header.order[entry].length,
                                               header.order[entry].fixed,
                                               header.order[entry].count);



        if (estimated < 12345678)
            continue;

       crypts = estimated - 12345678;

        // Find the state after num crypt operations, so we can skip to an
        // arbitrary position.
        if (calculate_number_state(john_numbers_state,
                                   header.order[entry].length,
                                   header.order[entry].fixed,
                                   header.order[entry].count,
                                   crypts) == false) {
            fprintf(stderr, "error: calculate_number_state() returned failure\n");
            return 1;
        }

        fprintf(stderr, "\tentry %u { %u, %u, %u }, ~%llu crypt()\n",
                        entry,
                        header.order[entry].length,
                        header.order[entry].fixed,
                        header.order[entry].count,
                        estimated);

        result = inc_key_loop(john_numbers_state,
                              header.order[entry].length,
                              header.order[entry].fixed,
                              header.order[entry].count);

        fprintf(stderr, "crypts executed %llu\n", result);
    }

    return 0;
}
