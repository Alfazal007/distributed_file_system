use crate::{
    state::{
        message::{TcpMessageFromStorage, TcpMessageType},
        storage_unit::TcpStorage,
    },
    tcp_connection_to_storage::loop_health_check_sender::health_check_send,
};
use std::sync::{Arc, Mutex};
use tokio::{
    io::{AsyncBufReadExt, BufReader},
    net::TcpListener,
};
use uuid::Uuid;

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
            println!("New tcp connection arrived");
            let shared_conn_state = Arc::new(Mutex::new(true));
            let connection_id = Uuid::new_v4().to_string();
            let (reader, writer) = socket.into_split();
            let mut buf_reader = BufReader::new(reader);
            let mut buf = Vec::new();
            tokio::spawn(health_check_send(writer, Arc::clone(&shared_conn_state)));
            loop {
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
                                    println!("here");
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
            let mut update_shared_state = shared_conn_state.lock().unwrap();
            *update_shared_state = false;
            tcp_storage_inner
                .lock()
                .expect("Deadlock in tcp message locking stage")
                .remove(&connection_id);
        });
    }
}
