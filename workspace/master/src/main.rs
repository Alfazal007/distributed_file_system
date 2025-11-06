use std::{
    sync::{Arc, Mutex},
    time::Duration,
};

use tokio::time::interval;

use crate::{
    http_connection_to_client::http_main::http_main, state::storage_unit::TcpStorage,
    tcp_connection_to_storage::tcp_main::tcp_main,
};

pub mod http_connection_to_client;
pub mod state;
pub mod tcp_connection_to_storage;

#[tokio::main]
async fn main() {
    let tcp_message_state = Arc::new(Mutex::new(TcpStorage::new()));
    let (shutdown_channel_sender, shutdown_channel_receiver) = tokio::sync::broadcast::channel(1);
    let mut tcp_handle = tokio::spawn(tcp_main(
        shutdown_channel_receiver.resubscribe(),
        tcp_message_state,
    ));
    let mut http_handle = tokio::spawn(http_main(shutdown_channel_receiver.resubscribe()));

    tokio::select! {
        _ = tokio::signal::ctrl_c() => {
            println!("Shutdown signal received");
            let _ = shutdown_channel_sender.send(());
        }
        tcp_result = &mut tcp_handle => {
            println!("TCP task ended unexpectedly");
            let _ = shutdown_channel_sender.send(());
            if let Ok(Err(e)) = tcp_result {
                eprintln!("TCP error: {}", e);
            }
        }
        http_result = &mut http_handle => {
            println!("HTTP task ended unexpectedly");
            let _ = shutdown_channel_sender.send(());
            if let Ok(Err(e)) = http_result {
                eprintln!("HTTP error: {}", e);
            }
        }
    };

    let _ = tokio::join!(tcp_handle, http_handle);
    println!("All services shut down");
}
