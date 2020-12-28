#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <test.capnp.h>
#include <valgrind/memcheck.h>

static int capn_size(struct capn *c)
{
    size_t headersz, datasz = 0;
    struct capn_ptr root;
    struct capn_segment *seg;
    uint32_t i;

    if (c->segnum == 0)
        return -1;

    root = capn_root(c);
    seg = root.seg;

    headersz = 8 * ((2 + c->segnum) / 2);

    for (i = 0; i < c->segnum; i++, seg = seg->next) {
        if (0 == seg)
            return -1;
        datasz += seg->len;
    }
    if (0 != seg)
        return -1;

    return (int) headersz+datasz;
}

static void print_vbits(char * prefix, char * vbits, size_t length)
{
    printf("%s: ", prefix);
    for (size_t idx = 0; idx < length; idx++)
    {
        printf("%02hhx", vbits[idx]);
    }
    printf("\n");
}

static ssize_t encode_message(uint8_t ** buf)
{
    struct Thing thing_enc = {
        .which = Thing_first,
        .first = EnumOne_oneA,
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
    assert(vresult_before <= 1);

    if (vresult_before == 1)
    {
        print_vbits("Before", vbits, sizeof thing_dec);
    }


    read_Thing(&thing_dec, p_thing_dec);
    const int vresult_after = VALGRIND_GET_VBITS(&thing_dec, vbits, sizeof thing_dec);
    assert(vresult_after <= 1);

    if (vresult_after == 1)
    {
        print_vbits("After ", vbits, sizeof thing_dec);
    }

    capn_free(&capn_dec);

    return thing_dec;
}

int main(void)
{
    uint8_t * buf;
    ssize_t buf_size = encode_message(&buf);
    const struct Thing thing_dec = decode_message(buf, buf_size);

    free(buf);

    assert(thing_dec.which == Thing_first);
    assert(thing_dec.first == EnumOne_oneA);
}
