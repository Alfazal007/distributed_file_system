use std::collections::HashMap;

#[derive(serde::Deserialize)]
pub enum TcpMessageType {
    JOIN,
    HEALTH,
}

#[derive(serde::Deserialize)]
pub struct TcpMessageFromStorage {
    pub message_type: TcpMessageType,
    pub message: Option<TcpHealthMessage>,
}

#[derive(serde::Deserialize)]
pub struct TcpHealthMessage {
    pub files_to_chunks: HashMap<String, Vec<String>>,
}
