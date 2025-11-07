use std::collections::HashMap;

use crate::state::connection_data_internal::ConnectionData;

#[derive(Debug)]
pub struct TcpStorage {
    pub connections: Vec<ConnectionData>,
}

impl TcpStorage {
    pub fn new() -> Self {
        Self {
            connections: Vec::new(),
        }
    }

    pub fn insert(&mut self, connection_id: &str) {
        self.connections.push(ConnectionData {
            connection_id: connection_id.to_string(),
            files_to_chunks: HashMap::new(),
        });
        self.print();
    }

    pub fn remove(&mut self, connection_id: &str) {
        self.connections
            .retain(|x| x.connection_id != connection_id);
    }

    pub fn print(&self) {
        println!("{:?}", self.connections);
    }
}
