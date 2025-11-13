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

    pub fn increment_index_and_return(&mut self) -> i32 {
        if self.index_to_write == -1 {
            self.index_to_write = 0;
            return 0;
        }
        self.index_to_write = (self.index_to_write + 1) % self.connections.len() as i32;
        return self.index_to_write;
    }
}
