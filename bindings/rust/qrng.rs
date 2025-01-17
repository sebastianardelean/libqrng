use std::os::raw::c_char;
use std::os::raw::{c_void, c_int, c_longlong, c_char,c_double, c_float, c_size_t};

typedef struct CoolStruct {
    int x;
    int y;
} CoolStruct;

void cool_function(int i, char c, CoolStruct* cs);

/* File: cool_bindings.rs */
#[repr(C)]
pub struct CoolStruct {
    pub x: cty::c_int,
    pub y: cty::c_int,
}

extern "C" {
    pub fn cool_function(
        i: cty::c_int,
        c: cty::c_char,
        cs: *mut CoolStruct
    );
}


extern "C" {
    pub fn qrng_open(
        device_domain_address: *const c_char
    ) -> c_int;

    pub fn qrng_close();

    pub fn qrng_random_stream(
        stream: *mut c_void,
        size: c_size_t
    ) -> c_int;

    pub fn qrng_random_double(
        min: c_double,
        max: c_double,
        samples: c_size_t,
        buffer: *mut c_double,
    ) -> c_int;


    pub fn qrng_random_float(
        min: c_float,
        max: c_float,
        samples: c_size_t,
        buffer: *mut c_float,
    ) -> c_int;

    pub fn qrng_random_int64(
        min: c_longlong,
        max: c_longlong,
        samples: c_size_t,
        buffer: *mut c_longlong,
    ) -> c_int;

    pub fn qrng_random_int32(
        min: c_int,
        max: c_int,
        samples: c_size_t,
        buffer: *mut c_int,
    ) -> c_int;

    pub fn qrng_firmware_info(
        buffer: *mut c_void,
    ) -> c_int;

    pub fn qrng_system_info(
        buffer: *mut c_void,
    ) -> c_int;
}
