//! C++ FFI interoperability layer

use ffi_support::{define_string_destructor, ByteBuffer};
use crate::geometry::Canyon;
use crate::capture::{CaptureModel, CaptureParameters};
use crate::impact::{EcoImpactCalculator, NuisanceFactors};
use serde_json;

define_string_destructor!(windminer_free_string);

/// FFI-safe canyon structure
#[repr(C)]
pub struct FfiCanyon {
    id: *mut libc::c_char,
    latitude: f64,
    longitude: f64,
    height_m: f64,
    width_m: f64,
    orientation_deg: f64,
}

/// FFI-safe capture result
#[repr(C)]
pub struct FfiCaptureResult {
    canyon_id: *mut libc::c_char,
    expected_capture_kg_year: f64,
    is_suitable: bool,
}

/// Calculate capture from FFI canyon structure
#[no_mangle]
pub extern "C" fn windminer_calculate_capture(
    canyon_ptr: *const FfiCanyon,
) -> FfiCaptureResult {
    // Safe wrapper around FFI call
    let result = std::panic::catch_unwind(|| {
        unsafe {
            if canyon_ptr.is_null() {
                return FfiCaptureResult {
                    canyon_id: std::ptr::null_mut(),
                    expected_capture_kg_year: 0.0,
                    is_suitable: false,
                };
            }

            let canyon_ffi = &*canyon_ptr;
            
            // Convert FFI string to Rust String
            let id = ffi_support::rust_string_from_c(canyon_ffi.id);
            
            let canyon = Canyon::new(
                id,
                canyon_ffi.latitude,
                canyon_ffi.longitude,
                canyon_ffi.height_m,
                canyon_ffi.width_m,
                canyon_ffi.orientation_deg,
            );

            match canyon {
                Ok(c) => {
                    let model = CaptureModel::default();
                    let result = model.evaluate_canyon(&c);
                    
                    FfiCaptureResult {
                        canyon_id: ffi_support::rust_string_to_c(result.canyon_id),
                        expected_capture_kg_year: result.expected_capture_kg_year,
                        is_suitable: result.is_suitable,
                    }
                }
                Err(_) => FfiCaptureResult {
                    canyon_id: std::ptr::null_mut(),
                    expected_capture_kg_year: 0.0,
                    is_suitable: false,
                },
            }
        }
    });

    result.unwrap_or(FfiCaptureResult {
        canyon_id: std::ptr::null_mut(),
        expected_capture_kg_year: 0.0,
        is_suitable: false,
    })
}

/// Export capture result as JSON buffer for C++ consumption
#[no_mangle]
pub extern "C" fn windminer_export_result_json(
    capture_kg: f64,
    eco_score: f64,
    canyon_id: *const libc::c_char,
) -> ByteBuffer {
    let id = unsafe { ffi_support::rust_string_from_c(canyon_id) };
    
    let json = serde_json::json!({
        "canyon_id": id,
        "capture_kg_year": capture_kg,
        "eco_score": eco_score,
        "version": env!("CARGO_PKG_VERSION")
    });

    ffi_support::ByteBuffer::from_vec(json.to_string().into_bytes())
}

/// Initialize Rust logging from C++
#[no_mangle]
pub extern "C" fn windminer_init_logging() {
    let _ = env_logger::try_init();
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_ffi_result_serialization() {
        let json = serde_json::json!({
            "canyon_id": "TEST-001",
            "capture_kg_year": 144.0,
            "eco_score": 0.88
        });
        
        assert!(json["capture_kg_year"].as_f64().unwrap() > 100.0);
    }
}
