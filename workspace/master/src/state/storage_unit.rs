use std::net::IpAddr;

use crate::{
    protos::data::HealthMessageFromStorageToMaster, state::connection_data_internal::ConnectionData,
};

#[derive(Debug)]
pub struct TcpStorage {
    pub connections: Vec<ConnectionData>,
    pub index_to_write: i32,
}

impl TcpStorage {
    pub fn new() -> Self {
        Self {
            connections: Vec::new(),
            index_to_write: -1,
        }
    }

    pub fn insert(&mut self, connection_id: &str, ip: IpAddr) {
        self.connections.push(ConnectionData {
            connection_id: connection_id.to_string(),
            files_to_chunks: Vec::new(),
            ip,
        });
        if self.index_to_write == -1 {
            self.index_to_write = 0;
        }
        self.print();
    }

    pub fn remove(&mut self, connection_id: &str) {
        self.connections
            .retain(|x| x.connection_id != connection_id);
    }

    pub fn update_state(
        &mut self,
        connection_id: &str,
        file_state: HealthMessageFromStorageToMaster,
    ) {
        if let Some(conn) = self
            .connections
            .iter_mut()
            .find(|c| c.connection_id == connection_id)
        {
            conn.update_state(file_state);
        } else {
            eprintln!("Connection not found: {}", connection_id);
        }
        self.print();
    }

    pub fn print(&self) {
        println!("{:?}", self.connections);
    }

    pub fn increment_index_and_return_addr(&mut self) -> Option<IpAddr> {
        if self.connections.len() == 0 {
            return None;
        }
        if self.index_to_write < 0 {
            self.index_to_write = 0;
        } else {
            self.index_to_write = (self.index_to_write + 1) % self.connections.len() as i32;
        }
        let conn = self.connections.get(self.index_to_write as usize);
        if let Some(conn) = conn {
            return Some(conn.ip);
        } else {
            return None;
        }
    }

    pub fn get_location(&self, file_name: &str, chunk_id: i32) -> Option<IpAddr> {
        for conn in self.connections.iter() {
            for files in conn.files_to_chunks.iter() {
                if files.filename == file_name {
                    for chunk in files.chunk_ids.iter() {
                        if *chunk == chunk_id {
                            return Some(conn.ip);
                        }
                    }
                }
            }
        }
        return None;
    }

    pub fn get_multiple_chunks(&self, file_name: &str, chunk_ids: Vec<i32>) -> Vec<Option<IpAddr>> {
        let mut res = Vec::new();
        for id in chunk_ids {
            res.push(self.get_location(file_name, id));
        }
        return res;
    }

    pub fn get_chunks_for_file(&self, file_name: &str) -> Vec<i32> {
        let mut res = Vec::new();
        for connection in self.connections.iter() {
            for mapping_inner in connection.files_to_chunks.iter() {
                if mapping_inner.filename == file_name {
                    for id in mapping_inner.chunk_ids.iter() {
                        res.push(id.clone());
                    }
                }
            }
        }
        res.sort();
        return res;
    }

    pub fn get_file_list(&mut self) -> Vec<String> {
        let mut res = Vec::new();
        if self.connections.len() == 0 {
            return res;
        }
        for conn_data in self.connections.iter() {
            for file_mappings in conn_data.files_to_chunks.iter() {
                res.push(file_mappings.filename.clone());
            }
        }
        return res;
    }
}
