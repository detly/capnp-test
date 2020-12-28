#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <test.capnp.h>
#include <valgrind/memcheck.h>

static void print_vbits(char * prefix, char * vbits, size_t length)
{
    printf("%s: ", prefix);
    for (size_t idx = 0; idx < length; idx++)
    {
        const int vbits_as_int = (int) vbits[idx] & 0xFF;
        printf("%02x", vbits_as_int);
    }
    printf("\n");
}

static ssize_t encode_message(uint8_t ** buf)
{
    struct Thing thing_enc = {
        .first = 1,
        .second = EnumOne_oneB,
        .third = 3,
        .fourth = 4,
        .fifth = EnumTwo_twoB,
    };

    struct capn capn_enc;
    capn_init_malloc(&capn_enc);
    capn_ptr capn_enc_root = capn_root(&capn_enc);

    Thing_ptr p_thing_enc = new_Thing(capn_enc_root.seg);
    write_Thing(&thing_enc, p_thing_enc);

    const int capn_result = capn_setp(capn_enc_root, 0, p_thing_enc.p);
    assert(capn_result == 0);

    const ssize_t buf_size = capn_size(&capn_enc);
    assert(buf_size > 0);

    *buf = malloc(buf_size);
    const ssize_t write_result = capn_write_mem(&capn_enc, *buf, buf_size, 0);
    assert(write_result == buf_size);
    capn_free(&capn_enc);

    return buf_size;
}

static struct Thing decode_message(uint8_t * buf, ssize_t buf_size)
{
    // Decode the message.
    struct capn capn_dec;
    const int capn_init_result = capn_init_mem(&capn_dec, buf, buf_size, 0);
    assert(capn_init_result == 0);

    Thing_ptr p_thing_dec;
    p_thing_dec.p = capn_getp(capn_root(&capn_dec), 0, 1);

    struct Thing thing_dec;

    // Valgrind checks.
    char * vbits = malloc(sizeof thing_dec);
    VALGRIND_MAKE_MEM_UNDEFINED(&thing_dec, sizeof thing_dec);
    const int vresult_before = VALGRIND_GET_VBITS(&thing_dec, vbits, sizeof thing_dec);
    assert(vresult_before == 1);

    print_vbits("1", vbits, sizeof thing_dec);

    read_Thing(&thing_dec, p_thing_dec);
    const int vresult_after = VALGRIND_GET_VBITS(&thing_dec, vbits, sizeof thing_dec);
    assert(vresult_after == 1);

    print_vbits("2", vbits, sizeof thing_dec);

    capn_free(&capn_dec);

    return thing_dec;
}

int main(void)
{
    uint8_t * buf;
    ssize_t buf_size = encode_message(&buf);
    const struct Thing thing_dec = decode_message(buf, buf_size);

    free(buf);

    assert(thing_dec.first == 1);
    assert(thing_dec.second == EnumOne_oneB);
    assert(thing_dec.third == 3);
    assert(thing_dec.fourth == 4);
    assert(thing_dec.fifth == EnumTwo_twoB);
}
