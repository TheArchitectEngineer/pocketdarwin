#[cfg(test)]
mod tests {
    use super::*;
    use std::ffi::CString;

    #[test]
    fn test_writeln() {
        let s = CString::new("Hello from Rust test!").unwrap();
        unsafe { i_writeln(s.as_ptr()); }
    }

    #[test]
    fn test_malloc_free() {
        unsafe {
            let size = 64;
            let ptr = i_malloc(size);
            assert!(!ptr.is_null(), "i_malloc returned null");

            // Fill memory with value 0xAA
            i_memset(ptr, 0xAA, size);

            // Verify first byte
            let first = *(ptr as *mut u8);
            assert_eq!(first, 0xAA);

            i_free(ptr, size);
        }
    }

    #[test]
    fn test_memcpy() {
        unsafe {
            let size = 8;
            let src = i_malloc(size);
            let dest = i_malloc(size);

            assert!(!src.is_null() && !dest.is_null());

            i_memset(src, 0x42, size);
            i_memcpy(dest, src, size);

            let first = *(dest as *mut u8);
            assert_eq!(first, 0x42);

            i_free(src, size);
            i_free(dest, size);
        }
    }
}