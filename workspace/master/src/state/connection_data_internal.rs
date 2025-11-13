use std::net::IpAddr;

use crate::protos::data::{FileMappings, HealthMessageFromStorageToMaster};

#[derive(Debug)]
pub struct ConnectionData {
    pub connection_id: String,
    pub ip: IpAddr,
    pub files_to_chunks: ::prost::alloc::vec::Vec<FileMappings>,
}

impl ConnectionData {
    pub fn update_state(&mut self, file_state: HealthMessageFromStorageToMaster) {
        self.files_to_chunks = file_state.file_mappings;
    }
}
