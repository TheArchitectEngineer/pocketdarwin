extern "C" {
    fn i_malloc(size: usize) -> *mut std::ffi::c_void;
    fn i_free(ptr: *mut std::ffi::c_void, size: usize);
    fn i_writeln(s: *const std::os::raw::c_char);
}

use std::ffi::CString;

fn main() {
    unsafe {
        let s = CString::new("*** The Standard Library works. ***").unwrap();
        i_writeln(s.as_ptr());

        let mem = i_malloc(100);
        i_free(mem, 100);
    }
}