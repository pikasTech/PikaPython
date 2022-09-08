#include "PikaNN.h"
#include "PikaNN_common.h"

extern uint8_t mnist_pic[28 * 28];
extern const uint8_t mdl_data[2408];
tm_err_t layer_cb(tm_mdl_t* mdl, tml_head_t* lh);
void parse_output(tm_mat_t* outs);

void _lm_test(void) {
    TM_PRINTF("mnist demo\n");
    tm_mdl_t mdl;

    for (int i = 0; i < 28 * 28; i++) {
        TM_PRINTF("%3d,", mnist_pic[i]);
        if (i % 28 == 27)
            TM_PRINTF("\n");
    }

    tm_mat_t in = {3, 28, 28, 1, {NULL}};
    tm_err_t res;
    tm_stat((tm_mdlbin_t*)mdl_data);

    res = tm_load(&mdl, mdl_data, NULL, layer_cb, &in);
    if (res != TM_OK) {
        TM_PRINTF("tm model load err %d\n", res);
        return;
    }

    uint32_t _start, _finish;
    float _time;
    _start = TM_GET_US();

    tm_mat_t in_uint8 = {3, 28, 28, 1, {(mtype_t*)mnist_pic}};
    tm_mat_t outs[1];
#if (TM_MDL_TYPE == TM_MDL_INT8) || (TM_MDL_TYPE == TM_MDL_INT16)
    res = tm_preprocess(&mdl, TMPP_UINT2INT, &in_uint8, &in);
#else
    res = tm_preprocess(mdl, TMPP_UINT2FP01, &in_uint8, in);
#endif
    // TM_DBGT_START();
    _start = TM_GET_US();
    res = tm_run(&mdl, &in, outs);
    // TM_DBGT("tm_run");

    _finish = TM_GET_US();
    _time = (float)(_finish - _start) / 1000.0;
    TM_PRINTF("===%s use %.3f ms\n", "tm_run", _time);
    //_start = TM_GET_US();
    if (res == TM_OK)
        parse_output(outs);
    else
        TM_PRINTF("tm run error: %d\n", res);

    tm_unload(&mdl);

    return;
}

void PikaNN_test(PikaObj* self) {
    _lm_test();
}
