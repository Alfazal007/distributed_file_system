use std::sync::{Arc, Mutex};

use tokio::{
    io::{AsyncBufReadExt, BufReader},
    net::{TcpListener, tcp},
    time::{Duration, interval},
};
use uuid::Uuid;

use crate::state::{
    message::{TcpMessageFromStorage, TcpMessageType},
    storage_unit::TcpStorage,
};

pub async fn accept_connections(
    listener: TcpListener,
    tcp_storage: Arc<Mutex<TcpStorage>>,
) -> Result<(), String> {
    loop {
        let tcp_storage_inner = Arc::clone(&tcp_storage);
        let (socket, addr) = listener
            .accept()
            .await
            .map_err(|e| format!("Failed to accept connection: {}", e))?;

        tokio::spawn(async move {
            let connection_id = Uuid::new_v4().to_string();
            let (reader, mut writer) = socket.into_split();
            let mut buf_reader = BufReader::new(reader);
            let mut buf = Vec::new();
            loop {
                tcp_storage_inner.lock().expect("dd").print();
                buf.clear();
                match buf_reader.read_until(b'\n', &mut buf).await {
                    Ok(0) => {
                        println!("Client {} disconnected", addr);
                        break;
                    }
                    Ok(_) => {
                        if let Ok(msg) = serde_json::from_slice::<TcpMessageFromStorage>(&buf) {
                            match msg.message_type {
                                TcpMessageType::JOIN => {
                                    tcp_storage_inner
                                        .lock()
                                        .expect("Deadlock in tcp message locking stage")
                                        .insert(&connection_id);
                                }
                                TcpMessageType::HEALTH => {}
                            }
                        }
                    }
                    Err(e) => {
                        println!("Error reading from {}: {}", addr, e);
                        break;
                    }
                }
            }
            tcp_storage_inner
                .lock()
                .expect("Deadlock in tcp message locking stage")
                .remove(&connection_id);
        });
    }
}
