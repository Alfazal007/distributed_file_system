use std::sync::{Arc, Mutex};

use crate::{
    state::storage_unit::TcpStorage, tcp_connection_to_storage::tcp_handler::accept_connections,
};
use tokio::{net::TcpListener, sync::broadcast::Receiver};

pub async fn tcp_main(
    mut shutdown_receiver: Receiver<()>,
    tcp_storage: Arc<Mutex<TcpStorage>>,
) -> Result<(), String> {
    let listener = TcpListener::bind("127.0.0.1:8001")
        .await
        .map_err(|e| format!("Failed to bind TCP listener: {}", e))?;

    println!("TCP listener running on 127.0.0.1:8001");

    tokio::select! {
        _ = shutdown_receiver.recv() => {
            println!("TCP connection shutting down");
            Err("Shutdown signal received".to_string())
        }
        result = accept_connections(listener, tcp_storage) => {
            result
        }
    }
}
