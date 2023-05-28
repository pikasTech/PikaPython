#![crate_type = "staticlib"]
mod arg_list;
mod class_info;
mod compiler;
mod import_info;
mod method_info;
mod my_string;
mod object_info;
mod py_arg;
mod py_type;
mod script;
mod version_info;
mod entry;
mod decorator;

use entry::*;

extern crate libc;

#[no_mangle]
pub extern "C" fn pika_binder() {
    pika_compiler_entry();
}
