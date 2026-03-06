use std::os::raw::c_char;

unsafe extern "C" {
    pub fn i_malloc(size: usize) -> *mut libc::c_void;
    pub fn i_free(ptr: *mut libc::c_void, size: usize);
    pub fn i_memset(ptr: *mut libc::c_void, value: i32, size: usize);
    pub fn i_memcpy(dest: *mut libc::c_void, src: *const libc::c_void, size: usize);
    pub fn i_writeln(s: *const c_char);
}