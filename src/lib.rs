#![allow(dead_code)]
#![allow(non_camel_case_types)]

use std::ffi::{c_char, c_int};

pub type NPType = u8;
pub type NPOption = u8;

pub const NP_TYPE_INFO: NPType = 0x00;
pub const NP_TYPE_REPLY: NPType = 0x01;
pub const NP_TYPE_UNSET: NPType = 0xff;

pub const NP_INFO_TEXT: NPOption = 0x00;

pub const NP_REPLY_VALID: NPOption = 0x00;
pub const NP_REPLY_BAD_TYPE: NPOption = 0x01;
pub const NP_REPLY_BAD_OPTION: NPOption = 0x02;
pub const NP_REPLY_BAD_SIZE: NPOption = 0x03;
pub const NP_REPLY_INVALID_DATA: NPOption = 0x04;
pub const NP_REPLY_UNSET: NPOption = 0xff;

pub const NP_UNSET_UNSET: NPOption = 0xff;

pub const NP_MIN_PKT_SIZE: usize = 4;
pub const NP_MAX_PKT_SIZE: usize = 4096;

#[repr(C)]
#[derive(Debug, PartialEq, Eq)]
pub struct np_packet {
    pub action: NPType,
    pub option: NPOption,
    pub size: u16,

    pub payload: *mut c_char,
}

#[repr(C)]
#[derive(Default)]
pub struct serialize_result {
    pub reply: u16,
    pub size: c_int,
}

extern "C" {
    pub fn print_packet(pkt: *mut np_packet);

    pub fn serialize(
        buffer: *mut [u8; NP_MAX_PKT_SIZE],
        pkt: *mut np_packet,
        result: *mut serialize_result,
    );

    pub fn deserialize(
        buffer: *mut [u8; NP_MAX_PKT_SIZE],
        pkt: *mut np_packet,
        result: *mut serialize_result,
    );
}

#[cfg(test)]
mod test {
    use crate::*;
    use std::ffi::CString;

    #[test]
    fn serialize_and_deserialize() {
        let payload = CString::new("Hello").expect("CString::new failed");

        let mut packet = np_packet {
            action: 0x00,
            option: 0x00,
            size: (payload.as_bytes().len() + NP_MIN_PKT_SIZE) as u16,
            payload: payload.into_raw(),
        };

        let mut buffer: [u8; NP_MAX_PKT_SIZE] = [0; NP_MAX_PKT_SIZE];
        let mut result: serialize_result = serialize_result::default();
        unsafe {
            serialize(&mut buffer, &mut packet, &mut result);

            deserialize(&mut buffer, &mut packet, &mut result);

            print_packet(&mut packet);
        }

        unsafe {
            assert_eq!(
                CString::from_raw(packet.payload),
                CString::new("Hello").expect("CString::new failed")
            );
        }
    }
}
